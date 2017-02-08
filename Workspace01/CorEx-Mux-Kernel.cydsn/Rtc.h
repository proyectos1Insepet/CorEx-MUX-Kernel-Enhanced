/* ========================================
 *
 * Copyright INSEPET SISTEMAS, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF SISTEMAS INSEPET.
 *
 * ========================================
@Created By: HIJH
@Date: Septembre de 2016
*/

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"

// system includes
#include <stdint.h>     // definicion estandar de tipos de datos
#include <stdbool.h>    // definicion estandar de tipo booleano para c
#include <math.h>

#define _RTC_I2C_SLAVE_ADDRESS_       0x68
#define _RTC_BUFFER_SIZE_             16   

enum _RTC_REGISTERS_
{
    CTRL_REG1 = 0x00,//Seconds
    CTRL_REG2 = 0x01,//Minutes
    CTRL_REG3 = 0x02,//Hours
    CTRL_REG4 = 0x03,//Day
    CTRL_REG5 = 0x04,//Date
    CTRL_REG6 = 0x05,//Month/Century
    CTRL_REG7 = 0x06,//Year
    
    CTRL_REG8 = 0x07,//Alarm1 Seconds
    CTRL_REG9 = 0x08,//Alarm1 Minutes
    CTRL_REG10 = 0x09,//Alarm1 Hours
    CTRL_REG11 = 0x0a,//Alarm1 Day / Date
    CTRL_REG12 = 0x0b,//Alarm2 Minutes
    CTRL_REG13 = 0x0c,//Alarm2 Hours
    CTRL_REG14 = 0x0d,//Alarm2 Day / Date
    CTRL_REG15 = 0x0e,//Control
    CTRL_REG16 = 0x0f,//Status
    CTRL_REG17 = 0x10,//Aging Offset
    CTRL_REG18 = 0x11,//Temperature MSB
    CTRL_REG19 = 0x12,//Temperature LSB
};

enum _RTC_VALUES_POS_
{
    RTC_SECONDS =       0x00,
    RTC_MINUTES =       0x01,
    RTC_HOUR =          0x02,
    RTC_DAY =           0x04,
    RTC_MONTH =         0x05,
    RTC_YEAR =          0x06
};

void ReadRTC();
void UpdateRTC();
void ClearRtcBuffer();
void StartRtc();
char8* GetRtcBuffer();
void I2CBusLock();
void I2CBusUnlock();

/* [] END OF FILE */
