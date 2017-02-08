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
#include "SinkManager.h"
#include "SinkObservables.h"

void SinkHandler();
void SinkBroadcast(SinkMessage *pmsg);
void SinkFirstFound(SinkMessage *pmsg);
void SinkAllInterested(SinkMessage *pmsg);
void SinkBroadcastForever(SinkMessage *pmsg);
void SinkCounterUpdateCallback(void *pdata);
void SinkReleaseByIDCallback(void *pparam);
void SinkReleaseByPtrCallback(void *pparam);

PSINKMESSAGEPTR _g_pretval[0x04];

SinkMessage _g_sinkmessagequeue[_MESSAGE_QUEUE_DEPTH_];
SinkSubscriber _g_sinksubscriberqueue[_SUBSCRIBER_QUEUE_DEPTH_];

//@Created By: HIJH
//@Septembre de 2016
void SinkManagerRegistration()
{
    uint8 index = 0;
    for(; index < _MESSAGE_QUEUE_DEPTH_; index++)
    {
        _g_sinkmessagequeue[index]._messageid = 0x00;
        _g_sinkmessagequeue[index]._messagestate = SINK_FREE;
        _g_sinkmessagequeue[index]._messagetype = NONE;
        memset(_g_sinkmessagequeue[index]._buffer, 0x00, sizeof(_g_sinkmessagequeue[index]._buffer)/sizeof(_g_sinkmessagequeue[index]._buffer[0]));
    }

    for(index = 0; index < _SUBSCRIBER_QUEUE_DEPTH_; index++)
    {
        _g_sinksubscriberqueue[index]._state = SINK_FREE;
        memset(_g_sinksubscriberqueue[index]._arrmsgid, 0x00, sizeof(_g_sinksubscriberqueue[index]._arrmsgid) / sizeof(_g_sinksubscriberqueue[index]._arrmsgid[0]));
        memset(_g_sinksubscriberqueue[index].Callback, 0x00, sizeof(_g_sinksubscriberqueue[index]._arrmsgid) / sizeof(_g_sinksubscriberqueue[index]._arrmsgid[0]));
    }
    
    //This subscriber from the very own SinkManager is the entry point
    //that will be used to update the timeout multipliers for the BROADCASTFOREVER messages.
    PSINKSUBSCRIBERPTR psubscriber = AllocateSubscriberSlot();
    if(psubscriber)
    {
        uint8 index = 0;
        //The maximum number of observable identifiers is _OBSERVED_IDENTIFIERS_
        //located in the header file "SinkManager.h"
        //Every identifier is paired with its respective Callback method pointer
        psubscriber->_arrmsgid[index] = SINK_UPDATE_COUNTERS;
        psubscriber->Callback[index++] = SinkCounterUpdateCallback;
        
        psubscriber->_arrmsgid[index] = SINK_RELEASE_BY_ID;
        psubscriber->Callback[index++] = SinkReleaseByIDCallback;        
        
        psubscriber->_arrmsgid[index] = SINK_RELEASE_BY_PTR;
        psubscriber->Callback[index++] = SinkReleaseByPtrCallback;        
    }
    
    GLOBAL_LOOP_PUSH(SinkHandler);
}

//@Created By: HIJH
//@Septembre de 2016
PSINKMESSAGEPTR AllocateMessageSlot()
{
    PSINKMESSAGEPTR pretval = NULL;
    
    //HIJH
    //This is to De-synchronize REENTRANT calls
    //CyDelayUs((uint16)((double)rand()/(double)RAND_MAX) + 1);
    RANDOMDELAY;

    FOR(uint8 index = 0, index < _MESSAGE_QUEUE_DEPTH_, index++)
    {
        if(_g_sinkmessagequeue[index]._messagestate == SINK_FREE)
        {
            _g_sinkmessagequeue[index]._selfkill = false;
            _g_sinkmessagequeue[index]._messagedelay = 0x0000;
            _g_sinkmessagequeue[index]._messagestate = SINK_ALLOCATED;
            _g_sinkmessagequeue[index]._messagetimeout = Timer_Tick_ReadCounter();
            _g_sinkmessagequeue[index]._messagetimeoutmultiplier = 0;
            memset(_g_sinkmessagequeue[index]._buffer, 0x00, _MESSAGE_LENGTH_);
            pretval = &_g_sinkmessagequeue[index];
            break;
        }
    }
    
    return pretval;
}

//@Created By: HIJH
//@Septembre de 2016
bool FindSinkMessage(uint8 messageid)
{
    bool retval = false;
    
    //HIJH
    //This is to De-synchronize REENTRANT calls
    //CyDelayUs((uint16)((double)rand()/(double)RAND_MAX) + 1);
    RANDOMDELAY;
    
    FOR(uint8 index = 0, index < _MESSAGE_QUEUE_DEPTH_, index++)
    {
        if(_g_sinkmessagequeue[index]._messageid == messageid)
        {
            retval = true;
            break;
        }
    }
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
PSINKMESSAGEPTR AllocateMessageSlotConditional(uint8 messageid)
{
    PSINKMESSAGEPTR pretval = NULL;
    
    //HIJH
    //This is to De-synchronize REENTRANT calls
    //CyDelayUs((uint16)((double)rand()/(double)RAND_MAX) + 1);
    RANDOMDELAY;
    
    FOR(uint8 index = 0, index < _MESSAGE_QUEUE_DEPTH_, index++)
    {
        if(_g_sinkmessagequeue[index]._messageid == messageid)
        {
            //_g_sinkmessagequeue[index]._selfkill = true;
            pretval = &_g_sinkmessagequeue[index];
            return pretval;
        }
    }

    FOR(index = 0, index < _MESSAGE_QUEUE_DEPTH_, index++)
    {
        if(_g_sinkmessagequeue[index]._messagestate == SINK_FREE)
        {
            _g_sinkmessagequeue[index]._selfkill = false;
            _g_sinkmessagequeue[index]._messagestate = SINK_ALLOCATED;
            _g_sinkmessagequeue[index]._messagetimeout = Timer_Tick_ReadCounter();
            _g_sinkmessagequeue[index]._messagetimeoutmultiplier = 0x0000;
            _g_sinkmessagequeue[index]._messagedelay = 0x0000;
            memset(_g_sinkmessagequeue[index]._buffer, 0x00, _MESSAGE_LENGTH_);
            pretval = &_g_sinkmessagequeue[index];
            break;
        }
    }
    
    return pretval;
}

PSINKMESSAGEPTR* AllocateMessageArraySlotConditional(uint8 messageid)
{
    FOR(uint8 arrindex = 0, arrindex < 0x04, arrindex++)
        _g_pretval[arrindex] = PNEAR_NULLPTR;
    
    //HIJH
    //This is to De-synchronize REENTRANT calls
    //CyDelayUs((uint16)((double)rand()/(double)RAND_MAX) + 1);
    RANDOMDELAY;

    arrindex = 0;
    FOR(uint8 index = 0, (index < _MESSAGE_QUEUE_DEPTH_) && (arrindex < 0x04), index++)
    {
        if(_g_sinkmessagequeue[index]._messageid == messageid)
            _g_pretval[arrindex++] = &_g_sinkmessagequeue[index];
    }
    
    return _g_pretval;
}

//@Created By: HIJH
//@Septembre de 2016
void ReleaseSinkMessage(PSINKMESSAGEPTR pmessage)
{
    pmessage->_messageid = 0x00;
    pmessage->_messagestate = SINK_FREE;
    pmessage->_messagetype = NONE;
    pmessage->_messagetimeout = 0x00;
}

//@Created By: HIJH
//@Septembre de 2016
void ReleaseSinkMessageByID(uint8 messageid)
{
    FOR(uint8 index = 0, index < _MESSAGE_QUEUE_DEPTH_, index++)
    {        
        PSINKMESSAGEPTR pmsg = &_g_sinkmessagequeue[index];
        if(pmsg->_messageid == messageid)
        {
            ReleaseSinkMessage(pmsg);
            break;
        }
    }
}

//@Created By: HIJH
//@Septembre de 2016
PSINKSUBSCRIBERPTR AllocateSubscriberSlot()
{
    PSINKSUBSCRIBERPTR pretval = NULL;

    uint8 index = 0;
    for(index = 0; index < _SUBSCRIBER_QUEUE_DEPTH_; index++)
    {
        if(_g_sinksubscriberqueue[index]._state == SINK_FREE)
        {
            _g_sinksubscriberqueue[index]._state = SINK_BUSY;
            pretval = &_g_sinksubscriberqueue[index];
            break;
        }
    }
    return pretval;
}

//@Created By: HIJH
//@Septembre de 2016
void SinkHandler()
{
    FOR(uint8 index = 0, index < _MESSAGE_QUEUE_DEPTH_, index++)
    {        
        PSINKMESSAGEPTR pmsg = &_g_sinkmessagequeue[index];
        if(pmsg->_messagestate == SINK_BUSY)
        {            
            switch(pmsg->_messagetype)
            {
                case BROADCAST:
                {
                    SinkBroadcast(&_g_sinkmessagequeue[index]);
                    ReleaseSinkMessage(&_g_sinkmessagequeue[index]);
                    break;
                }
                case FIRSTFOUND:
                {
                    SinkFirstFound(&_g_sinkmessagequeue[index]);
                    ReleaseSinkMessage(&_g_sinkmessagequeue[index]);
                    break;
                }
                case ALLINTERESTED:
                {
                    SinkAllInterested(&_g_sinkmessagequeue[index]);
                    ReleaseSinkMessage(&_g_sinkmessagequeue[index]);
                    break;
                }
                case DELAYEDALLINTERESTED:
                {
                    int32 timespan = ((Timer_Tick_ReadCounter() + _TIMEOUT_BASE_TIME_*_g_sinkmessagequeue[index]._messagetimeoutmultiplier) - _g_sinkmessagequeue[index]._messagetimeout);
                    if(((double)timespan / (double)_TIMEOUT_BASE_TIME_) > _g_sinkmessagequeue[index]._messagedelay)
                    {
                        SinkAllInterested(&_g_sinkmessagequeue[index]);
                        ReleaseSinkMessage(&_g_sinkmessagequeue[index]);
                        
                    }
                    break;
                }
                case FIRSTFOUNDFOREVER:
                {
                    int32 timespan = ((Timer_Tick_ReadCounter() + _TIMEOUT_BASE_TIME_*_g_sinkmessagequeue[index]._messagetimeoutmultiplier) - _g_sinkmessagequeue[index]._messagetimeout);
                    if(((double)timespan / (double)_TIMEOUT_BASE_TIME_) > _g_sinkmessagequeue[index]._messagetimeoutlimit)
                    {
                        if(((double)timespan / (double)_TIMEOUT_BASE_TIME_) > _g_sinkmessagequeue[index]._messagedelay)
                        {                            
                            SinkFirstFound(&_g_sinkmessagequeue[index]);
                            _g_sinkmessagequeue[index]._messagetimeout = Timer_Tick_ReadCounter();
                            _g_sinkmessagequeue[index]._messagetimeoutmultiplier = 0;
                            _g_sinkmessagequeue[index]._messagedelay = 0x0000;
                            if(_g_sinkmessagequeue[index]._selfkill)
                                ReleaseSinkMessage(&_g_sinkmessagequeue[index]);
                        }

                    }
                    break;
                }
                case BROADCASTFOREVER:
                {
                    int32 timespan = ((Timer_Tick_ReadCounter() + _TIMEOUT_BASE_TIME_*_g_sinkmessagequeue[index]._messagetimeoutmultiplier) - _g_sinkmessagequeue[index]._messagetimeout);
                    if(((double)timespan / (double)_TIMEOUT_BASE_TIME_) > _g_sinkmessagequeue[index]._messagetimeoutlimit)
                    {
                        if(((double)timespan / (double)_TIMEOUT_BASE_TIME_) > _g_sinkmessagequeue[index]._messagedelay)
                        {                            
                            SinkBroadcastForever(&_g_sinkmessagequeue[index]);
                            _g_sinkmessagequeue[index]._messagetimeout = Timer_Tick_ReadCounter();
                            _g_sinkmessagequeue[index]._messagetimeoutmultiplier = 0;
                            _g_sinkmessagequeue[index]._messagedelay = 0x0000;
                            if(_g_sinkmessagequeue[index]._selfkill)
                                ReleaseSinkMessage(&_g_sinkmessagequeue[index]);
                        }
                    }
                    break;
                }
                case ALLINTERESTEDFOREVER:
                {
                    int32 timespan = ((Timer_Tick_ReadCounter() + _TIMEOUT_BASE_TIME_*_g_sinkmessagequeue[index]._messagetimeoutmultiplier) - _g_sinkmessagequeue[index]._messagetimeout);
                    if(((double)timespan / (double)_TIMEOUT_BASE_TIME_) > _g_sinkmessagequeue[index]._messagetimeoutlimit)
                    {
                        if(((double)timespan / (double)_TIMEOUT_BASE_TIME_) > _g_sinkmessagequeue[index]._messagedelay)
                        {                            
                            SinkAllInterested(&_g_sinkmessagequeue[index]);
                            _g_sinkmessagequeue[index]._messagetimeout = Timer_Tick_ReadCounter();
                            _g_sinkmessagequeue[index]._messagetimeoutmultiplier = 0;
                            _g_sinkmessagequeue[index]._messagedelay = 0x0000;
                            if(_g_sinkmessagequeue[index]._selfkill)
                                ReleaseSinkMessage(&_g_sinkmessagequeue[index]);
                        }
                    }
                             
                    break;
                }
                case NONE:
                    continue;
            }
        }
    }
}

///@void SinkBroadcast(SinkMessage *pmsg)
///@Created by: HIJH
///@Date: Septembre 2 de 2016
///@Broadcasts to all of the registered observers and forwards the message to all of its observer methods
void SinkBroadcast(PSINKMESSAGEPTR pmsg)
{
    uint8 index0 = 0, index1 = 0;
    for(index0 = 0; index0 < _SUBSCRIBER_QUEUE_DEPTH_; index0++)
        for(index1 = 0; index1 < _OBSERVED_IDENTIFIERS_; index1++)
        {
            if(_g_sinksubscriberqueue[index0]._arrmsgid[index1] == 0x00)
                break;
            
            if(_g_sinksubscriberqueue[index0].Callback[index1])
                _g_sinksubscriberqueue[index0].Callback[index1](pmsg);
            else
                break;
        }
}

///@void SinkFirstFound(SinkMessage *pmsg)
///@Created by: HIJH
///@Date: Septembre 2 de 2016
///@Invokes the first observer registered for the messageid and forwards the message to the matching observer method
void SinkFirstFound(PSINKMESSAGEPTR pmsg)
{
    bool exit = false;
    uint8 index0 = 0, index1 = 0;
    for(index0 = 0; index0 < _SUBSCRIBER_QUEUE_DEPTH_; index0++)
    {
        for(index1 = 0; index1 < _OBSERVED_IDENTIFIERS_; index1++)
        {
            if(_g_sinksubscriberqueue[index0]._arrmsgid[index1] == 0x00)
                break;
            
            if(_g_sinksubscriberqueue[index0]._arrmsgid[index1] == pmsg->_messageid)
            {
                if(_g_sinksubscriberqueue[index0].Callback[index1])
                    _g_sinksubscriberqueue[index0].Callback[index1](pmsg);
                    
                exit = true;
                break;
            }
        }
        if(exit)
            break;
    }
}

///@void SinkAllInterested(SinkMessage *pmsg)
///@Created by: HIJH
///@Date: Septembre 2 de 2016
///@Invokes all the observers registered for the messageid and forwards the message to the matching observer method
void SinkAllInterested(PSINKMESSAGEPTR pmsg)
{
    uint8 index0 = 0, index1 = 0;
    for(index0 = 0; index0 < _SUBSCRIBER_QUEUE_DEPTH_; index0++)
    {
        for(index1 = 0; index1 < _OBSERVED_IDENTIFIERS_; index1++)
        {
            if(_g_sinksubscriberqueue[index0]._arrmsgid[index1] == 0x00)
                break;
            
            if(_g_sinksubscriberqueue[index0]._arrmsgid[index1] == pmsg->_messageid)
            {
                if(_g_sinksubscriberqueue[index0].Callback[index1])
                    _g_sinksubscriberqueue[index0].Callback[index1](pmsg);
                
                break;
            }
        }
    }
}

///@void SinkBroadcastForever(SinkMessage *pmsg)
///@Created by: HIJH
///@Date: Septembre 2 de 2016
///@Invokes all the observers registered and forwards the message to all of them
void SinkBroadcastForever(PSINKMESSAGEPTR pmsg)
{
    uint8 index0 = 0, index1 = 0;
    for(index0 = 0; index0 < _SUBSCRIBER_QUEUE_DEPTH_; index0++)
    {
        for(index1 = 0; index1 < _OBSERVED_IDENTIFIERS_; index1++)
        {
            if(_g_sinksubscriberqueue[index0]._arrmsgid[index1] == 0x00)
                break;
            
            if(_g_sinksubscriberqueue[index0].Callback[index1])
                _g_sinksubscriberqueue[index0].Callback[index1](pmsg);
        }
    }
}

//@Created By: HIJH
//@Septembre de 2016
void SinkCounterUpdateCallback(void *pdata)
{
    FOR(uint8 index = 0, index < _MESSAGE_QUEUE_DEPTH_, index++)
    {        
        SinkMessage *pmsg = &_g_sinkmessagequeue[index];
        if(pmsg->_messagetype == BROADCASTFOREVER || 
            pmsg->_messagetype == FIRSTFOUNDFOREVER ||
            pmsg->_messagetype == ALLINTERESTEDFOREVER ||
            pmsg->_messagetype == DELAYEDALLINTERESTED)
            pmsg->_messagetimeoutmultiplier++;
    }
}

//@Created By: HIJH
//@Septembre de 2016
void SinkReleaseByIDCallback(void *pparam)
{
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    ReleaseSinkMessageByID(pmsg->_buffer[0]);
}

//@Created By: HIJH
//@Septembre de 2016
void SinkReleaseByPtrCallback(void *pparam)
{
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    ReleaseSinkMessage((PSINKMESSAGEPTR)pmsg->_pdata);
}

/* [] END OF FILE */
