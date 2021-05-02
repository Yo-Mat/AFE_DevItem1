/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

/* [] END OF FILE */

/*
 * AFE440X Heart Rate Monitors and Low-Cost Pulse Oximeters
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 *  Andrew F. Davis <afd@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */
#ifndef _AFE440X_H
#define _AFE440X_H
    
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "project.h"
#include "FreeRTOS.h"

#define BIT(nr) (1UL << (nr))
/* AFE4404 I2C address */
#define AFE4404_I2C_ADDR        UINT8_C(0x58)  
/* AFE440X registers */
#define AFE440X_CONTROL0        0x00
#define AFE440X_LED2STC         0x01
#define AFE440X_LED2ENDC        0x02
#define AFE440X_LED1LEDSTC      0x03
#define AFE440X_LED1LEDENDC     0x04
#define AFE440X_ALED2STC        0x05
#define AFE440X_ALED2ENDC       0x06
#define AFE440X_LED1STC         0x07
#define AFE440X_LED1ENDC        0x08
#define AFE440X_LED2LEDSTC      0x09
#define AFE440X_LED2LEDENDC     0x0a
#define AFE440X_ALED1STC        0x0b
#define AFE440X_ALED1ENDC       0x0c
#define AFE440X_LED2CONVST      0x0d
#define AFE440X_LED2CONVEND     0x0e
#define AFE440X_ALED2CONVST     0x0f
#define AFE440X_ALED2CONVEND    0x10
#define AFE440X_LED1CONVST      0x11
#define AFE440X_LED1CONVEND     0x12
#define AFE440X_ALED1CONVST     0x13
#define AFE440X_ALED1CONVEND    0x14
#define AFE440X_ADCRSTSTCT0     0x15
#define AFE440X_ADCRSTENDCT0    0x16
#define AFE440X_ADCRSTSTCT1     0x17
#define AFE440X_ADCRSTENDCT1    0x18
#define AFE440X_ADCRSTSTCT2     0x19
#define AFE440X_ADCRSTENDCT2    0x1a
#define AFE440X_ADCRSTSTCT3     0x1b
#define AFE440X_ADCRSTENDCT3    0x1c
#define AFE440X_PRPCOUNT        0x1d
#define AFE440X_CONTROL1        0x1e
#define AFE440X_LEDCNTRL        0x22
#define AFE440X_CONTROL2        0x23
#define AFE440X_ALARM           0x29
#define AFE440X_LED2VAL         0x2a
#define AFE440X_ALED2VAL        0x2b
#define AFE440X_LED1VAL         0x2c
#define AFE440X_ALED1VAL        0x2d
#define AFE440X_LED2_ALED2VAL   0x2e
#define AFE440X_LED1_ALED1VAL   0x2f
#define AFE440X_CONTROL3        0x31
#define AFE440X_PDNCYCLESTC     0x32
#define AFE440X_PDNCYCLEENDC    0x33
/* CONTROL0 register fields */
#define AFE440X_CONTROL0_REG_READ       BIT(0)
#define AFE440X_CONTROL0_TM_COUNT_RST   BIT(1)
#define AFE440X_CONTROL0_SW_RESET       BIT(3)
/* CONTROL1 register fields */
#define AFE440X_CONTROL1_TIMEREN    BIT(8)
/* CONTROL2 register fields */
#define AFE440X_CONTROL2_PDN_AFE    BIT(0)
#define AFE440X_CONTROL2_PDN_RX     BIT(1)
#define AFE440X_CONTROL2_OSC_ENABLE	BIT(9)
#define AFE440X_CONTROL2_DYNAMIC4   BIT(3)
#define AFE440X_CONTROL2_DYNAMIC3   BIT(4)
#define AFE440X_CONTROL2_DYNAMIC2   BIT(14)
#define AFE440X_CONTROL2_DYNAMIC1   BIT(20)
/* CONTROL3 register fields */
#define AFE440X_CONTROL3_CLKDIV     GENMASK(2, 0)
/* CONTROL0 values */
#define AFE440X_CONTROL0_WRITE      0x0
#define AFE440X_CONTROL0_READ       0x1

#define AFE4404_DRIVER_NAME		"afe4404"
/* AFE4404 registers */
#define AFE4404_TIA_GAIN_SEP		0x20
#define AFE4404_TIA_GAIN		    0x21
#define AFE4404_ILED_CURRENT	    0x22
#define AFE4404_PROG_TG_STC		    0x34
#define AFE4404_PROG_TG_ENDC		0x35
#define AFE4404_LED3LEDSTC		    0x36
#define AFE4404_LED3LEDENDC		    0x37
#define AFE4404_CLKDIV_PRF		    0x39
#define AFE4404_OFFDAC			    0x3a
#define AFE4404_DEC		        	0x3d
#define AFE4404_AVG_LED2_ALED2VAL	0x3f
#define AFE4404_AVG_LED1_ALED1VAL	0x40
/* Other register fields */
#define AFE440X_ENABLE_CLKOUT	    BIT(9)
#define AFE440X_CLKDIV_PRF	        BIT(0)

typedef struct reg_sequence {
    uint8_t reg;
    uint32_t val;
} REG_SEQENCE_T;
#define REG_DATA_LEN    3

int8_t afe4404Read(CySCB_Type *I2cNo, uint8_t Reg, uint8_t *Data, int Len);
int8_t afe4404ContiRead(CySCB_Type *I2cNo, struct reg_sequence *reg_seq, unsigned int length);
int8_t afe4404Write(CySCB_Type *I2cNo, uint8_t Reg, uint8_t *Data, uint16_t Len);
int8_t afe4404ContiWrite(CySCB_Type *I2cNo, const struct reg_sequence *reg_seq, unsigned int length);

#endif /* _AFE440X_H */