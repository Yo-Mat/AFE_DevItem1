/*******************************************************************************
* File Name: common.h
*
* Version 1.0
*
* Description:
*  Contains the function prototypes and constants available to the example
*  project.
*
********************************************************************************
* Copyright 2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/
#ifndef COMMON_H
#define COMMON_H

#include <project.h>
#include <stdio.h>
#include "DEBUG_UART.h"


/***************************************
* Conditional Compilation Parameters
***************************************/
#define ENABLED                     (1u)
#define DISABLED                    (0u)
#define DEBUG_UART_ENABLED          ENABLED

/***************************************
*           API Constants
***************************************/
#define ADV_TIMER_TIMEOUT         (1u)               /* Сounts in s */


/***************************************
*      API Function Prototypes
***************************************/
int HostMain(void);

/* Function prototypes from debug.c */
void ShowValue(cy_stc_ble_gatt_value_t *value);
char HexToAscii(uint8_t value, uint8_t nibble);
void Set32ByPtr(uint8_t ptr[], uint32_t value);
void PrintStackVersion(void);
void PrintApiResult(cy_en_ble_api_result_t apiResult);
void ShowError(void);

/***************************************
*        Macros
***************************************/
#if (DEBUG_UART_ENABLED == ENABLED)
    #define DBG_PRINTF(...)                 (printf(__VA_ARGS__))
    #define DEBUG_UART_PUT_CHAR(...)           while(1UL != DEBUG_UART_Put(__VA_ARGS__))
    #define DEBUG_UART_GET_CHAR(...)          (DEBUG_UART_Get())
    #define DEBUG_UART_IS_TX_COMPLETE(...)    (DEBUG_UART_IsTxComplete())
    #define DEBUG_UART_WAIT_TX_COMPLETE(...)   while(DEBUG_UART_IS_TX_COMPLETE() == 0) ;    
    #define DEBUG_UART_SCB_CLEAR_RX_FIFO(...) (Cy_SCB_ClearRxFifo(DEBUG_UART_SCB__HW))
    #define UART_START(...)                 (DEBUG_UART_Start(__VA_ARGS__))
#else
    #define DBG_PRINTF(...)
    #define DEBUG_UART_PUT_CHAR(...)
    #define DEBUG_UART_GET_CHAR(...)          (0u)
    #define DEBUG_UART_IS_TX_COMPLETE(...)    (1u)
    #define DEBUG_UART_WAIT_TX_COMPLETE(...)  (0u)
    #define DEBUG_UART_SCB_CLEAR_RX_FIFO(...) (0u)
    #define UART_START(...)
#endif /* (DEBUG_UART_ENABLED == ENABLED) */

#define DEBUG_UART_NO_DATA                (char8) CY_SCB_UART_RX_NO_DATA


#endif /* COMMON_H */

/* [] END OF FILE */
