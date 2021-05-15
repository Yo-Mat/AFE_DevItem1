/******************************************************************************
* File Name: main_cm0p.c
*
* Version: 1.0
*
* Description: This code example demonstrates how to interface PSoCR 6 MCU with 
*              a BMI160 Motion Sensor over I2C interface from a FreeRTOS task.
*              Cortex-M0 is not used in this project. See the main_cm4.c for 
*              main functions.
*
* Related Document: CE223468_PSoC6_Interfacing_BMI160(I2C)_FreeRTOS.pdf
*
* Hardware Dependency: CY8CKIT-062-BLE PSoC 6 BLE Pioneer Kit
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

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
*  System entrance point. This function enables the Cortex-M4
*
* Parameters:
*  None
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    __enable_irq(); /* Enable global interrupts. */
        
    Cy_BLE_Start(0);
    /* Enable CM4.  CY_CORTEX_M4_APPL_ADDR must be updated if CM4 memory layout is changed. */
    Cy_SysEnableCM4(CY_CORTEX_M4_APPL_ADDR); 

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
        Cy_BLE_ProcessEvents();

        /* Enter sleep mode */
		Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
//		Cy_SysPm_Sleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
    }
}

/* [] END OF FILE */
