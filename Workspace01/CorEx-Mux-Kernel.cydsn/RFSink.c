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
#include <project.h>
#include "GlobalDefinitions.h"
#include "SinkManager.h"
#include "SinkObservables.h"
#include "UARTManager.h"
#include "Configuration.h"
#include "RFLink.h"
#include "Eeprom.h"

//extern uint8 _g_rfslaveheader[];
//void RFTEST(LPVOID pparam);

RFAutoRun _g_rfautorun;

void RFServerRequestHandler(LPVOID pparam);
void RFServerAutonomousMonitor(LPVOID pparam);

//@Created By: HIJH
//@Septembre de 2016
void RFSinkRegistration()
{
    PSINKSUBSCRIBERPTR psubscriber = AllocateSubscriberSlot();
    if(psubscriber)
    {
        uint8 index = 0;
        //The maximum number of observable identifiers is _OBSERVED_IDENTIFIERS_
        //located in the header file "SinkManager.h"
        //Every identifier is paired with its respective Callback method pointer
        
        psubscriber->_arrmsgid[index] = UART_RF_RECEPTION;
        psubscriber->Callback[index++] = RFServerRequestHandler;

        psubscriber->_arrmsgid[index] = UART_RF_AUTONOMOUS_MONITOR;
        psubscriber->Callback[index++] = RFServerAutonomousMonitor;
        
        //psubscriber->_arrmsgid[index] = UART_RF_TEST;
        //psubscriber->Callback[index++] = RFTEST;
    }
    
    /*_ALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
    if(psinkmsg)
    {
        psinkmsg->_messageid = UART_RF_TEST;
        psinkmsg->_messagetimeoutlimit = _SINK_TIMEOUT_1S_;
        psinkmsg->_messagetype = FIRSTFOUNDFOREVER;
        psinkmsg->_messagestate = SINK_BUSY;
    }
    */

    _ALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
    if(psinkmsg)
    {
        psinkmsg->_messageid = UART_RF_AUTONOMOUS_MONITOR;
        psinkmsg->_messagedelay = _SINK_TIMEOUT_10S_;
        psinkmsg->_messagetimeoutlimit = _SINK_TIMEOUT_1S_;
        psinkmsg->_messagetype = FIRSTFOUNDFOREVER;
        psinkmsg->_messagestate = SINK_BUSY;
    }
    
    _g_rfautorun._state = RF_NON_AUTONOMOUS;
    _g_rfautorun._toautonomoustimetoken = 0x0000;
    
    _g_rfautorun._timeouttoreceive = 2*_SINK_TIMEOUT_100MS_;
    _g_rfautorun._timeouttoreceivemultiplier = 0x0000;
    
    I2CBusLock();
    ClearEepromBuffer();
    LoadEepromPage(EEPROM_CONFIGURATION_PAGE7);
    //Timeout to autonomous mode in seconds, 1 byte (see Eeprom.h for the memory layout)
    _g_rfautorun._timeouttoautonomous = (uint8)GetEepromBuffer()[0x0A];
    I2CBusUnlock();
    
    _g_rfrx._rfcounter = 0x0000;
    _g_rfrx._rftimeoutmultiplier = 0x0000;
    _g_rfrx._rfperpetualcounter = 0x0000;
    
}

/*void RFTEST(LPVOID pparam)
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
        puartdisp->_messagetx[puartdisp->_messagelength++] = 0x01;
        puartdisp->_messagetx[puartdisp->_messagelength++] = 0xA1;
        puartdisp->_messagetx[puartdisp->_messagelength] = RawCRCCheck((char*)puartdisp->_messagetx, puartdisp->_messagelength - 1);
        puartdisp->_messagelength++;

        puartdisp->_messagestate = PENDING;
    }
}*/

//@Created By: HIJH
//@Octobre de 2016
//This is to detect the "Autonomous Condition", meaning that there is no Remote Peer (NSX).
//Upon this condition the unit works just with a few options available.
void RFServerAutonomousMonitor(LPVOID pparam)
{
    //PSINKMESSAGEPTR psinkmsg = (PSINKMESSAGEPTR)pparam;
    
    uint16 rolloverref = _g_rfautorun._toautonomoustimetoken;
    _g_rfautorun._toautonomoustimetoken++;
    //This is to detect the rollover condition on the "timetoken counter" and keep the module "Autonomous"
    if(rolloverref > _g_rfautorun._toautonomoustimetoken)
        _g_rfautorun._toautonomoustimetoken = _g_rfautorun._timeouttoautonomous;
    
    if(_g_rfautorun._toautonomoustimetoken >= _g_rfautorun._timeouttoautonomous)
        _g_rfautorun._state = RF_AUTONOMOUS;
    else
        _g_rfautorun._state = RF_NON_AUTONOMOUS;
    
}

/* [] END OF FILE */
