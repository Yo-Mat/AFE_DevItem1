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
#include "rtc_intv.h"

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

/*******************************************************************************/
/*                      Function Prototypes                                    */
/*******************************************************************************/
cy_en_rtc_status_t RtcInit(void);
cy_en_rtc_status_t RtcAlarmConfig(void);
void RtcInterruptHandler(void);
void RtcStepAlarm(void);

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
    }
}

static void RTC_Timer_init()
{
    //printf("Debug! : Started RTC_Timer_init()!\r\n");
    // 動作周期
    if (RtcInit() != CY_RTC_SUCCESS) {
        CY_ASSERT(0u);
    }
    else {
        if (TICK_INTERVAL == 1u && USE_MINUTES == 1u) {
            alarmConfig.secEn = CY_RTC_ALARM_ENABLE;
        }
        if (RtcAlarmConfig() != CY_RTC_SUCCESS) {
           CY_ASSERT(0u);
        }
        else {
		    Cy_RTC_SetInterruptMask(CY_RTC_INTR_ALARM2);
        }
    }
    RtcStepAlarm();     // RTCアラームリスタート
    Cy_SysInt_Init(&RTC_RTC_IRQ_cfg, RtcInterruptHandler);
    NVIC_EnableIRQ(RTC_RTC_IRQ_cfg.intrSrc);
    RTC_GetDateAndTime(&startTtime);
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
    // I2C開始
    I2C_1_Start();
    I2C_2_Start();

    UART_Start();
    setvbuf( stdin, NULL, _IONBF, 0 ); 
    setvbuf( stdout, NULL, _IONBF, 0 ); 

    AFE_init(AFE_ID);   // AFE初期化

    __enable_irq(); /* Enable global interrupts. */
    
    Cy_GPIO_Write(LED_RED_0_PORT, LED_RED_0_NUM, 1);
    
    /* Create LED Tasks. See the respective Task definition for more
       details of these tasks */
    //printf("Debug! : Create LED task!\r\n");
    xTaskCreate(Task_LED, "LED Task", AFE_OPT_SENSOR_STACK_SIZE,
                NULL, AFE_OPT_SENSOR_PRIORITY, NULL);
    //printf("Debug! : Create BLE task!\r\n");
    xTaskCreate(bleTask, "BLE Task", (4*1024), 0,2,0);

    RTC_Timer_init();   // 動作周期・時間タイマ初期化

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

/******************************************************************************
* Function Name: RtcInit
*******************************************************************************
*
* Summary: 
*  This function configures the RTC registers.
*
* Parameters:
*  None
*
* Return:
*  cy_en_rtc_status_t
*       CY_RTC_SUCCESS      : Time and date configuration is successfully done.
*       CY_RTC_BAD_PARAM    : Date values are not valid.
*       CY_RTC_INVALID_STATE: RTC is busy state
*
******************************************************************************/
cy_en_rtc_status_t RtcInit(void)
{
    uint32_t attempts = MAX_ATTEMPTS;
    cy_en_rtc_status_t result;
    
    /* Setting the time and date can fail. For example the RTC might be busy.
       Check the result and try again, if necessary.  */
    do
    {
        result = Cy_RTC_Init(&RTC_config);
        attempts--;
        
        Cy_SysLib_Delay(INIT_DELAY);
    } while(( result != CY_RTC_SUCCESS) && (attempts != 0u));
    
	return (result);
}

/******************************************************************************
* Function Name: RtcAlarmConfig
*******************************************************************************
*
* Summary: 
*  This function configures the custom RTC alarm to utilize the custom tick
*  timer interrupt.
*
* Parameters:
*  None
*
* Return:
*  cy_en_rtc_status_t
*       CY_RTC_SUCCESS      : Time and date configuration is successfully done.
*       CY_RTC_BAD_PARAM    : Date values are not valid.
*       CY_RTC_INVALID_STATE: RTC is busy state
*
******************************************************************************/
cy_en_rtc_status_t RtcAlarmConfig(void)
{
    uint32_t attempts = MAX_ATTEMPTS;
    cy_en_rtc_status_t result;

    /* 
       Setting the alarm can fail. For example the RTC might be busy. 
       Check the result and try again, if necessary.
    */
    do
	{
		result = Cy_RTC_SetAlarmDateAndTime((cy_stc_rtc_alarm_t const *)&alarmConfig, CY_RTC_ALARM_2);
		attempts--;
        
		Cy_SysLib_Delay(INIT_DELAY);
    } while(( result != CY_RTC_SUCCESS) && (attempts != 0u));
    
	return (result);
}

/******************************************************************************
* Function Name: RtcInterruptHandler
*******************************************************************************
*
* Summary: 
*  This is the general RTC interrupt handler in CPU NVIC.
*  It calls the Alarm2 interrupt handler if that is the interrupt that occurs.
*
* Parameters:
*  None
*
* Return:
*  None
*
******************************************************************************/
void RtcInterruptHandler(void)
{
    /* No DST parameters are required for the custom tick. */
    Cy_RTC_Interrupt(NULL, false);
}

/******************************************************************************
* Function Name: Cy_RTC_Alarm2Interrupt
*******************************************************************************
*
* Summary: 
*  The function overrides the __WEAK Cy_RTC_Alarm2Interrupt() in cy_rtc.c to 
*  handle CY_RTC_ALARM_2 interrupt.
*
* Parameters:
*  None
*
* Return:
*  None
*
******************************************************************************/
void Cy_RTC_Alarm2Interrupt(void)
{
    //printf("Debug! : Cycle of operating!\r\n"); CyDelay(10);
    /* the interrupt has fired, meaning time expired and the alarm went off */
	alarmFlag = 1u;  
}

/******************************************************************************
* Function Name: RtcStepAlarm
*******************************************************************************
*
* Summary: 
*  This function sets the time for CY_RTC_ALARM_2, and configures the interrupt.
*  The available periods are one second to sixty seconds and one minute to sixty 
*  minutes. 
*
* Parameters:
*  None
*
* Return:
*  None
*
******************************************************************************/
void RtcStepAlarm(void)
{
    /* Don't set next time, if the interval is one second or one minute */
    if(TICK_INTERVAL != 1u)
    {
        if (USE_MINUTES)  /* match minutes, and advance by minutes */
    	{
    		/* 
                Enable the correct matches. This is a periodic interrupt, but 
                happens every two or more minutes. Because we are stepping by 
                minutes, we need to match the minutes number. We also match 
                the seconds number so the alarm only goes off when both the 
                seconds and minutes match. Because we are not changing the value
                of the seconds in the alarm, the alarm happens only once when the
                minutes match.
            */
            alarmConfig.secEn = CY_RTC_ALARM_ENABLE;
            alarmConfig.minEn = CY_RTC_ALARM_ENABLE;

    		/* advance the minute by the specified interval */
    		alarmConfig.min += TICK_INTERVAL;

            /* keep it in range, 0-59 */
            alarmConfig.min = alarmConfig.min % MINUTES_PER_HOUR;
    	}
    	else   /* USE_SECONDS, alarm when the seconds match */
    	{
    		/* 
                Enable the correct matches. Because we are stepping by seconds, 
                we need to match just the seconds number.
            */
            alarmConfig.secEn = CY_RTC_ALARM_ENABLE;

    		/* advance the second by the specified interval */
    		alarmConfig.sec += TICK_INTERVAL;

            /* keep it in range, 0-59 */
            alarmConfig.sec = alarmConfig.sec % SECONDS_PER_MIN;
    	}
    	
    	/* update the alarm configuration */
    	if(RtcAlarmConfig() != CY_RTC_SUCCESS)
    	{
    	   /* If the operation fails, halt */
    	   CY_ASSERT(0u);
    	}
    }
}

/* [] END OF FILE */
