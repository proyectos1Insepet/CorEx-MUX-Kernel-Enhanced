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

#include <project.h>
#include "AuxPSoC.h"

//@Created By: HIJH
//@Septembre de 2016
uint8 WriteAuxPSoC(PNEAR_BYTE_PTR lpbuffer, uint16 buffersize, uint8 printerport)
{
	uint8 retval = 0x00;
    
    FOR(uint16 index = 0, index < buffersize, index++)
    {
        I2C_Bus_MasterClearStatus();
        retval = I2C_Bus_MasterSendStart(_AUXPSOC_I2C_SLAVE_ADDRESS_, I2C_Bus_WRITE_XFER_MODE);
        if(I2C_Bus_MSTR_NO_ERROR == retval)
        {
            I2C_Bus_MasterWriteByte(printerport);
            I2C_Bus_MasterWriteByte(lpbuffer[index]);
            
            I2C_Bus_MasterSendStop();
        }
    }
    
	return retval;
}
/* [] END OF FILE */
