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
@Date: Septembre 3 de 2016

*/
#include <project.h>
#include "GlobalDefinitions.h"
#include "Dispenser.h"
#include "RFLink.h"

#ifdef PRIME_PROTOCOL
    
PumpJobQueue _g_pumpjobqueue;
Pump _g_pumps[_MAX_NUMBER_OF_PUMPS_];
//This structure holds the entire configuration information of the dispenser acquired through SFs
DispenserLayoutInfo _g_dispenserlayoutinfo;

void PumpJobRotate(void *this);
PumpTransactionJobContext *PumpJobAllocate(void *this); 
void PumpJobEnqueue(void *this, PumpTransactionJobContext *pjob);
PumpTransactionJobContext *PumpJobQueuePeek(void *this); 
void PumpJobDequeue(void *this); 
void PumpJobQueueClear(void *this);
bool PumpIsInValidState(void *this);
void PumpTransactionalStateEnqueue(LPVOID pvthis, PNEAR_PUMPTRANSACTIONALSTATEPTR pts);
void PumpTransactionalStateDeallocate(LPVOID pvthis, PNEAR_PUMPTRANSACTIONALSTATEPTR pts);
PNEAR_PUMPTRANSACTIONALSTATEPTR PumpTransactionalStateAllocate(LPVOID pvthis);
PNEAR_PUMPTRANSACTIONALSTATEPTR PumpTransactionalStateDequeue(LPVOID pvthis);
void PumpTransactionalStateLock(LPVOID pvthis);
void PumpTransactionalStateUnlock(LPVOID pvthis);
bool PumpTransactionalStateFind(LPVOID pvthis, uint8 transtate);
void PumpCleanUp(LPVOID pparam);

//@Created By: HIJH
//@Septembre de 2016
void InitProtocol()
{
    _g_dispenserlayoutinfo._inconfiguration = true;
    _g_dispenserlayoutinfo._currentloopopen = false;
    
    FOR(uint8 index = 0, index < _MAX_NUMBER_OF_PUMPS_, index++)
    {
        _g_pumps[index]._pumpid = 0x00;
        _g_pumps[index]._pumpindex = index;
        _g_pumps[index]._pumpstate = PUMP_IDLE;
        _g_pumps[index]._currenthose = HOSEUNKNOWN;
        _g_pumps[index]._transhealth = _PUMP_OK_;
        _g_pumps[index]._transtate = _PUMP_IDLE_;
        _g_pumps[index]._pumprftransstate = RF_IDLE;
        _g_pumps[index]._pumplocked = false;
        _g_pumps[index]._restoreprice = false;
        _g_pumps[index]._print = false;
        _g_pumps[index]._acquiringstate = false;
        if(index == 0)
            _g_pumps[index]._roundrobinstate = _PUMP_RR_ACTIVE_;
        else
            _g_pumps[index]._roundrobinstate = _PUMP_RR_INACTIVE_;
        
        FOR(uint8 transindex = 0, transindex < _PUMP_MAX_QUEUE_TRANSTATES_, transindex++)
        {
            _g_pumps[index]._transqueue[transindex]._state = _PUMP_TRANSTATE_FREE_;
            _g_pumps[index]._transqueue[transindex]._transtate = _PUMP_POSITION_IDLE_;
            _g_pumps[index]._transqueue[transindex]._buffersize = 0;
            _g_pumps[index]._transqueue[transindex]._transtate = 0x00;
            _g_pumps[index].CleanUp = PumpCleanUp;
        }
        
        memset(&_g_pumps[index]._authorizationinfo, 0x00, sizeof(PumpAuthorizationInfo));

        _g_pumps[index].PumpTransQueueLock = PumpTransactionalStateLock;
        _g_pumps[index].PumpTransQueueUnlock = PumpTransactionalStateUnlock;
        
        _g_pumps[index].PumpTransQueueFind = PumpTransactionalStateFind;
        _g_pumps[index].PumpTransQueueEnqueue = PumpTransactionalStateEnqueue;
        _g_pumps[index].PumpTransQueueDequeue = PumpTransactionalStateDequeue;
        _g_pumps[index].PumpTransQueueAllocate = PumpTransactionalStateAllocate;
        _g_pumps[index].PumpTransQueueDeallocate = PumpTransactionalStateDeallocate;

        _g_pumps[index].PumpValidState = PumpIsInValidState;
    }

    _g_pumpjobqueue.Clear = PumpJobQueueClear;
    _g_pumpjobqueue.Allocate = PumpJobAllocate;
    _g_pumpjobqueue.Enqueue = PumpJobEnqueue;
    _g_pumpjobqueue.Peek = PumpJobQueuePeek;
    _g_pumpjobqueue.Dequeue = PumpJobDequeue;
    _g_pumpjobqueue.Rotate = PumpJobRotate;
    
    _g_pumpjobqueue.Clear(&_g_pumpjobqueue);
    
}

//@Created By: HIJH
//@Septembre de 2016
bool PumpIsInValidState(void *this)
{
    bool retval = false;
    PNEAR_PUMPPTR pthis = (PNEAR_PUMPPTR)this;
    switch(pthis->_pumpstate)
    {
        case PUMP_IDLE:
        case PUMP_CALLING:
        case PUMP_AUTHORIZED:
        case PUMP_BUSY:
        case PUMP_PEOT:
        case PUMP_FEOT:
        case PUMP_STOPPED:
        case PUMP_FAIL:
            retval = true;
            break;
    }
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
void PumpJobQueueClear(void *this)
{    
    PumpJobQueue *pthis = (PumpJobQueue*)this;    
    FOR(uint8 index = 0, index < _PUMP_JOB_QUEUE_SIZE_, index++)
    {
        pthis->_jobqueueheap[index]._state = QUEUE_JOB_FREE;
        pthis->_jobqueueheap[index]._ppump = NULL;
        pthis->_jobqueueheap[index]._prequest = NULL;
        pthis->_jobqueueheap[index]._presponse = NULL;
        
        pthis->_pjobqueueallocated[index] = NULL;
    }
    pthis->_queueindex = 0;
}

//@Created By: HIJH
//@Septembre de 2016
PPUMPTRANSACTIONJOBCONTEXTPTR PumpJobAllocate(void *this)
{    
    PPUMPTRANSACTIONJOBCONTEXTPTR pretval = NULL;
    PumpJobQueue *pthis = (PumpJobQueue*)this;
        
    FOR(uint8 index = 0, index < _PUMP_JOB_QUEUE_SIZE_, index++)
    {
        if(pthis->_jobqueueheap[index]._state == QUEUE_JOB_FREE)
        {
            pthis->_jobqueueheap[index]._state = QUEUE_JOB_ALLOCATED;
            pthis->_jobqueueheap[index]._delay = 0x0000;
            pthis->_jobqueueheap[index]._delaymultiplier = 0x0000;

            pthis->_jobqueueheap[index]._repushretriescounter = 0x0000;
            pthis->_jobqueueheap[index]._repushtimeoutseed = 0x0000;
            pthis->_jobqueueheap[index]._repushtimeoutmultiplier = 0x0000;
            
            pthis->_jobqueueheap[index]._retriescounter = 0x0000;
            pthis->_jobqueueheap[index]._timeoutseed = 0x0000;
            pthis->_jobqueueheap[index]._timeoutmultiplier = 0x0000;
            
            pthis->_jobqueueheap[index]._reenqueue = false;

            pthis->_jobqueueheap[index].Callback = PNEAR_NULLPTR;
            pthis->_jobqueueheap[index]._pdata = PNEAR_NULLPTR;
            
            pretval = &(pthis->_jobqueueheap[index]);
            break;
        }
    }
    return pretval;
}

//@Created By: HIJH
//@Septembre de 2016
void PumpJobEnqueue(void *this, PPUMPTRANSACTIONJOBCONTEXTPTR pjob)
{
    PumpJobQueue *pthis = (PumpJobQueue*)this;
    
    if(pthis->_queueindex < _PUMP_JOB_QUEUE_SIZE_)
    {
        pthis->_pjobqueueallocated[pthis->_queueindex] = pjob;
        pthis->_pjobqueueallocated[pthis->_queueindex]->_state = QUEUE_JOB_BUSY;
        pthis->_queueindex++;
    }
}

//@Created By: HIJH
//@Septembre de 2016
PPUMPTRANSACTIONJOBCONTEXTPTR PumpJobQueuePeek(void *this)
{   
    PPUMPTRANSACTIONJOBCONTEXTPTR pretval = NULL;
    PumpJobQueue *pthis = (PumpJobQueue*)this;
        
    if(pthis->_queueindex > 0)
        pretval = pthis->_pjobqueueallocated[0];
        
    return pretval;
}

//@Created By: HIJH
//@Septembre de 2016
void PumpJobDequeue(void *this)
{    
    PumpJobQueue *pthis = (PumpJobQueue*)this;
    
    if(pthis->_queueindex > 0)
    {
        pthis->_pjobqueueallocated[0]->_state = QUEUE_JOB_FREE;
        pthis->_pjobqueueallocated[0]->_ppump = NULL;
        pthis->_pjobqueueallocated[0]->_prequest = NULL;
        pthis->_pjobqueueallocated[0]->_presponse = NULL;
        FOR(uint8 index = 1, index < pthis->_queueindex, index++)
            pthis->_pjobqueueallocated[index - 1] = pthis->_pjobqueueallocated[index];

        FOR(index = (pthis->_queueindex - 1), index < _PUMP_JOB_QUEUE_SIZE_, index++)
            pthis->_pjobqueueallocated[index] = NULL;
            
        pthis->_queueindex--;
    }
}

//@Created By: HIJH
//@Septembre de 2016
void PumpJobRotate(void *this)
{    
    PumpJobQueue *pthis = (PumpJobQueue*)this;
    
    if(pthis->_queueindex > 0)
    {
        PPUMPTRANSACTIONJOBCONTEXTPTR ptopjob = pthis->_pjobqueueallocated[0x00];
        
        FOR(uint8 index = 1, index < pthis->_queueindex, index++)
            pthis->_pjobqueueallocated[index - 1] = pthis->_pjobqueueallocated[index];

        pthis->_pjobqueueallocated[pthis->_queueindex - 1] = ptopjob;
    }
}

//@Created By: HIJH
//@Septembre de 2016
PNEAR_PUMPTRANSACTIONALSTATEPTR PumpTransactionalStateDequeue(LPVOID pvthis)
{
    PNEAR_PUMPPTR pthispump = (PNEAR_PUMPPTR)pvthis;
    PNEAR_PUMPTRANSACTIONALSTATEPTR pretval = PNEAR_NULLPTR;
    
    FOR(uint8 index = 0, index < _PUMP_MAX_QUEUE_TRANSTATES_, index++)
    {
        if(pthispump->_transqueue[index]._state == _PUMP_TRANSTATE_BUSY_)
        {
            pretval = &(pthispump->_transqueue[index]);
            break;
        }
    }
    return pretval;
}

//@Created By: HIJH
//@Septembre de 2016
void PumpTransactionalStateEnqueue(LPVOID pvthis, PNEAR_PUMPTRANSACTIONALSTATEPTR pts)
{
    PNEAR_PUMPPTR pthispump = (PNEAR_PUMPPTR)pvthis;
    pts->_state = _PUMP_TRANSTATE_BUSY_;    
}

//@Created By: HIJH
//@Septembre de 2016
void PumpTransactionalStateDeallocate(LPVOID pvthis, PNEAR_PUMPTRANSACTIONALSTATEPTR pts)
{
    PNEAR_PUMPPTR pthispump = (PNEAR_PUMPPTR)pvthis;
    
    pts->_state = _PUMP_TRANSTATE_FREE_;
    
    FOR(uint8 index = 0, index < (_PUMP_MAX_QUEUE_TRANSTATES_ - 1), index++)
    {
        if(pthispump->_transqueue[index]._state == _PUMP_TRANSTATE_FREE_)
        {
            memcpy(&(pthispump->_transqueue[index]), &(pthispump->_transqueue[index + 1]), sizeof(PumpTransactionalState));
            pthispump->_transqueue[index + 1]._state = _PUMP_TRANSTATE_FREE_;
        }
    }
    
}

//@Created By: HIJH
//@Septembre de 2016
bool PumpTransactionalStateFind(LPVOID pvthis, uint8 transtate)
{
    bool retval = false;
    PNEAR_PUMPPTR pthispump = (PNEAR_PUMPPTR)pvthis;
    FOR(uint8 index = 0, index < _PUMP_MAX_QUEUE_TRANSTATES_, index++)
    {
        if(pthispump->_transqueue[index]._transtate == transtate)
        {
            retval = true;
            break;
        }
    }
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
PNEAR_PUMPTRANSACTIONALSTATEPTR PumpTransactionalStateAllocate(LPVOID pvthis)
{
    PNEAR_PUMPPTR pthispump = (PNEAR_PUMPPTR)pvthis;
    PNEAR_PUMPTRANSACTIONALSTATEPTR pretval = PNEAR_NULLPTR;

    //IF A SLOT COULDN'T BE ALLOCATED, THEN THE STATE WILL BE LOST!! 
    //THIS REQUIRES A FAST POLLING FROM THE REMOTE PEER
    //TO PICK UP CAUSAL STATES QUICKLY!!
    FOR(uint8 index = 0, index < _PUMP_MAX_QUEUE_TRANSTATES_, index++)
    {
        if(pthispump->_transqueue[index]._state == _PUMP_TRANSTATE_FREE_)
        {
            memset(pthispump->_transqueue[index]._buffer, 0x00, _PUMP_TRANSTATE_BUFFER_SIZE_);
            pthispump->_transqueue[index]._state = _PUMP_TRANSTATE_ALLOCATED_;
            pretval = &(pthispump->_transqueue[index]);
            break;
        }
    }
    return pretval;
}

void PumpTransactionalStateLock(LPVOID pvthis)
{
    PNEAR_PUMPPTR pthispump = (PNEAR_PUMPPTR)pvthis;
    do
    {
        //This is to Desynchronize REENTRANT calls
        CyDelayUs(5*(uint16)((double)rand()/(double)RAND_MAX) + 1);
    }
    while(pthispump->_transqueuelocked);
    //Transactional Queue Lock to avoid concurrent access to the Queue
    pthispump->_transqueuelocked = true;
}

void PumpTransactionalStateUnlock(LPVOID pvthis)
{
    PNEAR_PUMPPTR pthispump = (PNEAR_PUMPPTR)pvthis;
    pthispump->_transqueuelocked = false;
}
    

//@Created By: HIJH
//@Septembre de 2016
void PumpCleanUp(LPVOID pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    
    ppump->_datacontainer._var0 = 0x00;
    ppump->_datacontainer._var1 = 0x00;
    ppump->_datacontainer._var2 = 0x0000;
    ppump->_datacontainer._var3 = 0x0000;
    ppump->_datacontainer._var4 = 0x00000000;
    ppump->_datacontainer._var5 = 0x00000000;
    ppump->_datacontainer._var6 = 0x0000000000000000;
    ppump->_datacontainer._var7 = 0x0000000000000000;
    ppump->_datacontainer._var8 = 0x0000000000000000;
    ppump->_datacontainer._var9 = 0x0000000000000000;
    ppump->_datacontainer._varA = 0.0;
    ppump->_datacontainer._varB = 0.0;
}

#endif

/* [] END OF FILE */
