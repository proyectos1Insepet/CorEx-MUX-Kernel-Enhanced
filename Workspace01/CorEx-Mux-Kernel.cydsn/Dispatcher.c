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

#include "GlobalDefinitions.h"
#include "DispatcherDefinitions.h"
#include "RFDefinitions.h"

HandlerPtr GetCommandHandler(uint8 commandid);

//***********************************************************************************
//DISPATCHER HANDLER PROTOTYPES
//ADD HERE THE HANDLER PROTOTYPES
//***********************************************************************************
DispatchTable _g_dispatchtable[] =
{
    //***********************************************************************************
    //MARK BLOCK OF HANDLER ACCORDING TO THEIR RESPONSABILITY OR MODULE
    { _DISP_HANDLER0_, DispHandler }
    //***********************************************************************************
};
//***********************************************************************************

void LoopDispatcher()
{
    //Entering this block means that the RX buffer is full and requires attention.
    //First, the CRC must be validated to post the ACK/NACK signal, then the message 
    //block must be "Unscrambbled", and validated to see whether it belongs to a 
    //"multi-block" transaction. If it does, search for its container and reassemble.
    if(_g_networkingstack._datalinkackphase)
    {
        uint8 payloadsize = _g_networkingstack._rxbuffer[0];
        //Check whether the data isn't corrupted. The last byte in the buffer is the CRC
        //calculated on the transmitter side. Compare both localcrc and the remote crc to detect errors.
        char8 localcrc = CalcCRC(&_g_networkingstack._rxbuffer[1], payloadsize - 1);
        
        //Both checksums must be equal, if not then something corrupted the stream while transmitting
        if(localcrc == _g_networkingstack._rxbuffer[payloadsize])
        //The checksum comes in the last position but since the header occupies 3 bytes, they must be added
        {
            char8 lbuffer[] = { _RF_DATALINK_ACK_ };            
            TransportRF(lbuffer, 1);
            
            //Unscramble since the whole block has been scrambled
            //UnscrambleRxBuffer(1, payloadsize);
            
            //Checking how many blocks the message is composed by
            uint8 numblocks = _g_networkingstack._rxbuffer[1];
            //Checking current block index
            uint8 blockindex = _g_networkingstack._rxbuffer[2];
            //Checking block limit
            uint8 blocklimit = _g_networkingstack._rxbuffer[3];
            
            memcpy(&_g_networkingstack._transbuffer[_g_networkingstack._datalinktranspointer],
            &_g_networkingstack._rxbuffer[4], blocklimit);
            
            _g_networkingstack._datalinktranspointer += blocklimit;

            memset(_g_networkingstack._rxbuffer, 0x00, _SYMETRICAL_BUFFER_SIZE_);
            //Change this only until the end since it will keep locked the networking stack
            _g_networkingstack._datalinkackphase = FALSE;
            
            //This is the condition to detect the last block of the sequence
            //since all of the blocks must arrive in strictly order
            if(blockindex == (numblocks - 1))
            {
                //ACCORDING TO THE CONVENTION, THE FIRST BYTE IN THE DATA BLOCK MUST BE THE
                //HANDLER ID TO WHICH THE MESSAGE IS MAPPED.
                //NONE OF THE HANDLER METHODS CAN MAKE A CALL BACK TO THE NETWORKING STACK DIRECTLY!!!
                //THIS WILL BLOCK THE FIRMWARE SINCE THE ACK/NACK SIGNAL IS REQUIRED.
                //USE A STATE MACHINE MODEL TO INVOKE THE REQUIRED FUNCTION IN THE NEXT ITERATION OF THE GLOBAL LOOP
                HandlerPtr hptr = GetCommandHandler(_g_networkingstack._transbuffer[0]);
                if(hptr)
                    hptr();
                    

                _g_networkingstack._datalinktranspointer = 0x0000;
                memset(_g_networkingstack._transbuffer, 0x00, _TRANSACTIONAL_BUFFER_SIZE_);
                
            }
            
        }
        else
        {
            //Since there was a CRC mismatch, the buffer content must be discarded
            //and the transmitter informed about the failure
            char8 lbuffer[] = { _RF_DATALINK_NACK_ };
            
            //WARNING!!                
            //THIS IS VERY IMPORTANT SINCE WITH THIS FLAG WE START THE ACK/NACK PHASE
            //AND INSTRUCT THE STACK TO AVOID AN INFINITE LOOP!
            TransportRF(lbuffer, 1);
        }
                                
        
    }
}

void InitRFDispatcher()
{
    //Registering the dispatcher loop into the global loop
    GLOBAL_LOOP_PUSH(LoopDispatcher);
}

//Attempts to locate the handler mapped to the respective commandid
HandlerPtr GetCommandHandler(uint8 commandid)
{
    uint8 index = 0;
    HandlerPtr retval = NULL;
    for(;index < (sizeof(_g_dispatchtable) / sizeof(DispatchTable)); index++)
    {
        if(_g_dispatchtable[index]._commandid == commandid)
        {
            retval = _g_dispatchtable[index]._hptr;
            break;
        }
    }
    
    return retval;
}

/* [] END OF FILE */
