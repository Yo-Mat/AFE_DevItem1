/******************************************************************************
* File Name: led_task.c
*
* Version: 1.00
*
******************************************************************************/

/* Header file includes */
#include "led_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "project.h"
#include <stdio.h>

// タイミングデータ定義
#define AFE4404_TIMING_PAIRS			\
	{ AFE440X_PRPCOUNT,	    39999	},	\
	{ AFE440X_LED2LEDSTC,	0	    },	\
	{ AFE440X_LED2LEDENDC,	398	    },	\
	{ AFE440X_LED2STC,  	80	    },	\
	{ AFE440X_LED2ENDC,	    398	    },	\
	{ AFE440X_ADCRSTSTCT0,	5600	},	\
	{ AFE440X_ADCRSTENDCT0,	5606	},	\
	{ AFE440X_LED2CONVST,	5607	},	\
	{ AFE440X_LED2CONVEND,	6066	},	\
	{ AFE4404_LED3LEDSTC,	0/*400*/},	\
	{ AFE4404_LED3LEDENDC,	0/*798*/},	\
	{ AFE440X_ALED2STC,	    480	    },	\
	{ AFE440X_ALED2ENDC,	798	    },	\
	{ AFE440X_ADCRSTSTCT1,	6068	},	\
	{ AFE440X_ADCRSTENDCT1,	6074	},	\
	{ AFE440X_ALED2CONVST,	6075	},	\
	{ AFE440X_ALED2CONVEND,	6534	},	\
	{ AFE440X_LED1LEDSTC,	800	    },	\
	{ AFE440X_LED1LEDENDC,	1198	},	\
	{ AFE440X_LED1STC,	    880	    },	\
	{ AFE440X_LED1ENDC,	    1198	},	\
	{ AFE440X_ADCRSTSTCT2,	6536	},	\
	{ AFE440X_ADCRSTENDCT2,	6542	},	\
	{ AFE440X_LED1CONVST,	6543	},	\
	{ AFE440X_LED1CONVEND,	7003	},	\
	{ AFE440X_ALED1STC,	    1280	},	\
	{ AFE440X_ALED1ENDC,	1598	},	\
	{ AFE440X_ADCRSTSTCT3,	7005	},	\
	{ AFE440X_ADCRSTENDCT3,	7011	},	\
	{ AFE440X_ALED1CONVST,	7012	},	\
	{ AFE440X_ALED1CONVEND,	7471	},	\
	{ AFE440X_PDNCYCLESTC,	7671	},	\
	{ AFE440X_PDNCYCLEENDC,	39199	}
// AFE初期化レジスタ設定値
static const struct reg_sequence afe_init[] = {
	AFE4404_TIMING_PAIRS,
	{ AFE440X_CONTROL1,     AFE440X_CONTROL1_TIMEREN    },
	{ AFE4404_TIA_GAIN_SEP, AFE440X_TIAGAIN_ENSEPGAIN(TIA_G2)   },
	{ AFE4404_TIA_GAIN,     AFE440X_TIAGAIN_ENGAIN(TIA_G1)      },
	{ AFE4404_ILED_CURRENT, AFE440X_ILED_CURRENT(I_LED1,I_LED2,I_LED3)  },
	{ AFE440X_CONTROL2,     AFE440X_CONTROL2_OSC_ENABLE	},
	{ AFE440X_ALARM,        AFE440X_ENABLE_CLKOUT       },
	{ AFE440X_CONTROL3,     0                           },
	{ AFE4404_PROG_TG_STC,  0                           },
	{ AFE4404_PROG_TG_ENDC, 0                           },
	{ AFE4404_CLKDIV_PRF,   AFE440X_CLKDIV_PRF          },
	{ AFE4404_OFFDAC,       0                           },
    { AFE4404_DEC,          0                           },
};

// AFEデータ
static struct AfeData afe_data[AFE_ID_NUM] = {
    { false, false,
      {	{ AFE440X_LED2VAL,       0 },
        { AFE440X_LED2_ALED2VAL, 0 },
    	{ AFE440X_LED1VAL,       0 },
        { AFE440X_LED1_ALED1VAL, 0 } } },
    { false, false,
      {	{ AFE440X_LED2VAL,       0 },
        { AFE440X_LED2_ALED2VAL, 0 },
    	{ AFE440X_LED1VAL,       0 },
        { AFE440X_LED1_ALED1VAL, 0 } } }
};
#define AFE_RDY(a) afe_data[(a)].rdy

// AFE算出値
static struct CalValue afe_calval[AFE_ID_NUM];
uint8_t SpO2 = 15;
AFE_ID_E AFE_id = AFE_ID;

// AFE0割り込みハンドラ
static void AFE0_hdr()
{
    afe4404ContiRead(I2C_1_HW, (REG_SEQENCE_T *)&afe_data[AFE0_ID].data,
                     sizeof(afe_data[AFE0_ID].data)/sizeof(REG_SEQENCE_T));
    AFE_RDY(AFE0_ID) = true;
}

// AFE1割り込みハンドラ
static void AFE1_hdr()
{
    afe4404ContiRead(I2C_2_HW, (REG_SEQENCE_T *)&afe_data[AFE1_ID].data,
                     sizeof(afe_data[AFE1_ID].data)/sizeof(REG_SEQENCE_T));
    AFE_RDY(AFE1_ID) = true;
}

void AFE_act(AFE_ID_E afe_id)
{
    uint32_t pwrOn = AFE440X_CONTROL2_OSC_ENABLE;
    uint32_t pwrDwn = AFE440X_CONTROL2_OSC_ENABLE + AFE440X_CONTROL2_PDN_AFE;

    // AFE0動作
    if (afe_id == AFE0_ID) {
        NVIC_DisableIRQ(ISR_AFE1_cfg.intrSrc);  // AFE1 停止
        afe4404Write(I2C_2_HW, AFE440X_CONTROL2, (uint8_t*)&pwrDwn, REG_DATA_LEN);
        NVIC_EnableIRQ(ISR_AFE0_cfg.intrSrc);   // AFE0 許可
        afe4404Write(I2C_1_HW, AFE440X_CONTROL2, (uint8_t*)&pwrOn, REG_DATA_LEN);
    }
    // AFE1動作
    else if (afe_id == AFE1_ID) {
        NVIC_DisableIRQ(ISR_AFE0_cfg.intrSrc);  // AFE0 停止
        afe4404Write(I2C_1_HW, AFE440X_CONTROL2, (uint8_t*)&pwrDwn, REG_DATA_LEN);
        NVIC_EnableIRQ(ISR_AFE1_cfg.intrSrc);   // AFE1 許可
        afe4404Write(I2C_2_HW, AFE440X_CONTROL2, (uint8_t*)&pwrOn, REG_DATA_LEN);
    }
    // 両AFE停止
    else {
        NVIC_DisableIRQ(ISR_AFE0_cfg.intrSrc);  // AFE0 停止
        afe4404Write(I2C_1_HW, AFE440X_CONTROL2, (uint8_t*)&pwrDwn, REG_DATA_LEN);
        NVIC_DisableIRQ(ISR_AFE1_cfg.intrSrc);  // AFE1 停止
        afe4404Write(I2C_2_HW, AFE440X_CONTROL2, (uint8_t*)&pwrDwn, REG_DATA_LEN);
    }
    
}

// サンプリングカウンタをクリア
void AFE_clr(AFE_ID_E afe_id)
{
    memset(&afe_calval[afe_id], 0, sizeof(struct CalValue));
    afe_calval[afe_id].skip_cnt = SMP_SKIP;
}

void AFE_init(AFE_ID_E afe_id)
{
    uint32_t data = 0;
    
    printf("Start AFE Control\r\n");
    printf("Operation cycle: %3.1f min.\r\n", (double)PWM_CYC_config.period0/60000u);
    printf("Operating  time: %u sec.\r\n", (unsigned)PWM_TIM_config.period0/1000u);
    printf("  Selection AFE: %s\r\n", afe_id==AFE0_ID ? "AFE0" : "AFE1");
    printf("  LED1 TIG gain: %u korm\r\n", tia_reg[TIA2_ID]);
    printf("  LED1  current: %3.1f mA\r\n", I_LED2 * CURR_STEP);
    printf("  LED2 TIG gain: %u korm\r\n", tia_reg[TIA1_ID]);
    printf("  LED2  current: %3.1f mA\r\n\r\n", I_LED1 * CURR_STEP);
    printf("AFE,,LED2amb,LED1amb,,LED2,,,,,LED1,,,,,R\r\n");
    printf(",,,,,AC,MIN,MAX,DC,,AC,MIN,MAX,DC,,\r\n");
    AFE_clr(afe_id);   // サンプリングカウンタをクリア

    if (afe_id == AFE0_ID) {
        afe4404Write(I2C_1_HW, AFE440X_CONTROL0, (uint8_t*)&data, REG_DATA_LEN);
        afe4404ContiWrite(I2C_1_HW, (REG_SEQENCE_T *)&afe_init, sizeof(afe_init)/sizeof(REG_SEQENCE_T));
        Cy_SysInt_Init(&ISR_AFE0_cfg, AFE0_hdr);
    }
    else {
        afe4404Write(I2C_2_HW, AFE440X_CONTROL0, (uint8_t*)&data, REG_DATA_LEN);
        afe4404ContiWrite(I2C_2_HW, (REG_SEQENCE_T *)&afe_init, sizeof(afe_init)/sizeof(REG_SEQENCE_T));
        Cy_SysInt_Init(&ISR_AFE1_cfg, AFE1_hdr);
    }
    
    AFE_act(AFE_id);    // 動作AFE 
}

// AFEデータサンプリング
static bool AFE_samp(AFE_ID_E afe_id)
{
    bool retval = false;
    struct reg_sequence *data = afe_data[afe_id].data;  // AFEデータ
    struct CalValue *cval = &afe_calval[afe_id];        // AFE1算出値

    // センサデータ読込み
    cval->led2 = (double)(data+1)->val/10000;
    cval->aled2= (double)(data+1)->val/10000;
    cval->led1 = (double)(data+2)->val/10000;
    cval->aled1= (double)(data+3)->val/10000;
    //printf("led/aled,%f,%f,%f,%f\r\n", cval->led2, cval->aled2, cval->led1, cval->aled1);

    AFE_RDY(afe_id) = false;
    // サンプリング1回目
    if (cval->sample_cnt == 0) {
        // LED2/LED1の最小値,最大値を初期化
        cval->led2_min = cval->led2_max = cval->aled2;
        cval->led1_min = cval->led1_max = cval->aled1;
        //printf("min/max,%f,%f,%f,%f\r\n", cval->led2_min, cval->led2_max, cval->led1_min, cval->led1_max);
    }
    // サンプリング2回目以降
    else {
        // LED2/LED1の最小値,最大値を更新
        if (cval->led2_max < cval->aled2)
            cval->led2_max = cval->aled2;
        else if(cval->led2_min > cval->aled2)
            cval->led2_min = cval->aled2;
        if (cval->led1_max < cval->aled1)
            cval->led1_max = cval->aled1;
        else if(cval->led1_min > cval->aled1)
            cval->led1_min = cval->aled1;
        //printf("min/max,%f,%f,%f,%f\r\n", cval->led2_min, cval->led2_max, cval->led1_min, cval->led1_max);
    }
    // 指定サンプリング回カウントしない
    if (cval->skip_cnt <= 0)
        cval->sample_cnt++;
    else cval->skip_cnt--;
    
    // サンプリング最終回
    if (cval->sample_cnt >= SMP_NUM) {
        printf("%d,,", afe_id);
        printf("%f,%f,,", cval->aled2, cval->aled1);
        // LED2/LED1のAC値,DC値
        cval->led2_ac = cval->led2_max - cval->led2_min;
        cval->led2_dc = (cval->led2_max + cval->led2_min) / 2.0;
        cval->led1_ac = cval->led1_max - cval->led1_min;
        cval->led1_dc = (cval->led1_max + cval->led1_min) / 2.0;
        // R値を算出
        cval->R = (cval->led2_ac / cval->led2_dc) / (cval->led1_ac / cval->led1_dc);
        printf("%f,%f,%f,%f,,", cval->led2_ac, cval->led2_min, cval->led2_max, cval->led2_dc);
        printf("%f,%f,%f,%f,,", cval->led1_ac, cval->led1_min, cval->led1_max, cval->led1_dc);
        printf("%f\r\n", -3.9*cval->R+100.7);
        // BLE送信
        SpO2 = -3.9*cval->R+100.7;
        cy_stc_ble_gatt_handle_value_pair_t  handleValue = {
                     .attrHandle = CY_BLE_SPO2_SPO2DATA_CHAR_HANDLE,
                     .value.val = (uint8_t *)&SpO2,
                     .value.len = 1,
                    };
        Cy_BLE_GATTS_WriteAttributeValueLocal(&handleValue); 
        Cy_BLE_GATTS_SendNotification(&cy_ble_connHandle[0], &handleValue);
        // 窓を進める
        afe_calval[afe_id].sample_cnt = 0;
        retval = true;
    }
    else {
//        printf("\r\n");
    }
    return retval;
}

// LEDタスクメイン」
void Task_LED (void *pvParameters)
{  
    (void)pvParameters ;

    for(;;) {
        // データサンプリング
        if (AFE_RDY(AFE0_ID))
            AFE_samp(AFE0_ID);
        if (AFE_RDY(AFE1_ID))
            AFE_samp(AFE1_ID);
    }
}
