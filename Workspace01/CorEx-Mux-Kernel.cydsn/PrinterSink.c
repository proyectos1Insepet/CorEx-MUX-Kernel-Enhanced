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
#include "SinkManager.h"
#include "SinkObservables.h"
#include "Eeprom.h"
#include "Printer.h"

void Printer11Callback(void *pdata);
void Printer12Callback(void *pdata);
void Printer21Callback(void *pdata);
void Printer22Callback(void *pdata);
void Printer1GenericCallback(void *pdata);
void Printer2GenericCallback(void *pdata);
void Printer1GenericCallbacklogo(void *pdata);
void Printer2GenericCallbacklogo(void *pdata);

PrinterLayout _g_printerlayout;

extern uint8 _g_separator[_MAX_PRINTER_LINE_WIDTH_];
extern uint8 _g_padcharacters[_MAX_PRINTER_LINE_WIDTH_];

void PrinterSinkRegistration()
{
    PSINKSUBSCRIBERPTR psubscriber = AllocateSubscriberSlot();
    if(psubscriber)
    {
        uint8 index = 0;
        //The maximum number of observable identifiers is _OBSERVED_IDENTIFIERS_
        //located in the header file "SinkManager.h"
        //Every identifier is paired with its respective Callback method pointer
        psubscriber->_arrmsgid[index] = PRINTER11_JOB;
        psubscriber->Callback[index++] = Printer11Callback;

        psubscriber->_arrmsgid[index] = PRINTER12_JOB;
        psubscriber->Callback[index++] = Printer12Callback;
        
        psubscriber->_arrmsgid[index] = PRINTER21_JOB;
        psubscriber->Callback[index++] = Printer21Callback;

        psubscriber->_arrmsgid[index] = PRINTER22_JOB;
        psubscriber->Callback[index++] = Printer22Callback;

        psubscriber->_arrmsgid[index] = PRINTER1_GENERIC_JOB;
        psubscriber->Callback[index++] = Printer1GenericCallback;

        psubscriber->_arrmsgid[index] = PRINTER2_GENERIC_JOB;
        psubscriber->Callback[index++] = Printer2GenericCallback;
        
        psubscriber->_arrmsgid[index] = PRINTER1_GENERIC_JOB_LOGO;
        psubscriber->Callback[index++] = Printer1GenericCallbacklogo;

        psubscriber->_arrmsgid[index] = PRINTER2_GENERIC_JOB_LOGO;
        psubscriber->Callback[index++] = Printer2GenericCallbacklogo;
    }
    
    //This is the default printer port allocation
    _g_printerlayout._printerportside1 = PRINTER11_JOB;
    _g_printerlayout._printerportside2 = PRINTER21_JOB;
    //Loading printer port configuration from the EEPROM here ...
    
    memset(_g_padcharacters, _ASCII_SPACE_, _MAX_PRINTER_LINE_WIDTH_);
    memset(_g_separator, _ASCII_DASH_, _MAX_PRINTER_LINE_WIDTH_);
}

/* [] END OF FILE */
