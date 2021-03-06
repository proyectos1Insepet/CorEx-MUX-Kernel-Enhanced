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
@Date: Novembre de 2016
*/

#include <project.h>
#include "GlobalDefinitions.h"
#include "SinkManager.h"
#include "SinkObservables.h"
#include "UARTManager.h"
#include "Display.h"
#include "Dispenser.h"
#include "Eeprom.h"
#include "RFLink.h"
#include "Printer.h"
#include "Rtc.h"

#ifdef ENCORE_PROTOCOL

uint8_t FindLRC(char8 *_pbuffer);
uint8_t GetLRC(char8 *_pbuffer);
uint8 errorCounter = 0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////                 BLOCK OF AVAILABLE RESPONSE HANDLERS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void EncoreActivateProgramming(LPVOID pparam);
extern PNEARDISPENSERACTIVATEPROGRAMMING _g_ptractprogramming;

//@Created By: HIJH
//@Date: Septembre de 2016
//Updates the pump state into the pump structure
void AcquirePumpStateResponse(void *pparam)
{
    PPUMPTRANSACTIONJOBCONTEXTPTR pjob = (PPUMPTRANSACTIONJOBCONTEXTPTR)pparam;
    if(pjob)
    {
        uint8 prevstate = pjob->_ppump->_pumpstate;
        uint8 prevrfstate = pjob->_ppump->_pumprftransstate;
        pjob->_ppump->_pumpstate = ((pjob->_ppump->_rxbuffer[0x00] & 0xF0) >> 4);
        if(pjob->_ppump->PumpValidState(pjob->_ppump) && ((pjob->_ppump->_rxbuffer[0x00] & 0x0F)) == pjob->_ppump->_pumpid  ){
            errorCounter = 0;
            if(errorCounter == 0){
                pjob->_ppump->_transhealth = _PUMP_OK_;  
            }
        }
        else
        {
            errorCounter = errorCounter + 1;
            if(errorCounter == 3){
                pjob->_ppump->_transhealth = _PUMP_FAIL_;
                pjob->_ppump->_pumpstate = PUMP_FAIL;
                errorCounter = 1;
            }
        }   
        
        if(pjob->_ppump->_pumpstate == PUMP_CALLING)
            pjob->_ppump->_pumplocked = false;                
        
        if((pjob->_ppump->_pumpstate == PUMP_IDLE || pjob->_ppump->_pumpstate == PUMP_CALLING) && 
            (prevstate == PUMP_BUSY || prevstate == PUMP_AUTHORIZED))
            pjob->_ppump->_pumprftransstate = RF_ZERO_SALE; 
        
        if((pjob->_ppump->_pumpstate == PUMP_IDLE ) && 
            (prevrfstate == RF_ZERO_SALE ))
            pjob->_ppump->_pumprftransstate = RF_IDLE; 
        
        if((pjob->_ppump->_pumpstate == PUMP_IDLE   ) && 
            (pjob->_ppump->_pumprftransstate == RF_COPY_RECEIPT ))
            pjob->_ppump->_pumprftransstate = RF_IDLE;
                
                
        pjob->_ppump->_acquiringstate = false;
    }
}

//@Created By: HIJH
//@Date: Septembre de 2016
//Validates the response from the pump previous batch transfer (0xDp)
void AcquireDatatoPumpResponse(void *pparam)
{
    PPUMPTRANSACTIONJOBCONTEXTPTR pjob = (PPUMPTRANSACTIONJOBCONTEXTPTR)pparam;
    if(pjob)
    {
        if((0x0F & pjob->_ppump->_pumpid) == (pjob->_ppump->_rxbuffer[0x00] & 0x0F))
            pjob->_ppump->_pumpstate = ((pjob->_ppump->_rxbuffer[0x00] >> 4)& 0x0F);
    }
}

void ProcessPumpRTMoneyRequest(void *pparam)
{
    PPUMPTRANSACTIONJOBCONTEXTPTR pjob = (PPUMPTRANSACTIONJOBCONTEXTPTR)pparam;
    if(pjob)
    {
        //Bytes from index 0 to 7 are the digits for the Real Time Money Data in BCD format
        pjob->_ppump->_rxbuffer[0x00] = (pjob->_ppump->_rxbuffer[0x00] & 0x0F);
        pjob->_ppump->_rxbuffer[0x01] = (pjob->_ppump->_rxbuffer[0x01] & 0x0F);
        pjob->_ppump->_rxbuffer[0x02] = (pjob->_ppump->_rxbuffer[0x02] & 0x0F);
        pjob->_ppump->_rxbuffer[0x03] = (pjob->_ppump->_rxbuffer[0x03] & 0x0F);
        pjob->_ppump->_rxbuffer[0x04] = (pjob->_ppump->_rxbuffer[0x04] & 0x0F);
        pjob->_ppump->_rxbuffer[0x05] = (pjob->_ppump->_rxbuffer[0x05] & 0x0F);
        pjob->_ppump->_rxbuffer[0x06] = (pjob->_ppump->_rxbuffer[0x06] & 0x0F);
        pjob->_ppump->_rxbuffer[0x07] = (pjob->_ppump->_rxbuffer[0x07] & 0x0F);
        pjob->_ppump->_rtmoneyamount = LSDBCDBUFF2HEX(PBYTECAST(&pjob->_ppump->_rxbuffer[0x00]), 0x08);
    }
}

void ProcessPumpTransactionData(void *pparam)
{
    PPUMPTRANSACTIONJOBCONTEXTPTR pjob = (PPUMPTRANSACTIONJOBCONTEXTPTR)pparam;
    if(pjob)
    {
        {
            uint8 strmlrc = FindLRC(pjob->_ppump->_rxbuffer);
            uint8 locallrc = GetLRC(pjob->_ppump->_rxbuffer);
            
            if(strmlrc != locallrc && pjob->_reenqueuecounter < _MAX_RETRIES_)
            {
                pjob->_reenqueuecounter++;
                pjob->_reenqueue = true;
                return;
            }
            pjob->_reenqueue = false;

            if(_g_dispenserlayoutinfo._displaydigitsmode == 0x06)
            {            
                pjob->_ppump->_rxbuffer[0x09] = (pjob->_ppump->_rxbuffer[0x09] & 0x0F);//Hose id
                pjob->_ppump->_authorizationinfo._hoseid = pjob->_ppump->_rxbuffer[0x09];//Hose id
                
                //In this position is located the payment type (CREDIT: 0x0E , CASH:0x0D)
                if(pjob->_ppump->_trasactionbuffer[0x00] == 0x0D)
                {
                    I2CBusLock();
                    ClearEepromBuffer();
                    uint16 eeprompage = DispenserGetEepromPPUPageFromPumpID(pjob->_ppump->_pumpid);
                    LoadEepromPage(eeprompage);
                    switch(pjob->_ppump->_authorizationinfo._hoseid)
                    {
                        case 0x00:
                        //Six bytes only since the PPU is constrained to this length
                        memcpy(PBYTECAST(&pjob->_ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD)]), &GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_], GetBufferLengthFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD));
                        break;
                        case 0x01:
                        //Six bytes only since the PPU is constrained to this length
                        memcpy(PBYTECAST(&pjob->_ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD)]), &GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_], GetBufferLengthFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD));
                        break;
                        case 0x02:
                        //Six bytes only since the PPU is constrained to this length
                        memcpy(PBYTECAST(&pjob->_ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD)]), &GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_], GetBufferLengthFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD));
                        break;
                        case 0x03:
                        //Six bytes only since the PPU is constrained to this length
                        memcpy(PBYTECAST(&pjob->_ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD)]), &GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_], GetBufferLengthFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD));
                        break;
                    }
                    I2CBusUnlock();
                    
                    memcpy(PBYTECAST(&pjob->_ppump->_rxbuffer[0x0C]), PBYTECAST(&pjob->_ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD)]), 0x04);
                    
                    //Bytes from index 12 to 17 are the digits for the PPU used for the transaction
                    //Digits are in BCD format
                    pjob->_ppump->_rxbuffer[0x0C] = (pjob->_ppump->_rxbuffer[0x0C] & 0x0F);
                    pjob->_ppump->_rxbuffer[0x0D] = (pjob->_ppump->_rxbuffer[0x0D] & 0x0F);
                    pjob->_ppump->_rxbuffer[0x0E] = (pjob->_ppump->_rxbuffer[0x0E] & 0x0F);
                    pjob->_ppump->_rxbuffer[0x0F] = (pjob->_ppump->_rxbuffer[0x0F] & 0x0F);
                    pjob->_ppump->_authorizationinfo._price = LSDBCDBUFF2HEX(PBYTECAST(&pjob->_ppump->_rxbuffer[0x0C]), 0x04) * pow10(_g_dispenserlayoutinfo._moneydecimals - _g_dispenserlayoutinfo._ppudecimals);
                    
                }
                else
                {
                    pjob->_ppump->_rxbuffer[0x0C] = (pjob->_ppump->_rxbuffer[0x0C] & 0x0F);
                    pjob->_ppump->_rxbuffer[0x0D] = (pjob->_ppump->_rxbuffer[0x0D] & 0x0F);
                    pjob->_ppump->_rxbuffer[0x0E] = (pjob->_ppump->_rxbuffer[0x0E] & 0x0F);
                    pjob->_ppump->_rxbuffer[0x0F] = (pjob->_ppump->_rxbuffer[0x0F] & 0x0F);
                }
                
                //Bytes from index 19 to 26 are the digits for the fuel volume delivered in the transaction
                //Digits are in BCD format
                pjob->_ppump->_rxbuffer[0x11] = (pjob->_ppump->_rxbuffer[0x11] & 0x0F);
                pjob->_ppump->_rxbuffer[0x12] = (pjob->_ppump->_rxbuffer[0x12] & 0x0F);
                pjob->_ppump->_rxbuffer[0x13] = (pjob->_ppump->_rxbuffer[0x13] & 0x0F);
                pjob->_ppump->_rxbuffer[0x14] = (pjob->_ppump->_rxbuffer[0x14] & 0x0F);
                pjob->_ppump->_rxbuffer[0x15] = (pjob->_ppump->_rxbuffer[0x15] & 0x0F);
                pjob->_ppump->_rxbuffer[0x16] = (pjob->_ppump->_rxbuffer[0x16] & 0x0F);
                pjob->_ppump->_authorizationinfo._volumeamount = LSDBCDBUFF2HEX(PBYTECAST(&pjob->_ppump->_rxbuffer[0x11]), 0x06);
                
                ////////////////////////////////////////////////////////////////////////////////
                ////////////////////////////////////////////////////////////////////////////////
                
                //Bytes from index 28 to 35 are the digits for the money charged to the transaction
                //Digits are in BCD format
                pjob->_ppump->_rxbuffer[0x18] = (pjob->_ppump->_rxbuffer[0x18] & 0x0F);
                
                pjob->_ppump->_rxbuffer[0x19] = (pjob->_ppump->_rxbuffer[0x19] & 0x0F);
                pjob->_ppump->_rxbuffer[0x1A] = (pjob->_ppump->_rxbuffer[0x1A] & 0x0F);
                pjob->_ppump->_rxbuffer[0x1B] = (pjob->_ppump->_rxbuffer[0x1B] & 0x0F);
                pjob->_ppump->_rxbuffer[0x1C] = (pjob->_ppump->_rxbuffer[0x1C] & 0x0F);
                pjob->_ppump->_rxbuffer[0x1D] = (pjob->_ppump->_rxbuffer[0x1D] & 0x0F);
                //INQUIRY ABOUT THE EXTRA x10 RESULT SINCE THERE IS AN EXTRA 0 IN THE RESPONSE FOR THE TOTAL MONEY CHARGED TO THE TRANSACTION
                //MEANWHILE DIVIDING BY 10 TO REMOVE THE ZERO
                pjob->_ppump->_authorizationinfo._presetamount = LSDBCDBUFF2HEX(PBYTECAST(&pjob->_ppump->_rxbuffer[0x18]), 0x06);
                    
                ClearRtcBuffer();
                ReadRTC();
                //CAREFUL WITH RETURNS BETWEEN THE LOCK/UNLOCK CALLS
                //AND RECURSIVE CALLS THAT WOULD DEADLOCK!
                I2CBusLock();
            
                ClearEepromBuffer();
                //In this position is located the payment type (CREDIT: 0x0E , CASH:0x0D)
                if(pjob->_ppump->_trasactionbuffer[0x00] == 0x0D)
                {
                    //HERE WE STORE THE LAST TRANSACTION IN EEPROM! ALWAYS THE PREVIOUS TRANSACTION REPORT 
                    //WILL BE OVERWRITTEN AND LOST.
                    //IN CASE OF A "CREDIT SALE", THE WHOLE DATA WILL BE STORED IN A SEPARATE MEMORY REGION, 
                    //SINCE IN CASE OF A RF LINK DROP, THE STATE OF THE SALE COULD BE RECOVERED BY THE NSX.
                    LoadEepromPage(EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE);
                    if(GetEepromBuffer()[0] == ((pjob->_ppump->_rxbuffer[0x04] & 0x0F) + 1))
                    {
                        ClearEepromBuffer();
                        //Storing the current Date/Time for this transaction
                        memcpy(GetEepromBuffer(), GetRtcBuffer(), 0x07);
                        memcpy(&GetEepromBuffer()[0x07], pjob->_ppump->_rxbuffer, pjob->_ppump->_rxbuffersize);
                        StoreEepromPage(EEPROM_DISPENSER_PUMP12_TRANSACTION_PAGE);
                    }
                    else
                    {
                        ClearEepromBuffer();
                        LoadEepromPage(EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE);
                        if(GetEepromBuffer()[0] == ((pjob->_ppump->_rxbuffer[0x04] & 0x0F) + 1))
                        {
                            ClearEepromBuffer();
                            //Storing the current Date/Time for this transaction
                            memcpy(GetEepromBuffer(), GetRtcBuffer(), 0x07);
                            memcpy(&GetEepromBuffer()[0x07], pjob->_ppump->_rxbuffer, pjob->_ppump->_rxbuffersize);
                            StoreEepromPage(EEPROM_DISPENSER_PUMP22_TRANSACTION_PAGE);
                        }
                        else
                        {
                            ClearEepromBuffer();
                            LoadEepromPage(EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE);
                            if(GetEepromBuffer()[0] == ((pjob->_ppump->_rxbuffer[0x04] & 0x0F) + 1))
                            {
                                ClearEepromBuffer();
                                //Storing the current Date/Time for this transaction
                                memcpy(GetEepromBuffer(), GetRtcBuffer(), 0x07);
                                memcpy(&GetEepromBuffer()[0x07], pjob->_ppump->_rxbuffer, pjob->_ppump->_rxbuffersize);
                                StoreEepromPage(EEPROM_DISPENSER_PUMP32_TRANSACTION_PAGE);
                            }
                            else
                            {
                                ClearEepromBuffer();
                                LoadEepromPage(EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE);
                                if(GetEepromBuffer()[0] == ((pjob->_ppump->_rxbuffer[0x04] & 0x0F) + 1))
                                {
                                    ClearEepromBuffer();
                                    //Storing the current Date/Time for this transaction
                                    memcpy(GetEepromBuffer(), GetRtcBuffer(), 0x07);
                                    memcpy(&GetEepromBuffer()[0x07], pjob->_ppump->_rxbuffer, pjob->_ppump->_rxbuffersize);
                                    StoreEepromPage(EEPROM_DISPENSER_PUMP42_TRANSACTION_PAGE);
                                }
                            }
                        }
                    }
                }else{//CREDIT TRANSACTION
                    //HERE WE STORE THE LAST TRANSACTION IN EEPROM! ALWAYS THE PREVIOUS TRANSACTION REPORT 
                    //WILL BE OVERWRITTEN AND LOST.
                    //IN CASE OF A "CREDIT SALE", THE WHOLE DATA WILL BE STORED IN A SEPARATE MEMORY REGION, 
                    //SINCE IN CASE OF A RF LINK DROP, THE STATE OF THE SALE COULD BE RECOVERED BY THE NSX.
                    LoadEepromPage(EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE);
                    if(GetEepromBuffer()[0] == ((pjob->_ppump->_rxbuffer[0x04] & 0x0F) + 1))
                    {
                        ClearEepromBuffer();
                        //Storing the current Date/Time for this transaction
                        memcpy(GetEepromBuffer(), GetRtcBuffer(), 0x07);
                        memcpy(&GetEepromBuffer()[0x07], pjob->_ppump->_rxbuffer, pjob->_ppump->_rxbuffersize);
                        StoreEepromPage(EEPROM_DISPENSER_PUMP14_TRANSACTION_PAGE);
                    }
                    else
                    {
                        ClearEepromBuffer();
                        LoadEepromPage(EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE);
                        if(GetEepromBuffer()[0] == ((pjob->_ppump->_rxbuffer[0x04] & 0x0F) + 1))
                        {
                            ClearEepromBuffer();
                            //Storing the current Date/Time for this transaction
                            memcpy(GetEepromBuffer(), GetRtcBuffer(), 0x07);
                            memcpy(&GetEepromBuffer()[0x07], pjob->_ppump->_rxbuffer, pjob->_ppump->_rxbuffersize);
                            StoreEepromPage(EEPROM_DISPENSER_PUMP24_TRANSACTION_PAGE);
                        }
                        else
                        {
                            ClearEepromBuffer();
                            LoadEepromPage(EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE);
                            if(GetEepromBuffer()[0] == ((pjob->_ppump->_rxbuffer[0x04] & 0x0F) + 1))
                            {
                                ClearEepromBuffer();
                                //Storing the current Date/Time for this transaction
                                memcpy(GetEepromBuffer(), GetRtcBuffer(), 0x07);
                                memcpy(&GetEepromBuffer()[0x07], pjob->_ppump->_rxbuffer, pjob->_ppump->_rxbuffersize);
                                StoreEepromPage(EEPROM_DISPENSER_PUMP34_TRANSACTION_PAGE);
                            }
                            else
                            {
                                ClearEepromBuffer();
                                LoadEepromPage(EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE);
                                if(GetEepromBuffer()[0] == ((pjob->_ppump->_rxbuffer[0x04] & 0x0F) + 1))
                                {
                                    ClearEepromBuffer();
                                    //Storing the current Date/Time for this transaction
                                    memcpy(GetEepromBuffer(), GetRtcBuffer(), 0x07);
                                    memcpy(&GetEepromBuffer()[0x07], pjob->_ppump->_rxbuffer, pjob->_ppump->_rxbuffersize);
                                    StoreEepromPage(EEPROM_DISPENSER_PUMP44_TRANSACTION_PAGE);
                                }
                            }
                        }
                    }
                }
                I2CBusUnlock();
            }
            else
            {            
                pjob->_ppump->_rxbuffer[0x09] = (pjob->_ppump->_rxbuffer[0x09] & 0x0F);//Hose id
                pjob->_ppump->_authorizationinfo._hoseid = pjob->_ppump->_rxbuffer[0x09];//Hose id
                
                //In this position is located the payment type (CREDIT: 0x0E , CASH:0x0D)
                if(pjob->_ppump->_trasactionbuffer[0x00] == 0x0D)
                {
                    I2CBusLock();
                    ClearEepromBuffer();
                    uint16 eeprompage = DispenserGetEepromPPUPageFromPumpID(pjob->_ppump->_pumpid);
                    LoadEepromPage(eeprompage);
                    switch(pjob->_ppump->_authorizationinfo._hoseid)
                    {
                        case 0x00:
                        //Six bytes only since the PPU is constrained to this length
                        memcpy(PBYTECAST(&pjob->_ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD)]), &GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_], GetBufferLengthFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD));
                        break;
                        case 0x01:
                        //Six bytes only since the PPU is constrained to this length
                        memcpy(PBYTECAST(&pjob->_ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD)]), &GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_], GetBufferLengthFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD));
                        break;
                        case 0x02:
                        //Six bytes only since the PPU is constrained to this length
                        memcpy(PBYTECAST(&pjob->_ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD)]), &GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_], GetBufferLengthFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD));
                        break;
                        case 0x03:
                        //Six bytes only since the PPU is constrained to this length
                        memcpy(PBYTECAST(&pjob->_ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD)]), &GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_], GetBufferLengthFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD));
                        break;
                    }
                    I2CBusUnlock();
                    
                    memcpy(PBYTECAST(&pjob->_ppump->_rxbuffer[0x0C]), PBYTECAST(&pjob->_ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD)]), 0x06);                
                    
                    //Bytes from index 12 to 17 are the digits for the PPU used for the transaction
                    //Digits are in BCD format
                    pjob->_ppump->_rxbuffer[0x0C] = (pjob->_ppump->_rxbuffer[0x0C] & 0x0F);
                    pjob->_ppump->_rxbuffer[0x0D] = (pjob->_ppump->_rxbuffer[0x0D] & 0x0F);
                    pjob->_ppump->_rxbuffer[0x0E] = (pjob->_ppump->_rxbuffer[0x0E] & 0x0F);
                    pjob->_ppump->_rxbuffer[0x0F] = (pjob->_ppump->_rxbuffer[0x0F] & 0x0F);
                    pjob->_ppump->_rxbuffer[0x10] = (pjob->_ppump->_rxbuffer[0x10] & 0x0F);
                    pjob->_ppump->_rxbuffer[0x11] = (pjob->_ppump->_rxbuffer[0x11] & 0x0F);
                    pjob->_ppump->_authorizationinfo._price = LSDBCDBUFF2HEX(PBYTECAST(&pjob->_ppump->_rxbuffer[0x0C]), 0x06);
                    
                }
                else
                {
                    pjob->_ppump->_rxbuffer[0x0C] = (pjob->_ppump->_rxbuffer[0x0C] & 0x0F);
                    pjob->_ppump->_rxbuffer[0x0D] = (pjob->_ppump->_rxbuffer[0x0D] & 0x0F);
                    pjob->_ppump->_rxbuffer[0x0E] = (pjob->_ppump->_rxbuffer[0x0E] & 0x0F);
                    pjob->_ppump->_rxbuffer[0x0F] = (pjob->_ppump->_rxbuffer[0x0F] & 0x0F);
                    pjob->_ppump->_rxbuffer[0x10] = (pjob->_ppump->_rxbuffer[0x10] & 0x0F);
                    pjob->_ppump->_rxbuffer[0x11] = (pjob->_ppump->_rxbuffer[0x11] & 0x0F);
                }
                
                //Bytes from index 19 to 26 are the digits for the fuel volume delivered in the transaction
                //Digits are in BCD format
                pjob->_ppump->_rxbuffer[0x13] = (pjob->_ppump->_rxbuffer[0x13] & 0x0F);
                pjob->_ppump->_rxbuffer[0x14] = (pjob->_ppump->_rxbuffer[0x14] & 0x0F);
                pjob->_ppump->_rxbuffer[0x15] = (pjob->_ppump->_rxbuffer[0x15] & 0x0F);
                pjob->_ppump->_rxbuffer[0x16] = (pjob->_ppump->_rxbuffer[0x16] & 0x0F);
                pjob->_ppump->_rxbuffer[0x17] = (pjob->_ppump->_rxbuffer[0x17] & 0x0F);
                pjob->_ppump->_rxbuffer[0x18] = (pjob->_ppump->_rxbuffer[0x18] & 0x0F);
                pjob->_ppump->_rxbuffer[0x19] = (pjob->_ppump->_rxbuffer[0x19] & 0x0F);
                pjob->_ppump->_rxbuffer[0x1A] = (pjob->_ppump->_rxbuffer[0x1A] & 0x0F);
                pjob->_ppump->_authorizationinfo._volumeamount = LSDBCDBUFF2HEX(PBYTECAST(&pjob->_ppump->_rxbuffer[0x13]), 0x08);
                
                ////////////////////////////////////////////////////////////////////////////////
                ////////////////////////////////////////////////////////////////////////////////
                
                //Bytes from index 28 to 35 are the digits for the money charged to the transaction
                //Digits are in BCD format
                pjob->_ppump->_rxbuffer[0x1C] = (pjob->_ppump->_rxbuffer[0x1C] & 0x00);//Zero to mask out the decimal part 
                
                pjob->_ppump->_rxbuffer[0x1D] = (pjob->_ppump->_rxbuffer[0x1D] & 0x0F);
                pjob->_ppump->_rxbuffer[0x1E] = (pjob->_ppump->_rxbuffer[0x1E] & 0x0F);
                pjob->_ppump->_rxbuffer[0x1F] = (pjob->_ppump->_rxbuffer[0x1F] & 0x0F);
                pjob->_ppump->_rxbuffer[0x20] = (pjob->_ppump->_rxbuffer[0x20] & 0x0F);
                pjob->_ppump->_rxbuffer[0x21] = (pjob->_ppump->_rxbuffer[0x21] & 0x0F);
                pjob->_ppump->_rxbuffer[0x22] = (pjob->_ppump->_rxbuffer[0x22] & 0x0F);
                pjob->_ppump->_rxbuffer[0x23] = (pjob->_ppump->_rxbuffer[0x23] & 0x0F);
                //INQUIRY ABOUT THE EXTRA x10 RESULT SINCE THERE IS AN EXTRA 0 IN THE RESPONSE FOR THE TOTAL MONEY CHARGED TO THE TRANSACTION
                //MEANWHILE DIVIDING BY 10 TO REMOVE THE ZERO
                if(_g_dispenserlayoutinfo._displaydigitsmode == 0x07)
                    pjob->_ppump->_authorizationinfo._presetamount = (LSDBCDBUFF2HEX(PBYTECAST(&pjob->_ppump->_rxbuffer[0x1C]), 0x08)/0x0A);
                else
                    pjob->_ppump->_authorizationinfo._presetamount = LSDBCDBUFF2HEX(PBYTECAST(&pjob->_ppump->_rxbuffer[0x1C]), 0x08);
                    
                ClearRtcBuffer();
                ReadRTC();
                //CAREFUL WITH RETURNS BETWEEN THE LOCK/UNLOCK CALLS
                //AND RECURSIVE CALLS THAT WOULD DEADLOCK!
                I2CBusLock();
            
                ClearEepromBuffer();
                //In this position is located the payment type (CREDIT: 0x0E , CASH:0x0D)
                if(pjob->_ppump->_trasactionbuffer[0x00] == 0x0D)
                {
                    //HERE WE STORE THE LAST TRANSACTION IN EEPROM! ALWAYS THE PREVIOUS TRANSACTION REPORT 
                    //WILL BE OVERWRITTEN AND LOST.
                    //IN CASE OF A "CREDIT SALE", THE WHOLE DATA WILL BE STORED IN A SEPARATE MEMORY REGION, 
                    //SINCE IN CASE OF A RF LINK DROP, THE STATE OF THE SALE COULD BE RECOVERED BY THE NSX.
                    LoadEepromPage(EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE);
                    if(GetEepromBuffer()[0] == ((pjob->_ppump->_rxbuffer[0x04] & 0x0F) + 1))
                    {
                        ClearEepromBuffer();
                        //Storing the current Date/Time for this transaction
                        memcpy(GetEepromBuffer(), GetRtcBuffer(), 0x07);
                        memcpy(&GetEepromBuffer()[0x07], pjob->_ppump->_rxbuffer, pjob->_ppump->_rxbuffersize);
                        StoreEepromPage(EEPROM_DISPENSER_PUMP12_TRANSACTION_PAGE);
                    }
                    else
                    {
                        ClearEepromBuffer();
                        LoadEepromPage(EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE);
                        if(GetEepromBuffer()[0] == ((pjob->_ppump->_rxbuffer[0x04] & 0x0F) + 1))
                        {
                            ClearEepromBuffer();
                            //Storing the current Date/Time for this transaction
                            memcpy(GetEepromBuffer(), GetRtcBuffer(), 0x07);
                            memcpy(&GetEepromBuffer()[0x07], pjob->_ppump->_rxbuffer, pjob->_ppump->_rxbuffersize);
                            StoreEepromPage(EEPROM_DISPENSER_PUMP22_TRANSACTION_PAGE);
                        }
                        else
                        {
                            ClearEepromBuffer();
                            LoadEepromPage(EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE);
                            if(GetEepromBuffer()[0] == ((pjob->_ppump->_rxbuffer[0x04] & 0x0F) + 1))
                            {
                                ClearEepromBuffer();
                                //Storing the current Date/Time for this transaction
                                memcpy(GetEepromBuffer(), GetRtcBuffer(), 0x07);
                                memcpy(&GetEepromBuffer()[0x07], pjob->_ppump->_rxbuffer, pjob->_ppump->_rxbuffersize);
                                StoreEepromPage(EEPROM_DISPENSER_PUMP32_TRANSACTION_PAGE);
                            }
                            else
                            {
                                ClearEepromBuffer();
                                LoadEepromPage(EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE);
                                if(GetEepromBuffer()[0] == ((pjob->_ppump->_rxbuffer[0x04] & 0x0F) + 1))
                                {
                                    ClearEepromBuffer();
                                    //Storing the current Date/Time for this transaction
                                    memcpy(GetEepromBuffer(), GetRtcBuffer(), 0x07);
                                    memcpy(&GetEepromBuffer()[0x07], pjob->_ppump->_rxbuffer, pjob->_ppump->_rxbuffersize);
                                    StoreEepromPage(EEPROM_DISPENSER_PUMP42_TRANSACTION_PAGE);
                                }
                            }
                        }
                    }
                }else{//CREDIT TRANSACTION
                    //HERE WE STORE THE LAST TRANSACTION IN EEPROM! ALWAYS THE PREVIOUS TRANSACTION REPORT 
                    //WILL BE OVERWRITTEN AND LOST.
                    //IN CASE OF A "CREDIT SALE", THE WHOLE DATA WILL BE STORED IN A SEPARATE MEMORY REGION, 
                    //SINCE IN CASE OF A RF LINK DROP, THE STATE OF THE SALE COULD BE RECOVERED BY THE NSX.
                    LoadEepromPage(EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE);
                    if(GetEepromBuffer()[0] == ((pjob->_ppump->_rxbuffer[0x04] & 0x0F) + 1))
                    {
                        ClearEepromBuffer();
                        //Storing the current Date/Time for this transaction
                        memcpy(GetEepromBuffer(), GetRtcBuffer(), 0x07);
                        memcpy(&GetEepromBuffer()[0x07], pjob->_ppump->_rxbuffer, pjob->_ppump->_rxbuffersize);
                        StoreEepromPage(EEPROM_DISPENSER_PUMP14_TRANSACTION_PAGE);
                    }
                    else
                    {
                        ClearEepromBuffer();
                        LoadEepromPage(EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE);
                        if(GetEepromBuffer()[0] == ((pjob->_ppump->_rxbuffer[0x04] & 0x0F) + 1))
                        {
                            ClearEepromBuffer();
                            //Storing the current Date/Time for this transaction
                            memcpy(GetEepromBuffer(), GetRtcBuffer(), 0x07);
                            memcpy(&GetEepromBuffer()[0x07], pjob->_ppump->_rxbuffer, pjob->_ppump->_rxbuffersize);
                            StoreEepromPage(EEPROM_DISPENSER_PUMP24_TRANSACTION_PAGE);
                        }
                        else
                        {
                            ClearEepromBuffer();
                            LoadEepromPage(EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE);
                            if(GetEepromBuffer()[0] == ((pjob->_ppump->_rxbuffer[0x04] & 0x0F) + 1))
                            {
                                ClearEepromBuffer();
                                //Storing the current Date/Time for this transaction
                                memcpy(GetEepromBuffer(), GetRtcBuffer(), 0x07);
                                memcpy(&GetEepromBuffer()[0x07], pjob->_ppump->_rxbuffer, pjob->_ppump->_rxbuffersize);
                                StoreEepromPage(EEPROM_DISPENSER_PUMP34_TRANSACTION_PAGE);
                            }
                            else
                            {
                                ClearEepromBuffer();
                                LoadEepromPage(EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE);
                                if(GetEepromBuffer()[0] == ((pjob->_ppump->_rxbuffer[0x04] & 0x0F) + 1))
                                {
                                    ClearEepromBuffer();
                                    //Storing the current Date/Time for this transaction
                                    memcpy(GetEepromBuffer(), GetRtcBuffer(), 0x07);
                                    memcpy(&GetEepromBuffer()[0x07], pjob->_ppump->_rxbuffer, pjob->_ppump->_rxbuffersize);
                                    StoreEepromPage(EEPROM_DISPENSER_PUMP44_TRANSACTION_PAGE);
                                }
                            }
                        }
                    }
                }
                I2CBusUnlock();
            }
                
            //HERE WE ATTEMPT TO PUSH THE TRANSACTION INTO THE PUMP'S TRANSACTIONAL QUEUE.
            //THIS IS DONE ONLY IF THERE IS ROOM IN THE QUEUE, OTHERWISE THIS STATE WON'T
            //BE REPORTED TO THE REMOTE PEER.
            pjob->_ppump->PumpTransQueueLock(pjob->_ppump);
            PNEAR_PUMPTRANSACTIONALSTATEPTR pts = pjob->_ppump->PumpTransQueueAllocate(pjob->_ppump);
            pjob->_ppump->PumpTransQueueUnlock(pjob->_ppump);
            if(pts)
            {
                pts->_transtate = RF_MUX_CASH_SALE_END_REPORT_RESPONSE;
                memcpy(pts->_buffer, pjob->_ppump->_rxbuffer, pjob->_ppump->_rxbuffersize);
                pts->_buffersize = pjob->_ppump->_rxbuffersize;
                
                pjob->_ppump->PumpTransQueueLock(pjob->_ppump);
                pjob->_ppump->PumpTransQueueEnqueue(pjob->_ppump, pts);
                pjob->_ppump->PumpTransQueueUnlock(pjob->_ppump);
            }
            
        }  
    }
}

//@Created By: HIJH
//@Septembre de 2016
void ProcessPumpTotalsDataReport(void *pparam)
{
    PPUMPTRANSACTIONJOBCONTEXTPTR pjob = (PPUMPTRANSACTIONJOBCONTEXTPTR)pparam;
    if(pjob)
    {
        bool dataok = true;
        uint8 numhoses = 0x00;
        switch(pjob->_ppump->_pumpindex)
        {
            case 0x00:
            {
                numhoses = _g_dispenserlayoutinfo._hosesposition1;
                break;
            }
            case 0x01:
            {
                numhoses = _g_dispenserlayoutinfo._hosesposition2;
                break;
            }
        }
        
        pjob->_reenqueue = false;
        
        pjob->_ppump->PumpTransQueueLock(pjob->_ppump);
        bool transtatefound = pjob->_ppump->PumpTransQueueFind(pjob->_ppump, RF_MUX_TOTALS_REPORT_RESPONSE);
        PNEAR_PUMPTRANSACTIONALSTATEPTR pts = pjob->_ppump->PumpTransQueueAllocate(pjob->_ppump);
        PNEAR_PUMPTRANSACTIONALSTATEPTR ptstemp = pjob->_ppump->PumpTransQueueAllocate(pjob->_ppump);
        
        pjob->_ppump->PumpTransQueueUnlock(pjob->_ppump);
        pjob->_ppump->_transhealth = _PUMP_OK_;
        if(ptstemp)
        {
            if(pts && !transtatefound)
            {
                pts->_transtate = RF_MUX_TOTALS_REPORT_RESPONSE;
                
                memcpy(pts->_buffer, pjob->_ppump->_rxbuffer, pjob->_ppump->_rxbuffersize);
                pts->_buffersize = pjob->_ppump->_rxbuffersize;
                
                pjob->_ppump->PumpTransQueueDeallocate(pjob->_ppump,ptstemp);
                pjob->_ppump->PumpTransQueueLock(pjob->_ppump);
                pjob->_ppump->PumpTransQueueEnqueue(pjob->_ppump, pts);
                pjob->_ppump->PumpTransQueueUnlock(pjob->_ppump);
            }
            else
            {
                pjob->_ppump->PumpTransQueueLock(pjob->_ppump);
                pjob->_ppump->PumpTransQueueDeallocate(pjob->_ppump, pts);
                pjob->_ppump->PumpTransQueueUnlock(pjob->_ppump);
            }
            memset(pjob->_ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        }else{
            pjob->_ppump->PumpTransQueueDeallocate(pjob->_ppump, pts);
            pjob->_ppump->PumpTransQueueUnlock(pjob->_ppump);
        }
    }
}

//@Created By: HIJH
//@Septembre de 2016
void ProcessPumpTotalsDataPpuCheck(void *pparam)
{
    PPUMPTRANSACTIONJOBCONTEXTPTR pjob = (PPUMPTRANSACTIONJOBCONTEXTPTR)pparam;
    if(pjob)
    {
/*        
        
        PNEAR_PUMPTRANSACTIONALSTATEPTR pts = pjob->_ppump->PumpTransQueueAllocate(pjob->_ppump);
        if(pts)
        {
            pts->_transtate = _PUMP_POSITION_TRANSREPORT_;
            memcpy(pts->_buffer, pjob->_ppump->_rxbuffer, pjob->_ppump->_rxbuffersize);
            pts->_buffersize = pjob->_ppump->_rxbuffersize;
        }
        
        
        uint8 var = 0;//THIS IS A DUMMY VAR TO DROP THE WARNING IF THERE IS NO ASSIGNMENT!!
        uint8 index = 0;
        //All indexes were taken according to the documentation
        var = (pjob->_ppump->_rxbuffer[0x02 + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);//Hose id

        //Bytes from index 0x04 + _PUMP_TOTALS_BUFFER_SHIFT_*index to 0x0F + _PUMP_TOTALS_BUFFER_SHIFT_*index 
        //are the digits for the total fuel volume delivered until now on this pump
        //Digits are in BCD format
        var = (pjob->_ppump->_rxbuffer[0x04 + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x05 + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x06 + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x07 + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x08 + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x09 + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x0A + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x0B + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x0C + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x0D + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x0E + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x0F + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);

        //Bytes from index 0x11 + _PUMP_TOTALS_BUFFER_SHIFT_*index to 0x1C + _PUMP_TOTALS_BUFFER_SHIFT_*index 
        //are the digits for the total money charged until now on this pump
        //Digits are in BCD format
        var = (pjob->_ppump->_rxbuffer[0x11 + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x12 + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x13 + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x14 + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x15 + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x16 + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x17 + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x18 + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x19 + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x1A + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x1B + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x1C + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        
        //Bytes from index 0x1E + _PUMP_TOTALS_BUFFER_SHIFT_*index to 0x23 + _PUMP_TOTALS_BUFFER_SHIFT_*index 
        //are the digits for the reference PPU used on this pump/hose
        //Digits are in BCD format
        var = (pjob->_ppump->_rxbuffer[0x1E + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x1F + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x20 + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x21 + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x22 + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
        var = (pjob->_ppump->_rxbuffer[0x23 + _PUMP_TOTALS_BUFFER_SHIFT_*index] & 0x0F);
*/        
        memset(pjob->_ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
    }
}

//@Created By: HIJH
//@Date: Septembre de 2016
//This must be used to notify the log registry about the timeout issue
void PumpJobTimeout(void *pparam)
{
    PPUMPTRANSACTIONJOBCONTEXTPTR pjob = (PPUMPTRANSACTIONJOBCONTEXTPTR)pparam;
    if(pjob)
    {
        //Release the Current Loop and log if possible
        if(pjob->_retriescounter >= _MAX_RETRIES_)
        {
            pjob->_ppump->_pumpstate = PUMP_UNKNOWN;
            pjob->_ppump->_transhealth = _PUMP_FAIL_;
        }
    }
}

//@Created By: HIJH
//@Date: Septembre de 2016
//This must be used to notify the log registry about the timeout issue
void PumpStateJobTimeout(void *pparam)
{
    PPUMPTRANSACTIONJOBCONTEXTPTR pjob = (PPUMPTRANSACTIONJOBCONTEXTPTR)pparam;
    if(pjob)
    {
        //Release the Current Loop and log if possible
        if(pjob->_retriescounter >= _MAX_RETRIES_)
        {
            pjob->_ppump->_pumpstate = PUMP_UNKNOWN;
            pjob->_ppump->_transhealth = _PUMP_FAIL_;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////                 BLOCK OF AVAILABLE REQUEST HANDLERS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//@Created By: HIJH
//@Date: Septembre de 2016
//Sends the authorization request to the pump. This is a single byte command with no response
void PumpAuthorization(void *pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    if(ppump)
    {

        if(!ppump->_presetdone)
            return;

        uint8_t index = 0;
        memset(ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPENSER);
        if(puartdisp)
        {
            //Just one byte for the response
            puartdisp->_messagetx[index++] = PUMP_COMMAND(PUMP_AUTHORIZATION, ppump->_pumpid);
            puartdisp->_messagelength = 1;//One byte
            puartdisp->_messagestate = PENDING;
            
            ppump->_authorized = true;
        }
    }
}

//@Created By: HIJH
//@Date: Septembre de 2016
//Sends the authorization request to the pump. This is a single byte command with no response
void PumpStartupAuthorization(void *pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    if(ppump)
    {                
        uint8_t index = 0;
        memset(ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPENSER);
        if(puartdisp)
        {
            //Just one byte for the response
            puartdisp->_messagetx[index++] = PUMP_COMMAND(PUMP_AUTHORIZATION, ppump->_pumpid);
            puartdisp->_messagelength = 1;//One byte
            puartdisp->_messagestate = PENDING;
            
        }
        CyDelay(100);
    }
}

//@Created By: HIJH
//@Date: Septembre de 2016
//Sends the state request to the pump. This is a single byte command with one byte of response
void PumpStateRequest(void *pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    if(ppump)
    {
        uint8_t index = 0;
        memset(ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPENSER);
        if(puartdisp)
        {
            puartdisp->_messagetx[index++] = PUMP_COMMAND(PUMP_STATE_REQUEST, ppump->_pumpid);
            puartdisp->_messagelength = 1;//One byte
            puartdisp->_messagestate = PENDING;
        }
    }
}

//@Created By: HIJH
//@Date: Septembre de 2016
void PumpRTMoneyRequest(void *pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    if(ppump)
    {
        uint8_t index = 0;
        memset(ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPENSER);
        if(puartdisp)
        {
            puartdisp->_messagetx[index++] = PUMP_COMMAND(PUMP_REAL_TIME_MONEY_REQUEST, ppump->_pumpid);
            puartdisp->_messagelength = 1;//One byte
            puartdisp->_messagestate = PENDING;
        }
    }
}

//@Created By: HIJH
//@Date: Septembre de 2016
//Requests the last transaction data to the pump. This is a single byte command with a fixed length response.
void PumpTransactionDataRequest(void *pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    if(ppump)
    {        
        uint8_t index = 0;
        memset(ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPENSER);
        if(puartdisp)
        {
            puartdisp->_messagetx[index++] = PUMP_COMMAND(PUMP_TRANSACTION_DATA_REQUEST, ppump->_pumpid);
            puartdisp->_messagelength = 1;//One byte
            puartdisp->_messagestate = PENDING;
        }
    }
}

//@Created By: HIJH
//@Date: Septembre de 2016
//Requests the report of totals for the current pump. This is a single byte command with a variable length response,
//that depends on the number of available hoses.
void PumpTotalsDataRequest(void *pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    if(ppump)
    {        
        uint8_t index = 0;
        memset(ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPENSER);
        if(puartdisp)
        {
            puartdisp->_messagetx[index++] = PUMP_COMMAND(PUMP_TOTALS_DATA_REQUEST, ppump->_pumpid);
            puartdisp->_messagelength = 1;//One byte
            puartdisp->_messagestate = PENDING;
        }
    }
}

//@Created By: HIJH
//@Date: Septembre de 2016
//Requests the pump to enter in "SEND DATA" mode. This is a single byte command with a special one byte response,
//that indicates that the pump is ready to receive data.
void PumpDataToPump(void *pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    if(ppump)
    {
        uint8_t index = 0;        
        memset(ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPENSER);
        if(puartdisp)
        {
            puartdisp->_messagetx[index++] = PUMP_COMMAND(PUMP_DATA_TO_PUMP, ppump->_pumpid);
            puartdisp->_messagelength = 1;//One byte
            puartdisp->_messagestate = PENDING;
        }
    }
}


//@Created By: HIJH
//@Septembre de 2016
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This preset is a convention that applies to "Credit Transactions"
//The pump will hold all the necessary information for the preset.
//For this message the data comes as follows:
void PumpCreditPresetHose(void *pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    if(ppump)
    {
        uint8_t index = 0;        
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPENSER);
        memset(ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        if(puartdisp)
        {
////////////////////////////////////////////////////////////////////////////////////////////////////////////
            puartdisp->_messagetx[index++] = SOM;
            if(_g_dispenserlayoutinfo._displaydigitsmode == 0x06)
            {
                uint8 transtype = ((0xF0 | ppump->_authorizationinfo._presettype) == 0xF1)? 0xF2: 0xF1;;//preset type (0xF2 = Money, 0xF1 = Volume)
                if(transtype == 0xF1)
                    puartdisp->_messagetx[index++] = 0xE3;
                else
                    puartdisp->_messagetx[index++] = 0xE2;
                    
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0004 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE TYPE OF PRESET
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                puartdisp->_messagetx[index++] = transtype;
                puartdisp->_messagetx[index++] = 0xF4;
                puartdisp->_messagetx[index++] = 0xF6; //Hose Data next
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0006 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRODUCT TYPE
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                puartdisp->_messagetx[index++] = (0xE0 | (ppump->_authorizationinfo._hoseid & 0x0F));
                
                puartdisp->_messagetx[index++] = 0xF8; //Preset Data next
                //Here we copy the remaining 8 bytes for the preset value
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0008 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRESET VALUE (VOLUME OR MONEY)
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                memset(PBYTECAST(&ppump->_trasactionbuffer[0x0008]),0x00, 0x0008);
                B78AD90CF552D9B30A(ppump->_authorizationinfo._presetamount, PBYTECAST(&ppump->_trasactionbuffer[0x0008]));
                char8 *ptr = &ppump->_trasactionbuffer[0x0008];
                if(transtype == 0xF1) //Volume PRESET, then multiply by 1000
                {
                    CastVolumeValue(ptr, 0x08);
                    
                    FOR(uint8 buffndx = 0, buffndx < 0x05, buffndx++)
                        puartdisp->_messagetx[index++] = (0xE0 | (ptr[buffndx] & 0x0F)); //Translate from ASCII to BCD ==> LSD first!
                }
                else
                {
                    if(!(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02))
                        CastMoneyValue(ptr, 0x08);
                    
                    FOR(uint8 buffndx = 0, buffndx < 0x06, buffndx++)
                        puartdisp->_messagetx[index++] = (0xE0 | (ptr[buffndx] & 0x0F)); //Translate from ASCII to BCD ==> LSD first!
                }
            }
            else
            {
                puartdisp->_messagetx[index++] = 0xE1;
                    
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0004 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE TYPE OF PRESET
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                uint8 transtype = ((0xF0 | ppump->_authorizationinfo._presettype) == 0xF1)? 0xF2: 0xF1;;//preset type (0xF2 = Money, 0xF1 = Volume)
                puartdisp->_messagetx[index++] = transtype;

                puartdisp->_messagetx[index++] = 0xF6; //Hose Data next
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0006 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRODUCT TYPE
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                puartdisp->_messagetx[index++] = (0xE0 | (ppump->_authorizationinfo._hoseid & 0x0F));
                
                puartdisp->_messagetx[index++] = 0xF8; //Preset Data next
                //Here we copy the remaining 8 bytes for the preset value
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0008 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRESET VALUE (VOLUME OR MONEY)
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                memset(PBYTECAST(&ppump->_trasactionbuffer[0x0008]),0x00, 0x0008);
                B78AD90CF552D9B30A(ppump->_authorizationinfo._presetamount, PBYTECAST(&ppump->_trasactionbuffer[0x0008]));
                char8 *ptr = &ppump->_trasactionbuffer[0x0008];
                if(transtype == 0xF1) //Volume PRESET, then multiply by 1000
                    CastVolumeValue(ptr, 0x08);
                else if(!(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02))
                    CastMoneyValue(ptr, 0x08);
                    
                FOR(uint8 buffndx = 0, buffndx < 0x08, buffndx++)
                    puartdisp->_messagetx[index++] = (0xE0 | (ptr[buffndx] & 0x0F)); //Translate from ASCII to BCD ==> LSD first!
            }
            
            ppump->_presetdone = true;
            puartdisp->_messagetx[index++] = LRCn; //LRC next
            puartdisp->_messagetx[index++] = GetLRC(puartdisp->_messagetx);
            puartdisp->_messagetx[index++] = EOM;
////////////////////////////////////////////////////////////////////////////////////////////////////////////
            
            puartdisp->_messagelength = index;
            puartdisp->_messagestate = PENDING;
        }
    }
    
}

//@Created By: HIJH
//@Septembre de 2016
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Preset values will be placed within the SinkMessage's buffer. This is a convention that applies to 
//all of the available transactions. The pump function must deal with this while it is assembling the message.
//For this message the data comes as follows:
// - First byte is the preset type (Volume:1 / Money:2 )
// - Second byte is the Hose (A:0, B:1, C:2, D:3)
// - Block of 8 bytes for the preset value
//This block of data (10 bytes) must be moved to another position within the same buffer (even position)
void PumpPresetHose(void *pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    if(ppump)
    {
        uint8_t index = 0;        
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPENSER);
        memset(ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        if(puartdisp)
        {
////////////////////////////////////////////////////////////////////////////////////////////////////////////
            puartdisp->_messagetx[index++] = SOM;
            if(_g_dispenserlayoutinfo._displaydigitsmode == 0x06)
            {
                uint8 transtype = (0xF0 | ((ppump->_trasactionbuffer[0x0004] == 0x0F)? 0x02: 0x01));//preset type (0xF2 = Money, 0xF1 = Volume)
                if(transtype == 0xF1)
                    puartdisp->_messagetx[index++] = 0xE3;
                else
                    puartdisp->_messagetx[index++] = 0xE2;
                
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0004 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE TYPE OF PRESET
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                puartdisp->_messagetx[index++] = transtype;
                puartdisp->_messagetx[index++] = 0xF4;
                puartdisp->_messagetx[index++] = 0xF6; //Hose Data next
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0006 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRODUCT TYPE
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                puartdisp->_messagetx[index++] = (0xE0 | (0x0F & ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_SUBA_MANIJA)]));
                //puartdisp->_messagetx[index++] = 0xE0;
                _g_pumps[(ppump->_pumpid)-1]._currenthose = (0x0F & ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_SUBA_MANIJA)])+ 1;
                puartdisp->_messagetx[index++] = 0xF8; //Preset Data next
                //Here we copy the remaining 8 bytes for the preset value
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0008 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRESET VALUE (VOLUME OR MONEY)
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                char8 *ptr = &ppump->_trasactionbuffer[0x0008];
                if(transtype == 0xF1) //Volume PRESET, then multiply by 1000
                {
                    CastVolumeValue(ptr, 0x08);
                    
                    FOR(uint8 buffndx = 0, buffndx < 0x05, buffndx++)
                        puartdisp->_messagetx[index++] = (0xE0 | (ptr[7 - buffndx] & 0x0F)); //Translate from ASCII to BCD ==> LSD first!
                }
                else
                {
                    if(!(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02))
                        CastMoneyValue(ptr, 0x08);
                    
                    FOR(uint8 buffndx = 0, buffndx < 0x06, buffndx++)
                        puartdisp->_messagetx[index++] = (0xE0 | (ptr[7 - buffndx] & 0x0F)); //Translate from ASCII to BCD ==> LSD first!
                }
            }
            else
            {
                puartdisp->_messagetx[index++] = 0xE1;
                    
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0004 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE TYPE OF PRESET
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                uint8 transtype = (0xF0 | ((ppump->_trasactionbuffer[0x0004] == 0x0F)? 0x02: 0x01));//preset type (0xF2 = Money, 0xF1 = Volume)
                puartdisp->_messagetx[index++] = transtype;

                puartdisp->_messagetx[index++] = 0xF6; //Hose Data next
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0006 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRODUCT TYPE
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                puartdisp->_messagetx[index++] = (0xE0 | (0x0F & ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_SUBA_MANIJA)]));
                //puartdisp->_messagetx[index++] = 0xE0;
                _g_pumps[(ppump->_pumpid)-1]._currenthose = (0x0F & ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_SUBA_MANIJA)])+ 1;
                puartdisp->_messagetx[index++] = 0xF8; //Preset Data next
                //Here we copy the remaining 8 bytes for the preset value
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0008 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRESET VALUE (VOLUME OR MONEY)
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                char8 *ptr = &ppump->_trasactionbuffer[0x0008];
                if(transtype == 0xF1) //Volume PRESET, then multiply by 1000
                    CastVolumeValue(ptr, 0x08);
                else if(!(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02))
                    CastMoneyValue(ptr, 0x08);
                    
                FOR(uint8 buffndx = 0, buffndx < 0x08, buffndx++)
                    puartdisp->_messagetx[index++] = (0xE0 | (ptr[7 - buffndx] & 0x0F)); //Translate from ASCII to BCD ==> LSD first!
            }
            ppump->_presetdone = true;
            puartdisp->_messagetx[index++] = LRCn; //LRC next
            puartdisp->_messagetx[index++] = GetLRC(puartdisp->_messagetx);
            puartdisp->_messagetx[index++] = EOM;
////////////////////////////////////////////////////////////////////////////////////////////////////////////
            
            puartdisp->_messagelength = index;
            puartdisp->_messagestate = PENDING;
        }
    }
    
}

//@Created By: HIJH
//@Septembre de 2016
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Preset values will be placed within the SinkMessage's buffer. This is a convention that applies to 
//all of the available transactions. The pump function must deal with this while it is assembling the message.
//For this message the data comes as follows:
// - First byte is the preset type (Volume:1 / Money:2 )
// - Block of 8 bytes for the preset value
//This block of data (10 bytes) must be moved to another position within the same buffer (even position)
void PumpPresetNoHose(void *pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    if(ppump)
    {
        uint8_t index = 0;
        memset(ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPENSER);
        if(puartdisp)
        {
////////////////////////////////////////////////////////////////////////////////////////////////////////////
            puartdisp->_messagetx[index++] = SOM;
            if(_g_dispenserlayoutinfo._displaydigitsmode == 0x06)
            {
                uint8 transtype = (0xF0 | ((ppump->_trasactionbuffer[0x0004] == 0x0F)? 0x02: 0x01));//preset type (0xF2 = Money, 0xF1 = Volume)
                if(transtype == 0xF1)
                    puartdisp->_messagetx[index++] = 0xE6;
                else
                    puartdisp->_messagetx[index++] = 0xE5;

                ///THIS IS AN APPLIED CONVENTION ===>> 0x0004 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE TYPE OF PRESET
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                puartdisp->_messagetx[index++] = transtype;
                            
                puartdisp->_messagetx[index++] = 0xF8; //Preset Data next
                //Here we copy the remaining 8 bytes for the preset value
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0008 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRESET VALUE (VOLUME OR MONEY)
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                char8 *ptr = &ppump->_trasactionbuffer[0x0008];
                if(transtype == 0xF1) //Volume PRESET, then multiply by 1000
                {
                    CastVolumeValue(ptr, 0x08);
                
                    FOR(uint8 buffndx = 0, buffndx < 0x05, buffndx++)
                        puartdisp->_messagetx[index++] = (0xE0 | (ptr[7 - buffndx] & 0x0F)); //Translate from ASCII to BCD ==> LSD first!
                }
                else
                {
                    if(!(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02))
                        CastMoneyValue(ptr, 0x08);
                    
                    FOR(uint8 buffndx = 0, buffndx < 0x06, buffndx++)
                        puartdisp->_messagetx[index++] = (0xE0 | (ptr[7 - buffndx] & 0x0F)); //Translate from ASCII to BCD ==> LSD first!
                }
            }
            else
            {
                puartdisp->_messagetx[index++] = 0xE3;

                ///THIS IS AN APPLIED CONVENTION ===>> 0x0004 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE TYPE OF PRESET
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                uint8 transtype = (0xF0 | ((ppump->_trasactionbuffer[0x0004] == 0x0F)? 0x02: 0x01));//preset type (0xF2 = Money, 0xF1 = Volume)
                puartdisp->_messagetx[index++] = transtype;
                            
                puartdisp->_messagetx[index++] = 0xF8; //Preset Data next
                //Here we copy the remaining 8 bytes for the preset value
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0008 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRESET VALUE (VOLUME OR MONEY)
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                char8 *ptr = &ppump->_trasactionbuffer[0x0008];
                if(transtype == 0xF1) //Volume PRESET, then multiply by 1000
                    CastVolumeValue(ptr, 0x08);
                else if(!(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02))
                    CastMoneyValue(ptr, 0x08);
                
                FOR(uint8 buffndx = 0, buffndx < 0x08, buffndx++)
                    puartdisp->_messagetx[index++] = (0xE0 | (ptr[7 - buffndx] & 0x0F)); //Translate from ASCII to BCD ==> LSD first!
            }
            ppump->_presetdone = true;
            puartdisp->_messagetx[index++] = LRCn; //LRC next
            puartdisp->_messagetx[index++] = GetLRC(puartdisp->_messagetx);
            puartdisp->_messagetx[index++] = EOM;
////////////////////////////////////////////////////////////////////////////////////////////////////////////
            
            puartdisp->_messagelength = index;
            puartdisp->_messagestate = PENDING;
        }
    }
    
}

//@Created By: HIJH
//@Septembre de 2016
//Message to be notified to the remote peer (NSX)
void PumpPresetNotification(LPVOID pparam)
{
    PPUMPTRANSACTIONJOBCONTEXTPTR pjob = (PPUMPTRANSACTIONJOBCONTEXTPTR)pparam;
    if(pjob)
    {
        //HERE WE ATTEMPT TO PUSH THE TRANSACTION INTO THE PUMP'S TRANSACTIONAL QUEUE.
        //THIS IS DONE ONLY IF THERE IS ROOM IN THE QUEUE, OTHERWISE THIS STATE WON'T
        //BE REPORTED TO THE REMOTE PEER.
        pjob->_ppump->PumpTransQueueLock(pjob->_ppump);
        PNEAR_PUMPTRANSACTIONALSTATEPTR pts = pjob->_ppump->PumpTransQueueAllocate(pjob->_ppump);
        pjob->_ppump->PumpTransQueueUnlock(pjob->_ppump);
        if(pts)
        {
            uint8 index = 0;
            pts->_transtate = RF_MUX_CASH_SALE_START_REQUEST;
            //Pump ID always comes in position 0x00
            pts->_buffer[index++] = pjob->_ppump->_pumpid;
            //PRESET type
            pts->_buffer[index++] = (0xF0 | ((pjob->_ppump->_trasactionbuffer[0x0004] == 0x0F)? 0x02: 0x01));//preset type (0xF2 = Money, 0xF1 = Volume)
            
            //PRESET value (8 bytes)
            PSTRBUFFTOGG(PBYTECAST(&pjob->_ppump->_trasactionbuffer[0x0008]), 0x08);
            memcpy(&pts->_buffer[index], &pjob->_ppump->_trasactionbuffer[0x0008], 0x08);
            index += 0x08;
            //Car's Plate
            memcpy(&pts->_buffer[index], &pjob->_ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_DIGITE_PLACA)], GetBufferLengthFromDisplayID(DISPLAY_DIGITE_PLACA));
            index += GetBufferLengthFromDisplayID(DISPLAY_DIGITE_PLACA);
            pts->_buffer[index] = index;
            //Car's Milleage
            memcpy(&pts->_buffer[index], &pjob->_ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE)], GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE));
            index += GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE);
            
            pts->_buffersize = index;
            
            PNEAR_PUMPTRANSACTIONALSTATEPTR ptstemp = pjob->_ppump->PumpTransQueueAllocate(pjob->_ppump);
            if(ptstemp)
            {
                pjob->_ppump->PumpTransQueueDeallocate(pjob->_ppump,ptstemp);
                pjob->_ppump->PumpTransQueueLock(pjob->_ppump);
                pjob->_ppump->PumpTransQueueEnqueue(pjob->_ppump, pts);
                pjob->_ppump->PumpTransQueueUnlock(pjob->_ppump);
            }else
                pjob->_ppump->PumpTransQueueDeallocate(pjob->_ppump, pts);
        }
    }
}


//@Created By: HIJH
//@Septembre de 2016
//Message to be notified to the remote peer (NSX)
void PumpCreditPresetNotification(LPVOID pparam)
{
    PPUMPTRANSACTIONJOBCONTEXTPTR pjob = (PPUMPTRANSACTIONJOBCONTEXTPTR)pparam;
    if(pjob)
    {
        //HERE WE ATTEMPT TO PUSH THE TRANSACTION INTO THE PUMP'S TRANSACTIONAL QUEUE.
        //THIS IS DONE ONLY IF THERE IS ROOM IN THE QUEUE, OTHERWISE THIS STATE WON'T
        //BE REPORTED TO THE REMOTE PEER.
        pjob->_ppump->PumpTransQueueLock(pjob->_ppump);
        PNEAR_PUMPTRANSACTIONALSTATEPTR pts = pjob->_ppump->PumpTransQueueAllocate(pjob->_ppump);
        pjob->_ppump->PumpTransQueueUnlock(pjob->_ppump);
        if(pts)
        {
            uint8 index = 0;
            pts->_transtate = RF_MUX_CREDIT_PRESET_NOTIFICATION_REQUEST;
            //Pump ID always comes in position 0x00
            pts->_buffer[index++] = pjob->_ppump->_pumpid;
            pts->_buffersize = index;
            
            PNEAR_PUMPTRANSACTIONALSTATEPTR ptstemp = pjob->_ppump->PumpTransQueueAllocate(pjob->_ppump);
            if(ptstemp)
            {
                pjob->_ppump->PumpTransQueueDeallocate(pjob->_ppump,ptstemp);
                pjob->_ppump->PumpTransQueueLock(pjob->_ppump);
                pjob->_ppump->PumpTransQueueEnqueue(pjob->_ppump, pts);
                pjob->_ppump->PumpTransQueueUnlock(pjob->_ppump);
            }else
                pjob->_ppump->PumpTransQueueDeallocate(pjob->_ppump, pts);
        }
    }
}

//@Created By: HIJH
//@Septembre de 2016
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Preset values will be placed within the SinkMessage's buffer. This is a convention that applies to 
//all of the available transactions. The pump function must deal with this while it is assembling the message.
//For this message the data comes as follows:
// - First byte is the preset type (Volume:1 / Money:2 )
// - Second byte is the Hose (A:0, B:1, C:2, D:3)
// - Block of 8 bytes for the preset value
//This block of data (10 bytes) must be moved to another position within the same buffer (even position)
void PumpFullPresetHose(void *pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    if(ppump)
    {
        uint8_t index = 0;        
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPENSER);
        memset(ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        if(puartdisp)
        {
////////////////////////////////////////////////////////////////////////////////////////////////////////////
            puartdisp->_messagetx[index++] = SOM;
            if(_g_dispenserlayoutinfo._displaydigitsmode == 0x06)
            {
                puartdisp->_messagetx[index++] = 0xE2;

                ///THIS IS AN APPLIED CONVENTION ===>> 0x0004 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE TYPE OF PRESET
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                uint8 transtype = 0xF1;//preset type (0xF2 = Money, 0xF1 = Volume)
                puartdisp->_messagetx[index++] = transtype;//preset type (0xF2 = Money, 0xF1 = Volume)
                puartdisp->_messagetx[index++] = 0xF4;
                puartdisp->_messagetx[index++] = 0xF6; //Hose Data next
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0006 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRODUCT TYPE
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                puartdisp->_messagetx[index++] = (0xE0 | (0x0F & ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_SUBA_MANIJA)]));
                _g_pumps[(ppump->_pumpid)-1]._currenthose = (0x0F & ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_SUBA_MANIJA)])+ 1;
                puartdisp->_messagetx[index++] = 0xF8; //Preset Data next
                //Here we copy the remaining 8 bytes for the preset value
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0008 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRESET VALUE (VOLUME OR MONEY)
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                char8 *ptr = &ppump->_trasactionbuffer[0x0008];
                B78AD90CF552D9B30A(10 * _PUMP_MAXIMUM_PRESET_GALLONS_, (LPRAWPTR)ptr);

                PSTRBUFFTOGG(PBYTECAST(&ppump->_trasactionbuffer[0x0008]), 0x08);
                if(transtype == 0xF1) //Volume PRESET, then multiply by 1000
                    CastVolumeValue(ptr, 0x08);
                else if(!(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02))
                    CastMoneyValue(ptr, 0x08);

                FOR(uint8 buffndx = 0, buffndx < 0x06, buffndx++)
                    puartdisp->_messagetx[index++] = (0xE0 | (ptr[7 - buffndx] & 0x0F)); //Translate from ASCII to BCD ==> LSD first!
                    
            }
            else
            {
                puartdisp->_messagetx[index++] = 0xE1;

                ///THIS IS AN APPLIED CONVENTION ===>> 0x0004 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE TYPE OF PRESET
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                uint8 transtype = 0xF1;//preset type (0xF2 = Money, 0xF1 = Volume)
                puartdisp->_messagetx[index++] = transtype;//preset type (0xF2 = Money, 0xF1 = Volume)
                puartdisp->_messagetx[index++] = 0xF4;
                puartdisp->_messagetx[index++] = 0xF6; //Hose Data next
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0006 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRODUCT TYPE
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                puartdisp->_messagetx[index++] = (0xE0 | (0x0F & ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_SUBA_MANIJA)]));
                _g_pumps[(ppump->_pumpid)-1]._currenthose = (0x0F & ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_SUBA_MANIJA)])+ 1;
                puartdisp->_messagetx[index++] = 0xF8; //Preset Data next
                //Here we copy the remaining 8 bytes for the preset value
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0008 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRESET VALUE (VOLUME OR MONEY)
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                char8 *ptr = &ppump->_trasactionbuffer[0x0008];
                B78AD90CF552D9B30A(_PUMP_MAXIMUM_PRESET_GALLONS_, (LPRAWPTR)ptr);

                PSTRBUFFTOGG(PBYTECAST(&ppump->_trasactionbuffer[0x0008]), 0x08);
                if(transtype == 0xF1) //Volume PRESET, then multiply by 1000
                    CastVolumeValue(ptr, 0x08);
                else if(!(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02))
                    CastMoneyValue(ptr, 0x08);

                FOR(uint8 buffndx = 0, buffndx < 0x08, buffndx++)
                    puartdisp->_messagetx[index++] = (0xE0 | (ptr[7 - buffndx] & 0x0F)); //Translate from ASCII to BCD ==> LSD first!
                    
            }
            ppump->_presetdone = true;
            puartdisp->_messagetx[index++] = LRCn; //LRC next
            puartdisp->_messagetx[index++] = GetLRC(puartdisp->_messagetx);
            puartdisp->_messagetx[index++] = EOM;
////////////////////////////////////////////////////////////////////////////////////////////////////////////
            
            puartdisp->_messagelength = index;
            puartdisp->_messagestate = PENDING;
        }
    }
    
}

//@Created By: HIJH
//@Septembre de 2016
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Preset values will be placed within the SinkMessage's buffer. This is a convention that applies to 
//all of the available transactions. The pump function must deal with this while it is assembling the message.
//For this message the data comes as follows:
// - First byte is the preset type (Volume:1 / Money:2 )
// - Second byte is the Hose (A:0, B:1, C:2, D:3)
// - Block of 8 bytes for the preset value
//This block of data (10 bytes) must be moved to another position within the same buffer (even position)
void PumpFullPresetNoHose(void *pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    if(ppump)
    {
        uint8_t index = 0;        
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPENSER);
        memset(ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        if(puartdisp)
        {
            //This is required for the "Temination Handler" that will post the preset to the remote peer (NSX)
            ppump->_trasactionbuffer[0x0004] = 0x0F;
////////////////////////////////////////////////////////////////////////////////////////////////////////////
            puartdisp->_messagetx[index++] = SOM;
            if(_g_dispenserlayoutinfo._displaydigitsmode == 0x06)
            {
                puartdisp->_messagetx[index++] = 0xE6;
                
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0004 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE TYPE OF PRESET
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                uint8 transtype = 0xF1;//preset type (0xF2 = Money, 0xF1 = Volume)
                puartdisp->_messagetx[index++] = transtype;//preset type (0xF2 = Money, 0xF1 = Volume)
                
                puartdisp->_messagetx[index++] = 0xF8; //Preset Data next
                //Here we copy the remaining 8 bytes for the preset value
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0008 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRESET VALUE (VOLUME OR MONEY)
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                char8 *ptr = &ppump->_trasactionbuffer[0x0008];
                //B78AD90CF552D9B30A(_PUMP_MAXIMUM_PRESET_6_DIGITS_, (LPRAWPTR)ptr);
                B78AD90CF552D9B30A(10 * _PUMP_MAXIMUM_PRESET_GALLONS_, (LPRAWPTR)ptr);

                PSTRBUFFTOGG(PBYTECAST(&ppump->_trasactionbuffer[0x0008]), 0x08);
                if(transtype == 0xF1) //Volume PRESET, then multiply by 1000
                    CastVolumeValue(ptr, 0x08);
                else if(!(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02))
                    CastMoneyValue(ptr, 0x08);

                FOR(uint8 buffndx = 0, buffndx < 0x06, buffndx++)
                    puartdisp->_messagetx[index++] = (0xE0 | (ptr[7 - buffndx] & 0x0F)); //Translate from ASCII to BCD ==> LSD first!
                    
            }
            else
            {
                puartdisp->_messagetx[index++] = 0xE3;
                
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0004 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE TYPE OF PRESET
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                uint8 transtype = 0xF1;//preset type (0xF2 = Money, 0xF1 = Volume)
                puartdisp->_messagetx[index++] = 0xF1;//preset type (0xF2 = Money, 0xF1 = Volume)
                
                puartdisp->_messagetx[index++] = 0xF8; //Preset Data next
                //Here we copy the remaining 8 bytes for the preset value
                ///THIS IS AN APPLIED CONVENTION ===>> 0x0008 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRESET VALUE (VOLUME OR MONEY)
                ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
                char8 *ptr = &ppump->_trasactionbuffer[0x0008];
                //B78AD90CF552D9B30A(_PUMP_MAXIMUM_PRESET_6_DIGITS_, (LPRAWPTR)ptr);
                B78AD90CF552D9B30A(_PUMP_MAXIMUM_PRESET_GALLONS_, (LPRAWPTR)ptr);

                PSTRBUFFTOGG(PBYTECAST(&ppump->_trasactionbuffer[0x0008]), 0x08);
                if(transtype == 0xF1) //Volume PRESET, then multiply by 1000
                    CastVolumeValue(ptr, 0x08);
                else if(!(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02))
                    CastMoneyValue(ptr, 0x08);

                FOR(uint8 buffndx = 0, buffndx < 0x08, buffndx++)
                    puartdisp->_messagetx[index++] = (0xE0 | (ptr[7 - buffndx] & 0x0F)); //Translate from ASCII to BCD ==> LSD first!
            }
            ppump->_presetdone = true;
            puartdisp->_messagetx[index++] = 0xFB; //LRC next
            puartdisp->_messagetx[index++] = GetLRC(puartdisp->_messagetx);
            puartdisp->_messagetx[index++] = EOM;
////////////////////////////////////////////////////////////////////////////////////////////////////////////
            
            puartdisp->_messagelength = index;
            puartdisp->_messagestate = PENDING;
        }
    }
    
}

//@Created By: HIJH
//@Septembre de 2016
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//PPU value will be placed within the SinkMessage's buffer. This is a convention that applies to 
//all of the available transactions. The pump function must deal with this while it is assembling the message.
//For this message the data comes as follows:
// - First byte is the Hose (A:0, B:1, C:2, D:3)
// - Block of 6 bytes for the PPU value
//This block of data (6 bytes) must be moved to another position within the same buffer (even position)
void PumpPpuChange(void *pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    if(ppump)
    {
        uint8_t index = 0;  
        memset(ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPENSER);
        if(puartdisp)
        {
////////////////////////////////////////////////////////////////////////////////////////////////////////////
            puartdisp->_messagetx[index++] = SOM;
            if(_g_dispenserlayoutinfo._displaydigitsmode == 0x06)
                puartdisp->_messagetx[index++] = 0xE5;//Standard Price Data
            else
                puartdisp->_messagetx[index++] = 0xE3;//Long Price Data - Extended Two-Wire Mode
                
            puartdisp->_messagetx[index++] = 0xF4;
            puartdisp->_messagetx[index++] = 0xF6; //Hose Data next
            ///THIS IS AN APPLIED CONVENTION ===>> 0x0006 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRODUCT TYPE
            ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
            puartdisp->_messagetx[index++] = (0xE0 | (0x0F & ppump->_currenthose));
            
            I2CBusLock();
            ClearEepromBuffer();
            uint16 eeprompage = DispenserGetEepromPPUPageFromPumpID(ppump->_pumpid);
            LoadEepromPage(eeprompage);
            switch(ppump->_currenthose)
            {
                case 0x00:
                //Six bytes only since the PPU is constrained to this length
                memcpy(PBYTECAST(&ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD)]), &GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_], GetBufferLengthFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD));
                break;
                case 0x01:
                //Six bytes only since the PPU is constrained to this length
                memcpy(PBYTECAST(&ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD)]), &GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_], GetBufferLengthFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD));
                break;
                case 0x02:
                //Six bytes only since the PPU is constrained to this length
                memcpy(PBYTECAST(&ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD)]), &GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_], GetBufferLengthFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD));
                break;
                case 0x03:
                //Six bytes only since the PPU is constrained to this length
                memcpy(PBYTECAST(&ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD)]), &GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_], GetBufferLengthFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD));
                break;
            }
            I2CBusUnlock();

            puartdisp->_messagetx[index++] = 0xF7; //PPU Data next            
            //Here we copy the remaining 6 bytes for the preset value
            ///THIS IS AN APPLIED CONVENTION ===>> 0x0008 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRESET VALUE (VOLUME OR MONEY)
            ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
            uint8 bufferlength = GetBufferLengthFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD);
            char8 *ptr = &ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD)];
            
            if(_g_dispenserlayoutinfo._displaydigitsmode == 0x06)
            {
                FOR(uint8 buffndx = 0, buffndx < 4, buffndx++)//Standard Price Data
                    puartdisp->_messagetx[index++] = (0xE0 | (0x0F & ptr[buffndx])); //Translate from ASCII to BCD ==> LSD first!
            }
            else
            {
                FOR(uint8 buffndx = 0, buffndx < 6, buffndx++)//Long Price Data - Extended Two-Wire Mode
                    puartdisp->_messagetx[index++] = (0xE0 | (0x0F & ptr[buffndx])); //Translate from ASCII to BCD ==> LSD first!
            }

            memset(&ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD)], 0x00, GetBufferLengthFromDisplayID(DISPLAY_PRECIO_POR_UNIDAD));
            puartdisp->_messagetx[index++] = 0xFB; //LRC next
            puartdisp->_messagetx[index++] = GetLRC(puartdisp->_messagetx);
            puartdisp->_messagetx[index++] = EOM;
////////////////////////////////////////////////////////////////////////////////////////////////////////////
            
            puartdisp->_messagelength = index;
            puartdisp->_messagestate = PENDING;
            
            ppump->_currenthose++;
        }
    }    
}

void PumpDefault2CreditPpuChange(void *pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    if(ppump)
    {
        uint8_t index = 0;  
        memset(ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPENSER);
        if(puartdisp)
        {
////////////////////////////////////////////////////////////////////////////////////////////////////////////
            puartdisp->_messagetx[index++] = SOM;
            if(_g_dispenserlayoutinfo._displaydigitsmode == 0x06)
                puartdisp->_messagetx[index++] = 0xE5;//Standard Price Data
            else
                puartdisp->_messagetx[index++] = 0xE3;//Long Price Data - Extended Two-Wire Mode
            
            puartdisp->_messagetx[index++] = 0xF4;
            puartdisp->_messagetx[index++] = 0xF6; //Hose Data next
            ///THIS IS AN APPLIED CONVENTION ===>> 0x0006 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRODUCT TYPE
            ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
            puartdisp->_messagetx[index++] = (0xE0 | ppump->_authorizationinfo._hoseid);
            
            puartdisp->_messagetx[index++] = 0xF7; //PPU Data next            
            //Here we copy the remaining 6 bytes for the preset value
            ///THIS IS AN APPLIED CONVENTION ===>> 0x0008 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRESET VALUE (VOLUME OR MONEY)
            ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
            B78AD90CF552D9B30A(ppump->_authorizationinfo._price, PBYTECAST(&ppump->_trasactionbuffer[0x0008]));
            char8 *ptr = &ppump->_trasactionbuffer[0x0008];
            
            if(_g_dispenserlayoutinfo._displaydigitsmode == 0x06)
            {
                FOR(uint8 buffndx = 0, buffndx < 4, buffndx++)//Standard Price Data
                    puartdisp->_messagetx[index++] = (0xE0 | (0x0F & ptr[buffndx])); //Translate from ASCII to BCD ==> LSD first!
            }
            else
            {
                FOR(uint8 buffndx = 0, buffndx < 6, buffndx++)//Long Price Data - Extended Two-Wire Mode
                    puartdisp->_messagetx[index++] = (0xE0 | (0x0F & ptr[buffndx])); //Translate from ASCII to BCD ==> LSD first!
            }
            
            puartdisp->_messagetx[index++] = 0xFB; //LRC next
            puartdisp->_messagetx[index++] = GetLRC(puartdisp->_messagetx);
            puartdisp->_messagetx[index++] = EOM;
////////////////////////////////////////////////////////////////////////////////////////////////////////////
            
            puartdisp->_messagelength = index;
            puartdisp->_messagestate = PENDING;
            
            //This marker flag is set to notify the transactional engine about the price restore request at the end of the transaction
            ppump->_restoreprice = true;
        }
    }    
}

void PumpCredit2DefaultPpuChange(void *pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    if(ppump)
    {
        uint8_t index = 0;  
        memset(ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPENSER);
        if(puartdisp)
        {
////////////////////////////////////////////////////////////////////////////////////////////////////////////
            puartdisp->_messagetx[index++] = SOM;
            if(_g_dispenserlayoutinfo._displaydigitsmode == 0x06)
                puartdisp->_messagetx[index++] = 0xE5;//Standard Price Data
            else
                puartdisp->_messagetx[index++] = 0xE3;//Long Price Data - Extended Two-Wire Mode
            
            puartdisp->_messagetx[index++] = 0xF4;
            puartdisp->_messagetx[index++] = 0xF6; //Hose Data next
            ///THIS IS AN APPLIED CONVENTION ===>> 0x0006 INDEX POSITION WITHIN THE PUMP'S BUFFER POINTS TO THE PRODUCT TYPE
            ///THESE CODES MUST BE MATCHED OTHERWISE THE LOGIC WILL BREAKUP THE CODE!
            puartdisp->_messagetx[index++] = (0xE0 | ppump->_authorizationinfo._hoseid);

            puartdisp->_messagetx[index++] = 0xF7; //PPU Data next
            
            I2CBusLock();
            ClearEepromBuffer();
            uint16 eeprompage = DispenserGetEepromPPUPageFromPumpID(ppump->_pumpid);
            LoadEepromPage(eeprompage);
            switch(ppump->_authorizationinfo._hoseid)
            {
                case 0x00:
                //Six bytes only since the PPU is constrained to this length
                memcpy(PBYTECAST(&ppump->_trasactionbuffer[0x0008]), &GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_], 0x06);
                break;
                case 0x01:
                //Six bytes only since the PPU is constrained to this length
                memcpy(PBYTECAST(&ppump->_trasactionbuffer[0x0008]), &GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_], 0x06);
                break;
                case 0x02:
                //Six bytes only since the PPU is constrained to this length
                memcpy(PBYTECAST(&ppump->_trasactionbuffer[0x0008]), &GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_], 0x06);
                break;
                case 0x03:
                //Six bytes only since the PPU is constrained to this length
                memcpy(PBYTECAST(&ppump->_trasactionbuffer[0x0008]), &GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_], 0x06);
                break;
            }
            I2CBusUnlock();

            char8 *ptr = &ppump->_trasactionbuffer[0x0008];
            
            if(_g_dispenserlayoutinfo._displaydigitsmode == 0x06)
            {
                FOR(uint8 buffndx = 0, buffndx < 4, buffndx++)//Standard Price Data
                    puartdisp->_messagetx[index++] = (0xE0 | (0x0F & ptr[buffndx])); //Translate from ASCII to BCD ==> LSD first!
            }
            else
            {
                FOR(uint8 buffndx = 0, buffndx < 6, buffndx++)//Long Price Data - Extended Two-Wire Mode
                    puartdisp->_messagetx[index++] = (0xE0 | (0x0F & ptr[buffndx])); //Translate from ASCII to BCD ==> LSD first!
            }
            
            puartdisp->_messagetx[index++] = 0xFB; //LRC next
            puartdisp->_messagetx[index++] = GetLRC(puartdisp->_messagetx);
            puartdisp->_messagetx[index++] = EOM;
////////////////////////////////////////////////////////////////////////////////////////////////////////////
            
            puartdisp->_messagelength = index;
            puartdisp->_messagestate = PENDING;
        }
        ppump->_restoreprice = false;
        //This lock is to force the operator to activate/deactivate the hose once the credit transaction has finished
        #ifdef CREDIT_SALE_PUMP_LOCK
        ppump->_pumplocked = true;
        #endif
    }    
}

//@Created By: HIJH
//@Septembre de 2016
void PumpStopRequest(void *pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    if(ppump)
    {                
        uint8_t index = 0;
        memset(ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPENSER);
        if(puartdisp)
        {
            //Just one byte for the response
            puartdisp->_messagetx[index++] = PUMP_COMMAND(PUMP_PUMP_STOP, ppump->_pumpid);
            puartdisp->_messagelength = 1;//One byte
            puartdisp->_messagestate = PENDING;
        }        
    }
}

//@Created By: HIJH
//@Septembre de 2016
void PumpStopAllRequest(void *pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    if(ppump)
    {                
        uint8_t index = 0;
        memset(ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPENSER);
        if(puartdisp)
        {
            //Just one byte for the response
            puartdisp->_messagetx[index++] = PUMP_COMMAND(PUMP_ALL_PUMPS_STOP, ppump->_pumpid);
            puartdisp->_messagelength = 1;//One byte
            puartdisp->_messagestate = PENDING;
        }        
    }
}

//@Created By: HIJH
//@Septembre de 2016
void PumpCompleteConfiguration(LPVOID pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    if(ppump)
    {
        uint8_t index = 0;  
        memset(ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPENSER);
        if(puartdisp)
        {
////////////////////////////////////////////////////////////////////////////////////////////////////////////
            puartdisp->_messagetx[index++] = SOM;//Start Of Message
            puartdisp->_messagetx[index++] = 0xE9;//Data Length
            puartdisp->_messagetx[index++] = 0xFE;//Special Functions Next (0xB0 0xB0 0xBE)
            puartdisp->_messagetx[index++] = 0xEE;//Complete Pump Configuration Request (sf1)
            puartdisp->_messagetx[index++] = 0xE0;//Complete Pump Configuration Request (sf2)
            puartdisp->_messagetx[index++] = 0xE0;//Complete Pump Configuration Request (sf3)
            puartdisp->_messagetx[index++] = 0xFB;
            puartdisp->_messagetx[index++] = GetLRC(puartdisp->_messagetx);
            puartdisp->_messagetx[index++] = EOM;
////////////////////////////////////////////////////////////////////////////////////////////////////////////
            
            puartdisp->_messagelength = index;
            puartdisp->_messagestate = PENDING;
        }
    }
}

//@Created By: HIJH
//@Date: Septembre de 2016
//Receives the complete pump configuration information
void AcquirePumpCompleteConfiguration(void *pparam)
{
    PPUMPTRANSACTIONJOBCONTEXTPTR pjob = (PPUMPTRANSACTIONJOBCONTEXTPTR)pparam;
    if(pjob)    
    {
        uint8 index = 0;
        //NOT USED!! JUST FOR DEBUGGING PURPOSES!
        //The "index" variable has been kept in some lines of code just for reference.
        uint8 remblocks = (TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[7 + 45*index]) << 0x04) | TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[8 + 45*index]);
        //HERE WE ACQUIRE THE MISCELLANEOUS PUMP CONFIGURATION INFORMATION AND STORE IT INTO
        //THE PERSISTENT FILE SYSTEM (EEPROM)        
        if(pjob->_ppump->_rxbuffer[0x00] == 0xBA)
        {
            pjob->_ppump->_transhealth = _PUMP_OK_;
            ClearEepromBuffer();
            bool targetmemoryareafound = false;
            uint16 eeprompumpconfigmemorypageid = 0;
            //WE ALWAYS READ THE PAGE FROM THE EEPROM BEFORE SINCE AFTER MODIFYING ANY FIELD IT MUST BE STORED BACK
            eeprompumpconfigmemorypageid = (uint16)EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE;
            LoadEepromPage(eeprompumpconfigmemorypageid);
            if(TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[0x03]) != (GetEepromBuffer()[0x00] & 0x0F))
            {
                eeprompumpconfigmemorypageid = (uint16)EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE;
                LoadEepromPage(eeprompumpconfigmemorypageid);
                if(TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[0x03]) != (GetEepromBuffer()[0x00] & 0x0F))
                {
                    eeprompumpconfigmemorypageid = (uint16)EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE;
                    LoadEepromPage(eeprompumpconfigmemorypageid);
                    if(TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[0x03]) != (GetEepromBuffer()[0x00] & 0x0F))
                    {
                        eeprompumpconfigmemorypageid = (uint16)EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE;
                        LoadEepromPage(eeprompumpconfigmemorypageid);
                        if(TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[0x03]) == (GetEepromBuffer()[0x00] & 0x0F))
                            targetmemoryareafound = true;
                            
                    }else
                        targetmemoryareafound = true;
                }else
                    targetmemoryareafound = true;
            }else
                targetmemoryareafound = true;
            
            ///////////////////////////////////////////////////////////////////////////////////////
            //JUST RETURN SINCE IT MEANS THAT THE PUMP/POSITION IDENTIFIERS HAVE NOT YET BEEN DEFINED!!
            if(!targetmemoryareafound)
                return;
                
            //THE FIRST 16 BYTES ARE RESERVED FOR LOCAL PUMP CONFIGURATION INFORMATION
            LPRAWPTR ptreeprom = &(GetEepromBuffer()[0x10]);
            
            uint8 blkindex = 0;
            uint8 msglength = (TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[0x01]) << 0x04) | TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[0x02]);
            
            //0
            //Pump Type / Unit Type Code
            *ptreeprom = ((TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[0x09 + 2*blkindex]) & 0x0F) << 4) |
                            (TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[0x09 + (2*blkindex + 1)]) & 0x0F);

            //1
            ptreeprom++;
            blkindex++;
            //Conversion Factor Code
            *ptreeprom = ((TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[0x09 + 2*blkindex]) & 0x0F) << 4) |
                            (TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[0x09 + (2*blkindex + 1)]) & 0x0F);

            //2
            ptreeprom++;
            blkindex++;
            //Money Decimal Point Code
            *ptreeprom = ((TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[0x09 + 2*blkindex]) & 0x0F) << 4) |
                            (TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[0x09 + (2*blkindex + 1)]) & 0x0F);
                            
            //3
            ptreeprom++;
            blkindex++;
            //Volume Decimal Point Code
            *ptreeprom = ((TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[0x09 + 2*blkindex]) & 0x0F) << 4) |
                            (TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[0x09 + (2*blkindex + 1)]) & 0x0F);
            
            //4
            ptreeprom++;
            blkindex++;
            //PPU Decimal Point Code
            *ptreeprom = ((TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[0x09 + 2*blkindex]) & 0x0F) << 4) |
                            (TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[0x09 + (2*blkindex + 1)]) & 0x0F);

            //5
            ptreeprom++;
            blkindex++;
            //5/6/7/8 Digit Money Mode
            *ptreeprom = ((TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[0x09 + 2*blkindex]) & 0x0F) << 4) |
                            (TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[0x09 + (2*blkindex + 1)]) & 0x0F);

            //6
            ptreeprom++;
            blkindex++;
            //Auto On/Push to Start Mode
            *ptreeprom = ((TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[0x09 + 2*blkindex]) & 0x0F) << 4) |
                            (TranslateSpecialFuncionDigit(pjob->_ppump->_rxbuffer[0x09 + (2*blkindex + 1)]) & 0x0F);


            //STORING BACK THE EEPROM PAGE
            StoreEepromPage(eeprompumpconfigmemorypageid);
            AcquireDispenserLayoutInformation(&GetEepromBuffer()[0x10]);
         
            _g_dispenserlayoutinfo._inconfiguration = false;
            _ALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
            if(psinkmsg)
            {
                psinkmsg->_messageid = DISPENSER_LOAD_EEPROM_SETTINGS;
                psinkmsg->_messagedelay = _SINK_TIMEOUT_1S_;
                psinkmsg->_messagetype = FIRSTFOUND;
                psinkmsg->_messagestate = SINK_BUSY;
            }
            
            _REALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
            if(psinkmsg)
            {
                psinkmsg->_messageid = DISPENSER_CONFIGURE_PRICES;
                psinkmsg->_messagedelay = _SINK_TIMEOUT_2S_;
                psinkmsg->_messagetype = FIRSTFOUND;
                psinkmsg->_messagestate = SINK_BUSY;
            }
        }
    }
}

//@Created By: HIJH
//@Septembre de 2016
void AcquireDispenserLayoutInformation(LPVOID pparam)
{
    LPRAWPTR ptrbuff = (LPRAWPTR)pparam;
    
    _g_dispenserlayoutinfo._hosesposition3 = 0x00;
    _g_dispenserlayoutinfo._hosesposition4 = 0x00;
    //PLEASE CHECK THE DOCUMENTATION TO UNDERSTAND THE COMPARISON VALUES
    switch(ptrbuff[0x00])
    {
        //Multi-Product Dispenser
        case 0x10://MPD with 1 grade/hose per side
        {
            _g_dispenserlayoutinfo._hosesposition1 = 0x01;
            _g_dispenserlayoutinfo._hosesposition2 = 0x01;
            break;
        }
        case 0x11://MPD with 2 grades/hoses per side
        {
            _g_dispenserlayoutinfo._hosesposition1 = 0x02;
            _g_dispenserlayoutinfo._hosesposition2 = 0x02;
            break;
        }
        case 0x12://MPD with 3 grades/hoses per side
        {
            _g_dispenserlayoutinfo._hosesposition1 = 0x03;
            _g_dispenserlayoutinfo._hosesposition2 = 0x03;
            break;
        }
        case 0x13://MPD with 4 grades/hoses per side
        {
            _g_dispenserlayoutinfo._hosesposition1 = 0x04;
            _g_dispenserlayoutinfo._hosesposition2 = 0x04;
            break;
        }
        case 0x14://MPD with 5 grades/hoses per side
        {
            _g_dispenserlayoutinfo._hosesposition1 = 0x05;
            _g_dispenserlayoutinfo._hosesposition2 = 0x05;
            break;
        }
        case 0x15://MPD with 6 grades/hoses per side
        {
            _g_dispenserlayoutinfo._hosesposition1 = 0x05;
            _g_dispenserlayoutinfo._hosesposition2 = 0x05;
            break;
        }
        //Single-Hose Multi-Product Dispenser
        case 0x20://SHMPD 2+0 (2 grades, 1 hose/side)
        {
            _g_dispenserlayoutinfo._hosesposition1 = 0x01;
            _g_dispenserlayoutinfo._hosesposition2 = 0x01;
            break;
        }
        case 0x21://SHMPD 2+1 (3 grades, 2 hoses/side)
        {
            _g_dispenserlayoutinfo._hosesposition1 = 0x02;
            _g_dispenserlayoutinfo._hosesposition2 = 0x02;
            break;
        }
        case 0x22://SHMPD 3+0 (3 grades, 1 hose/side)
        {
            _g_dispenserlayoutinfo._hosesposition1 = 0x01;
            _g_dispenserlayoutinfo._hosesposition2 = 0x01;
            break;
        }
        case 0x23://SHMPD 3+1 (4 grades, 2 hoses/side)
        {
            _g_dispenserlayoutinfo._hosesposition1 = 0x02;
            _g_dispenserlayoutinfo._hosesposition2 = 0x02;
            break;
        }
        case 0x24://SHMPD 4+0 (4 grades, 1 hose/side)
        {
            _g_dispenserlayoutinfo._hosesposition1 = 0x01;
            _g_dispenserlayoutinfo._hosesposition2 = 0x01;
            break;
        }
        case 0x25://SHMPD 4+1 (5 grades, 2 hoses/side)
        {
            _g_dispenserlayoutinfo._hosesposition1 = 0x02;
            _g_dispenserlayoutinfo._hosesposition2 = 0x02;
            break;
        }
        case 0x26://SHMPD 5+0 (5 grades, 1 hose/side)
        {
            _g_dispenserlayoutinfo._hosesposition1 = 0x01;
            _g_dispenserlayoutinfo._hosesposition2 = 0x01;
            break;
        }
        case 0x27://SHMPD 5+1 (6 grades, 2 hoses/side)
        {
            _g_dispenserlayoutinfo._hosesposition1 = 0x02;
            _g_dispenserlayoutinfo._hosesposition2 = 0x02;
            break;
        }
        //MISSING (NOT NEEDED) ...
        //0x28 ~ 0x29
        //0x3X Multi-Hose Blender
        //0x4X Single-Hose Blender
        //0x50 MH Hybrid Blender
    }

    //0x01 => U.S. Gallons
    //0x02 => Imperial Gallons / UK Gallons
    //0x03 => Liters
    //0x04 => 1012 pulses/gallon (Hawaii, N/A for ECAL)
    _g_dispenserlayoutinfo._volumeunit = ptrbuff[0x01];
    
    
    //Money Decimal Points
    switch(ptrbuff[0x02])
    {
        case 0x01:
        _g_dispenserlayoutinfo._moneydecimals = 0;
        break;
        case 0x02:
        _g_dispenserlayoutinfo._moneydecimals = 1;
        break;
        case 0x03:
        _g_dispenserlayoutinfo._moneydecimals = 2;
        break;
        case 0x04:
        _g_dispenserlayoutinfo._moneydecimals = 3;
        break;
    }
    
    //Volume Decimal Points
    switch(ptrbuff[0x03])
    {
        case 0x01:
        _g_dispenserlayoutinfo._volumedecimals = 3;
        break;
        case 0x02:
        _g_dispenserlayoutinfo._volumedecimals = 1;
        break;
        case 0x03:
        _g_dispenserlayoutinfo._volumedecimals = 2;
        break;
        case 0x04:
        _g_dispenserlayoutinfo._volumedecimals = 3;
        break;
        case 0x05:
        _g_dispenserlayoutinfo._volumedecimals = 4;
        break;
        case 0x06:
        _g_dispenserlayoutinfo._volumedecimals = 5;
        break;
        case 0x07:
        _g_dispenserlayoutinfo._volumedecimals = 6;
        break;
        case 0x11:
        _g_dispenserlayoutinfo._volumedecimals = 0;
        break;
    }
    
    //PPU Decimal Points
    switch(ptrbuff[0x04])
    {
        case 0x01:
        _g_dispenserlayoutinfo._ppudecimals = 0;
        break;
        case 0x02:
        _g_dispenserlayoutinfo._ppudecimals = 1;
        break;
        case 0x03:
        _g_dispenserlayoutinfo._ppudecimals = 2;
        break;
        case 0x04:
        _g_dispenserlayoutinfo._ppudecimals = 3;
        break;
    }
    
    switch(ptrbuff[0x05])
    {
        case 0x00:
            _g_dispenserlayoutinfo._displaydigitsmode = 0x05; //5 money digits
            break;
        case 0x01:
            _g_dispenserlayoutinfo._displaydigitsmode = 0x06; //6 money digits
            break;
        case 0x02:
            _g_dispenserlayoutinfo._displaydigitsmode = 0x07; //7 money digits
            break;
        case 0x03:
            _g_dispenserlayoutinfo._displaydigitsmode = 0x08; //8 money digits
            break;
    }
    
    //_g_dispenserlayoutinfo._pointcommadecimalmode = _POINT_AS_DECIMAL_SEPARATOR_;
    _g_dispenserlayoutinfo._pointcommadecimalmode = _COMMA_AS_DECIMAL_SEPARATOR_;
}

//@Created By: HIJH
//@Date: Septembre de 2016
//Resolves the completeness for the arriving data after the SFs invocation
bool AcquirePumpSpecialFunctionResponseResolver(void *pparam, void *pbuffer, uint16 buffersize)
{
    bool retval = false;
    static uint8 index = 0;
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    //Since after the third byte we can start inquiry for the message length in words (this is according to the protocol)    
    //that is  the reason for the number "3"
    if((PSTRCAST(pbuffer)[0 + 45*index] == 0xBA) && buffersize >= 3)
    {
        //char8 buff[24];
        //memcpy(buff, PSTRCAST(pbuffer), buffersize);
        uint8 msglength = (TranslateSpecialFuncionDigit(PSTRCAST(pbuffer)[1 + 45*index]) << 0x04) | TranslateSpecialFuncionDigit(PSTRCAST(pbuffer)[2 + 45*index]);
        //plus 13 since there are some fixed bytes in the frame that sum that value
        //msglength must be multiplied by 2 since this length is in words instead
        //If more than 1 block is coming then the first blocks come full which means that they carry
        //the maximum frame size which is 45 bytes (13 fixed bytes + 32 bytes for the message)
        if(buffersize >= ((2*msglength + 13) + 45*index))
        {
            uint8 remblocks = (TranslateSpecialFuncionDigit(PSTRCAST(pbuffer)[7 + 45*index]) << 0x04) | TranslateSpecialFuncionDigit(PSTRCAST(pbuffer)[8 + 45*index]);
            if(remblocks == 0)
            {
                index = 0;
                retval = true;
            }else{
                index++;
            }
        }
    }
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
void PumpEnumeratorRequest(LPVOID pparam)
{
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    if(ppump)
    {
        uint8_t index = 0;
        memset(ppump->_rxbuffer, 0x00, _PUMP_RX_BUFFER_SIZE_);
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPENSER);
        if(puartdisp)
        {
            puartdisp->_messagetx[index++] = PUMP_COMMAND(PUMP_STATE_REQUEST, ppump->_pumpid);
            puartdisp->_messagelength = 1;//One byte
            puartdisp->_messagestate = PENDING;
        }
    }
}

//@Created By: HIJH
//@Septembre de 2016
void AcquirePumpEnumeratorResponse(LPVOID pparam)
{
    AcquirePumpStateResponse(pparam);
    PPUMPTRANSACTIONJOBCONTEXTPTR pjob = (PPUMPTRANSACTIONJOBCONTEXTPTR)pparam;
    if(pjob->_ppump->_datacontainer._var0 < 0xF)
    {
        switch(pjob->_ppump->_datacontainer._var1)
        {
            case 0:
            {
                
                ClearEepromBuffer();
                //Cleanup of the pump identifier in order to start the enumeration procedure
                LoadEepromPage(EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE);
                GetEepromBuffer()[0] = pjob->_ppump->_pumpid;
                StoreEepromPage(EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE);

                uint8 var0 = pjob->_ppump->_datacontainer._var0;
                uint8 var1 = pjob->_ppump->_datacontainer._var1;
                pjob->_ppump = &_g_pumps[var1 + 1];
                pjob->_ppump->_datacontainer._var1 = var1;
                pjob->_ppump->_datacontainer._var0 = var0;
                _g_dispenserlayoutinfo._numpositions++;
                
                break;
            }
            case 1:
            {
                
                ClearEepromBuffer();
                //Cleanup of the pump identifier in order to start the enumeration procedure
                LoadEepromPage(EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE);
                GetEepromBuffer()[0] = pjob->_ppump->_pumpid;
                StoreEepromPage(EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE);
                
                uint8 var0 = pjob->_ppump->_datacontainer._var0;
                uint8 var1 = pjob->_ppump->_datacontainer._var1;
                pjob->_ppump = &_g_pumps[var1 + 1];
                pjob->_ppump->_datacontainer._var1 = var1;
                pjob->_ppump->_datacontainer._var0 = var0;
                _g_dispenserlayoutinfo._numpositions++;
                
                break;
            }
            case 2:
            {
                
                ClearEepromBuffer();
                //Cleanup of the pump identifier in order to start the enumeration procedure
                LoadEepromPage(EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE);
                GetEepromBuffer()[0] = pjob->_ppump->_pumpid;
                StoreEepromPage(EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE);
                
                uint8 var0 = pjob->_ppump->_datacontainer._var0;
                uint8 var1 = pjob->_ppump->_datacontainer._var1;
                pjob->_ppump = &_g_pumps[var1 + 1];
                pjob->_ppump->_datacontainer._var1 = var1;
                pjob->_ppump->_datacontainer._var0 = var0;
                _g_dispenserlayoutinfo._numpositions++;
                
                break;
            }
            case 3:
            {
                
                ClearEepromBuffer();
                //Cleanup of the pump identifier in order to start the enumeration procedure
                LoadEepromPage(EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE);
                GetEepromBuffer()[0] = pjob->_ppump->_pumpid;                
                StoreEepromPage(EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE);
                _g_dispenserlayoutinfo._numpositions++;
                
                break;
            }
        }
        pjob->_ppump->_datacontainer._var1++;
        if(pjob->_ppump->_datacontainer._var1 < _MAX_NUMBER_OF_PUMPS_)
        {
            pjob->_ppump->_datacontainer._var0++;
            pjob->_ppump->_pumpid = pjob->_ppump->_datacontainer._var0;
        }else{//This is the end of the enumeration procedure
            pjob->_reenqueue = false;
            
            //Chance for any extra initialization
            if(_g_ptractprogramming == PNEAR_NULLPTR)
                _g_ptractprogramming = EncoreActivateProgramming;
        }
        
    }else{//This is the end of the enumeration procedure
        pjob->_reenqueue = false;
        
        //Chance for any extra initialization
        if(_g_ptractprogramming == PNEAR_NULLPTR)
            _g_ptractprogramming = EncoreActivateProgramming;
    }
}

//@Created By: HIJH
//@Septembre de 2016
void PumpEnumeratorTimeout(void *pparam)
{
    PPUMPTRANSACTIONJOBCONTEXTPTR pjob = (PPUMPTRANSACTIONJOBCONTEXTPTR)pparam;
    //_var0 holds the iterator index that goes from 0x0:pump16 to 0xF:pump15
    if(pjob->_ppump->_datacontainer._var0 < 0x10)
    {
        pjob->_retriescounter = 0;
        pjob->_ppump->_datacontainer._var0++;
        pjob->_ppump->_pumpid = pjob->_ppump->_datacontainer._var0;
    }else{
        pjob->_reenqueue = false;
        
        if(_g_ptractprogramming == PNEAR_NULLPTR)
            _g_ptractprogramming = EncoreActivateProgramming;
    }
}

//@Created By: HIJH
//@Septembre de 2016
bool AcquirePumpTotalsResponseResolver(void *pparam, void *pbuffer, uint16 buffersize)
{
    bool retval = false;
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    
    if(_g_dispenserlayoutinfo._displaydigitsmode == 0x06)
    {            
        switch(ppump->_pumpindex)
        {
            case 0x00:
                retval = (buffersize >= (_g_dispenserlayoutinfo._hosesposition1*_PUMP_TOTALS_BUFFER_SHIFT_4DIGITS_ + 0x04));
            break;
            case 0x01:
                retval = (buffersize >= (_g_dispenserlayoutinfo._hosesposition2*_PUMP_TOTALS_BUFFER_SHIFT_4DIGITS_ + 0x04));
            break;
            case 0x02:
                retval = (buffersize >= (_g_dispenserlayoutinfo._hosesposition3*_PUMP_TOTALS_BUFFER_SHIFT_4DIGITS_ + 0x04));
            break;
            case 0x03:
                retval = (buffersize >= (_g_dispenserlayoutinfo._hosesposition4*_PUMP_TOTALS_BUFFER_SHIFT_4DIGITS_ + 0x04));
            break;
        }
    }
    else
    {            
        switch(ppump->_pumpindex)
        {
            case 0x00:
                retval = (buffersize >= (_g_dispenserlayoutinfo._hosesposition1*_PUMP_TOTALS_BUFFER_SHIFT_ + 0x04));
            break;
            case 0x01:
                retval = (buffersize >= (_g_dispenserlayoutinfo._hosesposition2*_PUMP_TOTALS_BUFFER_SHIFT_ + 0x04));
            break;
            case 0x02:
                retval = (buffersize >= (_g_dispenserlayoutinfo._hosesposition3*_PUMP_TOTALS_BUFFER_SHIFT_ + 0x04));
            break;
            case 0x03:
                retval = (buffersize >= (_g_dispenserlayoutinfo._hosesposition4*_PUMP_TOTALS_BUFFER_SHIFT_ + 0x04));
            break;
        }
    }
    
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
bool AcquirePumpTransactionDataResponseResolver(void *pparam, void *pbuffer, uint16 buffersize)
{
    bool retval = false;
    PNEAR_PUMPPTR ppump = (PNEAR_PUMPPTR)pparam;
    
    if(_g_dispenserlayoutinfo._displaydigitsmode == 0x06)
    {            
        retval = (buffersize >= PUMP_TRANSACTION_DATA_REQUEST_RESPONSE_SIZE_4DIGITS);
    }
    else
    {
        retval = (buffersize >= PUMP_TRANSACTION_DATA_REQUEST_RESPONSE_SIZE);
    }
    
    return retval;
}

void AcquirePpuChangeResponse(void *pparam)
{
    PPUMPTRANSACTIONJOBCONTEXTPTR pjob = (PPUMPTRANSACTIONJOBCONTEXTPTR)pparam;
    if(pjob)
    {
        pjob->_ppump->_pumpstate = ((pjob->_ppump->_rxbuffer[0x00] & 0xF0) >> 4);
        
        switch(_g_dispenserlayoutinfo._numpositions)
        {
            case 0x02:
            {
                if(pjob->_ppump->_pumpindex == (_g_dispenserlayoutinfo._numpositions - 1) && (pjob->_ppump->_currenthose == _g_dispenserlayoutinfo._hosesposition2))
                    _g_dispenserlayoutinfo._inconfiguration = false;
                
                break;
            }
            case 0x04:
            {
                if(pjob->_ppump->_pumpindex == (_g_dispenserlayoutinfo._numpositions - 1) && (pjob->_ppump->_currenthose == _g_dispenserlayoutinfo._hosesposition4))
                    _g_dispenserlayoutinfo._inconfiguration = false;
                
                break;
            }
        }
    }
}

//@Created By: HIJH
//@Novembre de 2016
void EncoreActivateProgramming(LPVOID pparam)
{
    PNEAR_PUMPPTR ppump = &_g_pumps[0x00];
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(ppump)
    {
        ppump->_acquiringstate = true;
        ppump->_pcurrtransaction = GetTransactionByName(_PUMP_PROGRAM_MODE_);
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
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//THESE ARE SUPPORT METHODS THAT WILL BE USED IN THE TRANSACTIONAL MACHINE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Calculation of the LRC according to the documentation
uint8_t GetLRC(char8 *_pbuffer)
{
    uint8_t lrc = 0;
    while(*_pbuffer != 0xFB)
    {
        lrc += (*_pbuffer & 0x0F);
        _pbuffer++;
    }
    lrc += (*_pbuffer & 0x0F);
    return (((~lrc + 1) & 0x0F) | 0xE0);
}

uint8_t FindLRC(char8 *_pbuffer)
{
    uint8_t lrc = 0;
    while(*_pbuffer != 0xFB)
    {
        _pbuffer++;
    }
    return *(++_pbuffer);
}

//@Created By: HIJH
//@Septembre de 2016
void InitiateTransaction(void *pparam)
{
    PumpTransactionJobContext *pjob = (PumpTransactionJobContext*)pparam;
    if(pjob)
    {
        pjob->_ppump->_transhealth = _PUMP_OK_;
        pjob->_ppump->_transtate = _PUMP_BUSY_;
    }
}

//@Created By: HIJH
//@Septembre de 2016
void TerminateTransaction(void *pparam)
{
    PPUMPTRANSACTIONJOBCONTEXTPTR pjob = (PPUMPTRANSACTIONJOBCONTEXTPTR)pparam;
    if(pjob)
    {
        pjob->_ppump->_transhealth = _PUMP_OK_;
        pjob->_ppump->_transtate = _PUMP_IDLE_;
        
    }
}

void TerminateTransactionAndPrint(void *pparam)
{
    PPUMPTRANSACTIONJOBCONTEXTPTR pjob = (PPUMPTRANSACTIONJOBCONTEXTPTR)pparam;
    if(pjob)
    {
        pjob->_ppump->_transhealth = _PUMP_OK_;
        pjob->_ppump->_transtate = _PUMP_IDLE_;
        
        //If printing has been selected then post the print job
        if(pjob->_ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_DESEA_IMPRIMIR_RECIBO)] == 0x39)
        {
            _ALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
            if(psinkmsg)
            {
                switch(pjob->_ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_SIDE_DUMMY_DISPLAY)])
                {
                    case DISPLAY1:
                    psinkmsg->_messageid = _g_printerlayout._printerportside1;
                    break;
                    case DISPLAY2:
                    psinkmsg->_messageid = _g_printerlayout._printerportside2;
                    break;
                }
                psinkmsg->_messagetype = DELAYEDALLINTERESTED;
                //Wait for 30 seconds before sending the command to the printer!
                //This will give some time to allow the remote peer (NSX) to respond with the appropriate invoice number
                if(_g_rfautorun._state == RF_AUTONOMOUS)
                    psinkmsg->_messagedelay = _SINK_TIMEOUT_1S_;
                else
                    psinkmsg->_messagedelay = _SINK_TIMEOUT_30S_;
                
                //PUMP POSITION GOES IN THE ARRAY INDEX 0X00 (SEE THE FILE "PRINTERBLL.C")
                psinkmsg->_buffer[0x00] = pjob->_ppump->_pumpid;
                //PUMP HOSE TO SELECT THE PRODUCT NAME
                psinkmsg->_buffer[0x01] = pjob->_ppump->_authorizationinfo._hoseid;
                psinkmsg->_buffer[0x02] = 0x06;//Six bytes for the price (PPU)
                memset(&psinkmsg->_buffer[0x03], 0x00, 0x08);
                
                if(pjob->_ppump->_trasactionbuffer[0x00] == 0x0E && _g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                    B78AD90CF552D9B30A(pjob->_ppump->_authorizationinfo._price * 10, PBYTECAST(&psinkmsg->_buffer[0x03]));
                else
                    B78AD90CF552D9B30A(pjob->_ppump->_authorizationinfo._price, PBYTECAST(&psinkmsg->_buffer[0x03]));
                    
                psinkmsg->_buffer[0x09] = 0x08;//Eight bytes for the Volume
                memset(&psinkmsg->_buffer[0x0A], 0x00, 0x08);
                B78AD90CF552D9B30A(pjob->_ppump->_authorizationinfo._volumeamount, PBYTECAST(&psinkmsg->_buffer[0x0A]));
                psinkmsg->_buffer[0x12] = 0x08;//Eight bytes for the Preset
                memset(&psinkmsg->_buffer[0x13], 0x00, 0x08);
                B78AD90CF552D9B30A(pjob->_ppump->_authorizationinfo._presetamount, PBYTECAST(&psinkmsg->_buffer[0x13]));
                
                //This is the transaction type (Cash: 0x0D / Credit: 0x0E). See the documentation to understand these values
                psinkmsg->_buffer[0x1B] = pjob->_ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_FORMA_PAGO_DESEADA)];
                
                //This means "N/A". This will be printed once the timeout has expired and no invoice number
                //has been received
                psinkmsg->_buffer[0x20] = 0x4E;
                psinkmsg->_buffer[0x21] = 0x2F;
                psinkmsg->_buffer[0x22] = 0x41;
                //Car's plate
                psinkmsg->_buffer[0x2A] = 0x4E;
                psinkmsg->_buffer[0x2B] = 0x2F;
                psinkmsg->_buffer[0x2C] = 0x41;
                //Balance
                psinkmsg->_buffer[0x32] = 0x4E;
                psinkmsg->_buffer[0x33] = 0x2F;
                psinkmsg->_buffer[0x34] = 0x41;
                //Company
                psinkmsg->_buffer[0x3C] = 0x4E;
                psinkmsg->_buffer[0x3D] = 0x2F;
                psinkmsg->_buffer[0x3E] = 0x41;
                //Account Name
                psinkmsg->_buffer[0x50] = 0x4E;
                psinkmsg->_buffer[0x51] = 0x2F;
                psinkmsg->_buffer[0x52] = 0x41;
                //Daily Visits
                psinkmsg->_buffer[0x64] = 0x00;
                //Weekly Visits
                psinkmsg->_buffer[0x65] = 0x00;
                //Monthly Visits
                psinkmsg->_buffer[0x66] = 0x00;

                psinkmsg->_messagestate = SINK_BUSY;
            }   
            
        }        
    }
}

//@Created by: HIJH
//@Date: Septembre de 2016
//This method arranges the PRESET buffer when this is a "Volume PRESET".
//Volume PRESETS must be multiplied by 1000
void CastVolumeValue(char8 *pbuffer, uint8 buffersize)
{
    uint8 delimiterindex = 0x00;
    bool shiftbydelimiter = false;
    //If there is a "Comma" inside of the buffer, then just remove it before and count this for the shifting
    FOR(int8 index = (buffersize - 1), index > 0, index--)
    {
        if(pbuffer[index] == 0x2C && !shiftbydelimiter)
            shiftbydelimiter = true;

        if(shiftbydelimiter)
        {
            pbuffer[index] = pbuffer[index - 1];
            pbuffer[index - 1] = 0x00;
        }else
            delimiterindex++;
        
    }
    if(!shiftbydelimiter)
        delimiterindex = 0x00;
    
    int8 shiftcount = (delimiterindex - ((_g_dispenserlayoutinfo._displaydigitsmode == 0x06)?_g_dispenserlayoutinfo._volumedecimals - 1: _g_dispenserlayoutinfo._volumedecimals));
    //8 is the size of the allocated buffer space for the preset values according to the protocol.
    //The real buffer size allocated from the display side is 16 bytes. The buffer will be cast from
    //the end to the beginning, so the startup index has been calculated "explicitly", this is (8 - 1 - 3).
    //To multiply by 1000 we need to shift the entire buffer three bytes to shift the comma, so we
    //will subtract 3 bytes to the starting position in order to make the shifting.
    if(shiftcount > 0)
    {
        while(shiftcount > 0)
        {
            FOR(index = (buffersize - 1), index > 0, index--)
            {
                pbuffer[index] = pbuffer[index - 1];
                pbuffer[index] = 0x00;
            }
            shiftcount--;
        }
    }
    else if(shiftcount < 0)
    {
        while(shiftcount < 0)
        {
            FOR(index = 0, index < (buffersize  - 1), index++)
            {
                pbuffer[index] = pbuffer[index + 1];
                pbuffer[index + 1] = 0x30;
            }
            shiftcount++;
        }
    }
}

//@Created by: HIJH
//@Date: Septembre de 2016
//This method arranges the PRESET buffer when this is a "Money PRESET".
void CastMoneyValue(char8 *pbuffer, uint8 buffersize)
{
    uint8 delimiterindex = 0x00;
    bool shiftbydelimiter = false;
    //If there is a "Comma" inside of the buffer, then just remove it before and count this for the shifting
    FOR(int8 index = (buffersize - 1), index > 0, index--)
    {
        if(pbuffer[index] == 0x2C && !shiftbydelimiter)
            shiftbydelimiter = true;

        if(shiftbydelimiter)
        {
            pbuffer[index] = pbuffer[index - 1];
            pbuffer[index - 1] = 0x00;
        }else
            delimiterindex++;
        
    }
    if(!shiftbydelimiter)
        delimiterindex = 0x00;
    
    int8 shiftcount = (delimiterindex - _g_dispenserlayoutinfo._moneydecimals);
    //8 is the size of the allocated buffer space for the preset values according to the protocol.
    //The real buffer size allocated from the display side is 16 bytes. The buffer will be cast from
    //the end to the beginning, so the startup index has been calculated "explicitly", this is (8 - 1 - 3).
    //To multiply by 1000 we need to shift the entire buffer three bytes to shift the comma, so we
    //will subtract 3 bytes to the starting position in order to make the shifting.
    if(shiftcount > 0)
    {
        while(shiftcount > 0)
        {
            FOR(index = (buffersize - 1), index > 0, index--)
            {
                pbuffer[index] = pbuffer[index - 1];
                pbuffer[index] = 0x00;
            }
            shiftcount--;
        }
    }
    else if(shiftcount < 0)
    {
        while(shiftcount < 0)
        {
            FOR(index = 0, index < (buffersize  - 1), index++)
            {
                pbuffer[index] = pbuffer[index + 1];
                pbuffer[index + 1] = 0x30;
            }
            shiftcount++;
        }
    }
}

//This is a char map to translate from the special functions word domain to the normal byte domain
char8 _g_sfcharmap[][2] = 
{
    { 0xB1, 0x01 }, { 0xB2, 0x02 }, { 0xB3, 0x03 }, { 0xB4, 0x04 }, { 0xB5, 0x05 }, { 0xB6, 0x06 }, { 0xB7, 0x07 }, { 0xB8, 0x08 }, { 0xB9, 0x09 }, 
    { 0xC1, 0x0A }, { 0xC2, 0x0B }, { 0xC3, 0x0C }, { 0xC4, 0x0D }, { 0xC5, 0x0E }, { 0xC6, 0x0F }, { 0x00, 0x00 } 
};

//@Created By: HIJH
//@Septembre de 2016
char8 TranslateSpecialFuncionDigit(char8 refval)
{
    char8 retval = 0x00;
    PSTR ptrstr = (PSTR)&_g_sfcharmap[0];
    while(*ptrstr != 0x00)
    {
        if(ptrstr[0] == refval)
        {
            retval = ptrstr[1];
            break;
        }
        ptrstr++;
    }    
    return retval;
}

#endif

/* [] END OF FILE */
