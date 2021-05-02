/*******************************************************************************
* File Name: OPE_TIM.c
* Version 1.0
*
* Description:
*  This file provides the source code to the API for the OPE_TIM
*  component
*
********************************************************************************
* Copyright 2016-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "OPE_TIM.h"

/** Indicates whether or not the OPE_TIM has been initialized. 
*  The variable is initialized to 0 and set to 1 the first time 
*  OPE_TIM_Start() is called. This allows the Component to 
*  restart without reinitialization after the first call to 
*  the OPE_TIM_Start() routine.
*/
uint8_t OPE_TIM_initVar = 0U;

/** The instance-specific configuration structure. This should be used in the 
*  associated OPE_TIM_Init() function.
*/ 
cy_stc_tcpwm_counter_config_t const OPE_TIM_config =
{
        .period = 1000UL,
        .clockPrescaler = 0UL,
        .runMode = 1UL,
        .countDirection = 0UL,
        .compareOrCapture = 2UL,
        .compare0 = 16384UL,
        .compare1 = 16384UL,
        .enableCompareSwap = false,
        .interruptSources = 1UL,
        .captureInputMode = 3UL,
        .captureInput = CY_TCPWM_INPUT_CREATOR,
        .reloadInputMode = 3UL,
        .reloadInput = CY_TCPWM_INPUT_CREATOR,
        .startInputMode = 3UL,
        .startInput = CY_TCPWM_INPUT_CREATOR,
        .stopInputMode = 3UL,
        .stopInput = CY_TCPWM_INPUT_CREATOR,
        .countInputMode = 3UL,
        .countInput = CY_TCPWM_INPUT_CREATOR,
};


/*******************************************************************************
* Function Name: OPE_TIM_Start
****************************************************************************//**
*
*  Calls the OPE_TIM_Init() when called the first time and enables 
*  the OPE_TIM. For subsequent calls the configuration is left 
*  unchanged and the component is just enabled.
*
* \globalvars
*  \ref OPE_TIM_initVar
*
*******************************************************************************/
void OPE_TIM_Start(void)
{
    if (0U == OPE_TIM_initVar)
    {
        (void)Cy_TCPWM_Counter_Init(OPE_TIM_HW, OPE_TIM_CNT_NUM, &OPE_TIM_config); 

        OPE_TIM_initVar = 1U;
    }

    Cy_TCPWM_Enable_Multiple(OPE_TIM_HW, OPE_TIM_CNT_MASK);
    
    #if (OPE_TIM_INPUT_DISABLED == 7UL)
        Cy_TCPWM_TriggerStart(OPE_TIM_HW, OPE_TIM_CNT_MASK);
    #endif /* (OPE_TIM_INPUT_DISABLED == 7UL) */    
}


/* [] END OF FILE */
