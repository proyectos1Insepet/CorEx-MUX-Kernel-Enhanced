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
@Created By: HIJH
@Date: Septembre de 2016
*/
#include <project.h>
#include "SinkManager.h"
#include "SinkObservables.h"
#include "GlobalDefinitions.h"
#include "iButton.h"

void iButtonCRCReset();
bool iButtonCRCCheck(char* pbuffer);
uint8 iButtonRead(uint8 port);
void iButtonWrite(uint8 port, uint8 data);
bool iButtonResetPresenceSequence(uint8 port);


//@Created by: HIJH
//@Date: Septembre de 2016
//Validates and acquires the unique identifier from the related iButton port
void iButton1Scan(void *pparam)
{
    CyDelayFreq(0x00);
    
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(pmsg->_messageid != IBUTTON1_SCAN)
        return;
    
    if(iButtonResetPresenceSequence(_IBUTTON_PORT1_))
    {
        char8 crc = 0x00;
        iButtonWrite(_IBUTTON_PORT1_, _IBUTTON_READ_ROM_);
        FOR(uint8 index = 0, index < _IBUTTON_PAYLOAD_LENGTH_, index++)
            pmsg->_buffer[index] = iButtonRead(_IBUTTON_PORT1_);
        
        FOR(index = 0, index < (_IBUTTON_PAYLOAD_LENGTH_ - 1), index++)
            crc = CRCCheck(crc, pmsg->_buffer[index]);
        
        //iButton data stream integrity is ok after this check...
        if(crc == pmsg->_buffer[(_IBUTTON_PAYLOAD_LENGTH_ - 1)] && crc != 0x00)
            if(pmsg->Callback)
            {
                pmsg->_selfkill = true;
                pmsg->Callback(pmsg);
            }
    }
}

//@Created by: HIJH
//@Date: Septembre de 2016
//Validates and acquires the unique identifier from the related iButton port
void iButton2Scan(void *pparam)
{
    CyDelayFreq(0x00);
    
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(iButtonResetPresenceSequence(_IBUTTON_PORT2_))
    {
        char8 crc = 0x00;
        iButtonWrite(_IBUTTON_PORT2_, _IBUTTON_READ_ROM_);
        FOR(uint8 index = 0, index < _IBUTTON_PAYLOAD_LENGTH_, index++)
            pmsg->_buffer[index] = iButtonRead(_IBUTTON_PORT2_);
        
        FOR(index = 0, index < (_IBUTTON_PAYLOAD_LENGTH_ - 1), index++)
            crc = CRCCheck(crc, pmsg->_buffer[index]);
        
        //iButton data stream integrity is ok after this check...
        if(crc == pmsg->_buffer[(_IBUTTON_PAYLOAD_LENGTH_ - 1)] && crc != 0x00)
            if(pmsg->Callback)
            {
                pmsg->_selfkill = true;
                pmsg->Callback(pmsg);
            }
    }
}

//@Created By: HIJH
//@Septembre de 2016
void iButtonCRCReset()
{
    Reset_iButton_Write(HIGH);
    CRC_CLOCK_PULSE;
    Reset_iButton_Write(LOW);
}

//@Created By: HIJH
//@Septembre de 2016
uint8 iButtonCastCRCOutput()
{
    return (((ShiftReg_iButtonCRC3MSB_ReadRegValue() << 5) & 0xE0) | ((StatusReg_iButton5thCRCBit_Read() << 4) & 0x10) | (ShiftReg_iButtonCRC4LSB_ReadRegValue() & 0x0F));
}

//@Created By: HIJH
//@Septembre de 2016
bool iButtonCRCCheck(char* pbuffer)
{
    FOR(uint8 byteindex = 0, byteindex < _IBUTTON_PAYLOAD_LENGTH_, byteindex++)
    {
        FOR(uint8 bitindex = 0, bitindex < 8, bitindex++)
        {
            Input_iButton_Write((pbuffer[byteindex] >> bitindex) & 0x01);
            CRC_CLOCK_PULSE;
        }
    }
    return (iButtonCastCRCOutput() == 0x00);
}

/* [] END OF FILE */
