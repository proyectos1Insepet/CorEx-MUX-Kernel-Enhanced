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
#include <project.h>
#include "Configuration.h"
#include "GlobalDefinitions.h"
#include "UARTManager.h"
#include "SinkManager.h"
#include "SinkObservables.h"
#include "Dispenser.h"
#include "RFLink.h"

RFStreamInfo _g_rfrx;

//This is the header that comes in the request stream

uint16 RFGetPayloadLengthFromRequestCode(uint8 reqcode);
PayloadSizeResolver RFGetDynamicPayloadLengthFromRequestCode(uint8 reqcode);

//@Created By: HIJH
//@Septembre de 2016
//This will be invoked by the ISR once data has arrived the port.
//Upon finishing the buffering, returning "true" will free up the port's buffer
//@Update by: HIJH
//@Octobre de 2016
//A critical modification has been needed since a "cross-talk" condition among MUXs has been detected. 
//This condition corrupted the reception buffer blocking the normal reception.
bool RFUARTBufferResolver(char* pbuffer, uint16 buffersize)
{
    /////////////////////////////////////////////////////////////////////////////////////////
    //There must be at least _RF_STREAM_FIXED_LENGTH_HEADER_BLOCK_ bytes loaded in the buffer to 
    //figure out for the completeness of the data
    /////////////////////////////////////////////////////////////////////////////////////////
    if(buffersize < _RF_STREAM_FIXED_LENGTH_HEADER_BLOCK_)
        return false;

    //This flag indicates that the received data is not pointed to the current unit (MUX)
    bool messagefrommaster = false;
    bool messagefromsibling = false;
    //Header detection for the remote peer (NSX)
    //Header for the remaining slave units (Other MUXs)
    if((pbuffer[0x00] == _g_rfmasterheader[0x00]) && 
    (pbuffer[0x01] == _g_rfmasterheader[0x01]) && 
    (pbuffer[0x02] == _g_rfmasterheader[0x02]))
        messagefrommaster = true;
    else if((pbuffer[0x00] == _g_rfslaveheader[0x00]) && 
        (pbuffer[0x01] == _g_rfslaveheader[0x01]) && 
        (pbuffer[0x02] == _g_rfslaveheader[0x02]))
            messagefromsibling = true;
    
    bool ignoremessage = true;
    //If this point is skipped it means that some noise arrived the buffer        
    if(messagefrommaster || messagefromsibling)
    {
        //If the local station identifier does not match the targeted station identifier, then filter out!
        if((pbuffer[_RF_STREAM_TARGET_INDEX0_] == ((_g_stationidentifier >> 0) & 0xFF)) &&
            (pbuffer[_RF_STREAM_TARGET_INDEX1_] == ((_g_stationidentifier >> 8) & 0xFF)))
            ignoremessage = false;
    }
    
    //If there is no match on the above frame headers, then discard everything since
    //it means that some noise coming from a foreign source has corrupted the buffer
    if(messagefrommaster)
    {
        //Wait until the whole stream has arrived according to the payload map...
        uint16 expectedbuffersize = RFGetPayloadLengthFromRequestCode(pbuffer[_RF_STREAM_COMMAND_INDEX_]);
        if((expectedbuffersize > 0) && (buffersize < expectedbuffersize))
            return false;
        else if(expectedbuffersize == 0)//Variable buffer size that requires a "Buffer Size Resolver"
        {
            PayloadSizeResolver fpResolver = RFGetDynamicPayloadLengthFromRequestCode(pbuffer[_RF_STREAM_COMMAND_INDEX_]);
            if(fpResolver)
            {
                uint16 payloadsize = fpResolver(pbuffer);
                if(buffersize < payloadsize)
                    return false;
                //Here we measure the total length of the received buffer since it can not exceed the calculated value
                //This is to avoid a buffer overflow due to the maximum length of the a sink message
                if(payloadsize > _MESSAGE_LENGTH_)
                    return true;
                
            }else//IF VARIABLE LENGTH STREAM HAS BEEN DEFINED AND NO POINTER TO HANDLE IT, THEN CUT OFF THE PROCESS!!
                return true;
        }
        //SINCE BROADCAST MESSAGES AREN'T POINTED TO A SPECIFIC POSITION, THIS BYTE MUST BE SET TO 0x00 
        if(pbuffer[_RF_STREAM_POSITION_INDEX_] != 0x00)
        {
            if(!ignoremessage)
            {
                ignoremessage = true;
                //This block is meant to filter out any request that is not pointing in an existing position
                //within this MUX. Since this request is coming from a valid remote peer (NSX), the it will be 
                //received but won't be processed.
                FOR(uint8 index = 0, index < _g_dispenserlayoutinfo._numpositions, index++)
                {
                    if(_g_pumps[index]._pumpid == (pbuffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset))
                    {
                        ignoremessage = false;
                        break;
                    }
                }
            }
        }
        //If "ignoremessage" is false it means that the remote peer was talking with another sibling            
        if(!ignoremessage)
        {
            /////////////////////////////////////////////////////////////////////////////////////////    
            //Once the buffer is complete, transfer it through the message sink to the RF BLL handler
            PSINKMESSAGEPTR psinkmsg = AllocateMessageSlot();
            if(psinkmsg)
            {
                psinkmsg->_messageid = UART_RF_RECEPTION;
                psinkmsg->_messagetype = FIRSTFOUND;
                memcpy(psinkmsg->_buffer, (const void*)pbuffer, buffersize);
                psinkmsg->_buffersize = buffersize;
                psinkmsg->_messagestate = SINK_BUSY;
                
            }
        }
    }
    else if(messagefromsibling)//Entering here means that the incoming data comes from a sibling (Another MUX)
    {
        //Wait until the whole buffer has been filled up with the appropriate response from the sibling, then discard it!
        if(pbuffer[(buffersize - 1)] != 0xFF) // || pbuffer[(buffersize - 2)] != 0xEE || pbuffer[(buffersize - 3)] != 0xDD)
            return false;
    }
    return true;
}

//@Created By: HIJH
//@Septembre de 2016
void RFUARTSend(void *ptr)
{
    PUARTMESSAGEPTR pmessage = (PUARTMESSAGEPTR)ptr;
    FOR(uint8 index = 0, index < pmessage->_messagelength, index++)
    {
        pmessage->_puartmanager->PutChar(pmessage->_messagetx[index]);
        NO_INTERBYTE_DELAY;
    }
    //This is required for the remote peer receiver (CDG) to locate the frame end
    //pmessage->_puartmanager->PutChar(0xDD);
    //pmessage->_puartmanager->PutChar(0xEE);
    pmessage->_puartmanager->PutChar(0xFF);
}
/* [] END OF FILE */
