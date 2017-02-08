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
#include "Configuration.h"
#include "GlobalDefinitions.h"
#include "SinkManager.h"
#include "SinkObservables.h"
#include "Dispenser.h"
#include "Display.h"
#include "Printer.h"
#include "Eeprom.h"
#include "RFLink.h"
#include "Rtc.h"

void DispenserRestorePrices(LPVOID pparam);
void DispenserUnlockPumpsOnEOT(LPVOID pparam);
void DispenserLoadEEPROMSettings(LPVOID pparam);
PumpTransaction *GetTransactionByName(uint8 codename);

PumpCodeMapping _g_pumpcodemap[] = 
{
    { SIDE1, POSITIONA, &_g_pumps[0] },//This code comes from the screen code when selecting the pump (position)
    { SIDE1, POSITIONB, &_g_pumps[2] },//This code comes from the screen code when selecting the pump (position)
    { SIDE2, POSITIONA, &_g_pumps[3] },//This code comes from the screen code when selecting the pump (position)
    { SIDE2, POSITIONB, &_g_pumps[1] },//This code comes from the screen code when selecting the pump (position)
    { 0xFF,  0xFF, NULL }
};

PNEARDISPENSERACTIVATEPROGRAMMING _g_ptractprogramming = PNEAR_NULLPTR;

//@Create by: HIJH
//@Date: Septembre de 2016
//This method handles all of the incoming data that has been capture on the Display.
//All codes come from the assigned button event codes. By convention the first byte
//For more information to grasph the mapping convention, see the file "DisplayBufferScreenMap"
//THIS MODEL IS NOT FLEXIBLE ENOUGHT SINCE THERE ARE OTHER TRANSACTIONS NOT RELATED WITH
//PRESET THAT MUST BE MET!!! REQUIRES FIXING!!
void DispenserSide1Listener(void *pparam)
{
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    //PUMP TRANSACTION CREATION HERE!!!    
    Pump *ppump = NULL;
    //Selecting the PUMP (Position)
    PumpCodeMapping *ptr = _g_pumpcodemap;
    uint8 pumpposcode = pmsg->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SELECCIONE_POSICION)];
    while(ptr->_side != 0xFF)
    {
        if(ptr->_side == SIDE1 && ptr->_refcode == pumpposcode)
        {
            ppump = ptr->_ppump;
            break;
        }            
        ptr++;
    }
    if(ppump)
    {        
        memcpy(ppump->_trasactionbuffer, pmsg->_buffer, _MESSAGE_LENGTH_);
        ppump->_print = (ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_DESEA_IMPRIMIR_RECIBO)] == 0x39);
        uint8 paymentmethod = pmsg->_buffer[GetBufferIndexFromDisplayID(DISPLAY_FORMA_PAGO_DESEADA)];
        if(paymentmethod == 0x0D)
        {        
            uint8 transactionname = pmsg->_buffer[GetBufferIndexFromDisplayID(DISPLAY_FORMA_PROGRAMACION)];
            ppump->_pcurrtransaction = GetTransactionByName(transactionname);
        }
        else if(paymentmethod == 0x0E)
        {
            ppump->_pcurrtransaction = GetTransactionByName(_PUMP_FILL_CREDIT_);
        }
        
        ppump->_presetdone = false;
        PNEAR_PUMPTRANSACTIONJOBPTR ptrjob = &((PNEAR_PUMPTRANSACTIONPTR)ppump->_pcurrtransaction)->_jobs[0];
        if(ptrjob)
        {
            while(ptrjob->Request != NULL)
            {
                PPUMPTRANSACTIONJOBCONTEXTPTR pjobcntr = _g_pumpjobqueue.Allocate(&_g_pumpjobqueue);
                if(pjobcntr)
                {
                    pjobcntr->_prequest = ptrjob;
                    pjobcntr->_presponse = NULL;
                    pjobcntr->_ppump = ppump;
                    pjobcntr->_ppump->CleanUp(pjobcntr->_ppump);
                    _g_pumpjobqueue.Enqueue(&_g_pumpjobqueue, pjobcntr);
                }
                ptrjob++;
            }
        }
    }
}

//@Create by: HIJH
//@Date: Septembre de 2016
//This method handles all of the incoming data that has been capture on the Display.
//All codes come from the assigned button event codes. By convention the first byte
//For more information to grasph the mapping convention, see the file "DisplayBufferScreenMap"
//THIS MODEL IS NOT FLEXIBLE ENOUGHT SINCE THERE ARE OTHER TRANSACTIONS NOT RELATED WITH
//PRESET THAT MUST BE MET!!! REQUIRES FIXING!!
void DispenserSide2Listener(void *pparam)
{
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    //PUMP TRANSACTION CREATION HERE!!!    
    Pump *ppump = NULL;
    //Selecting the PUMP (Position)
    PumpCodeMapping *ptr = _g_pumpcodemap;
    uint8 pumpposcode = pmsg->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SELECCIONE_POSICION)];
    while(ptr->_side != 0xFF)
    {
        if(ptr->_side == SIDE2 && ptr->_refcode == pumpposcode)
        {
            ppump = ptr->_ppump;
            break;
        }            
        ptr++;
    }
    if(ppump)
    {
        
        memcpy(ppump->_trasactionbuffer, pmsg->_buffer, _MESSAGE_LENGTH_);
        ppump->_print = (ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_DESEA_IMPRIMIR_RECIBO)] == 0x39);
        uint8 paymentmethod = pmsg->_buffer[GetBufferIndexFromDisplayID(DISPLAY_FORMA_PAGO_DESEADA)];
        if(paymentmethod == 0x0D)
        {        
            uint8 transactionname = pmsg->_buffer[GetBufferIndexFromDisplayID(DISPLAY_FORMA_PROGRAMACION)];
            ppump->_pcurrtransaction = GetTransactionByName(transactionname);
        }
        else if(paymentmethod == 0x0E)
        {
            ppump->_pcurrtransaction = GetTransactionByName(_PUMP_FILL_CREDIT_);
        }
        
        ppump->_presetdone = false;
        PNEAR_PUMPTRANSACTIONJOBPTR ptrjob = ((PNEAR_PUMPTRANSACTIONPTR)ppump->_pcurrtransaction)->_jobs;
        if(ptrjob)
        {
            while(ptrjob->Request != NULL)
            {
                PPUMPTRANSACTIONJOBCONTEXTPTR pjobcntr = _g_pumpjobqueue.Allocate(&_g_pumpjobqueue);
                if(pjobcntr)
                {
                    pjobcntr->_prequest = ptrjob;
                    pjobcntr->_presponse = NULL;
                    pjobcntr->_ppump = ppump;
                    pjobcntr->_ppump->CleanUp(pjobcntr->_ppump);
                    _g_pumpjobqueue.Enqueue(&_g_pumpjobqueue, pjobcntr);
                }
                ptrjob++;
            }
        }
    }
}

//@Created By: HIJH
//@Date: Septembre de 2016
//Acquires the current pump/hose state
void DispenserUpdatePumpState(void *pparam)
{
    PNEAR_PUMPPTR ppump = PNEAR_NULLPTR;
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    FOR(uint8 index = 0, index < _g_dispenserlayoutinfo._numpositions, index++)
    {
        if(_g_pumps[index]._roundrobinstate == _PUMP_RR_ACTIVE_)
        {
            ppump = &_g_pumps[index];
            _g_pumps[index]._roundrobinstate = _PUMP_RR_INACTIVE_;
            if((index + 1) >= _g_dispenserlayoutinfo._numpositions)
                _g_pumps[0x00]._roundrobinstate = _PUMP_RR_ACTIVE_;
            else
                _g_pumps[(index + 1)]._roundrobinstate = _PUMP_RR_ACTIVE_;
            
            break;
        }
    }

    if(ppump)
    {
        ppump->_acquiringstate = true;
        ppump->_pcurrtransaction = GetTransactionByName(_PUMP_STATE_);
        PumpTransactionJob *ptrjob = &((PumpTransaction*)ppump->_pcurrtransaction)->_jobs[0];
        if(ptrjob)
        {
            while(ptrjob->Request != NULL)
            {
                PPUMPTRANSACTIONJOBCONTEXTPTR pjobcntr = _g_pumpjobqueue.Allocate(&_g_pumpjobqueue);
                if(pjobcntr)
                {
                    pjobcntr->_reenqueue = false;
                    pjobcntr->_prequest = ptrjob;
                    pjobcntr->_presponse = NULL;
                    pjobcntr->_ppump = ppump;
                    pjobcntr->_ppump->CleanUp(pjobcntr->_ppump);
                    //This callback will be invoked upon response (if exists)
                    pjobcntr->Callback = pmsg->Callback;
                    pjobcntr->_pdata = pmsg;
                    
                    _g_pumpjobqueue.Enqueue(&_g_pumpjobqueue, pjobcntr);
                }
                ptrjob++;
            }
        }
        //Pump Unlock by pushing the pump/position out from the EOT condition
        DispenserUnlockPumpsOnEOT(ppump);
        //Conditional price restoration after a credit sale
        DispenserRestorePrices(ppump);
    }
}

//@Created By: HIJH
//@Date: Septembre de 2016
//Unlocks pumps on EOT condition
void DispenserUnlockPumpsOnEOT(LPVOID pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    //The PUMP must be locked by EOT and without any current transaction
    if(((ppump->_pumpstate == PUMP_FEOT) || (ppump->_pumpstate == PUMP_PEOT)) && ppump->_authorized)
    {
        ppump->_pcurrtransaction = GetTransactionByName(_PUMP_EOT_);
        PumpTransactionJob *ptrjob = &((PumpTransaction*)ppump->_pcurrtransaction)->_jobs[0];
        if(ptrjob)
        {
            while(ptrjob->Request != NULL)
            {
                PPUMPTRANSACTIONJOBCONTEXTPTR pjobcntr = _g_pumpjobqueue.Allocate(&_g_pumpjobqueue);
                if(pjobcntr)
                {
                    pjobcntr->_reenqueue = false;
                    pjobcntr->_prequest = ptrjob;
                    pjobcntr->_presponse = NULL;
                    pjobcntr->_ppump = ppump;
                    pjobcntr->_ppump->CleanUp(pjobcntr->_ppump);
                    //This callback will be invoked upon response (if exists)
                    //pjobcntr->Callback = pmsg->Callback;
                    //pjobcntr->_pdata = pmsg;
                    
                    _g_pumpjobqueue.Enqueue(&_g_pumpjobqueue, pjobcntr);
                }
                ptrjob++;
            }
        }
        PSINKMESSAGEPTR psinkmsg = AllocateMessageSlot(DISPENSER_ACQUIRE_TOTALS);
        psinkmsg->_messageid = DISPENSER_ACQUIRE_TOTALS;
        psinkmsg->_messagetype = FIRSTFOUND;
        memcpy(psinkmsg->_buffer, (const void*)NULL, 0x00);
        psinkmsg->_buffer[0x00] = ppump->_pumpid;
        psinkmsg->_buffersize = 0x01;
        
        psinkmsg->_messagestate = SINK_BUSY;
        
        ppump->_authorized = false;
    }
}

void DispenserRestorePrices(LPVOID pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    //The PUMP must be locked by EOT and without any current transaction
    if((ppump->_pumpstate == PUMP_IDLE || ppump->_pumpstate == PUMP_CALLING) && ppump->_restoreprice)
    {
        ppump->_pcurrtransaction = GetTransactionByName(_PUMP_RESTORE_PRICE_);
        PumpTransactionJob *ptrjob = &((PumpTransaction*)ppump->_pcurrtransaction)->_jobs[0];
        if(ptrjob)
        {
            while(ptrjob->Request != NULL)
            {
                PPUMPTRANSACTIONJOBCONTEXTPTR pjobcntr = _g_pumpjobqueue.Allocate(&_g_pumpjobqueue);
                if(pjobcntr)
                {
                    pjobcntr->_reenqueue = false;
                    pjobcntr->_prequest = ptrjob;
                    pjobcntr->_presponse = NULL;
                    pjobcntr->_ppump = ppump;
                    pjobcntr->_ppump->CleanUp(pjobcntr->_ppump);
                    //This callback will be invoked upon response (if exists)
                    //pjobcntr->Callback = pmsg->Callback;
                    //pjobcntr->_pdata = pmsg;
                    
                    _g_pumpjobqueue.Enqueue(&_g_pumpjobqueue, pjobcntr);
                }
                ptrjob++;
            }
        }
    }
}

//@Created by: HIJH
//@Septembre de 2016
//Acquires the dispenser configuration information per pump, through the Special Function commands
void DispenserAcquireConfiguration(LPVOID pparam)
{
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    
    //Invoke the Dispenser's transactional model to acquire the configuration information
    //and then store it in the File System
    if(_g_dispenserlayoutinfo._numpositions == 0x00)
        _g_dispenserlayoutinfo._currentloopopen = true;

    FOR(uint8 index = 0, index < _g_dispenserlayoutinfo._numpositions, index++)
    {
        Pump *ppump = &_g_pumps[index];
        //Pass the buffered data to the pump structure
        if(ppump)
        {
            ppump->_authorized = true;
            DispenserUnlockPumpsOnEOT(ppump);
            ppump->_authorized = false;

            memcpy(ppump->_trasactionbuffer, pmsg->_buffer, _MESSAGE_LENGTH_);
            ppump->_pcurrtransaction = GetTransactionByName(_PUMP_COMPLETE_CONFIGURATION_);
            
            PumpTransactionJob *ptrjob = &((PumpTransaction*)ppump->_pcurrtransaction)->_jobs[0];
            if(ptrjob)
            {
                while(ptrjob->Request != NULL)
                {
                    PPUMPTRANSACTIONJOBCONTEXTPTR pjobcntr = _g_pumpjobqueue.Allocate(&_g_pumpjobqueue);
                    if(pjobcntr)
                    {
                        pjobcntr->_reenqueue = false;
                        pjobcntr->_prequest = ptrjob;
                        pjobcntr->_presponse = NULL;
                        pjobcntr->_ppump = ppump;
                        pjobcntr->_ppump->CleanUp(pjobcntr->_ppump);
                        _g_pumpjobqueue.Enqueue(&_g_pumpjobqueue, pjobcntr);
                    }
                    ptrjob++;
                }
            }
        }
        break;
    }
    
    if(_g_ptractprogramming)
        _g_ptractprogramming(pmsg);
    
    pmsg->_selfkill = true;
}

//@Created by: HIJH
//@Septembre de 2016
//Configures prices per pump/hose
void DispenserConfigurePrices(LPVOID pparam)
{
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    
    //Invoke the Dispenser's transactional model to acquire the configuration information
    //and then store it in the File System
    FOR(uint8 index = 0, index < _g_dispenserlayoutinfo._numpositions, index++)
    {
        Pump *ppump = &_g_pumps[index];
        //Pass the buffered data to the pump structure
        if(ppump)
        {
            uint8 numhoses = 0x00;
            switch(index)
            {
                case 0x00:
                    numhoses = _g_dispenserlayoutinfo._hosesposition1;
                    break;
                case 0x01:
                    numhoses = _g_dispenserlayoutinfo._hosesposition2;
                    break;
                case 0x02:
                    numhoses = _g_dispenserlayoutinfo._hosesposition3;
                    break;
                case 0x03:
                    numhoses = _g_dispenserlayoutinfo._hosesposition4;
                    break;
            }
            
            ppump->_currenthose = 0x00;
            FOR(uint8 hosendx = 0, hosendx < numhoses, hosendx++)
            {
                memset(ppump->_trasactionbuffer, 0x00, _MESSAGE_LENGTH_);
                ppump->_pcurrtransaction = GetTransactionByName(_PUMP_PPU_CHANGE_);
                
                PumpTransactionJob *ptrjob = &((PumpTransaction*)ppump->_pcurrtransaction)->_jobs[0];
                if(ptrjob)
                {
                    while(ptrjob->Request != NULL)
                    {
                        PPUMPTRANSACTIONJOBCONTEXTPTR pjobcntr = _g_pumpjobqueue.Allocate(&_g_pumpjobqueue);
                        if(pjobcntr)
                        {
                            pjobcntr->_reenqueue = false;
                            pjobcntr->_prequest = ptrjob;
                            pjobcntr->_presponse = NULL;
                            pjobcntr->_ppump = ppump;
                            pjobcntr->_ppump->CleanUp(pjobcntr->_ppump);
                            _g_pumpjobqueue.Enqueue(&_g_pumpjobqueue, pjobcntr);
                        }
                        ptrjob++;
                    }
                }
            }
        }
    }
    
    pmsg->_selfkill = true;
}

//@Created by: HIJH
//@Septembre de 2016
//Configures the price for the requested pump/hose
void DispenserConfigureSinglePrice(LPVOID pparam)
{
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    //Invoke the Dispenser's transactional model to acquire the configuration information
    //and then store it in the File System
    PNEAR_PUMPPTR ppump =  GetPumpFromRemotePositionID(pmsg->_buffer[0x00]);
    if(ppump)
    {
        ppump->_currenthose = pmsg->_buffer[0x01];
        
        I2CBusLock();
        ClearEepromBuffer();
        uint16 eeprompage = DispenserGetEepromPPUPageFromPumpID(ppump->_pumpid);
        LoadEepromPage(eeprompage);
        switch(ppump->_currenthose)
        {
            case 0x00:
            {
                if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                {
                    uint32 value = LSDBCDBUFF2HEX((PNEAR_BYTE_PTR)&pmsg->_buffer[0x02], pmsg->_buffer[0x08]);
                    value = (uint32)round(value / 10);
                    memset(&pmsg->_buffer[0x02], 0x00, pmsg->_buffer[0x08]);
                    B78AD90CF552D9B30A(value, (PNEAR_BYTE_PTR)&pmsg->_buffer[0x02]);
                }
                //Six bytes only since the PPU is constrained to this length
                memcpy(&GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_], &pmsg->_buffer[0x02], pmsg->_buffer[0x08]);
                break;
            }
            case 0x01:
            {
                if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                {
                    uint32 value = LSDBCDBUFF2HEX((PNEAR_BYTE_PTR)&pmsg->_buffer[0x02], pmsg->_buffer[0x08]);
                    value = (uint32)round(value / 10);
                    memset(&pmsg->_buffer[0x02], 0x00, pmsg->_buffer[0x08]);
                    B78AD90CF552D9B30A(value, (PNEAR_BYTE_PTR)&pmsg->_buffer[0x02]);
                }
                //Six bytes only since the PPU is constrained to this length
                memcpy(&GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_], &pmsg->_buffer[0x02], pmsg->_buffer[0x08]);
                break;
            }
            case 0x02:
            {
                if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                {
                    uint32 value = LSDBCDBUFF2HEX((PNEAR_BYTE_PTR)&pmsg->_buffer[0x02], pmsg->_buffer[0x08]);
                    value = (uint32)round(value / 10);
                    memset(&pmsg->_buffer[0x02], 0x00, pmsg->_buffer[0x08]);
                    B78AD90CF552D9B30A(value, (PNEAR_BYTE_PTR)&pmsg->_buffer[0x02]);
                }
                //Six bytes only since the PPU is constrained to this length
                memcpy(&GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_], &pmsg->_buffer[0x02], pmsg->_buffer[0x08]);
                break;
            }
            case 0x03:
            {
                if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                {
                    uint32 value = LSDBCDBUFF2HEX((PNEAR_BYTE_PTR)&pmsg->_buffer[0x02], pmsg->_buffer[0x08]);
                    value = (uint32)round(value / 10);
                    memset(&pmsg->_buffer[0x02], 0x00, pmsg->_buffer[0x08]);
                    B78AD90CF552D9B30A(value, (PNEAR_BYTE_PTR)&pmsg->_buffer[0x02]);
                }
                //Six bytes only since the PPU is constrained to this length
                memcpy(&GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_], &pmsg->_buffer[0x02], pmsg->_buffer[0x08]);
                break;
            }
        }
        StoreEepromPage(eeprompage);        
        I2CBusUnlock();
    
        ppump->_pcurrtransaction = GetTransactionByName(_PUMP_PPU_CHANGE_);
        PumpTransactionJob *ptrjob = &((PumpTransaction*)ppump->_pcurrtransaction)->_jobs[0];
        if(ptrjob)
        {
            while(ptrjob->Request != NULL)
            {
                PPUMPTRANSACTIONJOBCONTEXTPTR pjobcntr = _g_pumpjobqueue.Allocate(&_g_pumpjobqueue);
                if(pjobcntr)
                {
                    pjobcntr->_reenqueue = false;
                    pjobcntr->_prequest = ptrjob;
                    pjobcntr->_presponse = NULL;
                    pjobcntr->_ppump = ppump;
                    pjobcntr->_ppump->CleanUp(pjobcntr->_ppump);
                    _g_pumpjobqueue.Enqueue(&_g_pumpjobqueue, pjobcntr);
                }
                ptrjob++;
            }
        }

    }  
    pmsg->_selfkill = true;
}

//@Created by: HIJH
//@Septembre de 2016
//Acquires pump totals for the specified hose
void DispenserAcquireTotals(LPVOID pparam)
{
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    //Invoke the Dispenser's transactional model to acquire the configuration information
    //and then store it in the File System
    PNEAR_PUMPPTR ppump =  GetPumpFromRemotePositionID(pmsg->_buffer[0x00]);
    if(ppump)
    {
        ppump->_pcurrtransaction = GetTransactionByName(_PUMP_TOTALS_);
        PumpTransactionJob *ptrjob = &((PumpTransaction*)ppump->_pcurrtransaction)->_jobs[0];
        if(ptrjob)
        {
            while(ptrjob->Request != NULL)
            {
                PPUMPTRANSACTIONJOBCONTEXTPTR pjobcntr = _g_pumpjobqueue.Allocate(&_g_pumpjobqueue);
                if(pjobcntr)
                {
                    pjobcntr->_reenqueue = false;
                    pjobcntr->_prequest = ptrjob;
                    pjobcntr->_presponse = NULL;
                    pjobcntr->_ppump = ppump;
                    pjobcntr->_ppump->CleanUp(pjobcntr->_ppump);
                    _g_pumpjobqueue.Enqueue(&_g_pumpjobqueue, pjobcntr);
                }
                ptrjob++;
            }
        }

    }  
    pmsg->_selfkill = true;
}

//@Created by: HIJH
//@Septembre de 2016
//Loads the local configuration information
void DispenserEnumeratePumps(LPVOID pparam)
{
    
    ClearEepromBuffer();
    //Cleanup of the pump identifier in order to start the enumeration procedure
    LoadEepromPage(EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE);
    GetEepromBuffer()[0] = 0x00;
    StoreEepromPage(EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE);

    ClearEepromBuffer();
    //Cleanup of the pump identifier in order to start the enumeration procedure
    LoadEepromPage(EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE);
    GetEepromBuffer()[0] = 0x00;
    StoreEepromPage(EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE);
    
    ClearEepromBuffer();
    //Cleanup of the pump identifier in order to start the enumeration procedure
    LoadEepromPage(EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE);
    GetEepromBuffer()[0] = 0x00;
    StoreEepromPage(EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE);

    ClearEepromBuffer();
    //Cleanup of the pump identifier in order to start the enumeration procedure
    LoadEepromPage(EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE);
    GetEepromBuffer()[0] = 0x00;
    StoreEepromPage(EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE);
    
    _g_dispenserlayoutinfo._numpositions = 0;
    
    Pump *ppump = &_g_pumps[0];
    //Pass the buffered data to the pump structure
    if(ppump)
    {
        
        memset(ppump->_trasactionbuffer, 0x00, _PUMP_TRANSACTION_BUFFER_SIZE_);
        ppump->_pcurrtransaction = GetTransactionByName(_PUMP_NETWORK_ENUMERATOR_);
        
        PumpTransactionJob *ptrjob = &((PumpTransaction*)ppump->_pcurrtransaction)->_jobs[0];
        if(ptrjob)
        {
            while(ptrjob->Request != NULL)
            {
                PPUMPTRANSACTIONJOBCONTEXTPTR pjobcntr = _g_pumpjobqueue.Allocate(&_g_pumpjobqueue);
                if(pjobcntr)
                {
                    pjobcntr->_reenqueue = true;
                    pjobcntr->_prequest = ptrjob;
                    pjobcntr->_presponse = NULL;
                    pjobcntr->_ppump = ppump;
                    pjobcntr->_ppump->CleanUp(pjobcntr->_ppump);
                    pjobcntr->_ppump->_datacontainer._var0 = 0x1;
                    pjobcntr->_ppump->_datacontainer._var1 = 0x0;
                    pjobcntr->_ppump->_pumpid = pjobcntr->_ppump->_datacontainer._var0;
                    _g_pumpjobqueue.Enqueue(&_g_pumpjobqueue, pjobcntr);
                }
                ptrjob++;
            }
        }
    }    
}

//@Created by: HIJH
//@Date: Octobre de 2016
//Load the initial configuration information currently stored in the NVM
void DispenserLoadEEPROMSettings(LPVOID pparam)
{
    
    ClearEepromBuffer();
    LoadEepromPage(EEPROM_CONFIGURATION_PAGE7);
    //Dispenser Pumps Offset is stored at position 0x09 in this memory area (see Eeprom.h)
    _g_dispenserlayoutinfo._positionoffset = GetEepromBuffer()[0x09];

    //Saving Timeout to Autonomous Mode
    _g_rfautorun._timeouttoautonomous = GetEepromBuffer()[0x0A];
    
    //This is the default printer port allocation
    _g_printerlayout._printerportside1 = (GetEepromBuffer()[0x0B] != 0x00)?GetEepromBuffer()[0x0B]: PRINTER11_JOB;
    _g_printerlayout._printerportside2 = (GetEepromBuffer()[0x0C] != 0x00)?GetEepromBuffer()[0x0C]: PRINTER21_JOB;
    
    //////////////////////////////////////////////////////////////////////////////////
    //ONLY FOR TESTING PURPOSES
//    GetEepromBuffer()[0x0D] = 0x30;
//    GetEepromBuffer()[0x0E] = 0x32;
//    GetEepromBuffer()[0x0F] = 0x30;
//    GetEepromBuffer()[0x10] = 0x34;    
//    StoreEepromPage(EEPROM_CONFIGURATION_PAGE7);
    //////////////////////////////////////////////////////////////////////////////////
    
    ClearEepromBuffer();
    LoadEepromPage(EEPROM_CONFIGURATION_PAGE5);
    //Station Unique Identifier start at 0x00 and spans 2 bytes in this memory area (see Eeprom.h)
    _g_stationidentifier = (((uint16)GetEepromBuffer()[0x01] << 8) & 0xFF00) | ((uint16)GetEepromBuffer()[0x00]);

    //////////////////////////////////////////////////////////////////////////////////
    //HERE WE ARE FORCING A MINIMUM MONEY PRESET VALUE SINCE THERE IS NO 
    //CONFIGURATION STREAM FOR THIS OPTION
    ClearEepromBuffer();
    //Min Money Preset
    LoadEepromPage(EEPROM_CONFIGURATION_PAGE5);
    memset((PNEAR_BYTE_PTR)&GetEepromBuffer()[0x02], 0x00, 0x10);
    (&GetEepromBuffer()[0x02])[0x00] = 0x00;
    (&GetEepromBuffer()[0x02])[0x01] = 0x00;
    (&GetEepromBuffer()[0x02])[0x02] = 0x05;
    (&GetEepromBuffer()[0x02])[0x03] = 0x00;
    StoreEepromPage(EEPROM_CONFIGURATION_PAGE5);
    //////////////////////////////////////////////////////////////////////////////////
    
    //Min PRESET Money index 2 / 16 bytes (see Eeprom.h) Just using 8 bytes meanwhile
    _g_dispenserlayoutinfo._minmoneyvalue = LSDBCDBUFF2HEX(&GetEepromBuffer()[0x02], 0x08);

    ClearEepromBuffer();
    LoadEepromPage(EEPROM_CONFIGURATION_PAGE6);
    //Min PRESET Volumen index 0 / 16 bytes (see Eeprom.h) => Just using 8 bytes meanwhile
    _g_dispenserlayoutinfo._minvolumevalue = LSDBCDBUFF2HEX(&GetEepromBuffer()[0x00], 0x08);
    
    ClearEepromBuffer();
    LoadEepromPage(EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE);
    //Island Operator's Shift Work State [0x00:Closed, 0x01:Open] (5:1)
    #ifdef PUMPS_UNLOCKED
    _g_pumps[0x00]._positionlocked = (GetEepromBuffer()[0x05] == 0x00) && false;
    #endif
    #ifndef PUMPS_UNLOCKED
    _g_pumps[0x00]._positionlocked = (GetEepromBuffer()[0x05] == 0x00);
    #endif

    ClearEepromBuffer();
    LoadEepromPage(EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE);
    //Island Operator's Shift Work State [0x00:Closed, 0x01:Open] (5:1)
    #ifdef PUMPS_UNLOCKED
    _g_pumps[0x01]._positionlocked = (GetEepromBuffer()[0x05] == 0x00) && false;
    #endif
    #ifndef PUMPS_UNLOCKED
    _g_pumps[0x01]._positionlocked = (GetEepromBuffer()[0x05] == 0x00);
    #endif

    ClearEepromBuffer();
    LoadEepromPage(EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE);
    //Island Operator's Shift Work State [0x00:Closed, 0x01:Open] (5:1)
    #ifdef PUMPS_UNLOCKED
    _g_pumps[0x02]._positionlocked = (GetEepromBuffer()[0x05] == 0x00) && false;
    #endif
    #ifndef PUMPS_UNLOCKED
    _g_pumps[0x02]._positionlocked = (GetEepromBuffer()[0x05] == 0x00);
    #endif

    ClearEepromBuffer();
    LoadEepromPage(EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE);
    //Island Operator's Shift Work State [0x00:Closed, 0x01:Open] (5:1)
    #ifdef PUMPS_UNLOCKED
    _g_pumps[0x03]._positionlocked = (GetEepromBuffer()[0x05] == 0x00) && false;
    #endif
    #ifndef PUMPS_UNLOCKED
    _g_pumps[0x03]._positionlocked = (GetEepromBuffer()[0x05] == 0x00);
    #endif
    
    //FOR TESTING PURPOSES ONLY!!!
    #ifdef FORCE_DATETIME
    ClearRtcBuffer();
    GetRtcBuffer()[0x00] = 0x00;
    GetRtcBuffer()[0x01] = 0x44;
    GetRtcBuffer()[0x02] = 0b00010010;
    GetRtcBuffer()[0x03] = 0x05;
    GetRtcBuffer()[0x04] = 0x07;
    GetRtcBuffer()[0x05] = 0x10;
    GetRtcBuffer()[0x06] = 0x16;
    UpdateRTC();
    #endif

    #ifdef FORCE_PRICES
    uint8 index = 0x00;
    
    ClearEepromBuffer();
    GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_ + index++] = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_ + index++] = 0x05;
    GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_ + index++] = 0x08;
    GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_ + index++] = 0x09;

    index = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x02;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x01;

    index = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_ + index++] = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_ + index++] = 0x05;
    GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_ + index++] = 0x08;
    GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_ + index++] = 0x07;

    index = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x02;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x01;
    
    StoreEepromPage(EEPROM_DISPENSER_PUMP1_PPU_INFO_PAGE);

    index = 0x00;
    ClearEepromBuffer();
    GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_ + index++] = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_ + index++] = 0x05;
    GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_ + index++] = 0x08;
    GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_ + index++] = 0x08;

    index = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x05;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x08;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x06;

    index = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_ + index++] = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_ + index++] = 0x05;
    GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_ + index++] = 0x08;
    GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_ + index++] = 0x08;

    index = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_ + index++] = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_ + index++] = 0x05;
    GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_ + index++] = 0x08;
    GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_ + index++] = 0x06;
    
    StoreEepromPage(EEPROM_DISPENSER_PUMP2_PPU_INFO_PAGE);
    
    index = 0x00;
    ClearEepromBuffer();
    GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_ + index++] = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_ + index++] = 0x05;
    GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_ + index++] = 0x08;
    GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_ + index++] = 0x08;

    index = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x05;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x08;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x06;

    index = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_ + index++] = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_ + index++] = 0x05;
    GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_ + index++] = 0x08;
    GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_ + index++] = 0x08;

    index = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_ + index++] = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_ + index++] = 0x05;
    GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_ + index++] = 0x08;
    GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_ + index++] = 0x06;
    
    StoreEepromPage(EEPROM_DISPENSER_PUMP3_PPU_INFO_PAGE);

    index = 0x00;
    ClearEepromBuffer();
    GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_ + index++] = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_ + index++] = 0x05;
    GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_ + index++] = 0x08;
    GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_ + index++] = 0x08;

    index = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x05;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x08;
    GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_ + index++] = 0x06;

    index = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_ + index++] = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_ + index++] = 0x05;
    GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_ + index++] = 0x08;
    GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_ + index++] = 0x08;

    index = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_ + index++] = 0x00;
    GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_ + index++] = 0x05;
    GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_ + index++] = 0x08;
    GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_ + index++] = 0x06;
    
    StoreEepromPage(EEPROM_DISPENSER_PUMP4_PPU_INFO_PAGE);
    #endif
    
    StartRtc();
}

uint16 DispenserGetEepromPPUPageFromPumpID(uint8 pumpid)
{
    uint16 retval = 0x00;
    switch(_g_dispenserlayoutinfo._numpositions)
    {
        case 0x02:
        {
            if(_g_pumps[0x00]._pumpid == pumpid)
                retval = EEPROM_DISPENSER_PUMP1_PPU_INFO_PAGE;
            else if(_g_pumps[0x01]._pumpid == pumpid)
                retval = EEPROM_DISPENSER_PUMP2_PPU_INFO_PAGE;
            
            break;
        }
        case 0x04:
        {
            if(_g_pumps[0x00]._pumpid == pumpid)
                retval = EEPROM_DISPENSER_PUMP1_PPU_INFO_PAGE;
            else if(_g_pumps[0x01]._pumpid == pumpid)
                retval = EEPROM_DISPENSER_PUMP2_PPU_INFO_PAGE;
            else if(_g_pumps[0x02]._pumpid == pumpid)
                retval = EEPROM_DISPENSER_PUMP3_PPU_INFO_PAGE;
            else if(_g_pumps[0x03]._pumpid == pumpid)
                retval = EEPROM_DISPENSER_PUMP4_PPU_INFO_PAGE;
            
            break;
        }
    }
    return retval;
}

//@Created by: HIJH
//@Date: Octobre de 2016
//Gets a pump pointer to the requested positon identifier
PNEAR_PUMPPTR GetPumpFromRemotePositionID(uint8 positionid)
{
    PNEAR_PUMPPTR retval = PNEAR_NULLPTR;
    FOR(uint8 index = 0, index < _g_dispenserlayoutinfo._numpositions, index++)
    {
        if(positionid == _g_pumps[index]._pumpid)
        {
            retval = &_g_pumps[index];
            break;
        }
    }
    return retval;
}

/* [] END OF FILE */
