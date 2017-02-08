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

enum
{
    TRASMITTING,
    RECEIVING
};

#ifdef _RF_LOOP_TEST_TRANSMITTER_
    uint8 _peerstate = TRASMITTING;
#endif

#ifdef _RF_LOOP_TEST_RECEIVER_    
    uint8 _peerstate = RECEIVING;
#endif    

bool _firsttransmission = true;

void LoopHandler()
{
    if(_peerstate == TRASMITTING)
    {
        if(_firsttransmission)
        {
            //Test buffer to be transmitted to the communication peer
            char8 buffer[] = { _DISP_HANDLER0_, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7 };
            bool retval = TransportRF(buffer, sizeof(buffer) / sizeof(buffer[0]));
            if(retval)
                _peerstate = RECEIVING;
            else
                _firsttransmission = false;
        }
    }
}

void DispHandler()
{
    memset(_g_networkingstack._transbuffer, 0x00, sizeof(_g_networkingstack._transbuffer) / sizeof(_g_networkingstack._transbuffer[0]));
    _peerstate = TRASMITTING;
}

void InitTest()
{
    GLOBAL_LOOP_PUSH(LoopHandler);
}

/* [] END OF FILE */
