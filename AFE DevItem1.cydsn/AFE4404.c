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
 * AFE4404 Heart Rate Monitors and Low-Cost Pulse Oximeters
 *
 * Copyright (C) 2015-2016 Texas Instruments Incorporated - http://www.ti.com/
 *	Andrew F. Davis <afd@ti.com>
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
#include "AFE4404.h"

int8_t afe4404Read(CySCB_Type *I2cNo, uint8_t Reg, uint8_t *Data, int Len)
{   
    cy_stc_scb_i2c_context_t *i2c_context = I2cNo==I2C_1_HW ? &I2C_1_context:&I2C_2_context;
    
    Cy_SCB_I2C_MasterSendStart(I2cNo, AFE4404_I2C_ADDR, CY_SCB_I2C_WRITE_XFER, 0, i2c_context);
    Cy_SCB_I2C_MasterWriteByte(I2cNo, Reg,0,i2c_context);
    Cy_SCB_I2C_MasterSendReStart(I2cNo, AFE4404_I2C_ADDR, CY_SCB_I2C_READ_XFER, 0, i2c_context);
    for(int i=0; i<Len-1; i++) {
        Cy_SCB_I2C_MasterReadByte(I2cNo, CY_SCB_I2C_ACK, &Data[Len-i-1], 0 ,i2c_context);
    }
    Cy_SCB_I2C_MasterReadByte(I2cNo, CY_SCB_I2C_NAK, &Data[0], 0, i2c_context);
    Cy_SCB_I2C_MasterSendStop(I2cNo, 0, i2c_context);

    return 0;
}


int8_t afe4404ContiRead(CySCB_Type *I2cNo, struct reg_sequence *reg_seq, unsigned int length)
{   
    for(unsigned int i=0; i<length; i++){
        afe4404Read(I2cNo, reg_seq[i].reg, (uint8_t*)&reg_seq[i].val, REG_DATA_LEN);
    }
    
    return 0;
}


int8_t afe4404Write(CySCB_Type *I2cNo, uint8_t Reg, uint8_t *Data, uint16_t Len)
{   
    cy_stc_scb_i2c_context_t *i2c_context = I2cNo==I2C_1_HW ? &I2C_1_context:&I2C_2_context;
    
    Cy_SCB_I2C_MasterSendStart(I2cNo, AFE4404_I2C_ADDR, CY_SCB_I2C_WRITE_XFER, 0, i2c_context);
    Cy_SCB_I2C_MasterWriteByte(I2cNo, Reg, 0, i2c_context);
    for(int i=0; i<Len; i++) { 
       Cy_SCB_I2C_MasterWriteByte(I2cNo, Data[Len-i-1], 0, i2c_context);
    }
    Cy_SCB_I2C_MasterSendStop(I2cNo, 0, i2c_context);
    
    return 0;
}

int8_t afe4404ContiWrite(CySCB_Type *I2cNo, const struct reg_sequence *reg_seq, unsigned int length)
{   
    for(unsigned int i=0; i<length; i++){
        afe4404Write(I2cNo, reg_seq[i].reg, (uint8_t*)&reg_seq[i].val, REG_DATA_LEN);
    }
    
    return 0;
}


