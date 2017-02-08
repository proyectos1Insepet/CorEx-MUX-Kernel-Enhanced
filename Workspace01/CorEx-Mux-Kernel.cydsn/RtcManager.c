/* ========================================
 *
 * Copyright INSEPET SISTEMAS, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
@Created by: HIJH
@Date: Septembre 2 de 2016
*/
#include "GlobalDefinitions.h"
#include "Rtc.h"
#include "I2C_Bus.h"

extern volatile bool _g_i2cbuslock;

char8 _g_rtc[_RTC_BUFFER_SIZE_];

char8* GetRtcBuffer()
{
    return _g_rtc;
}

void ClearRtcBuffer()
{
    memset(_g_rtc, 0x00, _RTC_BUFFER_SIZE_);
}

//@Created By: HIJH
//@Septembre de 2016
void UpdateRTC()
{
    uint8 index = 0;
    I2C_Bus_MasterClearStatus();
                
    uint8 status = I2C_Bus_MasterSendStart(_RTC_I2C_SLAVE_ADDRESS_, I2C_Bus_WRITE_XFER_MODE);
    if(I2C_Bus_MSTR_NO_ERROR == status)
    {
        //Positioning on the Seconds Register
        I2C_Bus_MasterWriteByte(CTRL_REG1);
        //Now start writing
        //Seconds
        I2C_Bus_MasterWriteByte(_g_rtc[index++] & 0x7F);
        //Minutes
        I2C_Bus_MasterWriteByte(_g_rtc[index++] & 0x7F);
        //Hour
        I2C_Bus_MasterWriteByte((_g_rtc[index++] & 0b00111111));
        //Day of Week
        I2C_Bus_MasterWriteByte(_g_rtc[index++] & 0x07);
        //Day
        I2C_Bus_MasterWriteByte(_g_rtc[index++] & 0x3F);
        //Month
        I2C_Bus_MasterWriteByte(_g_rtc[index++] & 0x1F);
        //Year
        I2C_Bus_MasterWriteByte(_g_rtc[index++] & 0xFF);

    }
    I2C_Bus_MasterSendStop();
    
    //Turning on the RTC
    status = I2C_Bus_MasterSendStart(_RTC_I2C_SLAVE_ADDRESS_, I2C_Bus_WRITE_XFER_MODE);
    if(I2C_Bus_MSTR_NO_ERROR == status)
    {
        I2C_Bus_MasterWriteByte(0x0E);
        I2C_Bus_MasterWriteByte(0x04);
        I2C_Bus_MasterSendStop();
    }
}

//@Created By: HIJH
//@Septembre de 2016
void ReadRTC()
{
    I2C_Bus_MasterClearStatus();
    uint16 status = I2C_Bus_MasterSendStart(_RTC_I2C_SLAVE_ADDRESS_, I2C_Bus_WRITE_XFER_MODE);
    if(I2C_Bus_MSTR_NO_ERROR == status)
        I2C_Bus_MasterWriteByte(CTRL_REG1);

    I2C_Bus_MasterSendStop();
        
    I2C_Bus_MasterClearStatus();
    status = I2C_Bus_MasterSendStart(_RTC_I2C_SLAVE_ADDRESS_, I2C_Bus_READ_XFER_MODE);
    if(I2C_Bus_MSTR_NO_ERROR == status)
    {
        //Reading 7 bytes from the RTC starting at the CTRL_REG1 register address
        FOR(uint8 index = 0, index < 7, index++)
        {
            if(index < 6)
                _g_rtc[index] = I2C_Bus_MasterReadByte(I2C_Bus_ACK_DATA);
            else
                _g_rtc[index] = I2C_Bus_MasterReadByte(I2C_Bus_NAK_DATA);
                
        }
    }
    I2C_Bus_MasterSendStop();
}

void StartRtc()
{
    //Turning on the RTC
    uint8 status = I2C_Bus_MasterSendStart(_RTC_I2C_SLAVE_ADDRESS_, I2C_Bus_WRITE_XFER_MODE);
    if(I2C_Bus_MSTR_NO_ERROR == status)
    {
        I2C_Bus_MasterWriteByte(0x0E);
        I2C_Bus_MasterWriteByte(0x04);
        I2C_Bus_MasterSendStop();
    }
}



/* [] END OF FILE */
