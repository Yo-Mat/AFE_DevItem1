/******************************************************************************
* File Name: main_cm4.c
*
* Version: 1.0
*
* Description: This code example demonstrates how to interface PSoCR 6 MCU with 
*              a BMI160 Motion Sensor over I2C interface from a FreeRTOS task.
*
* Related Document: CE223468_PSoC6_Interfacing_BMI160(I2C)_FreeRTOS.pdf
*
* Hardware Dependency: CY8CKIT-062-BLE PSoC 6 BLE Pioneer Kit
*                      CY8CKIT-028-EPD E-INK Display Shield
*
*******************************************************************************
* Copyright (2018), Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* (“Software”), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries (“Cypress”) and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software (“EULA”).
*
* If no EULA applies, Cypress hereby grants you a personal, nonexclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress’s integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, 
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress 
* reserves the right to make changes to the Software without notice. Cypress 
* does not assume any liability arising out of the application or use of the 
* Software or any product or circuit described in the Software. Cypress does 
* not authorize its products for use in any products where a malfunction or 
* failure of the Cypress product may reasonably be expected to result in 
* significant property damage, injury or death (“High Risk Product”). By 
* including Cypress’s product in a High Risk Product, the manufacturer of such 
* system or application assumes all risk of such use and in doing so agrees to 
* indemnify Cypress against all liability.
*******************************************************************************/
/* Header file includes */
#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "AFE4404.h"
#include "led_task.h"
#include "bleTask.h"
#include <stdio.h>
#include <limits.h>

#define AFE_OPT_SENSOR_PRIORITY             (1)
#define AFE_OPT_SENSOR_STACK_SIZE           (400u)
#define AFE_OPT_SENSOR_COMMAND_QUEUE_LEN    (1u)
#define AFE_OPT_SENSOR_DATA_QUEUE_LEN       (1u)

uint8 Spo2Data = 0xff;
uint8 Spo2DataOld = 0xff;

SemaphoreHandle_t bleSemaphore;
uint8 SpO2Data_Notification_Enabled = 0;

void genericEventHandler(uint32_t event, void *eventParameter)
{
    cy_stc_ble_gatt_write_param_t   *writeData;
    //cy_stc_ble_conn_handle_t *ConnEvent;
    
    /* Take an action based on the current event */
    switch (event)
    {
        /* This event is received when the BLE stack is Started */
        case CY_BLE_EVT_STACK_ON:
        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
            Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
            //Cy_TCPWM_TriggerReloadOrIndex(PWM_DIM_HW,PWM_DIM_CNT_NUM);
            //Cy_TCPWM_PWM_Disable(PWM_DIM_HW,PWM_DIM_CNT_NUM);
    
        break;

        case CY_BLE_EVT_GATT_CONNECT_IND:
            //
            //PWM_Stop();
            //PWM_WriteCompare1(uint8/16 compare)
            
            //ConnEvent = (cy_stc_ble_conn_handle_t *)event;
            
            
            
            //Cy_TCPWM_TriggerReloadOrIndex(PWM_BLINK_HW,PWM_BLINK_CNT_NUM);
            //Cy_TCPWM_PWM_Disable(PWM_BLINK_HW,PWM_BLINK_CNT_NUM);     
            //PWM_DIM_Start();
            //Cy_BLE_GATTS_WriteRsp();
           
        break;
        
        case CY_BLE_EVT_GATTS_WRITE_REQ:
            writeData = (cy_stc_ble_gatt_write_param_t *)eventParameter;
            
            /* Extract the data and write to GATT database */
            Cy_BLE_GATTS_WriteAttributeValuePeer(&cy_ble_connHandle[0],&writeData->handleValPair);
            
            /* Send a Write Response to GATT Client */
            Cy_BLE_GATTS_WriteRsp(cy_ble_connHandle[0]);
            Cy_BLE_GATTS_WriteRsp(writeData->connHandle);
            
            /* XAxis Notify is enabled in the client device */
            if(writeData->handleValPair.attrHandle == CY_BLE_SPO2_SPO2DATA_SPO2CCCD_DESC_HANDLE)
            {  
                SpO2Data_Notification_Enabled = writeData->handleValPair.value.val[0];      
            }
            
            break;
        
        default:
            break;
    }
}
    
/*****************************************************************************\
 * Function:    bleInterruptNotify
 * Input:       void (it is called inside of the ISR)
 * Returns:     void
 * Description: 
 *   This is called back in the BLE ISR when an event has occured and needs to
 *   be processed.  It will then set/give the sempahore to tell the BLE task to
 *   process events.
\*****************************************************************************/
void bleInterruptNotify()
{
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(bleSemaphore, &xHigherPriorityTaskWoken); 
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/*****************************************************************************\
 * Function:    bleTask
 * Input:       A FreeRTOS Task - void * that is unused
 * Returns:     void
 * Description: 
 *  This task starts the BLE stack... and processes events when the bleSempahore
 *  is set by the ISR.
\*****************************************************************************/
cy_en_ble_api_result_t apiResult = CY_BLE_SUCCESS;
void bleTask(void *arg)
{
    (void)arg;
    
    //printf("BLE Task Started\r\n");

    bleSemaphore = xSemaphoreCreateCounting(UINT_MAX,0);
    
    Cy_BLE_Start(genericEventHandler);
    
    
    while(Cy_BLE_GetState() != CY_BLE_STATE_ON) // Get the stack going
    {
        Cy_BLE_ProcessEvents();
    }
    
    Cy_BLE_RegisterAppHostCallback(bleInterruptNotify);
    
    for(;;)
    {
        xSemaphoreTake(bleSemaphore,portMAX_DELAY);
        Cy_BLE_ProcessEvents();
        
        if(SpO2Data_Notification_Enabled)
        {
            if(Cy_BLE_GATT_GetBusyStatus(cy_ble_connHandle[0].attId) ==\
                   CY_BLE_STACK_STATE_FREE)
            {   
                cy_stc_ble_gatt_handle_value_pair_t  handleValue = {
                    .attrHandle = CY_BLE_SPO2_SPO2DATA_CHAR_HANDLE,
                    .value.val = (uint8_t *)&SpO2,
                    .value.len = 1};
                apiResult = Cy_BLE_GATTS_SendNotification(&cy_ble_connHandle[0], &handleValue);
                
                /* Check if the operation has been successful */
                if(apiResult == CY_BLE_SUCCESS)
                {
                    Cy_BLE_ProcessEvents();
                }
                else
                {
                    printf("Failure! : BLE - Sending notification, Error code: %x\r\n", apiResult);
                }
            }
        }   

        if (AFE_st == NOAFE_ID && UART_IsTxComplete()) {
            /* Enter sleep mode */
    		Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
//    		Cy_SysPm_Sleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
//            printf("Info! : BLE task is wake up!\r\n");
        }
    }
}


// 動作時間割り込みハンドラ
static void PWM_TIM_hdr()
{
    //printf("Info! : End of operating time!\r\n");
    NVIC_DisableIRQ(ISR_TIM_cfg.intrSrc);
    AFE_act(NOAFE_ID);
    // LED(RED)消灯
    Cy_GPIO_Write(LED_GREEN_PORT, LED_GREEN_NUM, 1);
}

// 動作周期割り込みハンドラ
static void PWM_CYC_hdr()
{
    //printf("Info! : Start operation time!\r\n");
    NVIC_ClearPendingIRQ(ISR_TIM_cfg.intrSrc);
    PWM_TIM_Init(&PWM_TIM_config);
    NVIC_EnableIRQ(ISR_TIM_cfg.intrSrc);
    AFE_clr(AFE_ID);
    AFE_act(AFE_ID);
    // LED(RED)点灯
    Cy_GPIO_Write(LED_GREEN_PORT, LED_GREEN_NUM, 0);
}

static void PWM_Timer_init()
{
    // 動作周期
    Cy_SysInt_Init(&ISR_CYC_cfg, PWM_CYC_hdr);
    NVIC_ClearPendingIRQ(ISR_CYC_cfg.intrSrc);
    NVIC_EnableIRQ(ISR_CYC_cfg.intrSrc);
    // 動作時間
    Cy_SysInt_Init(&ISR_TIM_cfg, PWM_TIM_hdr);
    NVIC_ClearPendingIRQ(ISR_TIM_cfg.intrSrc);
    NVIC_EnableIRQ(ISR_TIM_cfg.intrSrc);
}


/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
*  System entrance point. This function sets up user tasks and then starts 
*  the RTOS scheduler.
*
* Parameters:
*  None
*
* Return:
*  int
*
*******************************************************************************/
int main()
{
    // PWM開始
    PWM_AFE_Start();
    PWM_CYC_Start();
    PWM_TIM_Start();
    // I2C開始
    I2C_1_Start();
    I2C_2_Start();

    UART_Start();
    setvbuf( stdin, NULL, _IONBF, 0 ); 
    setvbuf( stdout, NULL, _IONBF, 0 ); 

    AFE_init(AFE_ID);   // AFE初期化
    PWM_Timer_init();   // 動作周期・時間タイマ初期化

    __enable_irq(); /* Enable global interrupts. */
    
    Cy_GPIO_Write(LED_RED_0_PORT, LED_RED_0_NUM, 1);
    
    /* Create LED Tasks. See the respective Task definition for more
       details of these tasks */       
    xTaskCreate(Task_LED, "LED Task", AFE_OPT_SENSOR_STACK_SIZE,
                NULL, AFE_OPT_SENSOR_PRIORITY, NULL);
    xTaskCreate(bleTask, "BLE Task", (4*1024), 0,2,0);

    /* Start the RTOS scheduler. This function should never return */
    vTaskStartScheduler();
    
    /* Should never get here! */ 
    printf("[Error] RTOS - scheduler crashed \r\n");
    
    /* Halt the CPU if scheduler exits */
    CY_ASSERT(0);
    
    for(;;)
    {
    }	
}

/*******************************************************************************
* Function Name: void vApplicationIdleHook(void)
********************************************************************************
*
* Summary:
*  This function is called when the RTOS in idle mode
*    
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void vApplicationIdleHook(void)
{
    /* Enter sleep-mode */
    Cy_SysPm_Sleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
}

/*******************************************************************************
* Function Name: void vApplicationStackOverflowHook(TaskHandle_t *pxTask, 
                                                    signed char *pcTaskName)
********************************************************************************
*
* Summary:
*  This function is called when a stack overflow has been detected by the RTOS
*    
* Parameters:
*  TaskHandle_t  : Handle to the task
*  signed char   : Name of the task
*
* Return:
*  None
*
*******************************************************************************/
void vApplicationStackOverflowHook(TaskHandle_t *pxTask, signed char *pcTaskName)
{
    /* Remove warning for unused parameters */
    (void)pxTask;
    (void)pcTaskName;
    
    /* Halt the CPU */
    CY_ASSERT(0);
}

/*******************************************************************************
* Function Name: void vApplicationMallocFailedHook(void)
********************************************************************************
*
* Summary:
*  This function is called when a memory allocation operation by the RTOS
*  has failed
*    
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void vApplicationMallocFailedHook(void)
{
    /* Print the error message if debug is enabled in uart_debug.h file */
//    DebugPrintf("Error!   : RTOS - Memory allocation failed \r\n");
    
    /* Halt the CPU */
    CY_ASSERT(0);
}

/* [] END OF FILE */
