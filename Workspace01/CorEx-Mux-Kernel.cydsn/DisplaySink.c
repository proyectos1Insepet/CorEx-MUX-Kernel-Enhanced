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
#include "SinkManager.h"
#include "SinkObservables.h"
#include "UARTManager.h"
#include "Display.h"

void InitDisplays();
void Display1Logic(void *pdata);
void Display2Logic(void *pdata);
void DisplayRaiseTimeoutsAndStateChanges(void *pparam);
void DisplayUpdateDateTime(void *pparam);
void DisplayUpdateHomeAnimation(void *pparam);
void ToggleScreen(void *pparam);
void HoseValidationCallback(LPVOID pparam);

//@Created By: HIJH
//@Septembre de 2016
void DisplaySinkRegistration()
{
    PSINKSUBSCRIBERPTR psubscriber = AllocateSubscriberSlot();
    if(psubscriber)
    {
        uint8 index = 0;
        //The maximum number of observable identifiers is _OBSERVED_IDENTIFIERS_
        //located in the header file "SinkManager.h"
        //Every identifier is paired with its respective Callback method pointer
        psubscriber->_arrmsgid[index] = DISPLAY1_RECEPTION;
        psubscriber->Callback[index++] = Display1Logic;
        
        psubscriber->_arrmsgid[index] = DISPLAY2_RECEPTION;
        psubscriber->Callback[index++] = Display2Logic;
        
        psubscriber->_arrmsgid[index] = GLOBAL_TICK_NOTIFICATION;
        psubscriber->Callback[index++] = DisplayRaiseTimeoutsAndStateChanges;

        psubscriber->_arrmsgid[index] = DISPLAY1_UPDATE_DATETIME;
        psubscriber->Callback[index++] = DisplayUpdateDateTime;
        
        psubscriber->_arrmsgid[index] = DISPLAY2_UPDATE_DATETIME;
        psubscriber->Callback[index++] = DisplayUpdateDateTime;
        
        #ifdef _DISPLAY_AV_SCREENS_
        psubscriber->_arrmsgid[index] = TOGGLE_SCREEN_AND_WRITE_TEST;
        psubscriber->Callback[index++] = ToggleScreen;
        #endif
    }

    psubscriber = AllocateSubscriberSlot();
    if(psubscriber)
    {
        uint8 index = 0;

        psubscriber->_arrmsgid[index] = DISPLAY1_ANIMATE_HOME;
        psubscriber->Callback[index++] = DisplayUpdateHomeAnimation;
        
        psubscriber->_arrmsgid[index] = DISPLAY2_ANIMATE_HOME;
        psubscriber->Callback[index++] = DisplayUpdateHomeAnimation;
    }
    
    //Display1 and Display2 initialization    
    InitDisplays();
    
    PSINKMESSAGEPTR psinkmsg = AllocateMessageSlot();
    if(psinkmsg)
    {
        psinkmsg->_messageid = GLOBAL_TICK_NOTIFICATION;
        psinkmsg->_messagetype = FIRSTFOUNDFOREVER;
        psinkmsg->_messagetimeoutlimit = _SINK_TIMEOUT_10MS_;
        psinkmsg->_messagestate = SINK_BUSY;
    }    
    
    psinkmsg = AllocateMessageSlotConditional(DISPLAY1_ANIMATE_HOME);
    if(psinkmsg)
    {
        _g_display1._pdata = psinkmsg;
        psinkmsg->_pdata = &_g_display1;
        psinkmsg->_messageid = DISPLAY1_ANIMATE_HOME;
        psinkmsg->_messagetype = FIRSTFOUNDFOREVER;
        psinkmsg->_messagetimeoutlimit = _SINK_TIMEOUT_100MS_;
        psinkmsg->_messagestate = SINK_BUSY;
    }

    psinkmsg = AllocateMessageSlotConditional(DISPLAY2_ANIMATE_HOME);
    if(psinkmsg)
    {
        _g_display2._pdata = psinkmsg;
        psinkmsg->_pdata = &_g_display2;
        psinkmsg->_messageid = DISPLAY2_ANIMATE_HOME;
        psinkmsg->_messagetype = FIRSTFOUNDFOREVER;
        psinkmsg->_messagetimeoutlimit = _SINK_TIMEOUT_100MS_;
        psinkmsg->_messagestate = SINK_BUSY;
    }
    
    /////////////////////////////////////////////////////////////////////////////////////////////////
    ///THIS BLOCK IS MEANT ONLY TO CAPTURE THE SCREEN IDS
    /////////////////////////////////////////////////////////////////////////////////////////////////  
    #ifdef _DISPLAY_AV_SCREENS_
    _REALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
    if(psinkmsg)
    {
        
        psinkmsg->_messageid = TOGGLE_SCREEN_AND_WRITE_TEST;
        psinkmsg->_messagetype = ALLINTERESTEDFOREVER;
        psinkmsg->_messagetimeoutlimit = _SINK_TIMEOUT_1S_;
        psinkmsg->_messagestate = SINK_BUSY;
        
    }
    #endif
    /////////////////////////////////////////////////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////////////////////////////////
///THIS BLOCK IS ONLY MEANT TO CAPTURE THE SCREEN IDS
/////////////////////////////////////////////////////////////////////////////////////////////////  
#ifdef _DISPLAY_AV_SCREENS_
uint16 _picindex = 0x00;
void ToggleScreen(void *pparam) 
{
    char pbuffer[] = { 0xAA, 0x70, (_picindex >> 8) & 0xFF, _picindex & 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
    //char pbuffer[] = { 0xAA, 0x52, 0xCC, 0x33, 0xC3, 0x3C };
    UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
    if(puartdisp)
    {
        puartdisp->_messagelength = sizeof(pbuffer) / sizeof(pbuffer[0]);
        memcpy(puartdisp->_messagetx, pbuffer, puartdisp->_messagelength);
        puartdisp->_messagestate = PENDING;
    }

    puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
    if(puartdisp)
    {
        char8 pstring[16];
        sprintf(pstring, "Indice Pantalla: %d", _picindex);
        FontData fontdata = { 0x02, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0xD500 };
        puartdisp->_messagelength = DisplayOutputString(0xd0, 0xbb, puartdisp->_messagetx, pstring, strlen(pstring), fontdata); 
        puartdisp->_messagestate = PENDING;
    }
    if(_picindex++ >= 200)
        _picindex = 0;
    
    
}
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////


//@Created by: HIJH
//@Date: Septembre de 2016
//Sets both displays to the initial screen configured in the initialization structures (see DisplayBLL.c)
void InitDisplays()
{   
    char pbuffer0[] = { 0xAA, 0x70, (_g_display1._pcurrflow->_scrid >> 8) & 0xFF, _g_display1._pcurrflow->_scrid & 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
    UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
    if(puartdisp)
    {
        _g_display1._psequesteredmessagesink = PNEAR_NULLPTR;
        puartdisp->_messagelength = sizeof(pbuffer0) / sizeof(pbuffer0[0]);
        memcpy(puartdisp->_messagetx, pbuffer0, puartdisp->_messagelength);
        puartdisp->_messagestate = PENDING;
    }

    char pbuffer1[] = { 0xAA, 0x70, (_g_display2._pcurrflow->_scrid >> 8) & 0xFF, _g_display2._pcurrflow->_scrid & 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
    puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
    if(puartdisp)
    {
        _g_display2._psequesteredmessagesink = PNEAR_NULLPTR;
        puartdisp->_messagelength = sizeof(pbuffer1) / sizeof(pbuffer1[0]);
        memcpy(puartdisp->_messagetx, pbuffer1, puartdisp->_messagelength);
        puartdisp->_messagestate = PENDING;
    }
}

//@Created by: HIJH
//@Date: Septembre de 2016
//This definition is located in the structure "DisplayFlow" when the field "timeoutlimit" is different from zero.
//The update is achieved only if the current id for either display has been defined to timeout. 
//This callback function will be invoked every 10 milliseconds
void DisplayRaiseTimeoutsAndStateChanges(void *pparam)
{
    PDISPLAYFLOWPTR pdispflowiterator = _g_displayflow;
    while(pdispflowiterator->_scrid != DISPLAY_NULL)
    {
        if(_g_display1._pcurrflow->_scrid == pdispflowiterator->_scrid)
        {
            bool timeoutraised = false;
            if(pdispflowiterator->_responsetimeoutlimit > 0)
            {
                _g_display1._timeoutmultiplier++;
                //VALIDATE THE TIMEOUT CONDITION HERE!!!
                double timespan = (_SINK_TIMEOUT_10MS_*_g_display1._timeoutmultiplier);
                if(timespan >= (double)_g_display1._pcurrflow->_responsetimeoutlimit)
                {
                    _g_display1._currscrcode = 0x00;
                    _g_display1._timeoutscrid = _g_display1._pcurrflow->_timeoutscrid;
                    
                    _ALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
                    if(psinkmsg)
                    {
                        char8 pdummybuffer[] = { 0xAA, 0x00, 0x00, 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
                        psinkmsg->_messageid = DISPLAY1_RECEPTION;
                        psinkmsg->_messagetype = FIRSTFOUND;
                        memcpy(psinkmsg->_buffer, (const void*)pdummybuffer, sizeof(pdummybuffer));
                        psinkmsg->_buffersize = sizeof(pdummybuffer);
                        psinkmsg->_messagestate = SINK_BUSY;
                        
                    }
                    timeoutraised = true;
                }
            }
            if(!timeoutraised)
            {
                FOR(uint8 index = 0, index < _MAX_DISPLAY_PATHS_, index++)
                {
                    if(_g_display1._currentstate == _g_display1._pcurrflow->_statecodes[index])
                    {
                        _g_display1._currscrcode = 0x00;
                        _g_display1._statechangescrid = _g_display1._pcurrflow->_statenodes[index];
                        
                        _ALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
                        if(psinkmsg)
                        {
                            char8 pdummybuffer[] = { 0xAA, 0x00, 0x00, 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
                            psinkmsg->_messageid = DISPLAY1_RECEPTION;
                            psinkmsg->_messagetype = FIRSTFOUND;
                            memcpy(psinkmsg->_buffer, (const void*)pdummybuffer, sizeof(pdummybuffer));
                            psinkmsg->_buffersize = sizeof(pdummybuffer);
                            psinkmsg->_messagestate = SINK_BUSY;
                            
                        }
                        break;
                    }
                }
            }
                        
            break;
        }        
        pdispflowiterator++;
    }    
    pdispflowiterator = _g_displayflow;
    while(pdispflowiterator->_scrid != DISPLAY_NULL)
    {
        if(_g_display2._pcurrflow->_scrid == pdispflowiterator->_scrid)
        {
            bool timeoutraised = false;
            if(pdispflowiterator->_responsetimeoutlimit > 0)
            {
                _g_display2._timeoutmultiplier++;
                //VALIDATE THE TIMEOUT CONDITION HERE!!!
                double timespan = (_SINK_TIMEOUT_10MS_*_g_display2._timeoutmultiplier);
                if(timespan > (double)_g_display2._pcurrflow->_responsetimeoutlimit)
                {
                    _g_display2._currscrcode = 0x00;
                    _g_display2._timeoutscrid = _g_display2._pcurrflow->_timeoutscrid;
                    
                    _ALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
                    if(psinkmsg)
                    {
                        char8 pdummybuffer[] = { 0xAA, 0x00, 0x00, 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
                        psinkmsg->_messageid = DISPLAY2_RECEPTION;
                        psinkmsg->_messagetype = FIRSTFOUND;
                        //memcpy(sinkmsg->_buffer, (const void*)pbuffer, buffersize);
                        memcpy(psinkmsg->_buffer, (const void*)pdummybuffer, sizeof(pdummybuffer));
                        psinkmsg->_buffersize = sizeof(pdummybuffer);
                        psinkmsg->_messagestate = SINK_BUSY;
                        
                    }
                    timeoutraised = true;
                }
            }
            if(!timeoutraised)
            {
                FOR(uint8 index = 0, index < _MAX_DISPLAY_PATHS_, index++)
                {
                    if(_g_display2._currentstate == _g_display2._pcurrflow->_statecodes[index])
                    {
                        _g_display2._currscrcode = 0x00;
                        _g_display2._statechangescrid = _g_display2._pcurrflow->_statenodes[index];
                        
                        _ALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
                        if(psinkmsg)
                        {
                            char8 pdummybuffer[] = { 0xAA, 0x00, 0x00, 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
                            psinkmsg->_messageid = DISPLAY2_RECEPTION;
                            psinkmsg->_messagetype = FIRSTFOUND;
                            memcpy(psinkmsg->_buffer, (const void*)pdummybuffer, sizeof(pdummybuffer));
                            psinkmsg->_buffersize = sizeof(pdummybuffer);
                            psinkmsg->_messagestate = SINK_BUSY;
                            
                        }
                        break;
                    }
                }
            }
            
            break;
        }
        pdispflowiterator++;
    }    
}

/* [] END OF FILE */
