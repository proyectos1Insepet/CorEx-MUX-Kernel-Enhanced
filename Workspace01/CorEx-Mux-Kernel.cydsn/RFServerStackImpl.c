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
#include "Display.h"
#include "RFLink.h"
#include "Printer.h"
#include "Eeprom.h"
#include "Rtc.h"

//@Created By: HIJH
//@Septembre de 2016
//This is the header that must be included in the response stream
uint8 _g_rfslaveheader[] = { 0xbc, 0xcb, 0xc8 };
uint8 _g_rfmasterheader[] = { 0xbc, 0xbb, 0xb8 };
uint8 _g_rfmasterfooter[] = { 0xaa, 0xbb, 0xcc };

void RFSpeedReport(uint8 command, uint8 position);

bool RFStateReqResp(void *pparam)
{
    bool retval = false;
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(pmsg)
    {
        //Checking the CRC first to proceed
        //CRC must be located at this position for this message (See the protocol description file)
        uint8 crc = RawCRCCheck(pmsg->_buffer, pmsg->_buffersize - 1);
        if(crc != RFGetCRC(pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_], PBYTECAST(pmsg->_buffer)))//If there is a mismatch then return immediately and send a Nack back
        {
            PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
            if(puartdisp)
            {
                puartdisp->_messagelength = 0;
                memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                puartdisp->_messagelength += 0x03;
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = _RF_NACK_;
                puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength - 1);
                puartdisp->_messagelength++;

                puartdisp->_messagestate = PENDING;
            }
            return retval;
        }
        
        FOR(uint8 index = 0x00, index < _g_dispenserlayoutinfo._numpositions, index++)
        {
            if(_g_pumps[index]._pumpid == (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset))
            {
                _g_pumps[index].PumpTransQueueLock(&_g_pumps[index]);
                //We always pick the top most item of the Pump's Transactional Queue
                PNEAR_PUMPTRANSACTIONALSTATEPTR pumptrans = _g_pumps[index].PumpTransQueueDequeue(&_g_pumps[index]);
                _g_pumps[index].PumpTransQueueUnlock(&_g_pumps[index]);
                                
                PNEAR_REQRESPTR ptreqresiterator =  GetRFRequestResolver(pumptrans->_transtate);
                if(ptreqresiterator)
                {
                    uint8 unmappedcmd = RFUnmapCommand2State(pumptrans->_transtate);
                    if(unmappedcmd != RF_MUX_NULL)
                        _g_pumps[index]._pumprftransstate = unmappedcmd;

                    //Here we attempt to resolve the request and dispatch the predefined response
                    //In case of any error (asserted as false), the error function pointer will be invoked
                    if(!ptreqresiterator->_prequestresponse(pmsg))
                        if(ptreqresiterator->_perror)
                            ptreqresiterator->_perror(pmsg);
                            
                    retval = true;
                }
                else
                {
                    //If there wasn't any pending transactional state in the pointed pump position, just
                    //return the Ack response in IDLE state
                    PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
                    if(puartdisp)
                    {
                        puartdisp->_messagelength = 0;
                        memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                        memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                        puartdisp->_messagelength += 0x03;
                        puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                        puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                        puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                        puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_];
                        puartdisp->_messagetx[puartdisp->_messagelength++] = _g_pumps[index]._pumprftransstate;
                        
                        puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength);
                        puartdisp->_messagelength++;
                        
                        puartdisp->_messagestate = PENDING;
                    }
                    retval = true;
                }
                break;
            }
        }
        #ifdef _DEBUG_
        RFSpeedReport(pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_], pmsg->_buffer[_RF_STREAM_POSITION_INDEX_]);
        #endif
    }
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
bool RFCreditSerialAuthReqResp(void *pparam)
{
    bool retval = false;
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(pmsg)
    {
        FOR(uint8 index = 0x00, index < _g_dispenserlayoutinfo._numpositions, index++)
        {
            if(_g_pumps[index]._pumpid  == (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset))
            {
                //We always pick the top most item from the Pump Transactional Queue
                _g_pumps[index].PumpTransQueueLock(&_g_pumps[index]);
                PNEAR_PUMPTRANSACTIONALSTATEPTR pumptrans = _g_pumps[index].PumpTransQueueDequeue(&_g_pumps[index]);
                PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
                if(puartdisp)
                {
                    puartdisp->_messagelength = 0;
                    memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                    memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                    puartdisp->_messagelength += 0x03;
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (pumptrans->_buffer[0x00] + _g_dispenserlayoutinfo._positionoffset);
                    puartdisp->_messagetx[puartdisp->_messagelength++] = RF_MUX_CREDIT_SERIAL_AUTH_RESPONSE;
                    puartdisp->_messagetx[puartdisp->_messagelength++] = _g_pumps[index]._pumprftransstate;//??ESTADOPOSICION??
                    //Here we copy the incoming buffer from the Hose ID, so we subtract one to remove the Pump ID that always comes in the first position
                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &pumptrans->_buffer[0x01], pumptrans->_buffersize - 1);
                    puartdisp->_messagelength += (pumptrans->_buffersize - 1);
                    
                    puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength - 1);
                    puartdisp->_messagelength++;

                    puartdisp->_messagestate = PENDING;
                }
                _g_pumps[index].PumpTransQueueDeallocate(&_g_pumps[index], pumptrans);
                _g_pumps[index].PumpTransQueueUnlock(&_g_pumps[index]);
                retval = true;

                break;
            }
        }

    }
    return retval;
}

bool RFCreditAuthorizationReqResp(void *pparam)
{
    bool retval = false;
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(pmsg)
    {
        //Checking the CRC first to proceed
        //CRC must be located at this position for this message (See the protocol description file)
        uint8 crc = RawCRCCheck(pmsg->_buffer, pmsg->_buffersize - 1);
        if(crc != RFGetCRC(pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_], PBYTECAST(pmsg->_buffer)))//If there is a mismatch then return immediately and send a Nack back
        {
            PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
            if(puartdisp)
            {
                puartdisp->_messagelength = 0;
                memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                puartdisp->_messagelength += 0x03;
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = _RF_NACK_;
                puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength - 1);
                puartdisp->_messagelength++;

                puartdisp->_messagestate = PENDING;
            }
            return retval;
        }
        FOR(uint8 index = 0x00, index < _g_dispenserlayoutinfo._numpositions, index++)
        {
            if((_g_pumps[index]._pumpid + _g_dispenserlayoutinfo._positionoffset) == pmsg->_buffer[_RF_STREAM_POSITION_INDEX_])            
            {
                //Authorization information
                _g_pumps[index]._authorizationinfo._authorized = (pmsg->_buffer[0x08] == 0x01);

                //GET THE RIGHT DISPLAY FOR THE POSITION TO CHANGE ITS STATE TO "DISPLAY_CREDIT_AUTHORIZATION_ACCEPTED"
                PDISPLAYLAYOUTPTR pdisplay = GetDisplayFromPumpID(pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset);
                //See the protocol description file to understand the applied indexes 
                _g_pumps[index]._authorizationinfo._presettype = pmsg->_buffer[0x11];
                _g_pumps[index]._authorizationinfo._presetamount  = LSDBCDBUFF2HEX((PNEAR_BYTE_PTR)&pmsg->_buffer[0x09], 0x08);
                _g_pumps[index]._authorizationinfo._hoseid = pmsg->_buffer[0x12];
                
                if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                    _g_pumps[index]._authorizationinfo._price = LSDBCDBUFF2HEX((PNEAR_BYTE_PTR)&pmsg->_buffer[0x13], 0x05) / 10;
                else
                    _g_pumps[index]._authorizationinfo._price = LSDBCDBUFF2HEX((PNEAR_BYTE_PTR)&pmsg->_buffer[0x13], 0x05);
                                
                if(pdisplay)
                {
                    if(_g_pumps[index]._authorizationinfo._authorized)
                        pdisplay->_currentstate = DISPLAY_CREDIT_AUTHORIZATION_ACCEPTED;
                    else
                    {
                        pdisplay->_currentstate = DISPLAY_CREDIT_AUTHORIZATION_REJECTED;
                        _g_pumps[index]._pumprftransstate = RF_IDLE;
                    }
                }
                
                retval = true;
                break;
            }
        }
        if(retval)
        {
            PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
            if(puartdisp)
            {
                puartdisp->_messagelength = 0;
                memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                puartdisp->_messagelength += 0x03;
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = _RF_ACK_;
                puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength - 1);
                puartdisp->_messagelength++;

                puartdisp->_messagestate = PENDING;
            }
        }
        
    }
    return retval;
}

bool RFCreditSalePresetReportReqResp(void *pparam)
{
    bool retval = true;
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(pmsg)
    {
        FOR(uint8 index = 0x00, index < _g_dispenserlayoutinfo._numpositions, index++)
        {
            if(_g_pumps[index]._pumpid  == (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset))
            {
                //We always pick the top most item from the Pump Transactional Queue
                _g_pumps[index].PumpTransQueueLock(&_g_pumps[index]);
                PNEAR_PUMPTRANSACTIONALSTATEPTR pumptrans = _g_pumps[index].PumpTransQueueDequeue(&_g_pumps[index]);
                //USED ONLY FOR TRANSACTIONAL STATE CONSISTENCY
                PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
                if(puartdisp)
                {
                    puartdisp->_messagelength = 0;
                    memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                    memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                    puartdisp->_messagelength += 0x03;
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                    puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                    puartdisp->_messagetx[puartdisp->_messagelength++] = RF_MUX_CREDIT_PRESET_NOTIFICATION_RESPONSE;
                    puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength - 1);
                    puartdisp->_messagelength++;

                    puartdisp->_messagestate = PENDING;
                }
                _g_pumps[index].PumpTransQueueDeallocate(&_g_pumps[index], pumptrans);
                _g_pumps[index].PumpTransQueueUnlock(&_g_pumps[index]);
            }
        }
    }
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
bool RFCashSalePresetReportReqResp(void *pparam)
{
    bool retval = false;
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(pmsg)
    {
        FOR(uint8 index = 0x00, index < _g_dispenserlayoutinfo._numpositions, index++)
        {
            if(_g_pumps[index]._pumpid == (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset))
            {
                _g_pumps[index].PumpTransQueueLock(&_g_pumps[index]);
                //We always pick the top most item of the Pump's Transactional Queue
                PNEAR_PUMPTRANSACTIONALSTATEPTR pumptrans = _g_pumps[index].PumpTransQueueDequeue(&_g_pumps[index]);
                PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
                if(puartdisp)
                {
                    puartdisp->_messagelength = 0;
                    memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                    memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                    puartdisp->_messagelength += 0x03;
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                    puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                    puartdisp->_messagetx[puartdisp->_messagelength++] = pumptrans->_transtate;
                    puartdisp->_messagetx[puartdisp->_messagelength++] = RFUnmapCommand2State(pumptrans->_transtate);
                    puartdisp->_messagetx[puartdisp->_messagelength++] = 0x00;//ID Grado/Hose not available
                    //PRESET type defined on this position (for further details see the function "PumpPresetNotification" in the file "PrimeImpl.c")
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (pumptrans->_buffer[0x01] & 0x0F);
                    //PRESET value as a BCD array starting at position 0x02
                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &pumptrans->_buffer[0x02], 0x08);
                    puartdisp->_messagelength += 0x08;
                    //RESERVED (see the protocol description file for this stream)
                    puartdisp->_messagelength++;
                    //Car's plate (8 bytes)
                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &pumptrans->_buffer[0x0A], GetBufferLengthFromDisplayID(DISPLAY_DIGITE_PLACA));
                    puartdisp->_messagelength += GetBufferLengthFromDisplayID(DISPLAY_DIGITE_PLACA);
                    //Car's milleage (10 bytes of room but only 8 copied)
                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &pumptrans->_buffer[0x0A], GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE));
                    puartdisp->_messagelength += (GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE) + 0x02);
                    
                    puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength - 1);
                    puartdisp->_messagelength++;

                    puartdisp->_messagestate = PENDING;
                }
                _g_pumps[index].PumpTransQueueDeallocate(&_g_pumps[index], pumptrans);
                _g_pumps[index].PumpTransQueueUnlock(&_g_pumps[index]);
                retval = true;
                
                break;
            }
        }
        if(!retval)
        {
            //THIS POINT SHOULDN'T EVER BE REACHED SINCE IT MEANS THAT SOMETHING REALLY WEIRD HAPPENED!!
            PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
            if(puartdisp)
            {
                puartdisp->_messagelength = 0;
                memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                puartdisp->_messagelength += 0x03;
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = _RF_NACK_;
                puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength - 1);
                puartdisp->_messagelength++;

                puartdisp->_messagestate = PENDING;
            }
        }
        
    }
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
bool RFCashCreditSaleFinalReportReqResp(void *pparam)
{
    bool retval = false;
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(pmsg)
    {
        FOR(uint8 index = 0x00, index < _g_dispenserlayoutinfo._numpositions, index++)
        {
            if(_g_pumps[index]._pumpid  == (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset))
            {
                _g_pumps[index].PumpTransQueueLock(&_g_pumps[index]);
                //We always pick the top most item of the Pump's Transactional Queue
                PNEAR_PUMPTRANSACTIONALSTATEPTR pumptrans = _g_pumps[index].PumpTransQueueDequeue(&_g_pumps[index]);
                PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
                if(puartdisp)
                {
                    puartdisp->_messagelength = 0;
                    memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                    memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                    puartdisp->_messagelength += 0x03;
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                    puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                    puartdisp->_messagetx[puartdisp->_messagelength++] = pumptrans->_transtate;
                    puartdisp->_messagetx[puartdisp->_messagelength++] = 0x00;//?? ESTADOPOSITION ??
                    
                    if(_g_dispenserlayoutinfo._displaydigitsmode == 0x06)
                    {
                        //See the protocol documentation to understand the used indexes (Transaction Data Request)
                        puartdisp->_messagetx[puartdisp->_messagelength++] = pumptrans->_buffer[0x09];//ID Grado/Hose
                        
                        //PPU data
                        if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                        {
                            puartdisp->_messagetx[puartdisp->_messagelength++] = 0x00;
                            memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &pumptrans->_buffer[0x0C], 0x04);
                            puartdisp->_messagelength += 0x04;
                        }
                        else
                        {
                            memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &pumptrans->_buffer[0x0C], 0x04);                    
                            puartdisp->_messagelength += 0x04;
                            puartdisp->_messagetx[puartdisp->_messagelength++] = 0x00;
                        }
                        
                        //Volume data                        
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &pumptrans->_buffer[0x11], 0x06);
                        puartdisp->_messagelength += 0x06;
                        puartdisp->_messagetx[puartdisp->_messagelength++] = 0x00;
                        puartdisp->_messagetx[puartdisp->_messagelength++] = 0x00;
                        
                        //Money data
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &pumptrans->_buffer[0x18], 0x06);
                        puartdisp->_messagelength += 0x06;
                        puartdisp->_messagetx[puartdisp->_messagelength++] = 0x00;
                        puartdisp->_messagetx[puartdisp->_messagelength++] = 0x00;
                    }
                    else
                    {
                        //See the protocol documentation to understand the used indexes (Transaction Data Request)
                        puartdisp->_messagetx[puartdisp->_messagelength++] = pumptrans->_buffer[0x09];//ID Grado/Hose
                        //PPU data
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &pumptrans->_buffer[0x0C], 0x05);
                        puartdisp->_messagelength += 0x05;
                        //Volume data
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &pumptrans->_buffer[0x13], 0x08);
                        puartdisp->_messagelength += 0x08;
                        //Money data => the startup position is 0x1C but it goes with an extra decimal digit.
                        //The applied position skips this digit
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &pumptrans->_buffer[0x1D], 0x07);
                        puartdisp->_messagelength += 0x08;
                    }

                    puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength - 1);
                    puartdisp->_messagelength++;

                    puartdisp->_messagestate = PENDING;
                }
                _g_pumps[index]._pumprftransstate = RF_IDLE;
                _g_pumps[index].PumpTransQueueDeallocate(&_g_pumps[index], pumptrans);
                _g_pumps[index].PumpTransQueueUnlock(&_g_pumps[index]);
                retval = true;
                
                break;
            }
        }
        if(!retval)
        {
            //THIS POINT SHOULDN'T EVER BE REACHED SINCE IT MEANS THAT SOMETHING REALLY BAD HAPPENED!!
            PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
            if(puartdisp)
            {
                puartdisp->_messagelength = 0;
                memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                puartdisp->_messagelength += 0x03;
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = _RF_NACK_;
                puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength - 1);
                puartdisp->_messagelength++;

                puartdisp->_messagestate = PENDING;
            }
        }
    }
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
bool RFTotalsDataReqResp(void *pparam)
{
    bool retval = false;
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(pmsg)
    {
        //Checking the CRC first to proceed
        //CRC must be located at this position for this message (See the protocol description file)
        uint8 crc = RawCRCCheck(pmsg->_buffer, pmsg->_buffersize - 1);
        if(crc != RFGetCRC(pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_], PBYTECAST(pmsg->_buffer)))//If there is a mismatch then return immediately and send a Nack back
        {
            PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
            if(puartdisp)
            {
                puartdisp->_messagelength = 0;
                memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                puartdisp->_messagelength += 0x03;
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = RF_MUX_TOTALS_RESPONSE;
                puartdisp->_messagetx[puartdisp->_messagelength++] = _RF_NACK_;
                puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength - 1);
                puartdisp->_messagelength++;

                puartdisp->_messagestate = PENDING;
            }
            return retval;
        }
        if(!retval)
        {
            bool bfound = false;
            FOR(uint8 index = 0x00, index < _g_dispenserlayoutinfo._numpositions, index++)
            {
                if(_g_pumps[index]._pumpid  == (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset))
                {
                    bfound = _g_pumps[index].PumpTransQueueFind(&_g_pumps[index], RF_MUX_TOTALS_REPORT_RESPONSE);
                    break;
                }
            }
            if(!bfound)
            {
                PSINKMESSAGEPTR *parrsinkmsg = AllocateMessageArraySlotConditional(DISPENSER_ACQUIRE_TOTALS);
                FOR(index = 0, index < 0x04, index++)
                {
                    if(parrsinkmsg[index] == PNEAR_NULLPTR)
                        break;

                    if(parrsinkmsg[index]->_buffer[0x00] == (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset))
                    {
                        bfound = true;
                        break;
                    }
                }
                if(!bfound)
                {
                    PSINKMESSAGEPTR psinkmsg = AllocateMessageSlot(DISPENSER_ACQUIRE_TOTALS);
                    psinkmsg->_messageid = DISPENSER_ACQUIRE_TOTALS;
                    psinkmsg->_messagetype = FIRSTFOUND;
                    psinkmsg->_buffer[0x00] = (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset);
                    psinkmsg->_buffersize = 0x01;
                    
                    psinkmsg->_messagestate = SINK_BUSY;
                }
            }
        }
    }
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
bool RFTotalsDataReportReqResp(void *pparam)
{
    bool retval = false;
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(pmsg)
    {
        //Checking the CRC first to proceed
        //CRC must be located at this position for this message (See the protocol description file)
        uint8 crc = RawCRCCheck(pmsg->_buffer, pmsg->_buffersize - 1);
        if(crc != RFGetCRC(pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_], PBYTECAST(pmsg->_buffer)))//If there is a mismatch then return immediately and send a Nack back
        {
            PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
            if(puartdisp)
            {
                puartdisp->_messagelength = 0;
                memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                puartdisp->_messagelength += 0x03;
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = RF_MUX_TOTALS_REPORT_RESPONSE;
                puartdisp->_messagetx[puartdisp->_messagelength++] = _RF_NACK_;
                puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength - 1);
                puartdisp->_messagelength++;

                puartdisp->_messagestate = PENDING;
            }
            return retval;
        }
        FOR(uint8 index = 0x00, index < _g_dispenserlayoutinfo._numpositions, index++)
        {
            if((_g_pumps[index]._pumpid + _g_dispenserlayoutinfo._positionoffset) == pmsg->_buffer[_RF_STREAM_POSITION_INDEX_])            
            {
                _g_pumps[index].PumpTransQueueLock(&_g_pumps[index]);
                //We always pick the top most item of the Pump's Transactional Queue
                PNEAR_PUMPTRANSACTIONALSTATEPTR pumptrans = _g_pumps[index].PumpTransQueueDequeue(&_g_pumps[index]);
                if(pumptrans->_transtate == RF_MUX_TOTALS_REPORT_RESPONSE)
                {
                    PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
                    if(puartdisp)
                    {
                        puartdisp->_messagelength = 0x00;
                        memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));                        
                        memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                        puartdisp->_messagelength += 0x03;
                        puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                        puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                        puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                        puartdisp->_messagetx[puartdisp->_messagelength++] = RF_MUX_TOTALS_REPORT_RESPONSE;
                        puartdisp->_messagetx[puartdisp->_messagelength++] = 0x00;//DATA BLOCK AVAILABLE
                        
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
                        
                        uint8 payloadlengthindex = puartdisp->_messagelength;
                        puartdisp->_messagelength++;//Advance one position to point to the payload block start
                        //PROCESS THE ENTIRE BUFFER FOR ALL OF THE HOSES
                        uint8 blkindex = 0;
                        FOR(uint8 hoseindex = 0, hoseindex < numhoses, hoseindex++)
                        {
                            if(_g_dispenserlayoutinfo._displaydigitsmode == 0x06)
                            {
                                //All indexes were taken according to the documentation
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA_4D(hoseindex, 0x02)] & 0x0F);//Hose id

                                //Bytes from index 0x04 + _PUMP_TOTALS_BUFFER_SHIFT_*index to 0x0F + _PUMP_TOTALS_BUFFER_SHIFT_*index 
                                //are the digits for the total fuel volume delivered until now on this pump
                                //Digits are in BCD format
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA_4D(hoseindex, 0x04)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA_4D(hoseindex, 0x05)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA_4D(hoseindex, 0x06)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA_4D(hoseindex, 0x07)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA_4D(hoseindex, 0x08)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA_4D(hoseindex, 0x09)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA_4D(hoseindex, 0x0A)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA_4D(hoseindex, 0x0B)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = 0x00;
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = 0x00;
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = 0x00;
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = 0x00;

                                //Bytes from index 0x11 + _PUMP_TOTALS_BUFFER_SHIFT_*index to 0x1C + _PUMP_TOTALS_BUFFER_SHIFT_*index 
                                //are the digits for the total money charged until now on this pump
                                //Digits are in BCD format
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA_4D(hoseindex, 0x0D)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA_4D(hoseindex, 0x0E)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA_4D(hoseindex, 0x0F)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA_4D(hoseindex, 0x10)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA_4D(hoseindex, 0x11)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA_4D(hoseindex, 0x12)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA_4D(hoseindex, 0x13)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA_4D(hoseindex, 0x14)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = 0x00;
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = 0x00;
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = 0x00;
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = 0x00;
                                
                            }
                            else
                            {            
                                
                                //All indexes were taken according to the documentation
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x02)] & 0x0F);//Hose id

                                //Bytes from index 0x04 + _PUMP_TOTALS_BUFFER_SHIFT_*index to 0x0F + _PUMP_TOTALS_BUFFER_SHIFT_*index 
                                //are the digits for the total fuel volume delivered until now on this pump
                                //Digits are in BCD format
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x04)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x05)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x06)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x07)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x08)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x09)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x0A)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x0B)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x0C)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x0D)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x0E)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x0F)] & 0x0F);

                                //Bytes from index 0x11 + _PUMP_TOTALS_BUFFER_SHIFT_*index to 0x1C + _PUMP_TOTALS_BUFFER_SHIFT_*index 
                                //are the digits for the total money charged until now on this pump
                                //Digits are in BCD format
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x11)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x12)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x13)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x14)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x15)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x16)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x17)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x18)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x19)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x1A)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x1B)] & 0x0F);
                                puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x1C)] & 0x0F);
                                
                                //Bytes from index 0x1E + _PUMP_TOTALS_BUFFER_SHIFT_*index to 0x23 + _PUMP_TOTALS_BUFFER_SHIFT_*index 
                                //are the digits for the reference PPU used on this pump/hose
                                //Digits are in BCD format
                                /*COMMENTED OUT SINCE IT IS NOT REQUIRED
    //                            puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x1E)] & 0x0F);
    //                            puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x1F)] & 0x0F);
    //                            puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x20)] & 0x0F);
    //                            puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x21)] & 0x0F);
    //                            puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x22)] & 0x0F);
    //                            puartdisp->_messagetx[puartdisp->_messagelength + blkindex++] = (pumptrans->_buffer[PUMP_TDR_HOSE_DATA(hoseindex, 0x23)] & 0x0F);
                                */
                            }
                        }
                        puartdisp->_messagetx[payloadlengthindex] = blkindex;
                        puartdisp->_messagetx[puartdisp->_messagelength + blkindex] = RawCRCCheck((PSTR)puartdisp->_messagetx, (puartdisp->_messagelength + blkindex) - 1);
                        puartdisp->_messagelength += (blkindex + 0x01);

                        puartdisp->_messagestate = PENDING;
                    }
                    _g_pumps[index].PumpTransQueueDeallocate(&_g_pumps[index], pumptrans);
                    retval = true;
                }
                _g_pumps[index].PumpTransQueueUnlock(&_g_pumps[index]);
                break;
            }
        }
        if(!retval)
        {
            PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
            if(puartdisp)
            {
                puartdisp->_messagelength = 0;
                memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                puartdisp->_messagelength += 0x03;
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = RF_MUX_TOTALS_REPORT_RESPONSE;
                puartdisp->_messagetx[puartdisp->_messagelength++] = _RF_ACK_;
                puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength - 1);
                puartdisp->_messagelength++;

                puartdisp->_messagestate = PENDING;
            }
            retval = true;
        }
    }
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
bool RFPriceChangeReqResp(void *pparam)
{
    bool retval = false;
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(pmsg)
    {
        //Checking the CRC first to proceed
        //CRC must be located at this position for this message (See the protocol description file)
        uint8 crc = RawCRCCheck(pmsg->_buffer, pmsg->_buffersize - 1);
        if(crc != RFGetCRC(pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_], PBYTECAST(pmsg->_buffer)))//If there is a mismatch then return immediately and send a Nack back
        {
            PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
            if(puartdisp)
            {
                puartdisp->_messagelength = 0;
                memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                puartdisp->_messagelength += 0x03;
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = _RF_NACK_;
                puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength - 1);
                puartdisp->_messagelength++;

                puartdisp->_messagestate = PENDING;
            }
            return retval;
        }
         
        _ALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
        if(psinkmsg)
        {
            uint8 index = 0;
            
            psinkmsg->_messageid = DISPENSER_CONFIGURE_SINGLE_PRICE;
            psinkmsg->_messagetimeout = _SINK_TIMEOUT_500MS_;
            //This will give some time to the pump to recover from the last command and proceed with the next price change
            psinkmsg->_messagedelay = (uint16)(_SINK_TIMEOUT_500MS_*(double)rand()/(double)RAND_MAX) + _SINK_TIMEOUT_500MS_;
            psinkmsg->_messagetype = FIRSTFOUNDFOREVER;
            
            psinkmsg->_buffer[index++] = (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset);
            psinkmsg->_buffer[index++] = pmsg->_buffer[0x08];//Remote Hose/Product Identifier (must be translated to local)
            //Indexes and buffer length according to the protocol description for this command
            memcpy(&psinkmsg->_buffer[index], &pmsg->_buffer[0x09], 0x05);//five bytes for the price
            index += 0x06;//shift six bytes to store the numbers of bytes for the price
            psinkmsg->_buffer[index++] = 0x06;
            psinkmsg->_buffersize = index;
            
            psinkmsg->_messagestate = SINK_BUSY;
        }
        
        //If there wasn't any pending transactional state in the pointed pump position, just
        //return the Ack response in IDLE state
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
        if(puartdisp)
        {
            puartdisp->_messagelength = 0;
            memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
            memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
            puartdisp->_messagelength += 0x03;
            puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
            puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
            puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
            puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_];
            puartdisp->_messagetx[puartdisp->_messagelength++] = _RF_ACK_;
            puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength);
            puartdisp->_messagelength++;

            puartdisp->_messagestate = PENDING;
            retval = true;
        }
    }
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
bool RFGeneralPrintReqResp(void *pparam)
{
    bool retval = false;
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(pmsg)
    {
        //Checking the CRC first to proceed
        //CRC must be located at this position for this message (See the protocol description file)
        uint8 crc = RawCRCCheck(pmsg->_buffer, pmsg->_buffersize - 1);
        if(crc != RFGetCRC(pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_], PBYTECAST(pmsg->_buffer)))//If there is a mismatch then return immediately and send a Nack back
        {
            PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
            if(puartdisp)
            {
                puartdisp->_messagelength = 0;
                memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                puartdisp->_messagelength += 0x03;
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = _RF_NACK_;
                puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength - 1);
                puartdisp->_messagelength++;

                puartdisp->_messagestate = PENDING;
            }
            return retval;
        }
                
        _ALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
        if(psinkmsg)
        {
            psinkmsg->_messagetype = FIRSTFOUND;
            PDISPLAYLAYOUTPTR pdisplay = GetDisplayFromPumpID(pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset);
            if(pdisplay)
            {
                if(pdisplay->_dispid == DISPLAY1)
                {
                    psinkmsg->_messageid = PRINTER1_GENERIC_JOB;
                    if(_g_printerlayout._printerportside1 != PRINTER11_JOB)
                        psinkmsg->_messageid = PRINTER2_GENERIC_JOB;
                }
                else
                {
                    psinkmsg->_messageid = PRINTER2_GENERIC_JOB;
                    if(_g_printerlayout._printerportside2 != PRINTER21_JOB)
                        psinkmsg->_messageid = PRINTER1_GENERIC_JOB;                
                }
            }
                            
            memcpy(&psinkmsg->_buffer[0x01], &pmsg->_buffer[0x09], pmsg->_buffer[0x08]);
            psinkmsg->_buffersize = pmsg->_buffer[0x08];
            
            psinkmsg->_messagestate = SINK_BUSY;
        }
        
        //If there wasn't any pending transactional state in the pointed pump position, just
        //return the Ack response in IDLE state
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
        if(puartdisp)
        {
            puartdisp->_messagelength = 0;
            memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
            memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
            puartdisp->_messagelength += 0x03;
            puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
            puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
            puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
            puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_];
            puartdisp->_messagetx[puartdisp->_messagelength++] = _RF_ACK_;
            puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength);
            puartdisp->_messagelength++;

            puartdisp->_messagestate = PENDING;
            retval = true;
        }
    }
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
bool RFCreditSalePrintReqResp(void *pparam)
{
    bool retval = false;
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(pmsg)
    {
        PNEAR_PUMPPTR ppump = PNEAR_NULLPTR;
        FOR(uint8 pindex = 0x00, pindex < _g_dispenserlayoutinfo._numpositions, pindex++)
        {
            if(_g_pumps[pindex]._pumpid  == (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset))
            {
                ppump = &_g_pumps[pindex];
                break;
            }
        }
        if(ppump)
        {
            //DOUBLE CHECK SINCE IT ALREADY HAS BEEN SELECTED FROM THE SERVER STACK
            if(pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_] == RF_MUX_CREDIT_SALE_PRINT_REQUEST)
            {
                //Checking the CRC first to proceed
                //CRC must be located at this position for this message (See the protocol description file)
                uint8 crc = RawCRCCheck(pmsg->_buffer, pmsg->_buffersize - 1);
                if(crc != RFGetCRC(pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_], PBYTECAST(pmsg->_buffer)))//If there is a mismatch then return immediately and send a Nack back
                {
                    PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
                    if(puartdisp)
                    {
                        puartdisp->_messagelength = 0;
                        memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                        memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                        puartdisp->_messagelength += 0x03;
                        puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                        puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                        puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                        puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_];
                        puartdisp->_messagetx[puartdisp->_messagelength++] = _RF_NACK_;
                        puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength - 1);
                        puartdisp->_messagelength++;

                        puartdisp->_messagestate = PENDING;
                    }
                    return retval;
                }
                //If printing has been selected then post the print job
                PSINKMESSAGEPTR psinkmsg = PNEAR_NULLPTR;
                switch(ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_SIDE_DUMMY_DISPLAY)])
                {
                    case DISPLAY1:
                    {
                        AllocateMessageArraySlotConditional(_g_printerlayout._printerportside1);
                        FOR(uint8 arrindex = 0, arrindex < 0x04, arrindex++)
                        {
                            if(_g_pretval[arrindex] != PNEAR_NULLPTR)
                            {
                                if(_g_pretval[arrindex]->_buffer[0x00] == ppump->_pumpid)
                                {
                                    psinkmsg = _g_pretval[arrindex];
                                    break;
                                }
                            }else
                                break;
                        }   
                        psinkmsg->_messageid = _g_printerlayout._printerportside1;
                        break;
                    }
                    case DISPLAY2:
                    {
                        AllocateMessageArraySlotConditional(_g_printerlayout._printerportside2);
                        FOR(uint8 arrindex = 0, arrindex < 0x04, arrindex++)
                        {
                            if(_g_pretval[arrindex] != PNEAR_NULLPTR)
                            {
                                if(_g_pretval[arrindex]->_buffer[0x00] == ppump->_pumpid)
                                {
                                    psinkmsg = _g_pretval[arrindex];
                                    break;
                                }
                            }else
                                break;
                        }   
                        psinkmsg->_messageid = _g_printerlayout._printerportside2;
                        break;
                    }
                }
                //This validation is to avoid re-throwing the print command in case that the
                //invoice number command arrives late. If this condition is met it means that the
                //message is still awaiting (delayed)
                if(psinkmsg)
                {
                    psinkmsg->_messagetype = FIRSTFOUND;
                    psinkmsg->_messagedelay = 0x0000;
                    //memcpy(psinkmsg->_buffer, (const void*)NULL, 0x00);

                    uint8 index = 0x20 + 0x00;
                    //PROCESS THE REQUEST HERE SINCE THE REMOTE PEER HAS ALREADY RECEIVED THE TRANSACTION REPORT!!
                    //COPY THE N BYTES OF THE INVOICE NUMBER INTO THE LAST N BYTES OF THE "ppump->_trasactionbuffer"
                    //Invoice Number (10 bytes)
                    memcpy(&psinkmsg->_buffer[index], &pmsg->_buffer[0x08], 0x0A);
                    index += 0x0A;
                    //Car's Plate (8 bytes)
                    memcpy(&psinkmsg->_buffer[index], &pmsg->_buffer[0x12], 0x08);
                    index += 0x08;
                    //Balance (10 bytes)
                    memcpy(&psinkmsg->_buffer[index], &pmsg->_buffer[0x1A], 0x0A);
                    index += 0x0A;
                    //Company (20 bytes)
                    memcpy(&psinkmsg->_buffer[index], &pmsg->_buffer[0x24], 0x14);
                    index += 0x14;
                    //Account Name (20 bytes)
                    memcpy(&psinkmsg->_buffer[index], &pmsg->_buffer[0x38], 0x14);
                    index += 0x14;
                    //Daily Visits (1 byte)
                    memcpy(&psinkmsg->_buffer[index], &pmsg->_buffer[0x4C], 0x01);
                    index += 0x01;
                    //Weekly Visits (1 byte)
                    memcpy(&psinkmsg->_buffer[index], &pmsg->_buffer[0x4D], 0x01);
                    index += 0x01;
                    //Monthly Visits (1 byte)
                    memcpy(&psinkmsg->_buffer[index], &pmsg->_buffer[0x4E], 0x01);
                    index += 0x01;
                    //Daily Volume (7 bytes)
                    memcpy(&psinkmsg->_buffer[index], &pmsg->_buffer[0x4F], 0x07);
                    index += 0x07;
                    //Weekly Volume (7 bytes)
                    memcpy(&psinkmsg->_buffer[index], &pmsg->_buffer[0x56], 0x07);
                    index += 0x07;
                    //Monthly Volume (7 bytes)
                    memcpy(&psinkmsg->_buffer[index], &pmsg->_buffer[0x5D], 0x07);
                    index += 0x07;
                    //Account Type (16 bytes)
                    memcpy(&psinkmsg->_buffer[index], &pmsg->_buffer[0x64], 0x0F);
                    index += 0x0F;

                    psinkmsg->_buffer[index] =  pmsg->_buffer[0x74];
                    index += 0x01;
                    
                    psinkmsg->_messagestate = SINK_BUSY;
                }
                    
                    
                retval = true;
            }
            
        }
    }
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
bool RFCashSalePrintReqResp(void *pparam)
{
    bool retval = false;
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(pmsg)
    {
        PNEAR_PUMPPTR ppump = PNEAR_NULLPTR;
        FOR(uint8 pindex = 0x00, pindex < _g_dispenserlayoutinfo._numpositions, pindex++)
        {
            if(_g_pumps[pindex]._pumpid  == (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset))
            {
                ppump = &_g_pumps[pindex];
                break;
            }
        }
        if(ppump)
        {
            //DOUBLE CHECK SINCE IT ALREADY HAS BEEN SELECTED FROM THE SERVER STACK
            if(pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_] == RF_MUX_CASH_SALE_PRINT_REQUEST)
            {
                //Checking the CRC first to proceed
                //CRC must be located at this position for this message (See the protocol description file)
                uint8 crc = RawCRCCheck(pmsg->_buffer, pmsg->_buffersize - 1);
                if(crc != RFGetCRC(pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_], PBYTECAST(pmsg->_buffer)))//If there is a mismatch then return immediately and send a Nack back
                {
                    PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
                    if(puartdisp)
                    {
                        puartdisp->_messagelength = 0;
                        memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                        memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                        puartdisp->_messagelength += 0x03;
                        puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                        puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                        puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                        puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_];
                        puartdisp->_messagetx[puartdisp->_messagelength++] = _RF_NACK_;
                        puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength - 1);
                        puartdisp->_messagelength++;

                        puartdisp->_messagestate = PENDING;
                    }
                    return retval;
                }
                //If printing has been selected then post the print job
                if(ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_DESEA_IMPRIMIR_RECIBO)] == 0x39)
                {
                    PSINKMESSAGEPTR psinkmsg = PNEAR_NULLPTR;
                    switch(ppump->_trasactionbuffer[GetBufferIndexFromDisplayID(DISPLAY_SIDE_DUMMY_DISPLAY)])
                    {
                        case DISPLAY1:
                        {
                            AllocateMessageArraySlotConditional(_g_printerlayout._printerportside1);
                            FOR(uint8 arrindex = 0, arrindex < 0x04, arrindex++)
                            {
                                if(_g_pretval[arrindex] != PNEAR_NULLPTR)
                                {
                                    if(_g_pretval[arrindex]->_buffer[0x00] == ppump->_pumpid)
                                    {
                                        psinkmsg = _g_pretval[arrindex];
                                        break;
                                    }
                                }else
                                    break;
                            }   
                            psinkmsg->_messageid = _g_printerlayout._printerportside1;
                            break;
                        }
                        case DISPLAY2:
                        {
                            AllocateMessageArraySlotConditional(_g_printerlayout._printerportside2);
                            FOR(uint8 arrindex = 0, arrindex < 0x04, arrindex++)
                            {
                                if(_g_pretval[arrindex] != PNEAR_NULLPTR)
                                {
                                    if(_g_pretval[arrindex]->_buffer[0x00] == ppump->_pumpid)
                                    {
                                        psinkmsg = _g_pretval[arrindex];
                                        break;
                                    }
                                }else
                                    break;
                            }   
                            psinkmsg->_messageid = _g_printerlayout._printerportside2;
                            break;
                        }
                    }
                    //This validation is to avoid re-throwing the print command in case that the
                    //invoice number command arrives late. If this condition is met it means that the
                    //message is still awaiting (delayed)
                    if(psinkmsg)
                    {
                        psinkmsg->_messagetype = FIRSTFOUND;
                        psinkmsg->_messagedelay = 0x0000;
                        //memcpy(psinkmsg->_buffer, (const void*)NULL, 0x00);
                        
                        //PUMP POSITION GOES IN THE ARRAY INDEX 0X00 (SEE THE FILE "PRINTERBLL.C")
                        psinkmsg->_buffer[0x00] = ppump->_pumpid;
//                        //PUMP HOSE TO SELECT THE PRODUCT NAME
//                        psinkmsg->_buffer[0x01] = ppump->_authorizationinfo._hoseid;
//                        psinkmsg->_buffer[0x02] = 0x06;//Six bytes for the price (PPU)
//                        B78AD90CF552D9B30A(ppump->_authorizationinfo._price, PBYTECAST(&psinkmsg->_buffer[0x03]));
//                        psinkmsg->_buffer[0x09] = 0x08;//Eight bytes for the Volume
//                        B78AD90CF552D9B30A(ppump->_authorizationinfo._volumeamount, PBYTECAST(&psinkmsg->_buffer[0x0A]));
//                        psinkmsg->_buffer[0x12] = 0x08;//Eight bytes for the Volume
//                        B78AD90CF552D9B30A(ppump->_authorizationinfo._presetamount, PBYTECAST(&psinkmsg->_buffer[0x13]));
                        
                        //PROCESS THE REQUEST HERE SINCE THE REMOTE PEER HAS ALREADY RECEIVED THE TRANSACTION REPORT!!
                        //COPY THE N BYTES OF THE INVOICE NUMBER INTO THE LAST N BYTES OF THE "ppump->_trasactionbuffer"
                        memcpy(&psinkmsg->_buffer[0x20], &pmsg->_buffer[0x08], 0x0A);
                        
                        psinkmsg->_messagestate = SINK_BUSY;
                    }
                        
                }
                
                retval = true;
            }
            
        }
    }
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
//The last credit sale is always stored in EEPROM just in case of a RF link failure
bool RFPendingCreditSaleReqResp(void *pparam)
{
    bool retval = false;
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(pmsg)
    {
        //Checking the CRC first to proceed
        //CRC must be located at this position for this message (See the protocol description file)
        uint8 crc = RawCRCCheck(pmsg->_buffer, pmsg->_buffersize - 1);
        if(crc != RFGetCRC(pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_], PBYTECAST(pmsg->_buffer)))//If there is a mismatch then return immediately and send a Nack back
        {
            PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
            if(puartdisp)
            {
                puartdisp->_messagelength = 0;
                memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                puartdisp->_messagelength += 0x03;
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = _RF_NACK_;
                puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength - 1);
                puartdisp->_messagelength++;

                puartdisp->_messagestate = PENDING;
            }
            return retval;
        }
        
        //If there wasn't any pending transactional state in the pointed pump position, just
        //return the Ack response in IDLE state
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
        if(puartdisp)
        {
            puartdisp->_messagelength = 0;
            memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
            memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
            puartdisp->_messagelength += 0x03;
            puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
            puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
            puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
            puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_];
            puartdisp->_messagetx[puartdisp->_messagelength++] = 0x00;//??ESTADOPOSICION??

            uint8 arrsizes[0x03];
            uint8 arrindexes[0x03];
            //Indexes and Sizes to map the EOT buffer into the "Last Sale Recovery" response buffer
            if(_g_dispenserlayoutinfo._displaydigitsmode == 0x06)
            {
                uint8 tarrsizes[]   = { 0x04, 0x06, 0x06 };
                uint8 tarrindexes[] = { 0x0C, 0x11, 0x18 };
                memcpy(arrsizes, tarrsizes, 0x03);
                memcpy(arrindexes, tarrindexes, 0x03);
            }
            else
            {
                uint8 tarrsizes[]   = { 0x05, 0x08, 0x08 };
                uint8 tarrindexes[] = { 0x0C, 0x13, 0x1C };
                memcpy(arrsizes, tarrsizes, 0x03);
                memcpy(arrindexes, tarrindexes, 0x03);
            }
            
            I2CBusLock();
            ClearEepromBuffer();
            LoadEepromPage(EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE);
            if(GetEepromBuffer()[0x00] == (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset))
            {
                ClearEepromBuffer();
                //Second part of the credit transaction (data captured from the Transaction Data Request)
                LoadEepromPage(EEPROM_DISPENSER_PUMP14_TRANSACTION_PAGE);

                //Day
                puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_DAY] & 0x3F);
                //Month
                puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_MONTH] & 0x1F);
                //Year
                puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_YEAR] & 0xFF);
                
                //Hours
                puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_HOUR] & 0x3F);
                //Minutes
                puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_MINUTES] & 0x7F);
                
                PNEAR_PUMPPTR ppump = GetPumpFromRemotePositionID(pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset);
                if(ppump)
                {
                    //Remotely referenced Hose Id
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (&GetEepromBuffer()[0x07])[0x09] & 0x0F;
                }else
                    puartdisp->_messagelength++;
                                
                ClearEepromBuffer();
                //First part of the credit transaction (data captured from the display)
                LoadEepromPage(EEPROM_DISPENSER_PUMP13_TRANSACTION_PAGE);
                
                //Copying the serial identifier (iButton)
                memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &GetEepromBuffer()[GetBufferIndexFromDisplayID(DISPLAY_ESPERANDO_ID)], GetBufferLengthFromDisplayID(DISPLAY_ESPERANDO_ID));
                puartdisp->_messagelength += GetBufferLengthFromDisplayID(DISPLAY_ESPERANDO_ID);
                //Copying the milleage
                memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &GetEepromBuffer()[GetBufferIndexFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE)], GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE));
                puartdisp->_messagelength += (GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE) + 0x02);//Adding two bytes since the display buffer uses only 8 bytes and the protocol requires 10 bytes

                ClearEepromBuffer();
                //Second part of the credit transaction (data captured from the Transaction Data Request)
                LoadEepromPage(EEPROM_DISPENSER_PUMP14_TRANSACTION_PAGE);
                                
//                PNEAR_BYTE_PTR ptr = &(&GetEepromBuffer()[0x07])[arrindexes[0x00]];
//                FOR(uint8 pndx = 0, pndx < arrsizes[0x00], pndx++)
//                    ptr[pndx] &= 0x0F;

                
                uint16 reflength = puartdisp->_messagelength;
                
                //Copying the Ppu (only 5 bytes are copied! 1 byte missed from the protocol)
                if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                {
                    puartdisp->_messagetx[puartdisp->_messagelength++] = 0x00;
                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x0C], 0x04);
                    puartdisp->_messagelength += 0x04;
                }
                else
                {
                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x0C], 0x04);
                    puartdisp->_messagelength += 0x05;
                }
                
//                ptr = &(&GetEepromBuffer()[0x07])[arrindexes[0x01]];
//                FOR(pndx = 0, pndx < arrsizes[0x01], pndx++)
//                    ptr[pndx] &= 0x0F;
//
                //Copying the total Volume for this transaction
                if(_g_dispenserlayoutinfo._displaydigitsmode == 0x07)
                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x13], 0x08);
                else
                {
                    if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x11], 0x06);
                    else
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x11], 0x06);
                    
                }
                puartdisp->_messagelength += 0x08;

//                ptr = &(&GetEepromBuffer()[0x07])[arrindexes[0x02]];
//                FOR(pndx = 0, pndx < arrsizes[0x02], pndx++)
//                    ptr[pndx] &= 0x0F;
//
                //Copying the total Money for this transaction
                if(_g_dispenserlayoutinfo._displaydigitsmode == 0x07)
                {
                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x1D], 0x07);
                    puartdisp->_messagelength += 0x08;
                }
                else
                {
                    if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x18], 0x06);
                    else
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x18], 0x06);
                    
                    puartdisp->_messagelength += 0x08;
                }
                    
                PNEAR_BYTE_PTR ptr = (PNEAR_BYTE_PTR)&puartdisp->_messagetx[reflength];
                FOR(uint8 pndx = 0, pndx < (puartdisp->_messagelength - reflength), pndx++)
                    ptr[pndx] &= 0x0F;                
            }
            else
            {
                ClearEepromBuffer();
                LoadEepromPage(EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE);
                if(GetEepromBuffer()[0x00] == (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset))
                {
                    ClearEepromBuffer();
                    //Second part of the credit transaction (data captured from the Transaction Data Request)
                    LoadEepromPage(EEPROM_DISPENSER_PUMP24_TRANSACTION_PAGE);

                    //Day
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_DAY] & 0x3F);
                    //Month
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_MONTH] & 0x1F);
                    //Year
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_YEAR] & 0xFF);
                    
                    //Hours
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_HOUR] & 0x3F);
                    //Minutes
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_MINUTES] & 0x7F);
                    
                    PNEAR_PUMPPTR ppump = GetPumpFromRemotePositionID(pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset);
                    if(ppump)
                    {
                        //Remotely referenced Hose Id
                        puartdisp->_messagetx[puartdisp->_messagelength++] = (&GetEepromBuffer()[0x07])[0x09] & 0x0F;
                    }else
                        puartdisp->_messagelength++;

                    ClearEepromBuffer();
                    //First part of the credit transaction (data captured from the display)
                    LoadEepromPage(EEPROM_DISPENSER_PUMP23_TRANSACTION_PAGE);

                    //Copying the serial identifier (iButton)
                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &GetEepromBuffer()[GetBufferIndexFromDisplayID(DISPLAY_ESPERANDO_ID)], GetBufferLengthFromDisplayID(DISPLAY_ESPERANDO_ID));
                    puartdisp->_messagelength += GetBufferLengthFromDisplayID(DISPLAY_ESPERANDO_ID);
                    //Copying the milleage
                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &GetEepromBuffer()[GetBufferIndexFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE)], GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE));
                    puartdisp->_messagelength += (GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE) + 0x02);//Adding two bytes since the display buffer uses only 8 bytes and the protocol requires 10 bytes

                    ClearEepromBuffer();
                    //Second part of the credit transaction (data captured from the Transaction Data Request)
                    LoadEepromPage(EEPROM_DISPENSER_PUMP24_TRANSACTION_PAGE);
                    
//                    PNEAR_BYTE_PTR ptr = &(&GetEepromBuffer()[0x07])[arrindexes[0x00]];
//                    FOR(uint8 pndx = 0, pndx < arrsizes[0x00], pndx++)
//                        ptr[pndx] &= 0x0F;
                    
                    uint16 reflength = puartdisp->_messagelength;
                    
                    //Copying the Ppu (only 5 bytes are copied! 1 byte missed from the protocol)
                    if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                    {
                        puartdisp->_messagetx[puartdisp->_messagelength++] = 0x00;
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x0C], 0x04);
                        puartdisp->_messagelength += 0x04;
                    }
                    else
                    {
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x0C], 0x04);
                        puartdisp->_messagelength += 0x05;
                    }
                    
    //                ptr = &(&GetEepromBuffer()[0x07])[arrindexes[0x01]];
    //                FOR(pndx = 0, pndx < arrsizes[0x01], pndx++)
    //                    ptr[pndx] &= 0x0F;
    //
                    //Copying the total Volume for this transaction
                    if(_g_dispenserlayoutinfo._displaydigitsmode == 0x07)
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x13], 0x08);
                    else
                    {
                        if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                            memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x11], 0x06);
                        else
                            memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x11], 0x06);
                        
                    }
                    puartdisp->_messagelength += 0x08;

    //                ptr = &(&GetEepromBuffer()[0x07])[arrindexes[0x02]];
    //                FOR(pndx = 0, pndx < arrsizes[0x02], pndx++)
    //                    ptr[pndx] &= 0x0F;
    //
                    //Copying the total Money for this transaction
                    if(_g_dispenserlayoutinfo._displaydigitsmode == 0x07)
                    {
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x1D], 0x07);
                        puartdisp->_messagelength += 0x08;
                    }
                    else
                    {
                        if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                            memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x18], 0x06);
                        else
                            memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x18], 0x06);
                        
                        puartdisp->_messagelength += 0x08;
                    }
                        
                    PNEAR_BYTE_PTR ptr = (PNEAR_BYTE_PTR)&puartdisp->_messagetx[reflength];
                    FOR(uint8 pndx = 0, pndx < (puartdisp->_messagelength - reflength), pndx++)
                        ptr[pndx] &= 0x0F;   
                }
                else
                {
                    ClearEepromBuffer();
                    LoadEepromPage(EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE);
                    if(GetEepromBuffer()[0x00] == (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset))
                    {
                        ClearEepromBuffer();
                        //Second part of the credit transaction (data captured from the Transaction Data Request)
                        LoadEepromPage(EEPROM_DISPENSER_PUMP34_TRANSACTION_PAGE);

                        //Day
                        puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_DAY] & 0x3F);
                        //Month
                        puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_MONTH] & 0x1F);
                        //Year
                        puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_YEAR] & 0xFF);
                        
                        //Hours
                        puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_HOUR] & 0x3F);
                        //Minutes
                        puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_MINUTES] & 0x7F);
                        
                        PNEAR_PUMPPTR ppump = GetPumpFromRemotePositionID(pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset);
                        if(ppump)
                        {
                            //Remotely referenced Hose Id
                            puartdisp->_messagetx[puartdisp->_messagelength++] = (&GetEepromBuffer()[0x07])[0x09] & 0x0F;
                        }else
                            puartdisp->_messagelength++;

                        ClearEepromBuffer();
                        //First part of the credit transaction (data captured from the display)
                        LoadEepromPage(EEPROM_DISPENSER_PUMP33_TRANSACTION_PAGE);

                        //Copying the serial identifier (iButton)
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &GetEepromBuffer()[GetBufferIndexFromDisplayID(DISPLAY_ESPERANDO_ID)], GetBufferLengthFromDisplayID(DISPLAY_ESPERANDO_ID));
                        puartdisp->_messagelength += GetBufferLengthFromDisplayID(DISPLAY_ESPERANDO_ID);
                        //Copying the milleage
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &GetEepromBuffer()[GetBufferIndexFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE)], GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE));
                        puartdisp->_messagelength += (GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE) + 0x02);//Adding two bytes since the display buffer uses only 8 bytes and the protocol requires 10 bytes

                        ClearEepromBuffer();
                        //Second part of the credit transaction (data captured from the Transaction Data Request)
                        LoadEepromPage(EEPROM_DISPENSER_PUMP34_TRANSACTION_PAGE);

//                        PNEAR_BYTE_PTR ptr = &(&GetEepromBuffer()[0x07])[arrindexes[0x00]];
//                        FOR(uint8 pndx = 0, pndx < arrsizes[0x00], pndx++)
//                            ptr[pndx] &= 0x0F;
                        
                        uint16 reflength = puartdisp->_messagelength;
                        
                        //Copying the Ppu (only 5 bytes are copied! 1 byte missed from the protocol)
                        if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                        {
                            puartdisp->_messagetx[puartdisp->_messagelength++] = 0x00;
                            memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x0C], 0x04);
                            puartdisp->_messagelength += 0x04;
                        }
                        else
                        {
                            memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x0C], 0x04);
                            puartdisp->_messagelength += 0x05;
                        }
                        
        //                ptr = &(&GetEepromBuffer()[0x07])[arrindexes[0x01]];
        //                FOR(pndx = 0, pndx < arrsizes[0x01], pndx++)
        //                    ptr[pndx] &= 0x0F;
        //
                        //Copying the total Volume for this transaction
                        if(_g_dispenserlayoutinfo._displaydigitsmode == 0x07)
                            memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x13], 0x08);
                        else
                        {
                            if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                                memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x11], 0x06);
                            else
                                memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x11], 0x06);
                            
                        }
                        puartdisp->_messagelength += 0x08;

        //                ptr = &(&GetEepromBuffer()[0x07])[arrindexes[0x02]];
        //                FOR(pndx = 0, pndx < arrsizes[0x02], pndx++)
        //                    ptr[pndx] &= 0x0F;
        //
                        //Copying the total Money for this transaction
                        if(_g_dispenserlayoutinfo._displaydigitsmode == 0x07)
                        {
                            memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x1D], 0x07);
                            puartdisp->_messagelength += 0x08;
                        }
                        else
                        {
                            if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                                memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x18], 0x06);
                            else
                                memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x18], 0x06);
                            
                            puartdisp->_messagelength += 0x08;
                        }
                            
                        PNEAR_BYTE_PTR ptr = (PNEAR_BYTE_PTR)&puartdisp->_messagetx[reflength];
                        FOR(uint8 pndx = 0, pndx < (puartdisp->_messagelength - reflength), pndx++)
                            ptr[pndx] &= 0x0F;   
                    }
                    else
                    {
                        ClearEepromBuffer();
                        LoadEepromPage(EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE);
                        if(GetEepromBuffer()[0x00] == (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset))
                        {
                            ClearEepromBuffer();
                            //Second part of the credit transaction (data captured from the Transaction Data Request)
                            LoadEepromPage(EEPROM_DISPENSER_PUMP44_TRANSACTION_PAGE);

                            //Day
                            puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_DAY] & 0x3F);
                            //Month
                            puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_MONTH] & 0x1F);
                            //Year
                            puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_YEAR] & 0xFF);
                            
                            //Hours
                            puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_HOUR] & 0x3F);
                            //Minutes
                            puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_MINUTES] & 0x7F);
                            
                            PNEAR_PUMPPTR ppump = GetPumpFromRemotePositionID(pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset);
                            if(ppump)
                            {
                                //Remotely referenced Hose Id
                                puartdisp->_messagetx[puartdisp->_messagelength++] = (&GetEepromBuffer()[0x07])[0x09] & 0x0F;
                            }else
                                puartdisp->_messagelength++;

                            ClearEepromBuffer();
                            //First part of the credit transaction (data captured from the display)
                            LoadEepromPage(EEPROM_DISPENSER_PUMP43_TRANSACTION_PAGE);

                            //Copying the serial identifier (iButton)
                            memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &GetEepromBuffer()[GetBufferIndexFromDisplayID(DISPLAY_ESPERANDO_ID)], GetBufferLengthFromDisplayID(DISPLAY_ESPERANDO_ID));
                            puartdisp->_messagelength += GetBufferLengthFromDisplayID(DISPLAY_ESPERANDO_ID);
                            //Copying the milleage
                            memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &GetEepromBuffer()[GetBufferIndexFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE)], GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE));
                            puartdisp->_messagelength += (GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE) + 0x02);//Adding two bytes since the display buffer uses only 8 bytes and the protocol requires 10 bytes

                            ClearEepromBuffer();
                            //Second part of the credit transaction (data captured from the Transaction Data Request)
                            LoadEepromPage(EEPROM_DISPENSER_PUMP44_TRANSACTION_PAGE);

//                            PNEAR_BYTE_PTR ptr = &(&GetEepromBuffer()[0x07])[arrindexes[0x00]];
//                            FOR(uint8 pndx = 0, pndx < arrsizes[0x00], pndx++)
//                                ptr[pndx] &= 0x0F;
                            
                            uint16 reflength = puartdisp->_messagelength;
                            
                            //Copying the Ppu (only 5 bytes are copied! 1 byte missed from the protocol)
                            if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                            {
                                puartdisp->_messagetx[puartdisp->_messagelength++] = 0x00;
                                memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x0C], 0x04);
                                puartdisp->_messagelength += 0x04;
                            }
                            else
                            {
                                memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x0C], 0x04);
                                puartdisp->_messagelength += 0x05;
                            }
                            
            //                ptr = &(&GetEepromBuffer()[0x07])[arrindexes[0x01]];
            //                FOR(pndx = 0, pndx < arrsizes[0x01], pndx++)
            //                    ptr[pndx] &= 0x0F;
            //
                            //Copying the total Volume for this transaction
                            if(_g_dispenserlayoutinfo._displaydigitsmode == 0x07)
                                memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x13], 0x08);
                            else
                            {
                                if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x11], 0x06);
                                else
                                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x11], 0x06);
                                
                            }
                            puartdisp->_messagelength += 0x08;

            //                ptr = &(&GetEepromBuffer()[0x07])[arrindexes[0x02]];
            //                FOR(pndx = 0, pndx < arrsizes[0x02], pndx++)
            //                    ptr[pndx] &= 0x0F;
            //
                            //Copying the total Money for this transaction
                            if(_g_dispenserlayoutinfo._displaydigitsmode == 0x07)
                            {
                                memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x1D], 0x07);
                                puartdisp->_messagelength += 0x08;
                            }
                            else
                            {
                                if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x18], 0x06);
                                else
                                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x18], 0x06);
                                
                                puartdisp->_messagelength += 0x08;
                            }
                                
                            PNEAR_BYTE_PTR ptr = (PNEAR_BYTE_PTR)&puartdisp->_messagetx[reflength];
                            FOR(uint8 pndx = 0, pndx < (puartdisp->_messagelength - reflength), pndx++)
                                ptr[pndx] &= 0x0F;   

                        }
                    }
                }
            }
            I2CBusUnlock();

            puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength);
            puartdisp->_messagelength++;
            
            puartdisp->_messagestate = PENDING;
            retval = true;
        }
            
    }
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
bool RFPendingCashSaleReqResp(void *pparam)
{
    bool retval = false;
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(pmsg)
    {
        //Checking the CRC first to proceed
        //CRC must be located at this position for this message (See the protocol description file)
        uint8 crc = RawCRCCheck(pmsg->_buffer, pmsg->_buffersize - 1);
        if(crc != RFGetCRC(pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_], PBYTECAST(pmsg->_buffer)))//If there is a mismatch then return immediately and send a Nack back
        {
            PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
            if(puartdisp)
            {
                puartdisp->_messagelength = 0;
                memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                puartdisp->_messagelength += 0x03;
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = _RF_NACK_;
                puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength - 1);
                puartdisp->_messagelength++;

                puartdisp->_messagestate = PENDING;
            }
            return retval;
        }
        
        //If there wasn't any pending transactional state in the pointed pump position, just
        //return the Ack response in IDLE state
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
        if(puartdisp)
        {
            puartdisp->_messagelength = 0;
            memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
            memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
            puartdisp->_messagelength += 0x03;
            puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
            puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
            puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
            puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_];
            puartdisp->_messagetx[puartdisp->_messagelength++] = 0x00;//??ESTADOPOSICION??

            uint8 arrsizes[0x03];
            uint8 arrindexes[0x03];
            //Indexes and Sizes to map the EOT buffer into the "Last Sale Recovery" response buffer
            if(_g_dispenserlayoutinfo._displaydigitsmode == 0x06)
            {
                uint8 tarrsizes[]   = { 0x04, 0x06, 0x06 };
                uint8 tarrindexes[] = { 0x0C, 0x11, 0x18 };
                memcpy(arrsizes, tarrsizes, 0x03);
                memcpy(arrindexes, tarrindexes, 0x03);
            }
            else
            {
                uint8 tarrsizes[]   = { 0x05, 0x08, 0x08 };
                uint8 tarrindexes[] = { 0x0C, 0x13, 0x1C };
                memcpy(arrsizes, tarrsizes, 0x03);
                memcpy(arrindexes, tarrindexes, 0x03);
            }
            
            I2CBusLock();
            ClearEepromBuffer();
            LoadEepromPage(EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE);
            if(GetEepromBuffer()[0x00] == (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset))
            {
                ClearEepromBuffer();
                //Second part of the cash transaction (data captured from the Transaction Data Request)
                LoadEepromPage(EEPROM_DISPENSER_PUMP12_TRANSACTION_PAGE);

                //Day
                puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_DAY] & 0x3F);
                //Month
                puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_MONTH] & 0x1F);
                //Year
                puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_YEAR] & 0xFF);
                
                //Hours
                puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_HOUR] & 0x3F);
                //Minutes
                puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_MINUTES] & 0x7F);
                                
                PNEAR_PUMPPTR ppump = GetPumpFromRemotePositionID(pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset);
                if(ppump)
                {
                    //Remotely referenced Hose Id
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (&GetEepromBuffer()[0x07])[0x09] & 0x0F;
                }else
                    puartdisp->_messagelength++;
                
                uint16 reflength = puartdisp->_messagelength;
                
                //Copying the Ppu (only 5 bytes are copied! 1 byte missed from the protocol)
                if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                {
                    puartdisp->_messagetx[puartdisp->_messagelength++] = 0x00;
                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x0C], 0x04);
                    puartdisp->_messagelength += 0x04;
                }
                else
                {
                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x0C], 0x04);
                    puartdisp->_messagelength += 0x05;
                }

                //Copying the total Volume for this transaction
                if(_g_dispenserlayoutinfo._displaydigitsmode == 0x07)
                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x13], 0x08);
                else
                {
                    if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x11], 0x06);
                    else
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x11], 0x06);
                    
                }
                puartdisp->_messagelength += 0x08;
                
                //Copying the total Money for this transaction
                if(_g_dispenserlayoutinfo._displaydigitsmode == 0x07)
                {
                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x1D], 0x07);
                    puartdisp->_messagelength += 0x08;
                }
                else
                {
                    if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x18], 0x06);
                    else
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x18], 0x06);
                    
                    puartdisp->_messagelength += 0x08;
                }
                    
                PNEAR_BYTE_PTR ptr = (PNEAR_BYTE_PTR)&puartdisp->_messagetx[reflength];
                FOR(uint8 pndx = 0, pndx < (puartdisp->_messagelength - reflength), pndx++)
                    ptr[pndx] &= 0x0F;
                    
            }
            else
            {
                ClearEepromBuffer();
                LoadEepromPage(EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE);
                if(GetEepromBuffer()[0x00] == (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset))
                {
                    ClearEepromBuffer();
                    //Second part of the cash transaction (data captured from the Transaction Data Request)
                    LoadEepromPage(EEPROM_DISPENSER_PUMP22_TRANSACTION_PAGE);

                    //Day
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_DAY] & 0x3F);
                    //Month
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_MONTH] & 0x1F);
                    //Year
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_YEAR] & 0xFF);
                    
                    //Hours
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_HOUR] & 0x3F);
                    //Minutes
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_MINUTES] & 0x7F);
                    
                    PNEAR_PUMPPTR ppump = GetPumpFromRemotePositionID(pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset);
                    if(ppump)
                    {
                        //Remotely referenced Hose Id
                        puartdisp->_messagetx[puartdisp->_messagelength++] = (&GetEepromBuffer()[0x07])[0x09] & 0x0F;
                    }else
                        puartdisp->_messagelength++;

                    uint16 reflength = puartdisp->_messagelength;
                    
                    //Copying the Ppu (only 5 bytes are copied! 1 byte missed from the protocol)
                    if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                    {
                        puartdisp->_messagetx[puartdisp->_messagelength++] = 0x00;
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x0C], 0x04);
                        puartdisp->_messagelength += 0x04;
                    }
                    else
                    {
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x0C], 0x04);
                        puartdisp->_messagelength += 0x05;
                    }

                    //Copying the total Volume for this transaction
                    if(_g_dispenserlayoutinfo._displaydigitsmode == 0x07)
                    {
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x13], 0x08);
                        puartdisp->_messagelength += 0x08;
                    }
                    else
                    {
                        if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                            memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x11], 0x06);
                        else
                            memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x11], 0x06);
                        
                        puartdisp->_messagelength += 0x08;
                    }

                    //Copying the total Money for this transaction
                    if(_g_dispenserlayoutinfo._displaydigitsmode == 0x07)
                    {
                        memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x1D], 0x07);
                        puartdisp->_messagelength += 0x08;
                    }
                    else
                    {
                        if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                            memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x18], 0x06);
                        else
                            memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x18], 0x06);
                            
                        puartdisp->_messagelength += 0x08;
                    }
                        
                    PNEAR_BYTE_PTR ptr = (PNEAR_BYTE_PTR)&puartdisp->_messagetx[reflength];
                    FOR(uint8 pndx = 0, pndx < (puartdisp->_messagelength - reflength), pndx++)
                        ptr[pndx] &= 0x0F;
                        
                }
                else
                {
                    ClearEepromBuffer();
                    LoadEepromPage(EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE);
                    if(GetEepromBuffer()[0x00] == (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset))
                    {
                        ClearEepromBuffer();
                        //Second part of the cash transaction (data captured from the Transaction Data Request)
                        LoadEepromPage(EEPROM_DISPENSER_PUMP32_TRANSACTION_PAGE);

                        //Day
                        puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_DAY] & 0x3F);
                        //Month
                        puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_MONTH] & 0x1F);
                        //Year
                        puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_YEAR] & 0xFF);
                        
                        //Hours
                        puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_HOUR] & 0x3F);
                        //Minutes
                        puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_MINUTES] & 0x7F);
                        
                        PNEAR_PUMPPTR ppump = GetPumpFromRemotePositionID(pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset);
                        if(ppump)
                        {
                            //Remotely referenced Hose Id
                            puartdisp->_messagetx[puartdisp->_messagelength++] = (&GetEepromBuffer()[0x07])[0x09] & 0x0F;
                        }else
                            puartdisp->_messagelength++;
                        
                        uint16 reflength = puartdisp->_messagelength;
                                                
                        //Copying the Ppu (only 5 bytes are copied! 1 byte missed from the protocol)
                        if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                        {
                            puartdisp->_messagetx[puartdisp->_messagelength++] = 0x00;
                            memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x0C], 0x04);
                            puartdisp->_messagelength += 0x04;
                        }
                        else
                        {
                            memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x0C], 0x04);
                            puartdisp->_messagelength += 0x05;
                        }
                            
                        //Copying the total Volume for this transaction
                        if(_g_dispenserlayoutinfo._displaydigitsmode == 0x07)
                        {
                            memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x13], 0x08);
                            puartdisp->_messagelength += 0x08;
                        }
                        else
                        {
                            if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                                memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x11], 0x06);
                            else    
                                memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x11], 0x06);
                                
                            puartdisp->_messagelength += 0x08;
                        }

                        //Copying the total Money for this transaction
                        if(_g_dispenserlayoutinfo._displaydigitsmode == 0x07)
                        {
                            memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x1D], 0x07);
                            puartdisp->_messagelength += 0x08;
                        }
                        else
                        {
                            if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                                memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x18], 0x06);
                            else
                                memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x18], 0x06);
                            
                            puartdisp->_messagelength += 0x08;
                        }
                            
                        PNEAR_BYTE_PTR ptr = (PNEAR_BYTE_PTR)&puartdisp->_messagetx[reflength];
                        FOR(uint8 pndx = 0, pndx < (puartdisp->_messagelength - reflength), pndx++)
                            ptr[pndx] &= 0x0F;
                            
                    }
                    else
                    {
                        ClearEepromBuffer();
                        LoadEepromPage(EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE);
                        if(GetEepromBuffer()[0x00] == (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset))
                        {
                            ClearEepromBuffer();
                            //Second part of the cash transaction (data captured from the Transaction Data Request)
                            LoadEepromPage(EEPROM_DISPENSER_PUMP42_TRANSACTION_PAGE);

                            //Day
                            puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_DAY] & 0x3F);
                            //Month
                            puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_MONTH] & 0x1F);
                            //Year
                            puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_YEAR] & 0xFF);
                            
                            //Hours
                            puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_HOUR] & 0x3F);
                            //Minutes
                            puartdisp->_messagetx[puartdisp->_messagelength++] = (GetEepromBuffer()[RTC_MINUTES] & 0x7F);
                            
                            PNEAR_PUMPPTR ppump = GetPumpFromRemotePositionID(pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset);
                            if(ppump)
                            {
                                //Remotely referenced Hose Id
                                puartdisp->_messagetx[puartdisp->_messagelength++] = (&GetEepromBuffer()[0x07])[0x09] & 0x0F;
                            }else
                                puartdisp->_messagelength++;
                            
                            uint16 reflength = puartdisp->_messagelength;
                            
                            //Copying the Ppu (only 5 bytes are copied! 1 byte missed from the protocol)
                            if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                            {
                                puartdisp->_messagetx[puartdisp->_messagelength++] = 0x00;
                                memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x0C], 0x05);
                                puartdisp->_messagelength += 0x04;
                            }
                            else
                            {
                                memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x0C], 0x05);
                                puartdisp->_messagelength += 0x05;
                            }   

                            //Copying the total Volume for this transaction
                            if(_g_dispenserlayoutinfo._displaydigitsmode == 0x07)
                            {
                                memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x13], 0x08);
                                puartdisp->_messagelength += 0x08;
                            }
                            else    
                            {
                                if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x11], 0x06);
                                else
                                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x11], 0x06);
                                
                                puartdisp->_messagelength += 0x08;
                            }

                            //Copying the total Money for this transaction
                            if(_g_dispenserlayoutinfo._displaydigitsmode == 0x07)
                            {
                                memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x1D], 0x07);
                                puartdisp->_messagelength += 0x08;
                            }
                            else
                            {
                                if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
                                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x18], 0x06);
                                else
                                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&GetEepromBuffer()[0x07])[0x18], 0x06);
                                
                                puartdisp->_messagelength += 0x08;
                            }
                            
                            PNEAR_BYTE_PTR ptr = (PNEAR_BYTE_PTR)&puartdisp->_messagetx[reflength];
                            FOR(uint8 pndx = 0, pndx < (puartdisp->_messagelength - reflength), pndx++)
                                ptr[pndx] &= 0x0F;
                                
                        }
                    }
                }
            }
            I2CBusUnlock();
        
            puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength);
            puartdisp->_messagelength++;

            puartdisp->_messagestate = PENDING;
            retval = true;
        }
            
    }
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
bool RFBroadcastConfigurationReqResp(void *pparam)
{
    bool retval = false;
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(pmsg)
    {
        uint8 index = 0;
        //Checking the CRC first to proceed
        //CRC must be located at this position for this message (See the protocol description file)
        uint8 crc = RawCRCCheck(pmsg->_buffer, pmsg->_buffersize - 1);
        if(crc != RFGetCRC(pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_], PBYTECAST(pmsg->_buffer)))//If there is a mismatch then return immediately and send a Nack back
        {
            PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
            if(puartdisp)
            {
                puartdisp->_messagelength = 0;
                memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                puartdisp->_messagelength += 0x03;
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = _RF_NACK_;
                puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength);
                puartdisp->_messagelength++;

                puartdisp->_messagestate = PENDING;
            }
            return retval;
        }
        
        I2CBusLock();
        
        ClearEepromBuffer();
        LoadEepromPage(EEPROM_CONFIGURATION_PAGE7);
        //See (Eeprom.h) to understand the used index values
        //Saving the Money Symbol
        GetEepromBuffer()[0x00] = pmsg->_buffer[0x07];
        GetEepromBuffer()[0x01] = pmsg->_buffer[0x08];
        //Saving Timeout to Autonomous Mode
        GetEepromBuffer()[0x0A] = pmsg->_buffer[0x0E];
        //Number of Printed Copies
        GetEepromBuffer()[0x08] = pmsg->_buffer[0x10];
        
        //"0204" IS THE DEFAULT PASSWORD FOR THE CONFIGURATION OPTIONS        
        GetEepromBuffer()[0x0D] = 0x30;
        GetEepromBuffer()[0x0E] = 0x32;
        GetEepromBuffer()[0x0F] = 0x30;
        GetEepromBuffer()[0x10] = 0x34;    
        
        StoreEepromPage(EEPROM_CONFIGURATION_PAGE7);
        
        index = 0;
        ClearRtcBuffer();
        //Seconds
        GetRtcBuffer()[index++] = 0x00;
        //Minutes
        GetRtcBuffer()[index++] = pmsg->_buffer[0x0D];
        //Hour
        GetRtcBuffer()[index++] = pmsg->_buffer[0x0C];
        //Day of Week
        GetRtcBuffer()[index++] = pmsg->_buffer[0x0E];
        //Day
        GetRtcBuffer()[index++] = pmsg->_buffer[0x09];
        //Month
        GetRtcBuffer()[index++] = pmsg->_buffer[0x0A];
        //Year
        GetRtcBuffer()[index++] = pmsg->_buffer[0x0B];
        //Updating the RTC, please wait...
        UpdateRTC();
        
        ClearEepromBuffer();
        //Copying the first heading line (30 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_FIRST32START_], &pmsg->_buffer[0x11], 0x1E);
        //Copying the second heading line (30 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_SECOND32START_], &pmsg->_buffer[0x2F], 0x1E);
        StoreEepromPage(EEPROM_CONFIGURATION_PAGE0);
        
        ClearEepromBuffer();
        //Copying the third heading line (30 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_FIRST32START_], &pmsg->_buffer[0x4D], 0x1E);
        //Copying the fourth heading line (30 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_SECOND32START_], &pmsg->_buffer[0x6B], 0x1E);
        StoreEepromPage(EEPROM_CONFIGURATION_PAGE1);

        ClearEepromBuffer();
        //Copying the fifth heading line (30 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_FIRST32START_], &pmsg->_buffer[0x89], 0x1E);
        //Copying the first footer line (30 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_SECOND32START_], &pmsg->_buffer[0xA7], 0x1E);
        StoreEepromPage(EEPROM_CONFIGURATION_PAGE2);

        ClearEepromBuffer();
        //Copying the second footer line (30 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_FIRST32START_], &pmsg->_buffer[0xC5], 0x1E);
        //Copying the third footer line (30 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_SECOND32START_], &pmsg->_buffer[0xE3], 0x1E);
        StoreEepromPage(EEPROM_CONFIGURATION_PAGE3);
        
        ClearEepromBuffer();
        //Position #1
        //Copying the first product name (16 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_], &pmsg->_buffer[0x101], 0x10);
        //Copying the second product name (16 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_], &pmsg->_buffer[0x111], 0x10);
        //Copying the third product name (16 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_], &pmsg->_buffer[0x121], 0x10);
        //Copying the fourth product name (16 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_], &pmsg->_buffer[0x131], 0x10);
        StoreEepromPage(EEPROM_DISPENSER_PUMP1_PRODUCT_NAMES_PAGE);
        
        ClearEepromBuffer();
        //Position #2
        //Copying the first product name (16 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_], &pmsg->_buffer[0x101], 0x10);
        //Copying the second product name (16 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_], &pmsg->_buffer[0x111], 0x10);
        //Copying the third product name (16 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_], &pmsg->_buffer[0x121], 0x10);
        //Copying the fourth product name (16 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_], &pmsg->_buffer[0x131], 0x10);
        StoreEepromPage(EEPROM_DISPENSER_PUMP2_PRODUCT_NAMES_PAGE);
        
        ClearEepromBuffer();
        //Position #3
        //Copying the first product name (16 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_], &pmsg->_buffer[0x101], 0x10);
        //Copying the second product name (16 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_], &pmsg->_buffer[0x111], 0x10);
        //Copying the third product name (16 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_], &pmsg->_buffer[0x121], 0x10);
        //Copying the fourth product name (16 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_], &pmsg->_buffer[0x131], 0x10);
        StoreEepromPage(EEPROM_DISPENSER_PUMP3_PRODUCT_NAMES_PAGE);
        
        ClearEepromBuffer();
        //Position #4
        //Copying the first product name (16 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_], &pmsg->_buffer[0x101], 0x10);
        //Copying the second product name (16 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_], &pmsg->_buffer[0x111], 0x10);
        //Copying the third product name (16 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_], &pmsg->_buffer[0x121], 0x10);
        //Copying the fourth product name (16 bytes)
        memcpy(&GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_], &pmsg->_buffer[0x131], 0x10);
        StoreEepromPage(EEPROM_DISPENSER_PUMP4_PRODUCT_NAMES_PAGE);
        
        I2CBusUnlock();
        
        _ALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
        if(psinkmsg)
        {
            psinkmsg->_messageid = DISPENSER_LOAD_EEPROM_SETTINGS;
            psinkmsg->_messagedelay = 0x0000;
            psinkmsg->_messagetype = FIRSTFOUND;
            psinkmsg->_messagestate = SINK_BUSY;
        }
        
        //PRODUCT NAME HOSE #5 DISCARDED BY NOW!!!
        retval = true;
    }
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
bool RFPositionConfigurationReqResp(void *pparam)
{
    bool retval = false;
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(pmsg)
    {
        //Checking the CRC first to proceed
        //CRC must be located at this position for this message (See the protocol description file)
        uint8 crc = RawCRCCheck(pmsg->_buffer, pmsg->_buffersize - 1);
        if(crc != RFGetCRC(pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_], PBYTECAST(pmsg->_buffer)))//If there is a mismatch then return immediately and send a Nack back
        {
            PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
            if(puartdisp)
            {
                puartdisp->_messagelength = 0;
                memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                puartdisp->_messagelength += 0x03;
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = _RF_NACK_;
                puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength);
                puartdisp->_messagelength++;

                puartdisp->_messagestate = PENDING;
            }
            return retval;
        }
        
        I2CBusLock();
        uint8 realposition = (uint8)pmsg->_buffer[0x05] - _g_dispenserlayoutinfo._positionoffset;
        //This is the Product Code / Hose Mapping. This block is critical since with these mappings,
        //the device (MUX) will be able to resolve which hose code must send to the PRESET when it 
        //receives the PRESET data for a requested Authorization (ie: Credit Payment)
        ClearEepromBuffer();
        //See (Eeprom.h) and the protocol description file to understand the applied index values
        LoadEepromPage(EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE);
        if(GetEepromBuffer()[0x00] == realposition)
        {
            GetEepromBuffer()[0x01] = pmsg->_buffer[0x08];
            GetEepromBuffer()[0x02] = pmsg->_buffer[0x09];
            GetEepromBuffer()[0x03] = pmsg->_buffer[0x0A];
            GetEepromBuffer()[0x04] = pmsg->_buffer[0x0B];
            GetEepromBuffer()[0x05] = pmsg->_buffer[0x0C];
            StoreEepromPage(EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE);
            _g_pumps[0x00]._positionlocked = (pmsg->_buffer[0x0C] == 0x00)? true: false;
        }else{
            ClearEepromBuffer();
            //See (Eeprom.h) and the protocol description file to understand the applied index values
            LoadEepromPage(EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE);
            if(GetEepromBuffer()[0x00] == realposition)
            {
                GetEepromBuffer()[0x01] = pmsg->_buffer[0x08];
                GetEepromBuffer()[0x02] = pmsg->_buffer[0x09];
                GetEepromBuffer()[0x03] = pmsg->_buffer[0x0A];
                GetEepromBuffer()[0x04] = pmsg->_buffer[0x0B];
                GetEepromBuffer()[0x05] = pmsg->_buffer[0x0C];
                StoreEepromPage(EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE);
                _g_pumps[0x01]._positionlocked = (pmsg->_buffer[0x0C] == 0x00)? true: false;
            }else{
                ClearEepromBuffer();
                //See (Eeprom.h) and the protocol description file to understand the applied index values
                LoadEepromPage(EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE);
                if(GetEepromBuffer()[0x00] == realposition)
                {
                    GetEepromBuffer()[0x01] = pmsg->_buffer[0x08];
                    GetEepromBuffer()[0x02] = pmsg->_buffer[0x09];
                    GetEepromBuffer()[0x03] = pmsg->_buffer[0x0A];
                    GetEepromBuffer()[0x04] = pmsg->_buffer[0x0B];
                    GetEepromBuffer()[0x05] = pmsg->_buffer[0x0C];
                    StoreEepromPage(EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE);
                    _g_pumps[0x02]._positionlocked = (pmsg->_buffer[0x0C] == 0x00)? true: false;
                }else{
                    ClearEepromBuffer();
                    //See (Eeprom.h) and the protocol description file to understand the applied index values
                    LoadEepromPage(EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE);
                    if(GetEepromBuffer()[0x00] == realposition)
                    {
                        GetEepromBuffer()[0x01] = pmsg->_buffer[0x08];
                        GetEepromBuffer()[0x02] = pmsg->_buffer[0x09];
                        GetEepromBuffer()[0x03] = pmsg->_buffer[0x0A];
                        GetEepromBuffer()[0x04] = pmsg->_buffer[0x0B];
                        GetEepromBuffer()[0x05] = pmsg->_buffer[0x0C];
                        StoreEepromPage(EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE);
                        _g_pumps[0x03]._positionlocked = (pmsg->_buffer[0x0C] == 0x00)? true: false;
                    }
                }
            }
        }
        I2CBusUnlock();
        retval = true;
    }
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
bool RFIslandOperDataReqResp(void *pparam)
{
    bool retval = false;
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(pmsg)
    {
        FOR(uint8 index = 0x00, index < _g_dispenserlayoutinfo._numpositions, index++)
        {
            if(_g_pumps[index]._pumpid == (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset))
            {
                //We always pick the top most item from the Pump Transactional Queue
                _g_pumps[index].PumpTransQueueLock(&_g_pumps[index]);
                PNEAR_PUMPTRANSACTIONALSTATEPTR pumptrans = _g_pumps[index].PumpTransQueueDequeue(&_g_pumps[index]);
                PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
                if(puartdisp)
                {
                    uint8 index = 0x00;
                    puartdisp->_messagelength = 0;
                    memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                    memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                    puartdisp->_messagelength += 0x03;
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (pumptrans->_buffer[0x00] + _g_dispenserlayoutinfo._positionoffset);
                    puartdisp->_messagetx[puartdisp->_messagelength++] = RF_MUX_ISLAND_OPER_DATA_RESPONSE;
                    puartdisp->_messagetx[puartdisp->_messagelength++] = 0x00;//??
                    //Here we copy the incoming buffer from the Hose ID, so we subtract one to remove the Pump ID that always comes in the first position
                    memcpy(&puartdisp->_messagetx[puartdisp->_messagelength], &(&pumptrans->_buffer[0x01])[index], GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_CEDULA));
                    puartdisp->_messagelength += (GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_CEDULA) + 0x03);
                    index += GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_CEDULA);
                    
                    memcpy(&puartdisp->_messagetx[0x13], &(&pumptrans->_buffer[0x01])[index], GetBufferLengthFromDisplayID(DISPLAY_INGRESE_PASSWORD));
                    puartdisp->_messagelength += (GetBufferLengthFromDisplayID(DISPLAY_INGRESE_PASSWORD) +  0x03);
                    index += GetBufferLengthFromDisplayID(DISPLAY_INGRESE_PASSWORD);
                    
                    puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength - 1);
                    puartdisp->_messagelength++;

                    puartdisp->_messagestate = PENDING;
                }
                _g_pumps[index].PumpTransQueueDeallocate(&_g_pumps[index], pumptrans);
                _g_pumps[index].PumpTransQueueUnlock(&_g_pumps[index]);
                retval = true;

                break;
            }
        }
    }
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
bool RFIslandAdminReqResp(void *pparam)
{
    bool retval = false;
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(pmsg)
    {
        //Checking the CRC first to proceed
        //CRC must be located at this position for this message (See the protocol description file)
        uint8 crc = RawCRCCheck(pmsg->_buffer, pmsg->_buffersize - 1);
        if(crc != RFGetCRC(pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_], PBYTECAST(pmsg->_buffer)))//If there is a mismatch then return immediately and send a Nack back
        {
            PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
            if(puartdisp)
            {
                puartdisp->_messagelength = 0;
                memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                puartdisp->_messagelength += 0x03;
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                puartdisp->_messagetx[puartdisp->_messagelength++] = RF_MUX_ISLAND_ADMIN_REQUEST;
                puartdisp->_messagetx[puartdisp->_messagelength++] = _RF_NACK_;
                puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength);
                puartdisp->_messagelength++;

                puartdisp->_messagestate = PENDING;
            }
            return retval;
        }

        uint8 realposition = (uint8)pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset;
        //This is the Lock/Unlock state of the current position
        
        bool operok = false;
        PDISPLAYLAYOUTPTR pdisplay = GetDisplayFromPumpID(realposition);
        if(pmsg->_buffer[0x07] == PUMP_CLOSED)
        {
            pdisplay->_currentstate = DISPLAY_WORK_SHIFT_OPERATOR_ACCEPTED;
            pdisplay->_val0 = realposition;
            pdisplay->_val1 = pmsg->_buffer[0x07];
            operok = true;
        }
        else if(pmsg->_buffer[0x07] == PUMP_OPEN)
        {
            pdisplay->_currentstate = DISPLAY_WORK_SHIFT_OPERATOR_ACCEPTED;
            pdisplay->_val0 = realposition;
            pdisplay->_val1 = pmsg->_buffer[0x07];
            operok = true;
        }
        else
        {
            pdisplay->_currentstate = DISPLAY_WORK_SHIFT_OPERATOR_REJECTED;
            pdisplay->_val0 = realposition;
            pdisplay->_val1 = pmsg->_buffer[0x07];
        }

        PNEAR_PUMPPTR ppump = GetPumpFromRemotePositionID(realposition);
        if(operok)
        {
            I2CBusLock();
            ClearEepromBuffer();
            //See (Eeprom.h) and the protocol description file to understand the applied index values
            LoadEepromPage(EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE);
            if(GetEepromBuffer()[0x00] == realposition)
            {
                GetEepromBuffer()[0x05] = pdisplay->_val1;
                if(ppump)
                    ppump->_positionlocked = (GetEepromBuffer()[0x05] == PUMP_CLOSED);
                
                StoreEepromPage(EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE);
            }else{
                ClearEepromBuffer();
                //See (Eeprom.h) and the protocol description file to understand the applied index values
                LoadEepromPage(EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE);
                if(GetEepromBuffer()[0x00] == realposition)
                {
                    GetEepromBuffer()[0x05] = pdisplay->_val1;
                    if(ppump)
                        ppump->_positionlocked = (GetEepromBuffer()[0x05] == PUMP_CLOSED);
                    
                    StoreEepromPage(EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE);
                }else{
                    ClearEepromBuffer();
                    //See (Eeprom.h) and the protocol description file to understand the applied index values
                    LoadEepromPage(EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE);
                    if(GetEepromBuffer()[0x00] == realposition)
                    {
                        GetEepromBuffer()[0x05] = pdisplay->_val1;
                        if(ppump)
                            ppump->_positionlocked = (GetEepromBuffer()[0x05] == PUMP_CLOSED);
                        
                        StoreEepromPage(EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE);
                    }else{
                        ClearEepromBuffer();
                        //See (Eeprom.h) and the protocol description file to understand the applied index values
                        LoadEepromPage(EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE);
                        if(GetEepromBuffer()[0x00] == realposition)
                        {
                            GetEepromBuffer()[0x05] = pdisplay->_val1;
                            if(ppump)
                                ppump->_positionlocked = (GetEepromBuffer()[0x05] == PUMP_CLOSED);
                            
                            StoreEepromPage(EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE);
                        }
                    }
                }
            }    
            I2CBusUnlock();
        }
        if(ppump)
            ppump->_pumprftransstate = RF_IDLE;
    }
    return retval;
}

//@Created by: HIJH
//@Date: Octobre de 2016
//Resolves the size of the buffer for the "General Print" message. If the buffer is complete
//then it reports back "true" to the Resolver.
uint16 GeneralPrintRequestPayloadSizeResolver(LPVOID pbuffer)
{
    //Four additional bytes that include the CRC + some extra bytes included in the stream (see the protocol documentation for further explanations)
    return (uint16)(PBYTECAST(pbuffer)[0x08] + _RF_STREAM_FIXED_LENGTH_HEADER_BLOCK_ + 0x03);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//@Created by: HIJH
//@Date: Octobre de 2016
//Function created to report the current number of requests per second
void RFSpeedReport(uint8 command, uint8 position)
{
    char8 buffer[16];
    FontData fontdata;
    _g_rfrx._rfcounter++;
    if(_g_rfrx._rftimeoutmultiplier >= _TIMEOUT_BASE_TIME_)
    {
        fontdata._size = 0x01;
        fontdata._backcolor = 0xFFFF;
        fontdata._forecolor = 0x0000;
        fontdata._opaquebackground = DISP_TEXT_OPAQUE;
        
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
        if(puartdisp)
        {
            memset(buffer, 0x00, sizeof(buffer));
            sprintf(buffer, "[%d Rps]", _g_rfrx._rfcounter);
                
            //MOVE ALL OF THESE TEXTS TO THE EEPROM
            puartdisp->_messagelength = DisplayOutputString(0x010, 0x1AA, puartdisp->_messagetx, buffer, strlen(buffer), fontdata);
            puartdisp->_messagestate = PENDING;
        }
        _g_rfrx._rftimeoutmultiplier = 0;
        _g_rfrx._rfcounter = 0x0000;
    }
    _g_rfrx._rfperpetualcounter++;
    {
        fontdata._size = 0x01;
        fontdata._backcolor = 0xFFFF;
        fontdata._forecolor = 0x0000;
        fontdata._opaquebackground = DISP_TEXT_OPAQUE;
        
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
        if(puartdisp)
        {
            memset(buffer, 0x00, sizeof(buffer));
            sprintf(buffer, "[%ld]", _g_rfrx._rfperpetualcounter);
                
            //MOVE ALL OF THESE TEXTS TO THE EEPROM
            puartdisp->_messagelength = DisplayOutputString(0x00E, 0x198, puartdisp->_messagetx, buffer, strlen(buffer), fontdata);
            puartdisp->_messagestate = PENDING;
        }
    }
    {
        fontdata._size = 0x01;
        fontdata._backcolor = 0xFFFF;
        fontdata._forecolor = 0x0000;
        fontdata._opaquebackground = DISP_TEXT_OPAQUE;
        
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
        if(puartdisp)
        {
            memset(buffer, 0x00, sizeof(buffer));
            sprintf(buffer, "[0x%2X]", command);
                
            //MOVE ALL OF THESE TEXTS TO THE EEPROM
            puartdisp->_messagelength = DisplayOutputString(0x00E, 0x186, puartdisp->_messagetx, buffer, strlen(buffer), fontdata);
            puartdisp->_messagestate = PENDING;
        }
    }
    {
        fontdata._size = 0x01;
        fontdata._backcolor = 0xFFFF;
        fontdata._forecolor = 0x0000;
        fontdata._opaquebackground = DISP_TEXT_OPAQUE;
        
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
        if(puartdisp)
        {
            memset(buffer, 0x00, sizeof(buffer));
            sprintf(buffer, "[%d]", position);
                
            //MOVE ALL OF THESE TEXTS TO THE EEPROM
            puartdisp->_messagelength = DisplayOutputString(0x00E, 0x174, puartdisp->_messagetx, buffer, strlen(buffer), fontdata);
            puartdisp->_messagestate = PENDING;
        }
    }
}
/////////////////////////
bool RFCopyPrintReqResp(void *pparam)
{
    bool retval = false;
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    if(pmsg)
    {
        FOR(uint8 index = 0x00, index < _g_dispenserlayoutinfo._numpositions, index++)
        {
            PNEAR_PUMPPTR ppump = GetPumpFromRemotePositionID(_g_pumps[index]._pumpid);
            if(_g_pumps[index]._pumpid  == (pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset))
            {
                //We always pick the top most item from the Pump Transactional Queue
                _g_pumps[index].PumpTransQueueLock(&_g_pumps[index]);
                PNEAR_PUMPTRANSACTIONALSTATEPTR pumptrans = _g_pumps[index].PumpTransQueueDequeue(&_g_pumps[index]);
                //USED ONLY FOR TRANSACTIONAL STATE CONSISTENCY
                PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_RF);
                
                if(puartdisp)
                {
                    puartdisp->_messagelength = 0;
                    memset(puartdisp->_messagetx, 0x00, sizeof(puartdisp->_messagetx));
                    memcpy(puartdisp->_messagetx, _g_rfslaveheader, 0x03); //Explicit buffer length to avoid wasting CPU cycles in sizeof!
                    puartdisp->_messagelength += 0x03;
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)(_g_stationidentifier & 0xFF);
                    puartdisp->_messagetx[puartdisp->_messagelength++] = (uint8)((_g_stationidentifier >> 8) & 0xFF);
                    puartdisp->_messagetx[puartdisp->_messagelength++] = pmsg->_buffer[_RF_STREAM_POSITION_INDEX_];
                    puartdisp->_messagetx[puartdisp->_messagelength++] = RF_MUX_COPY_REQUEST_RESPONSE;
                    puartdisp->_messagetx[puartdisp->_messagelength++] = 0x0B;//_g_pumps[index]._pumpid;
                    puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((PSTR)puartdisp->_messagetx, puartdisp->_messagelength - 1);
                    puartdisp->_messagelength++;

                    puartdisp->_messagestate = PENDING;
                }
                _g_pumps[index].PumpTransQueueDeallocate(&_g_pumps[index], pumptrans);
                _g_pumps[index].PumpTransQueueUnlock(&_g_pumps[index]);                 
            }
             if(ppump)
                    ppump->_pumprftransstate = RF_IDLE;            
        }
        _ALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
    	if(psinkmsg)
    	{
    		psinkmsg->_messagetype = FIRSTFOUND;
    		PDISPLAYLAYOUTPTR pdisplay = GetDisplayFromPumpID(pmsg->_buffer[_RF_STREAM_POSITION_INDEX_] - _g_dispenserlayoutinfo._positionoffset);
    		if(pdisplay)
    		{
    			if(pdisplay->_dispid == DISPLAY1)
                {
                    psinkmsg->_messageid = PRINTER1_GENERIC_JOB_LOGO;
                    if(_g_printerlayout._printerportside1 != PRINTER11_JOB)
                        psinkmsg->_messageid = PRINTER2_GENERIC_JOB_LOGO;
                }
                else
                {
                    psinkmsg->_messageid = PRINTER2_GENERIC_JOB_LOGO;
                    if(_g_printerlayout._printerportside2 != PRINTER21_JOB)
                        psinkmsg->_messageid = PRINTER1_GENERIC_JOB_LOGO;                
                }
    		}
    						
    		memcpy(&psinkmsg->_buffer[0x01], &pmsg->_buffer[0x09], pmsg->_buffer[0x08]);
    		psinkmsg->_buffersize = pmsg->_buffer[0x08];
    		
    		psinkmsg->_messagestate = SINK_BUSY;
    	}
    }
    return retval;    
}


/* [] END OF FILE */
