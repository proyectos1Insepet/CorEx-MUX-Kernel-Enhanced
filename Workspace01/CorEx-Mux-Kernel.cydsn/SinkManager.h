/* ========================================
 *
 * Copyright SISTEMAS INSEPET, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF SISTEMAS INSEPET.
 *
 * Created by: HIJH
 * Date: Septembre 2 de 2016
 *
 * ========================================
*/

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"


// system includes
#include <math.h>
#include <stdint.h>
#include <stdbool.h>


///*****************************************************************************************
///GLOBAL CONSTANTS FOR SOME TIMEOUT DEFINITIONS TO APPLY TO THE MESSAGES
#define _SINK_TIMEOUT_1MS_          1
#define _SINK_TIMEOUT_10MS_         10
#define _SINK_TIMEOUT_15MS_         15
#define _SINK_TIMEOUT_20MS_         20
#define _SINK_TIMEOUT_30MS_         30
#define _SINK_TIMEOUT_50MS_         50
#define _SINK_TIMEOUT_70MS_         70
#define _SINK_TIMEOUT_100MS_        100
#define _SINK_TIMEOUT_200MS_        200
#define _SINK_TIMEOUT_250MS_        250
#define _SINK_TIMEOUT_500MS_        500
#define _SINK_TIMEOUT_1S_           1000
#define _SINK_TIMEOUT_2S_           2000
#define _SINK_TIMEOUT_5S_           5000
#define _SINK_TIMEOUT_10S_          10000
#define _SINK_TIMEOUT_15S_          15000
#define _SINK_TIMEOUT_20S_          20000
#define _SINK_TIMEOUT_30S_          30000
#define _SINK_TIMEOUT_40S_          40000
#define _SINK_TIMEOUT_45S_          45000
#define _SINK_TIMEOUT_50S_          50000
#define _SINK_TIMEOUT_60S_          60000
#define _SINK_TIMEOUT_65S_          65000
#define _SINK_TIMEOUT_70S_          70000
#define _SINK_TIMEOUT_80S_          80000
#define _SINK_TIMEOUT_90S_          90000

#define     _MESSAGE_QUEUE_DEPTH_       48
#define     _SUBSCRIBER_QUEUE_DEPTH_    16
#define     _MESSAGE_LENGTH_            384
#define     _OBSERVED_IDENTIFIERS_      8

#define _TIMEOUT_BASE_TIME_             1000

typedef enum _MESSAGE_TYPE_
{
    NONE = 0x01,
    BROADCAST,
    FIRSTFOUND,
    ALLINTERESTED,
    DELAYEDALLINTERESTED,
    FIRSTFOUNDFOREVER,
    BROADCASTFOREVER,
    ALLINTERESTEDFOREVER
    
} MESSAGETYPE;

typedef enum _MESSAGE_STATE_
{
    SINK_FREE = 0x01,
    SINK_ALLOCATED,
    SINK_BUSY
} MESSAGESTATE;

typedef struct _SINK_MESSAGE_
{
    volatile bool _selfkill;
    volatile uint8 _messageid;
    volatile uint8 _messagetype;
    volatile uint8 _messagestate;
    //These parameters are only for messages allowed to generate timeous
    volatile uint16 _messagedelay;
    volatile uint16 _messagetimeout;
    volatile uint16 _messagetimeoutmultiplier;
    volatile uint16 _messagetimeoutlimit;    
    char8 _buffer[_MESSAGE_LENGTH_];
    volatile uint16 _buffersize;
    //Data structure associated to the Message that may help the subscriber (server) to
    //make decisions and/or alter a client data structure directly.
    void *_pdata;
    void (*Callback)(void *pbuffer);
} SinkMessage, *PSINKMESSAGEPTR;

typedef struct _SINK_SUBSCRIBER_
{
    volatile uint8 _state;
    //array of observable identifiers
    uint8 _arrmsgid[_OBSERVED_IDENTIFIERS_];
    void (*Callback[_OBSERVED_IDENTIFIERS_])(void *pbuffer);
} SinkSubscriber, *PSINKSUBSCRIBERPTR;

#define _ALLOCATE_SINKMESSAGE_SLOT(a)       \
PSINKMESSAGEPTR a = AllocateMessageSlot()
#define _REALLOCATE_SINKMESSAGE_SLOT(a)     \
a = AllocateMessageSlot()

#define _ALLOCATE_SINKMESSAGE_SLOT_CONDITIONAL(a,b)     \
PSINKMESSAGEPTR a = AllocateMessageSlotConditional(b)
#define _REALLOCATE_SINKMESSAGE_SLOT_CONDITIONAL(a,b)     \
a = AllocateMessageSlotConditional(b)

void SinkManagerRegistration();
PSINKMESSAGEPTR AllocateMessageSlot();
PSINKMESSAGEPTR AllocateMessageSlotConditional(uint8 messageid);
PSINKMESSAGEPTR* AllocateMessageArraySlotConditional(uint8 messageid);
PSINKSUBSCRIBERPTR AllocateSubscriberSlot();
void ReleaseSinkMessage(PSINKMESSAGEPTR pmessage);
void ReleaseSinkMessageByID(uint8 messageid);
bool FindSinkMessage(uint8 messageid);

extern PSINKMESSAGEPTR _g_pretval[0x04];
extern SinkMessage _g_sinkmessagequeue[_MESSAGE_QUEUE_DEPTH_];
extern SinkSubscriber _g_sinksubscriberqueue[_SUBSCRIBER_QUEUE_DEPTH_];

/* [] END OF FILE */
