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

#include <project.h>
#include "GlobalDefinitions.h"
#include "Eeprom.h"

volatile bool _g_i2cbuslock = false;

uint8 _g_eeprom[_EEPROM_PAGE_RAWSIZE_];

//@Created By: HIJH
//@Septembre de 2016
uint8* GetEepromBuffer()
{
    return &_g_eeprom[2];
}

//@Created By: HIJH
//@Septembre de 2016
void ClearEepromBuffer()
{
    memset(_g_eeprom, 0x00, _EEPROM_PAGE_RAWSIZE_);
}

//@Created by: HIJH
//@Date: Octobre de 2016
//This function provides exclusive access to the I2C bus by blocking 
//the signaling a flag. It attempts to "Desynchronize"
void I2CBusLock()
{
    //I2C Bus Lock REMOVE DUE TO A WEIRD BEHAVIOR
    _g_i2cbuslock = true;
}

void I2CBusUnlock()
{
    //I2C Bus Lock to avoid concurrent access to the bus    
    _g_i2cbuslock = false;
}


//@Created By: HIJH
//@Septembre de 2016
void LoadEepromPage(uint16 page)
{
    //I2C_Bus_MasterStatus();
    
    I2C_Bus_MasterClearStatus();    
    _g_eeprom[0] = (page >> 8) & 0xFF; _g_eeprom[1] = page & 0xFF;
    uint16 status = I2C_Bus_MasterSendStart(_EEPROM_SLAVE_ADDRESS_BY_MEMARRAY_, I2C_Bus_WRITE_XFER_MODE);
    if(I2C_Bus_MSTR_NO_ERROR == status)
    {
        FOR(uint8 index = 0, index < 2, index++)
            I2C_Bus_MasterWriteByte(_g_eeprom[index]);
            
        status = I2C_Bus_MasterSendRestart(_EEPROM_SLAVE_ADDRESS_BY_MEMARRAY_, I2C_Bus_READ_XFER_MODE);
        if(I2C_Bus_MSTR_NO_ERROR == status)
        {
            FOR(index = 2, index < _EEPROM_PAGE_RAWSIZE_, index++)
                _g_eeprom[index] = I2C_Bus_MasterReadByte(I2C_Bus_ACK_DATA);
                
        }
        I2C_Bus_MasterSendStop();
    }
}

//@Created By: HIJH
//@Septembre de 2016
void StoreEepromPage(uint16 page)
{    
    uint8 retries = 0x00;
    //The WP signal must be LOW to allow the writting op
    //Write protection enabled
    eepromWP_Write(0x00);
    uint16 status = 0x00;
    do
    {
        I2C_Bus_MasterClearStatus();    
        _g_eeprom[0] = (page >> 8) & 0xFF; _g_eeprom[1] = page & 0xFF;  
        status = I2C_Bus_MasterSendStart(_EEPROM_SLAVE_ADDRESS_BY_MEMARRAY_, I2C_Bus_WRITE_XFER_MODE);
        if(I2C_Bus_MSTR_NO_ERROR == status)
        {
            FOR(uint8 index = 0, index < _EEPROM_PAGE_RAWSIZE_, index++)
                I2C_Bus_MasterWriteByte(_g_eeprom[index]);

            I2C_Bus_MasterSendStop();
            CyDelay(5);
        }
    }while(I2C_Bus_MSTR_NO_ERROR != status && retries++ < 10);
    //Write protection disabled
    eepromWP_Write(0x01);
}

/* [] END OF FILE */
