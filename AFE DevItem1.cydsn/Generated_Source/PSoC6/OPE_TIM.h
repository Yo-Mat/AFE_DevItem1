/*******************************************************************************
* File Name: OPE_TIM.h
* Version 1.0
*
* Description:
*  This file provides constants and parameter values for the OPE_TIM
*  component.
*
********************************************************************************
* Copyright 2016-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(OPE_TIM_CY_TCPWM_COUNTER_PDL_H)
#define OPE_TIM_CY_TCPWM_COUNTER_PDL_H

#include "cyfitter.h"
#include "tcpwm/cy_tcpwm_counter.h"

   
/*******************************************************************************
* Variables
*******************************************************************************/
/**
* \addtogroup group_globals
* @{
*/
extern uint8_t  OPE_TIM_initVar;
extern cy_stc_tcpwm_counter_config_t const OPE_TIM_config;
/** @} group_globals */


/***************************************
*  Conditional Compilation Parameters
***************************************/

#define OPE_TIM_INIT_COMPARE_OR_CAPTURE    (2UL)


/***************************************
*        Function Prototypes
****************************************/
/**
* \addtogroup group_general
* @{
*/
void OPE_TIM_Start(void);
__STATIC_INLINE cy_en_tcpwm_status_t OPE_TIM_Init(cy_stc_tcpwm_counter_config_t const *config);
__STATIC_INLINE void OPE_TIM_DeInit(void);
__STATIC_INLINE void OPE_TIM_Enable(void);
__STATIC_INLINE void OPE_TIM_Disable(void);
__STATIC_INLINE uint32_t OPE_TIM_GetStatus(void);

#if(CY_TCPWM_COUNTER_MODE_CAPTURE == OPE_TIM_INIT_COMPARE_OR_CAPTURE)
    __STATIC_INLINE uint32_t OPE_TIM_GetCapture(void);
    __STATIC_INLINE uint32_t OPE_TIM_GetCaptureBuf(void);
#else
    __STATIC_INLINE void OPE_TIM_SetCompare0(uint32_t compare0);
    __STATIC_INLINE uint32_t OPE_TIM_GetCompare0(void);
    __STATIC_INLINE void OPE_TIM_SetCompare1(uint32_t compare1);
    __STATIC_INLINE uint32_t OPE_TIM_GetCompare1(void);
    __STATIC_INLINE void OPE_TIM_EnableCompareSwap(bool enable);
#endif /* (CY_TCPWM_COUNTER_MODE_CAPTURE == OPE_TIM_INIT_COMPARE_OR_CAPTURE) */

__STATIC_INLINE void OPE_TIM_SetCounter(uint32_t count);
__STATIC_INLINE uint32_t OPE_TIM_GetCounter(void);
__STATIC_INLINE void OPE_TIM_SetPeriod(uint32_t period);
__STATIC_INLINE uint32_t OPE_TIM_GetPeriod(void);
__STATIC_INLINE void OPE_TIM_TriggerStart(void);
__STATIC_INLINE void OPE_TIM_TriggerReload(void);
__STATIC_INLINE void OPE_TIM_TriggerStop(void);
__STATIC_INLINE void OPE_TIM_TriggerCapture(void);
__STATIC_INLINE uint32_t OPE_TIM_GetInterruptStatus(void);
__STATIC_INLINE void OPE_TIM_ClearInterrupt(uint32_t source);
__STATIC_INLINE void OPE_TIM_SetInterrupt(uint32_t source);
__STATIC_INLINE void OPE_TIM_SetInterruptMask(uint32_t mask);
__STATIC_INLINE uint32_t OPE_TIM_GetInterruptMask(void);
__STATIC_INLINE uint32_t OPE_TIM_GetInterruptStatusMasked(void);
/** @} general */


/***************************************
*           API Constants
***************************************/

/**
* \addtogroup group_macros
* @{
*/
/** This is a ptr to the base address of the TCPWM instance */
#define OPE_TIM_HW                 (OPE_TIM_TCPWM__HW)

/** This is a ptr to the base address of the TCPWM CNT instance */
#define OPE_TIM_CNT_HW             (OPE_TIM_TCPWM__CNT_HW)

/** This is the counter instance number in the selected TCPWM */
#define OPE_TIM_CNT_NUM            (OPE_TIM_TCPWM__CNT_IDX)

/** This is the bit field representing the counter instance in the selected TCPWM */
#define OPE_TIM_CNT_MASK           (1UL << OPE_TIM_CNT_NUM)
/** @} group_macros */

#define OPE_TIM_INPUT_MODE_MASK    (0x3U)
#define OPE_TIM_INPUT_DISABLED     (7U)


/*******************************************************************************
* Function Name: OPE_TIM_Init
****************************************************************************//**
*
* Invokes the Cy_TCPWM_Counter_Init() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_tcpwm_status_t OPE_TIM_Init(cy_stc_tcpwm_counter_config_t const *config)
{
    return(Cy_TCPWM_Counter_Init(OPE_TIM_HW, OPE_TIM_CNT_NUM, config));
}


/*******************************************************************************
* Function Name: OPE_TIM_DeInit
****************************************************************************//**
*
* Invokes the Cy_TCPWM_Counter_DeInit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void OPE_TIM_DeInit(void)                   
{
    Cy_TCPWM_Counter_DeInit(OPE_TIM_HW, OPE_TIM_CNT_NUM, &OPE_TIM_config);
}

/*******************************************************************************
* Function Name: OPE_TIM_Enable
****************************************************************************//**
*
* Invokes the Cy_TCPWM_Enable_Multiple() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void OPE_TIM_Enable(void)                   
{
    Cy_TCPWM_Enable_Multiple(OPE_TIM_HW, OPE_TIM_CNT_MASK);
}


/*******************************************************************************
* Function Name: OPE_TIM_Disable
****************************************************************************//**
*
* Invokes the Cy_TCPWM_Disable_Multiple() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void OPE_TIM_Disable(void)                  
{
    Cy_TCPWM_Disable_Multiple(OPE_TIM_HW, OPE_TIM_CNT_MASK);
}


/*******************************************************************************
* Function Name: OPE_TIM_GetStatus
****************************************************************************//**
*
* Invokes the Cy_TCPWM_Counter_GetStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t OPE_TIM_GetStatus(void)                
{
    return(Cy_TCPWM_Counter_GetStatus(OPE_TIM_HW, OPE_TIM_CNT_NUM));
}


#if(CY_TCPWM_COUNTER_MODE_CAPTURE == OPE_TIM_INIT_COMPARE_OR_CAPTURE)
    /*******************************************************************************
    * Function Name: OPE_TIM_GetCapture
    ****************************************************************************//**
    *
    * Invokes the Cy_TCPWM_Counter_GetCapture() PDL driver function.
    *
    *******************************************************************************/
    __STATIC_INLINE uint32_t OPE_TIM_GetCapture(void)               
    {
        return(Cy_TCPWM_Counter_GetCapture(OPE_TIM_HW, OPE_TIM_CNT_NUM));
    }


    /*******************************************************************************
    * Function Name: OPE_TIM_GetCaptureBuf
    ****************************************************************************//**
    *
    * Invokes the Cy_TCPWM_Counter_GetCaptureBuf() PDL driver function.
    *
    *******************************************************************************/
    __STATIC_INLINE uint32_t OPE_TIM_GetCaptureBuf(void)            
    {
        return(Cy_TCPWM_Counter_GetCaptureBuf(OPE_TIM_HW, OPE_TIM_CNT_NUM));
    }
#else
    /*******************************************************************************
    * Function Name: OPE_TIM_SetCompare0
    ****************************************************************************//**
    *
    * Invokes the Cy_TCPWM_Counter_SetCompare0() PDL driver function.
    *
    *******************************************************************************/
    __STATIC_INLINE void OPE_TIM_SetCompare0(uint32_t compare0)      
    {
        Cy_TCPWM_Counter_SetCompare0(OPE_TIM_HW, OPE_TIM_CNT_NUM, compare0);
    }


    /*******************************************************************************
    * Function Name: OPE_TIM_GetCompare0
    ****************************************************************************//**
    *
    * Invokes the Cy_TCPWM_Counter_GetCompare0() PDL driver function.
    *
    *******************************************************************************/
    __STATIC_INLINE uint32_t OPE_TIM_GetCompare0(void)              
    {
        return(Cy_TCPWM_Counter_GetCompare0(OPE_TIM_HW, OPE_TIM_CNT_NUM));
    }


    /*******************************************************************************
    * Function Name: OPE_TIM_SetCompare1
    ****************************************************************************//**
    *
    * Invokes the Cy_TCPWM_Counter_SetCompare1() PDL driver function.
    *
    *******************************************************************************/
    __STATIC_INLINE void OPE_TIM_SetCompare1(uint32_t compare1)      
    {
        Cy_TCPWM_Counter_SetCompare1(OPE_TIM_HW, OPE_TIM_CNT_NUM, compare1);
    }


    /*******************************************************************************
    * Function Name: OPE_TIM_GetCompare1
    ****************************************************************************//**
    *
    * Invokes the Cy_TCPWM_Counter_GetCompare1() PDL driver function.
    *
    *******************************************************************************/
    __STATIC_INLINE uint32_t OPE_TIM_GetCompare1(void)              
    {
        return(Cy_TCPWM_Counter_GetCompare1(OPE_TIM_HW, OPE_TIM_CNT_NUM));
    }


    /*******************************************************************************
    * Function Name: OPE_TIM_EnableCompareSwap
    ****************************************************************************//**
    *
    * Invokes the Cy_TCPWM_Counter_EnableCompareSwap() PDL driver function.
    *
    *******************************************************************************/
    __STATIC_INLINE void OPE_TIM_EnableCompareSwap(bool enable)  
    {
        Cy_TCPWM_Counter_EnableCompareSwap(OPE_TIM_HW, OPE_TIM_CNT_NUM, enable);
    }
#endif /* (CY_TCPWM_COUNTER_MODE_CAPTURE == OPE_TIM_INIT_COMPARE_OR_CAPTURE) */


/*******************************************************************************
* Function Name: OPE_TIM_SetCounter
****************************************************************************//**
*
* Invokes the Cy_TCPWM_Counter_SetCounter() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void OPE_TIM_SetCounter(uint32_t count)          
{
    Cy_TCPWM_Counter_SetCounter(OPE_TIM_HW, OPE_TIM_CNT_NUM, count);
}


/*******************************************************************************
* Function Name: OPE_TIM_GetCounter
****************************************************************************//**
*
* Invokes the Cy_TCPWM_Counter_GetCounter() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t OPE_TIM_GetCounter(void)               
{
    return(Cy_TCPWM_Counter_GetCounter(OPE_TIM_HW, OPE_TIM_CNT_NUM));
}


/*******************************************************************************
* Function Name: OPE_TIM_SetPeriod
****************************************************************************//**
*
* Invokes the Cy_TCPWM_Counter_SetPeriod() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void OPE_TIM_SetPeriod(uint32_t period)          
{
    Cy_TCPWM_Counter_SetPeriod(OPE_TIM_HW, OPE_TIM_CNT_NUM, period);
}


/*******************************************************************************
* Function Name: OPE_TIM_GetPeriod
****************************************************************************//**
*
* Invokes the Cy_TCPWM_Counter_GetPeriod() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t OPE_TIM_GetPeriod(void)                
{
    return(Cy_TCPWM_Counter_GetPeriod(OPE_TIM_HW, OPE_TIM_CNT_NUM));
}


/*******************************************************************************
* Function Name: OPE_TIM_TriggerStart
****************************************************************************//**
*
* Invokes the Cy_TCPWM_TriggerStart() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void OPE_TIM_TriggerStart(void)             
{
    Cy_TCPWM_TriggerStart(OPE_TIM_HW, OPE_TIM_CNT_MASK);
}


/*******************************************************************************
* Function Name: OPE_TIM_TriggerReload
****************************************************************************//**
*
* Invokes the Cy_TCPWM_TriggerReloadOrIndex() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void OPE_TIM_TriggerReload(void)     
{
    Cy_TCPWM_TriggerReloadOrIndex(OPE_TIM_HW, OPE_TIM_CNT_MASK);
}


/*******************************************************************************
* Function Name: OPE_TIM_TriggerStop
****************************************************************************//**
*
* Invokes the Cy_TCPWM_TriggerStopOrKill() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void OPE_TIM_TriggerStop(void)
{
    Cy_TCPWM_TriggerStopOrKill(OPE_TIM_HW, OPE_TIM_CNT_MASK);
}


/*******************************************************************************
* Function Name: OPE_TIM_TriggerCapture
****************************************************************************//**
*
* Invokes the Cy_TCPWM_TriggerCaptureOrSwap() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void OPE_TIM_TriggerCapture(void)     
{
    Cy_TCPWM_TriggerCaptureOrSwap(OPE_TIM_HW, OPE_TIM_CNT_MASK);
}


/*******************************************************************************
* Function Name: OPE_TIM_GetInterruptStatus
****************************************************************************//**
*
* Invokes the Cy_TCPWM_GetInterruptStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t OPE_TIM_GetInterruptStatus(void)       
{
    return(Cy_TCPWM_GetInterruptStatus(OPE_TIM_HW, OPE_TIM_CNT_NUM));
}


/*******************************************************************************
* Function Name: OPE_TIM_ClearInterrupt
****************************************************************************//**
*
* Invokes the Cy_TCPWM_ClearInterrupt() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void OPE_TIM_ClearInterrupt(uint32_t source)     
{
    Cy_TCPWM_ClearInterrupt(OPE_TIM_HW, OPE_TIM_CNT_NUM, source);
}


/*******************************************************************************
* Function Name: OPE_TIM_SetInterrupt
****************************************************************************//**
*
* Invokes the Cy_TCPWM_SetInterrupt() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void OPE_TIM_SetInterrupt(uint32_t source)
{
    Cy_TCPWM_SetInterrupt(OPE_TIM_HW, OPE_TIM_CNT_NUM, source);
}


/*******************************************************************************
* Function Name: OPE_TIM_SetInterruptMask
****************************************************************************//**
*
* Invokes the Cy_TCPWM_SetInterruptMask() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void OPE_TIM_SetInterruptMask(uint32_t mask)     
{
    Cy_TCPWM_SetInterruptMask(OPE_TIM_HW, OPE_TIM_CNT_NUM, mask);
}


/*******************************************************************************
* Function Name: OPE_TIM_GetInterruptMask
****************************************************************************//**
*
* Invokes the Cy_TCPWM_GetInterruptMask() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t OPE_TIM_GetInterruptMask(void)         
{
    return(Cy_TCPWM_GetInterruptMask(OPE_TIM_HW, OPE_TIM_CNT_NUM));
}


/*******************************************************************************
* Function Name: OPE_TIM_GetInterruptStatusMasked
****************************************************************************//**
*
* Invokes the Cy_TCPWM_GetInterruptStatusMasked() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t OPE_TIM_GetInterruptStatusMasked(void)
{
    return(Cy_TCPWM_GetInterruptStatusMasked(OPE_TIM_HW, OPE_TIM_CNT_NUM));
}

#endif /* OPE_TIM_CY_TCPWM_COUNTER_PDL_H */


/* [] END OF FILE */
