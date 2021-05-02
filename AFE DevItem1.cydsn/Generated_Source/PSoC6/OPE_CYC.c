/*******************************************************************************
* File Name: OPE_CYC.c
* Version 1.0
*
* Description:
*  This file provides the source code to the API for the OPE_CYC
*  component
*
********************************************************************************
* Copyright 2016-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "OPE_CYC.h"

/** Indicates whether or not the OPE_CYC has been initialized. 
*  The variable is initialized to 0 and set to 1 the first time 
*  OPE_CYC_Start() is called. This allows the Component to 
*  restart without reinitialization after the first call to 
*  the OPE_CYC_Start() routine.
*/
uint8_t OPE_CYC_initVar = 0U;

/** The instance-specific configuration structure. This should be used in the 
*  associated OPE_CYC_Init() function.
*/ 
cy_stc_tcpwm_counter_config_t const OPE_CYC_config =
{
        .period = 6000UL,
        .clockPrescaler = 0UL,
        .runMode = 0UL,
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
* Function Name: OPE_CYC_Start
****************************************************************************//**
*
*  Calls the OPE_CYC_Init() when called the first time and enables 
*  the OPE_CYC. For subsequent calls the configuration is left 
*  unchanged and the component is just enabled.
*
* \globalvars
*  \ref OPE_CYC_initVar
*
*******************************************************************************/
void OPE_CYC_Start(void)
{
    if (0U == OPE_CYC_initVar)
    {
        (void)Cy_TCPWM_Counter_Init(OPE_CYC_HW, OPE_CYC_CNT_NUM, &OPE_CYC_config); 

        OPE_CYC_initVar = 1U;
    }

    Cy_TCPWM_Enable_Multiple(OPE_CYC_HW, OPE_CYC_CNT_MASK);
    
    #if (OPE_CYC_INPUT_DISABLED == 7UL)
        Cy_TCPWM_TriggerStart(OPE_CYC_HW, OPE_CYC_CNT_MASK);
    #endif /* (OPE_CYC_INPUT_DISABLED == 7UL) */    
}


/* [] END OF FILE */
