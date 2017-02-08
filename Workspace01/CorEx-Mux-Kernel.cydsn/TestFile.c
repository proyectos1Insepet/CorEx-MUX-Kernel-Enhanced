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
 THIS FILE IS INTENDED FOR TEST PURPOSES ONLY!!!
 MUST BE REMOVED DISCRETIONARILY
*/
#include <project.h>
#include "SinkManager.h"
#include "SinkObservables.h"

#include "UARTManager.h"

void BlinkLEDOniButton2(void *pparam);

void InitTest()
{
/*
    SinkSubscriber* subscriber = AllocateSubscriberSlot();
    if(subscriber)
    {
        uint8 index = 0;
        //The maximum number of observable identifiers is _OBSERVED_IDENTIFIERS_
        //located in the header file "SinkManager.h"
        //Every identifier is paired with its respective Callback method pointer
        subscriber->_arrmsgid[index] = BLINK_LED_ON_IBUTTON2_PORT;
        subscriber->Callback[index++] = BlinkLEDOniButton2;
    }

    SinkMessage* sinkmsg = AllocateMessageSlot();    
    if(sinkmsg)
    {
        sinkmsg->_messageid = BLINK_LED_ON_IBUTTON2_PORT;
        sinkmsg->_messagetype = ALLINTERESTEDFOREVER;
        sinkmsg->_messagetimeoutlimit = _SINK_TIMEOUT_1S_;
        memcpy(sinkmsg->_buffer, (const void*)NULL, 0x00);
        sinkmsg->_messagestate = SINK_BUSY;
    }
*/
}

void BlinkLEDOniButton2(void *pparam)
{
    //LED TOGGLING
    iButton2_Write(!iButton2_Read());
}

/* [] END OF FILE */
