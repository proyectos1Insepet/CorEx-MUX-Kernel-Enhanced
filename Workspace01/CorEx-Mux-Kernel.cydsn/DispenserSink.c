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

#include "Dispenser.h"

extern PumpResponseContext _g_respctxt;

uint16 _g_dispenserupdatetimeout = 0x0000;
uint16 _g_dispenserupdatetimeoutmultiplier = 0x0000;

void InitProtocol();
void DispenserHandler();
void DispenserTimeoutHandler();
void DispenserUpdateTimeouts(void *pparam);
void DispenserDataReceptionHandler();

//Messages coming from the Displays
void DispenserSide1Listener(void *pparam);
void DispenserSide2Listener(void *pparam);
//Invoked once on MUX startup. This call is delayed a few seconds to allow for the general initialization
void DispenserEnumeratePumps(LPVOID pparam);
void DispenserLoadEEPROMSettings(LPVOID pparam);
void DispenserPriceRestore(LPVOID pparam);
void DispenserRestorePrices(LPVOID pparam);
void DispenserUnlockPumpsOnEOT(LPVOID pparam);
void DispenserAcquireConfiguration(LPVOID pparam);
void DispenserConfigurePrices(LPVOID pparam);
void DispenserConfigureSinglePrice(LPVOID pparam);
void DispenserAcquireTotals(LPVOID pparam);
void DispenserDaylyRestart(LPVOID pparam);

void DispenserSinkRegistration()
{
    InitProtocol();
    
    PSINKSUBSCRIBERPTR psubscriber = AllocateSubscriberSlot();
    if(psubscriber)
    {
        uint8 index = 0;
        //The maximum number of observable identifiers is _OBSERVED_IDENTIFIERS_
        //located in the header file "SinkManager.h"
        //Every identifier is paired with its respective Callback method pointer
        
        psubscriber->_arrmsgid[index] = DISPLAY1_MESSAGE;
        psubscriber->Callback[index++] = DispenserSide1Listener;

        psubscriber->_arrmsgid[index] = DISPLAY2_MESSAGE;
        psubscriber->Callback[index++] = DispenserSide2Listener;

        psubscriber->_arrmsgid[index] = DISPENSER_UPDATE_PUMP_TRANSACTION_TIMEOUTS;
        psubscriber->Callback[index++] = DispenserUpdateTimeouts;

        psubscriber->_arrmsgid[index] = DISPENSER_ACQUIRE_PUMP_STATE;
        psubscriber->Callback[index++] = DispenserUpdatePumpState;

        psubscriber->_arrmsgid[index] = DISPENSER_LOAD_EEPROM_SETTINGS;
        psubscriber->Callback[index++] = DispenserLoadEEPROMSettings;

    }

    psubscriber = AllocateSubscriberSlot();
    if(psubscriber)
    {
        uint8 index = 0;
        
        psubscriber->_arrmsgid[index] = DISPENSER_ENUMERATE_VISIBLE_PUMPS;
        psubscriber->Callback[index++] = DispenserEnumeratePumps;
        
        psubscriber->_arrmsgid[index] = DISPENSER_ACQUIRE_CONFIGURATION_INFORMATION;
        psubscriber->Callback[index++] = DispenserAcquireConfiguration;
        
        psubscriber->_arrmsgid[index] = DISPENSER_CONFIGURE_PRICES;
        psubscriber->Callback[index++] = DispenserConfigurePrices;
        
        psubscriber->_arrmsgid[index] = DISPENSER_CONFIGURE_SINGLE_PRICE;
        psubscriber->Callback[index++] = DispenserConfigureSinglePrice;
        
        psubscriber->_arrmsgid[index] = DISPENSER_ACQUIRE_TOTALS;
        psubscriber->Callback[index++] = DispenserAcquireTotals;

        psubscriber->_arrmsgid[index] = DISPENSER_RESTORE_PRICES;
        psubscriber->Callback[index++] = DispenserRestorePrices;
        
        psubscriber->_arrmsgid[index] = DISPENSER_PUMP_HANDLER;
        psubscriber->Callback[index++] = DispenserHandler;
    }    
    //This perpetual call is in charge of updating the global pump state array
    //so the different modules could inquiry on those states
    //This one-shot call is in charge of loading the current dispenser configuration regarding
    //money, volume and ppu decimal formats, pump layout, pump model, display type, volume unit etc.

    #ifdef FAST_STARTUP
    _ALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
    if(psinkmsg)
    {
        psinkmsg->_messageid = DISPENSER_ENUMERATE_VISIBLE_PUMPS;
        psinkmsg->_messagedelay = _SINK_TIMEOUT_10S_;
        psinkmsg->_messagetype = DELAYEDALLINTERESTED;
        psinkmsg->_messagestate = SINK_BUSY;
    }    
    _REALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
    if(psinkmsg)
    {
        psinkmsg->_messageid = DISPENSER_ACQUIRE_CONFIGURATION_INFORMATION;
        psinkmsg->_messagedelay = _SINK_TIMEOUT_15S_;
        psinkmsg->_messagetype = DELAYEDALLINTERESTED;
        psinkmsg->_messagestate = SINK_BUSY;
    }
    _REALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
    if(psinkmsg)
    {
        psinkmsg->_messageid = WATCHDOG_STARTUP;
        psinkmsg->_messagedelay = _SINK_TIMEOUT_20S_;
        psinkmsg->_messagetype = DELAYEDALLINTERESTED;
        psinkmsg->_messagestate = SINK_BUSY;
    }
    _REALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
    if(psinkmsg)
    {
        psinkmsg->_messageid = DISPENSER_ACQUIRE_PUMP_STATE;
        psinkmsg->_messagedelay = _SINK_TIMEOUT_20S_;
        #ifndef ADVANTAGE_PROTOCOL
        psinkmsg->_messagetimeoutlimit = _SINK_TIMEOUT_250MS_;
        #endif
        #ifdef ADVANTAGE_PROTOCOL
        psinkmsg->_messagetimeoutlimit = _SINK_TIMEOUT_500MS_;
        #endif
        psinkmsg->_messagetype = FIRSTFOUNDFOREVER;
        psinkmsg->_messagestate = SINK_BUSY;
    }
    #endif
    #ifdef SLOW_STARTUP
    _ALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
    if(psinkmsg)
    {
        psinkmsg->_messageid = DISPENSER_ENUMERATE_VISIBLE_PUMPS;
        psinkmsg->_messagedelay = _SINK_TIMEOUT_40S_;
        psinkmsg->_messagetype = DELAYEDALLINTERESTED;
        psinkmsg->_messagestate = SINK_BUSY;
    }
    
    _REALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
    if(psinkmsg)
    {
        psinkmsg->_messageid = DISPENSER_ACQUIRE_CONFIGURATION_INFORMATION;
        psinkmsg->_messagedelay = _SINK_TIMEOUT_45S_;
        psinkmsg->_messagetype = DELAYEDALLINTERESTED;
        psinkmsg->_messagestate = SINK_BUSY;
    }
    _REALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
    if(psinkmsg)
    {
        psinkmsg->_messageid = WATCHDOG_STARTUP;
        psinkmsg->_messagedelay = _SINK_TIMEOUT_50S_;
        psinkmsg->_messagetype = DELAYEDALLINTERESTED;
        psinkmsg->_messagestate = SINK_BUSY;
    }
    _REALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
    if(psinkmsg)
    {
        psinkmsg->_messageid = DISPENSER_ACQUIRE_PUMP_STATE;
        psinkmsg->_messagedelay = _SINK_TIMEOUT_50S_;
        psinkmsg->_messagetimeoutlimit = _SINK_TIMEOUT_250MS_;
        psinkmsg->_messagetype = FIRSTFOUNDFOREVER;
        psinkmsg->_messagestate = SINK_BUSY;
    }
    #endif

    _REALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
    if(psinkmsg)
    {
        psinkmsg->_messageid = DISPENSER_PUMP_HANDLER;
        psinkmsg->_messagedelay = 0x0000;
        #ifndef ADVANTAGE_PROTOCOL
        psinkmsg->_messagetimeoutlimit = _SINK_TIMEOUT_20MS_;
        #endif
        #ifdef ADVANTAGE_PROTOCOL
        psinkmsg->_messagetimeoutlimit = _SINK_TIMEOUT_30MS_;
        #endif
        psinkmsg->_messagetype = FIRSTFOUNDFOREVER;
        psinkmsg->_messagestate = SINK_BUSY;
    }
            
    _g_dispenserupdatetimeout = Timer_DispenserState_ReadCounter();
    
    GLOBAL_LOOP_PUSH(DispenserTimeoutHandler);
    GLOBAL_LOOP_PUSH(DispenserDataReceptionHandler);
}

//@Create by: HIJH
//@Date: Septembre de 2016
//This handler checks the transactional queue for pending job requests to execute.
//Once a job is executed, the response is awaited (if required) and monitored for 
//timeouts and retries. It also validates a set of pre-conditions before
//executing the job.
void DispenserHandler()
{
    PPUMPTRANSACTIONJOBCONTEXTPTR pjob = _g_pumpjobqueue.Peek(&_g_pumpjobqueue);
    if(pjob && pjob->_prequest)
    {
        bool oneshot = false;
        bool preconditions = false;
        bool precondexecuted = false;
        
        _g_respctxt._bufferready = false;
        _g_respctxt._responsesize = 0x00;
        memset(_g_respctxt._rxbuffer, 0x00, 512);
        memset(pjob->_ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        
        FOR(uint8 index = 0, index < _MAX_CONDITION_CODES_, index++)
        {
            if(pjob->_prequest->_preconditions[index] != 0xFF)
            {
                //If the precondition is marked as "One Shot", it will be evaluated only once
                preconditions = true;
                if(!oneshot)
                    oneshot = pjob->_prequest->_oneshotpreconditions[index];
                    
                //If the condition is met then execute the Request
                if(pjob->_ppump->_pumpstate == pjob->_prequest->_preconditions[index])                
                {
                    if(pjob->_prequest->Initialization)
                        pjob->_prequest->Initialization(pjob->_ppump);                    
                    
                    if(pjob->_prequest->Response)
                    {
                        _g_respctxt._pparam = pjob->_ppump;
                        ///////////////////////////////////////////////////////////////////
                        //This variable holds the expected buffer size for the response
                        if(pjob->_prequest->_responsesize > 0)
                            _g_respctxt._responsesize = pjob->_prequest->_responsesize;
                        else
                            _g_respctxt.ResponseSizeResolver = pjob->_prequest->ResponseSizeResolver;
                        ///////////////////////////////////////////////////////////////////
                        pjob->_prequest->Request(pjob->_ppump);
                        pjob->_presponse = pjob->_prequest;
                        pjob->_prequest = NULL;
                        
                        //Initialize the reference tick if it has not yet been initialized
                        pjob->_timeoutseed = Timer_DispenserState_ReadCounter();
                        
                    }else{//Dequeue since no response is required for this request
                        pjob->_prequest->Request(pjob->_ppump);
                        if(pjob->Callback)
                            pjob->Callback(pjob->_pdata);
                        
                        _g_pumpjobqueue.Dequeue(&_g_pumpjobqueue);
                        pjob = NULL;
                    }
                    precondexecuted = true;
                    break;
                }
            }else
                break;
        }
        if(!preconditions)
        {
            if(pjob->_prequest->Initialization)
                pjob->_prequest->Initialization(pjob->_ppump);                    
            
            if(pjob->_prequest->Response)
            {
                _g_respctxt._pparam = pjob->_ppump;
                ///////////////////////////////////////////////////////////////////
                //This variable holds the expected buffer size for the response
                if(pjob->_prequest->_responsesize > 0)
                    _g_respctxt._responsesize = pjob->_prequest->_responsesize;
                else
                    _g_respctxt.ResponseSizeResolver = pjob->_prequest->ResponseSizeResolver;
                ///////////////////////////////////////////////////////////////////
                pjob->_prequest->Request(pjob->_ppump);
                pjob->_presponse = pjob->_prequest;
                pjob->_prequest = NULL;

                //Initialize the reference tick if it has not yet been initialized
                pjob->_timeoutseed = Timer_DispenserState_ReadCounter();
                
            }else{//Since no response is required for this request!!
                pjob->_prequest->Request(pjob->_ppump);
                if(pjob->Callback)
                    pjob->Callback(pjob->_pdata);
                    
                _g_pumpjobqueue.Dequeue(&_g_pumpjobqueue);
                pjob = NULL;
            }
        }
        else if(!precondexecuted)
        {
            //If the precondition is marked as "One Shot", it will be evaluated only once
            if(oneshot)
            {
                _g_pumpjobqueue.Dequeue(&_g_pumpjobqueue);
                pjob = NULL;
            }
            else
            {
                //If none of the preconditions are met, then "Re-push" until there is a match
                //This is pretty useful for the "EOT" state since once the delivery starts, 
                //the transaction needs to wait for the "EOT" or "IDLE" condition on the pump.
                _g_pumpjobqueue.Rotate(&_g_pumpjobqueue);
            }
        }
    }
}

//@Create by: HIJH
//@Date: Septembre de 2016
//This handler checks the transactional queue for pending job requests to execute.
//Once a job is executed, the response is awaited (if required) and monitored for 
//timeouts and retries. The handler also validates a set of pre conditions to 
//execute the job.
void DispenserTimeoutHandler()
{
    PPUMPTRANSACTIONJOBCONTEXTPTR pjob = _g_pumpjobqueue.Peek(&_g_pumpjobqueue);
    //Timeout handling
    if(pjob && pjob->_presponse)
    {
        if(pjob->_presponse->_responsetimeoutlimit != _NO_TIMEOUT_)
        {
            int32 timespan = ((Timer_DispenserState_ReadCounter() + _TIMEOUT_BASE_TIME_*pjob->_timeoutmultiplier) - pjob->_timeoutseed);
            if(((double)timespan / (double)_TIMEOUT_BASE_TIME_) > pjob->_presponse->_responsetimeoutlimit)
            {
                ///TURNING IT INTO A REQUEST AGAIN FOR A LIMITED NUMBER OF RETRIES!
                pjob->_timeoutmultiplier = 0x0000;
                pjob->_timeoutseed = 0x0000;
                
                pjob->_retriescounter++;
                //We invoke the timeout Callback if it has been defined
                if(pjob->_presponse->TimeoutCallback)
                    pjob->_presponse->TimeoutCallback(pjob);

                if(pjob->_retriescounter > _MAX_RETRIES_){
                    _g_pumpjobqueue.Dequeue(&_g_pumpjobqueue);
                    pjob = NULL;
                }else{
                    pjob->_prequest = pjob->_presponse;
                    pjob->_presponse = NULL;
                }
                
                Dispenser_ClearRxBuffer();
                Dispenser_Enable();
            }
        }
    }
}

//@Create by: HIJH
//@Date: Septembre de 2016
//This handler receives the responses from the pump to job requests on top of the queue.
//Once a job is executed, the response is awaited (if required) and monitored for 
//timeouts and retries. The handler also validates a set of post conditions to 
//answer the job.
//void DispenserDataReceptionHandler(void *pparam)
void DispenserDataReceptionHandler()
{
    if(!_g_respctxt._bufferready)
        return;

    //SinkMessage *psinkmsg = (SinkMessage*)pparam;
    /////////////////////////////////////////////////////////////////////////////////////////
    //TODO: PLACE HERE ALL THE NECESSARY CODE TO RESOLVE THE COMPLETENESS OF THE ARRIVED STREAM
    /////////////////////////////////////////////////////////////////////////////////////////
    PPUMPTRANSACTIONJOBCONTEXTPTR pjob = _g_pumpjobqueue.Peek(&_g_pumpjobqueue);
    if(pjob && pjob->_presponse)
    {
        pjob->_timeoutmultiplier = 0x0000;
        pjob->_timeoutseed = 0x0000;
        memcpy(pjob->_ppump->_rxbuffer, _g_respctxt._rxbuffer, _g_respctxt._responsesize);
        pjob->_ppump->_rxbuffersize = _g_respctxt._responsesize;
        
        bool oneshot = false;
        bool postconditions = false;
        bool postcondexecuted = false;
        FOR(uint8 index = 0, index < _MAX_CONDITION_CODES_, index++)
        {
            //If none the post conditions are met, then this job item MUST timeout
            //so it will be launched again for a predefined number of retries (see the code in the "DispenserSink" file)
            if(pjob->_presponse->_postconditions[index] != 0xFF)
            {
                postconditions = true;
                if(!oneshot)
                    oneshot = pjob->_presponse->_oneshotpostconditions[index];
                 
                pjob->_presponse->Response(pjob);
                //If the condition is met then execute the response Callback
                if(pjob->_ppump->_pumpstate == pjob->_presponse->_postconditions[index])
                {
                    if(!pjob->_reenqueue)
                    {
                        if(pjob->_presponse->Finalization)
                            pjob->_presponse->Finalization(pjob);
                            
                        if(pjob->Callback)
                            pjob->Callback(pjob->_pdata);
                    }
                    
                    if(!pjob->_reenqueue)
                    {
                        _g_pumpjobqueue.Dequeue(&_g_pumpjobqueue);
                        pjob = NULL;
                    }else{
                        pjob->_prequest = pjob->_presponse;
                        pjob->_presponse = NULL;
                    }
                    postcondexecuted = true;
                    break;
                }
            }else
                break;
        }
        if(!postconditions)
        {                
            pjob->_presponse->Response(pjob);
            
            if(!pjob->_reenqueue)
            {
                if(pjob->_presponse->Finalization)
                    pjob->_presponse->Finalization(pjob);
                
                if(pjob->Callback)
                    pjob->Callback(pjob->_pdata);
            }
            
            //This is required for the enumeration model
            if(!pjob->_reenqueue)
            {
                _g_pumpjobqueue.Dequeue(&_g_pumpjobqueue);
                pjob = NULL;
            }else{
                pjob->_prequest = pjob->_presponse;
                pjob->_presponse = PNEAR_NULLPTR;
            }
        }
        else if(!postcondexecuted)
        {
            if(oneshot)
            {
                _g_pumpjobqueue.Dequeue(&_g_pumpjobqueue);
                pjob = NULL;
            }
            else
            {
                if(pjob->_presponse->_retryinsteadofrepush)
                {
                    if(pjob->_repushretriescounter >= _MAX_RETRIES_)
                    {
                        _g_pumpjobqueue.Dequeue(&_g_pumpjobqueue);
                        pjob = NULL;
                    }
                    else
                    {
                        pjob->_repushretriescounter++;
                        pjob->_prequest = pjob->_presponse;
                        pjob->_presponse = NULL;
                    }
                }
                else
                {
                    //If none of the postconditions are met, then "Re-push" until there is a match
                    //This is pretty useful for the "EOT" state since once the delivery starts, 
                    //the transaction needs to wait for the "EOT" or "IDLE" condition on the pump.
                    if(pjob->_repushtimeoutseed == 0x0000)
                        pjob->_repushtimeoutseed = Timer_DispenserState_ReadCounter();

                    //This pointer is NULL but just for the sake of clarity
                    pjob->_prequest = pjob->_presponse;
                    pjob->_presponse = PNEAR_NULLPTR;
                    _g_pumpjobqueue.Rotate(&_g_pumpjobqueue);

                }
            }
        }
    }
    //CHANGE BY HIJH
    //NOV 18 2016
    //PLACEMENT OF THIS BLOCK OUT OF THE MAIN IF
    _g_respctxt._bufferready = false;
    _g_respctxt._responsesize = 0x00;
    memset(_g_respctxt._rxbuffer, 0x00, 512);
    memset(pjob->_ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
    
    Dispenser_ClearRxBuffer();
    Dispenser_Enable();
}

//This updater is invoked every millisecond, so the multiplier counts per elapsed millisecond
void DispenserUpdateTimeouts(void *pparam)
{
    _g_dispenserupdatetimeoutmultiplier++;
    //Update of the transaction timeouts every 10 milliseconds
        
    PPUMPTRANSACTIONJOBCONTEXTPTR pjob = _g_pumpjobqueue.Peek(&_g_pumpjobqueue);
    if(pjob && pjob->_presponse)
    {
        if(pjob->_presponse->_responsetimeoutlimit != _NO_TIMEOUT_)
            pjob->_timeoutmultiplier++;
            
        if(pjob->_presponse->_repushtimeoutlimit != _NO_TIMEOUT_)
            pjob->_repushtimeoutmultiplier++;
    }
        
    _g_dispenserupdatetimeout = Timer_DispenserState_ReadCounter();
    _g_dispenserupdatetimeoutmultiplier = 0x0000;   
}
