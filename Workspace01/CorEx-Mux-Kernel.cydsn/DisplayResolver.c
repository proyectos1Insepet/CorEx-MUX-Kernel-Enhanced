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
#include "GlobalDefinitions.h"
#include "SinkManager.h"
#include "SinkObservables.h"
#include "UARTManager.h"
#include "Display.h"

//@Created By: HIJH
//@Septembre de 2016
bool Display1BufferResolver(char8* pbuffer, uint16 buffersize)
{
    //Display frame delimiters
    char8 pdelimiter[] = { 0xAA, 0xCC, 0x33, 0xC3, 0x3C };
    uint16 delimitersize = sizeof(pdelimiter);
    /////////////////////////////////////////////////////////////////////////////////////////
    //TODO: PLACE HERE ALL THE NECESSARY CODE TO RESOLVE THE COMPLETENESS OF THE ARRIVED DATA
    /////////////////////////////////////////////////////////////////////////////////////////
    if(pbuffer[0] != pdelimiter[0] && buffersize >= 0x08) 
        return true;
    
    if(
    (pbuffer[buffersize - 1] != pdelimiter[delimitersize - 1]) || 
    (pbuffer[buffersize - 2] != pdelimiter[delimitersize - 2]) ||
    (pbuffer[buffersize - 3] != pdelimiter[delimitersize - 3]) || 
    (pbuffer[buffersize - 4] != pdelimiter[delimitersize - 4]))
        return false;
    
    /////////////////////////////////////////////////////////////////////////////////////////    
    
    SinkMessage* sinkmsg = AllocateMessageSlot();
    if(sinkmsg)
    {
        sinkmsg->_messageid = DISPLAY1_RECEPTION;
        sinkmsg->_messagetype = FIRSTFOUND;
        memcpy(sinkmsg->_buffer, (const void*)pbuffer, 0x08);
        sinkmsg->_buffersize = buffersize;
        sinkmsg->_messagestate = SINK_BUSY;
        
        Display1_DisableRxInt();
    }
    return true;
}

//@Created By: HIJH
//@Septembre de 2016
bool Display2BufferResolver(char8* pbuffer, uint16 buffersize)
{    
    //Display frame delimiters
    char8 pdelimiter[] = { 0xAA, 0xCC, 0x33, 0xC3, 0x3C };
    uint16 delimitersize = sizeof(pdelimiter);
    /////////////////////////////////////////////////////////////////////////////////////////
    //TODO: PLACE HERE ALL THE NECESSARY CODE TO RESOLVE THE COMPLETENESS OF THE ARRIVED DATA
    /////////////////////////////////////////////////////////////////////////////////////////
    if(pbuffer[0] != pdelimiter[0] && buffersize >= 0x08) 
        return true;
    
    if(
    (pbuffer[buffersize - 1] != pdelimiter[delimitersize - 1]) || 
    (pbuffer[buffersize - 2] != pdelimiter[delimitersize - 2]) ||
    (pbuffer[buffersize - 3] != pdelimiter[delimitersize - 3]) || 
    (pbuffer[buffersize - 4] != pdelimiter[delimitersize - 4]))
        return false;
    
    /////////////////////////////////////////////////////////////////////////////////////////    
    
    SinkMessage* sinkmsg = AllocateMessageSlot();
    if(sinkmsg)
    {
        sinkmsg->_messageid = DISPLAY2_RECEPTION;
        sinkmsg->_messagetype = FIRSTFOUND;
        memcpy(sinkmsg->_buffer, (const void*)pbuffer, 0x08);
        sinkmsg->_buffersize = buffersize;
        sinkmsg->_messagestate = SINK_BUSY;
        
        Display2_DisableRxInt();
    }        
    return true;
}

//@Created By: HIJH
//@Septembre de 2016
void Dislay1UARTSend(void *ptr)
{
    UARTMessage *pmessage = (UARTMessage*)ptr;
    if(pmessage)
    {
        FOR(uint8 index = 0, index < pmessage->_messagelength, index++)
            pmessage->_puartmanager->PutChar(pmessage->_messagetx[index]);
    }
}

//@Created By: HIJH
//@Septembre de 2016
void Dislay2UARTSend(void *ptr)
{
    UARTMessage *pmessage = (UARTMessage*)ptr;
    if(pmessage)
    {
        FOR(uint8 index = 0, index < pmessage->_messagelength, index++)
            pmessage->_puartmanager->PutChar(pmessage->_messagetx[index]);
    }
}

/* [] END OF FILE */
