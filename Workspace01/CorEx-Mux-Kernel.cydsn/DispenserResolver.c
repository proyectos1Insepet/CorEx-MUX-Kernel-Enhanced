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
#include "GlobalDefinitions.h"
#include "SinkManager.h"
#include "SinkObservables.h"
#include "UARTManager.h"
#include "Dispenser.h"

PumpResponseContext _g_respctxt;

//@Created By: HIJH
//@Septembre de 2016
bool DispenserBufferResolver(char* pbuffer, uint16 buffersize)
{
    if(_g_respctxt._responsesize > 0)
    {
        if(buffersize < _g_respctxt._responsesize)
            return false;
    }else{
        //This must be used in case of a response with dynamic payload size
        if(!_g_respctxt.ResponseSizeResolver(_g_respctxt._pparam, pbuffer, buffersize))
            return false;
    }
    
    memcpy(_g_respctxt._rxbuffer, pbuffer, buffersize);
    _g_respctxt._responsesize = buffersize;
    _g_respctxt._bufferready = true;
    
    Dispenser_DisableRxInt();
    
    return true;
}

/*
@GlobalUARTSend standard method used for the UART Manager
@Created by: HIJH
@Date: Août 30 de 2016
Since all of the communications are posted through the same channel, the state machine carry all of the
flags required to notify through the layers.

The time interval between Words shall be at least 68 msec according to the documentation

DO NOT USE "CyDelay*" FUNCTIONS SINCE IT HAS BEEN PROVEN THAT THOSE FUNCTIONS GENERATE A DEADLOCK AT SOME
POINT. ACCORDING TO THE DOCUMENTATION MAYBE IT IS SOMETHING RELATED WITH THE CURRENT FREQ THAT NEEDS TO
BE DEFINED THROUGH A CALL TO "CyDelayFreq".
*/
void DispenserUARTSend(void *ptr)
{
    PUARTMESSAGEPTR pmessage = (PUARTMESSAGEPTR)ptr;
    if(pmessage)
    {
        _INTERFRAME_WAIT_;
        FOR(uint8 index = 0, index < pmessage->_messagelength, index++)
        {
            pmessage->_puartmanager->PutChar(pmessage->_messagetx[index]);
            _INTERWORD_WAIT_;
        }
    }        
}

/* [] END OF FILE */
