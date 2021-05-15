/******************************************************************************
* File Name: led_task.h
*
* Version: 1.00
*
*******************************************************************************/

/* Include Guard */
#ifndef LED_TASK_H
#define LED_TASK_H

#include "AFE4404.h"

typedef enum AfeId {
    AFE0_ID,
    AFE1_ID,
    NOAFE_ID,
    AFE_ID_NUM,
} AFE_ID_E;

typedef enum LedId {
    LED2_ID,
    LED1_ID,
    LED_ID_NUM,
} LED_ID_E;

// AFEデータ
typedef struct AfeData {
    volatile bool ena;
    volatile bool rdy;
    struct reg_sequence data[4];
} AFE_DATA_T;

// AFE算出値
typedef struct CalValue {
    double led2, aled2;                 // LED2データ
    double led1, aled1;                 // LED1データ
    uint16_t sample_cnt;                // サンプル数
    uint8  skip_cnt;                    // サンプリングスキップ数
    double led2_min, led2_max;          // LED2最小値、最大値
    double led1_min, led1_max;          // LED1最小値、最大値
    double led2_sum, led1_sum;          // LED2/LED1データ積算値
    double led2_dc, led2_ac;            // LED2 AC/DC値
    double led1_dc, led1_ac;            // LED1 AC/DC値
    double R;                           // LED受光強度
} CAL_VALUE_T;

// TIAゲインテーブル
#define TIA_GAIN_NUM 7
static const uint32_t tia_gain[TIA_GAIN_NUM] = {6,0,1,2,3,4,5};  // 1M,500k,250k,100,50k,25k,10k
static const uint16_t tia_reg[TIA_GAIN_NUM] = {1000,500,250,100,50,25,10};
// LED電流値 最小,最大
static const uint8_t LED_CURR_MIN = 6;  // 4.8mA step +0.8mA
static const uint8_t LED_CURR_MAX = 63; // 50.4mA

// 初期値
#define SMP_NUM     100     // サンプル数(時間窓)
#define AFE_ID      AFE0_ID // 選択AFE
#define SMP_SKIP    5       // サンプルスキップ数
// TIAゲイン
#define TIA2_ID     6       // LED2 TIA 初期値ID(1kΩ)
#define TIA1_ID     6       // LED1 TIA 初期値ID(1kΩ)
#define EnsepGain   1       // separate gain enabled(1) or not(0).
#define TIA_G2      5       // resistance for second TIA 初期値(1kΩ)
#define TIA_G1      5       // resistance for first TIA 初期値(1kΩ)
#define TIA_C2      1       // capacitance for second TIA
#define TIA_C1      1       // capacitance for first TIA
#define RROG_t_en   0       // This bit replaces the ADC_RDY output with a fullyprogrammable signal from the timing engine
#define AFE440X_TIAGAIN_ENSEPGAIN(a)   (a) + (TIA_C2 << 3 ) + (EnsepGain  << 15 )
#define AFE440X_TIAGAIN_ENGAIN(a)      (a) + (TIA_C1 << 3 ) + (RROG_t_en  << 8 )
// LED電流
#define I_LED1      6       // IR led current 0-63 (=0-50.4mA) 初期値(*0.8mA)
#define I_LED2      6       // RED led current 0-63 (=0-50.4mA) 初期値(*0.8mA)
#define I_LED3      0       // led current 0-63 (=0-50mA)
#define AFE440X_ILED_CURRENT(a,b,c)        (a) + ((b) << 6 ) + ((c) << 12 )
#define CURR_STEP   0.8

extern uint8_t SpO2;
extern AFE_ID_E AFE_id;
extern AFE_ID_E AFE_st;

void AFE_clr(AFE_ID_E afe_id);
void AFE_init(AFE_ID_E afe_id);
void AFE_act(AFE_ID_E afe_id);
void Task_LED (void *pvParameters );    

#endif /* LED_TASK_H */
/* [] END OF FILE */
