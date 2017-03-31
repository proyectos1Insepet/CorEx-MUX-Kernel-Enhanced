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
#include "SinkManager.h"
#include "SinkObservables.h"
#include "Dispenser.h"
#include "Display.h"
#include "Eeprom.h"
#include "Printer.h"
#include "AuxPSoC.h"
#include "Rtc.h"

uint8 PadSpacesToCenter(uint8 stringlength);
uint8 MeasurePrinterASCIIBufferLength(PNEAR_BYTE_PTR ptrbuffer);

void Print(PSINKMESSAGEPTR pmsg, uint8 printerport);
void PrintGeneric(PSINKMESSAGEPTR pmsg, uint8 printerport);
void GetProductNameFromPumpAndHose(uint8 pumpid, uint8 hoseid, PNEAR_BYTE_PTR pbuffer);
uint8 FormatNumberLSDFirst(PNEAR_BYTE_PTR pinput, uint8 inputsize, PNEAR_BYTE_PTR poutput, uint8 decimals);

uint8 _g_linefeed[] = {_ASCII_LINEFEED_, _ASCII_CR_};
uint8 _g_padcharacters[_MAX_PRINTER_LINE_WIDTH_];
uint8 _g_auxprinterbuffer[_MAX_PRINTER_LINE_WIDTH_];
uint8 _g_separator[_MAX_PRINTER_LINE_WIDTH_];
uint8 _g_autocut[] = {  0x1D, 0x56, 0x31 };
uint8 _g_starting_position[] = {  0x1D, 0x0C };


#ifdef PRINTER_TYPE_PANEL
    uint8 _g_logoprintcmd[] = { 0x0A, 0x1B, 0x40, 0x0A, 0x0A, 0x0A };
#endif
#ifdef PRINTER_TYPE_KIOSK
    uint8 _g_logoprintcmd[] = { 0x0A, 0x1D, 0x4C, 0x07, 0x00, 0x0A };
#endif

extern uint16 _g_logosize;
extern uint8 *_g_ptrlogo;

void PrintLogo(uint8 printerport)
{
    WriteAuxPSoC((PNEAR_BYTE_PTR)_g_ptrlogo, _g_logosize, printerport);
    WriteAuxPSoC((PNEAR_BYTE_PTR)_g_logoprintcmd, 0x06, printerport);
}

//@Created by: HIJH
//@Date: Octobre 2 de 2016
//Printer spooler for the printer on port J14 (RS232)
//Printer ports have been separated to give the chance for 
//fine adjustments on the print layout between printers independenly.
void Printer11Callback(void *pdata)
{
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pdata;
    
    I2CBusLock();
    ClearEepromBuffer();
    LoadEepromPage(EEPROM_CONFIGURATION_PAGE7);
    uint8 numberofprintedcopies = GetEepromBuffer()[0x08];
    I2CBusUnlock();    
    FOR(uint8 index = 0, index < numberofprintedcopies, index++)
    {
        #ifndef _NO_LOGO_
            PrintLogo(PRINTER_PORT1_SIDE1);
        #endif
            Print(pmsg, PRINTER_PORT1_SIDE1);
    }
    #ifdef PRINTER_TYPE_KIOSK
        //Auto cut command
        PRINTER_AUTOCUT(PRINTER_PORT1_SIDE1);        
        //PRINTER_RESET(PRINTER_PORT1_SIDE1);        
    #endif
}

//@Created by: HIJH
//@Date: Octobre 2 de 2016
//Printer spooler for the printer on port J16 (TTL)
//Printer ports have been separated to give the chance for 
//fine adjustments on the print layout between printers independenly.
void Printer12Callback(void *pdata)
{
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pdata;

    I2CBusLock();
    ClearEepromBuffer();
    LoadEepromPage(EEPROM_CONFIGURATION_PAGE7);
    uint8 numberofprintedcopies = GetEepromBuffer()[0x08];
    I2CBusUnlock();    
    FOR(uint8 index = 0, index < numberofprintedcopies, index++)
    {
        #ifndef _NO_LOGO_
            PrintLogo(PRINTER_PORT1_SIDE2);
        #endif
        Print(pmsg, PRINTER_PORT1_SIDE2);
    }
    #ifdef PRINTER_TYPE_KIOSK
        //Auto cut command
        PRINTER_AUTOCUT(PRINTER_PORT1_SIDE2);        
        //PRINTER_RESET(PRINTER_PORT1_SIDE2);        
    #endif
}

//@Created by: HIJH
//@Date: Octobre 2 de 2016
//Printer spooler for the printer on port J15 (RS232)
//Printer ports have been separated to give the chance for 
//fine adjustments on the print layout between printers independenly.
void Printer21Callback(void *pdata)
{
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pdata;

    I2CBusLock();
    ClearEepromBuffer();
    LoadEepromPage(EEPROM_CONFIGURATION_PAGE7);
    uint8 numberofprintedcopies = GetEepromBuffer()[0x08];
    I2CBusUnlock();    
    FOR(uint8 index = 0, index < numberofprintedcopies, index++)
    {
        #ifndef _NO_LOGO_
            PrintLogo(PRINTER_PORT2_SIDE1);
        #endif
        Print(pmsg, PRINTER_PORT2_SIDE1);
    }
    #ifdef PRINTER_TYPE_KIOSK
        //Auto cut command
        PRINTER_AUTOCUT(PRINTER_PORT2_SIDE1);        
        //PRINTER_RESET(PRINTER_PORT2_SIDE1);        
    #endif
}

//@Created by: HIJH
//@Date: Octobre 2 de 2016
//Printer spooler for the printer on port J17 (TTL)
//Printer ports have been separated to give the chance for 
//fine adjustments on the print layout between printers independenly.
void Printer22Callback(void *pdata)
{
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pdata;

    I2CBusLock();
    ClearEepromBuffer();
    LoadEepromPage(EEPROM_CONFIGURATION_PAGE7);
    uint8 numberofprintedcopies = GetEepromBuffer()[0x08];
    I2CBusUnlock();    
    FOR(uint8 index = 0, index < numberofprintedcopies, index++)
    {
        #ifndef _NO_LOGO_
            PrintLogo(PRINTER_PORT2_SIDE2);
        #endif
        Print(pmsg, PRINTER_PORT2_SIDE2);
    }
    #ifdef PRINTER_TYPE_KIOSK
        //Auto cut command
        PRINTER_AUTOCUT(PRINTER_PORT2_SIDE2);        
        //PRINTER_RESET(PRINTER_PORT2_SIDE2);        
    #endif
}

//@Created by: HIJH
//@Date: Octobre 2 de 2016
//Printer spooler for the printer on port J17 (TTL)
//Printer ports have been separated to give the chance for 
//fine adjustments on the print layout between printers independenly.
void Printer1GenericCallback(void *pdata)
{
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pdata;
    PrintGeneric(pmsg, PRINTER_PORT1_SIDE1);
}

void Printer2GenericCallback(void *pdata)
{
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pdata;
    PrintGeneric(pmsg, PRINTER_PORT2_SIDE1);
}

//@Created by: HIJH
//@Date: Octobre de 2016
//Counts the number of characters until either an EOS (End Of String, \0 = 0x00)
//or the maximum length for the printer line is reached.
uint8 MeasurePrinterASCIIBufferLength(PNEAR_BYTE_PTR ptrbuffer)
{
    uint8 retval = 0x00;
    while((ptrbuffer[retval] !=  _EOS_) && (retval < _MAX_PRINTER_LINE_WIDTH_))
        retval++;
    
    return retval;
}
//////////////LOGO
void Printer1GenericCallbacklogo(void *pdata)
{
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pdata;
    PrintLogo(PRINTER_PORT1_SIDE1);    
}
void Printer2GenericCallbacklogo(void *pdata)
{
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pdata;
    PrintLogo(PRINTER_PORT2_SIDE1);    
}
//@Created by: HIJH
//@Date: Octobre de 2016
//Counts the number of Spaces to add in order to center the printed text
uint8 PadSpacesToCenter(uint8 stringlength)
{
    return (uint8)((_MAX_PRINTER_LINE_WIDTH_ - stringlength)/2);
}

void GetProductNameFromPumpAndHose(uint8 pumpid, uint8 hoseid, PNEAR_BYTE_PTR pbuffer)
{
    ClearEepromBuffer();
    LoadEepromPage(EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE);
    if(pumpid == GetEepromBuffer()[0])
    {
        uint8 memid = 0x00;
        switch(hoseid)
        {
            case 0x00:
            memid = GetEepromBuffer()[0x01];
            break;
            case 0x01:
            memid = GetEepromBuffer()[0x02];
            break;
            case 0x02:
            memid = GetEepromBuffer()[0x03];
            break;
            case 0x03:
            memid = GetEepromBuffer()[0x04];
            break;
        }
        
        ClearEepromBuffer();
        LoadEepromPage(EEPROM_DISPENSER_PUMP1_PRODUCT_NAMES_PAGE);
        switch(memid)
        {
            case 0x01:
            memcpy(pbuffer, &GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_], _EEPROM_QUARTER_PAGE_SIZE_);
            break;
            case 0x02:
            memcpy(pbuffer, &GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_], _EEPROM_QUARTER_PAGE_SIZE_);
            break;
            case 0x03:
            memcpy(pbuffer, &GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_], _EEPROM_QUARTER_PAGE_SIZE_);
            break;
            case 0x04:
            memcpy(pbuffer, &GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_], _EEPROM_QUARTER_PAGE_SIZE_);
            break;
        }
        return;
    }
    ClearEepromBuffer();    
    LoadEepromPage(EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE);
    if(pumpid == GetEepromBuffer()[0])
    {
        uint8 memid = 0x00;
        switch(hoseid)
        {
            case 0x00:
            memid = GetEepromBuffer()[0x01];
            break;
            case 0x01:
            memid = GetEepromBuffer()[0x02];
            break;
            case 0x02:
            memid = GetEepromBuffer()[0x03];
            break;
            case 0x03:
            memid = GetEepromBuffer()[0x04];
            break;
        }
        
        ClearEepromBuffer();
        LoadEepromPage(EEPROM_DISPENSER_PUMP2_PRODUCT_NAMES_PAGE);
        switch(memid)
        {
            case 0x01:
            memcpy(pbuffer, &GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_], _EEPROM_QUARTER_PAGE_SIZE_);
            break;
            case 0x02:
            memcpy(pbuffer, &GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_], _EEPROM_QUARTER_PAGE_SIZE_);
            break;
            case 0x03:
            memcpy(pbuffer, &GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_], _EEPROM_QUARTER_PAGE_SIZE_);
            break;
            case 0x04:
            memcpy(pbuffer, &GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_], _EEPROM_QUARTER_PAGE_SIZE_);
            break;
        }
        return;
    }
    ClearEepromBuffer();    
    LoadEepromPage(EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE);
    if(pumpid == GetEepromBuffer()[0])
    {
        uint8 memid = 0x00;
        switch(hoseid)
        {
            case 0x00:
            memid = GetEepromBuffer()[0x01];
            break;
            case 0x01:
            memid = GetEepromBuffer()[0x02];
            break;
            case 0x02:
            memid = GetEepromBuffer()[0x03];
            break;
            case 0x03:
            memid = GetEepromBuffer()[0x04];
            break;
        }
        
        ClearEepromBuffer();
        LoadEepromPage(EEPROM_DISPENSER_PUMP3_PRODUCT_NAMES_PAGE);
        switch(memid)
        {
            case 0x01:
            memcpy(pbuffer, &GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_], _EEPROM_QUARTER_PAGE_SIZE_);
            break;
            case 0x02:
            memcpy(pbuffer, &GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_], _EEPROM_QUARTER_PAGE_SIZE_);
            break;
            case 0x03:
            memcpy(pbuffer, &GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_], _EEPROM_QUARTER_PAGE_SIZE_);
            break;
            case 0x04:
            memcpy(pbuffer, &GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_], _EEPROM_QUARTER_PAGE_SIZE_);
            break;
        }
        return;
    }
    ClearEepromBuffer();    
    LoadEepromPage(EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE);
    if(pumpid == GetEepromBuffer()[0])
    {
        uint8 memid = 0x00;
        switch(hoseid)
        {
            case 0x00:
            memid = GetEepromBuffer()[0x01];
            break;
            case 0x01:
            memid = GetEepromBuffer()[0x02];
            break;
            case 0x02:
            memid = GetEepromBuffer()[0x03];
            break;
            case 0x03:
            memid = GetEepromBuffer()[0x04];
            break;
        }
        
        ClearEepromBuffer();
        LoadEepromPage(EEPROM_DISPENSER_PUMP4_PRODUCT_NAMES_PAGE);
        switch(memid)
        {
            case 0x01:
            memcpy(pbuffer, &GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_], _EEPROM_QUARTER_PAGE_SIZE_);
            break;
            case 0x02:
            memcpy(pbuffer, &GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_], _EEPROM_QUARTER_PAGE_SIZE_);
            break;
            case 0x03:
            memcpy(pbuffer, &GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_], _EEPROM_QUARTER_PAGE_SIZE_);
            break;
            case 0x04:
            memcpy(pbuffer, &GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_], _EEPROM_QUARTER_PAGE_SIZE_);
            break;
        }
        return;
    }
}

uint8 FormatNumberLSDFirst(PNEAR_BYTE_PTR pinput, uint8 inputsize, PNEAR_BYTE_PTR poutput, uint8 decimals)
{
    memcpy(poutput, pinput, inputsize);
    //Apply the decimal separator character only if it has decimal places
    if(decimals > 0)
    {
        memcpy(&poutput[decimals + 1], &pinput[decimals], inputsize - decimals);
        poutput[decimals] = (_g_dispenserlayoutinfo._pointcommadecimalmode)? _ASCII_POINT_: _ASCII_COMMA_;
        poutput[decimals + 1] = (poutput[decimals + 1] | 0x30);
        PNEAR_BYTE_PTR pref = poutput;
        while(*pref != ((_g_dispenserlayoutinfo._pointcommadecimalmode)? _ASCII_POINT_: _ASCII_COMMA_))
        {
            *pref |= 0x30;
            pref++;
        }
        inputsize++;
    }
    //Adjust the whole buffer to the end of its size
    while(poutput[(inputsize - 1)] == 0x0)
    {
        FOR(uint8 index = 0, index < (inputsize - 1), index++)
        {
            if(poutput[(inputsize - 1)  - index] == 0x00)
            {
                poutput[(inputsize - 1)  - index] = poutput[(inputsize - 1)  - (index + 1)];
                poutput[(inputsize - 1)  - (index + 1)] = 0x00;
            }else
                break;
        }
    }
    //Flip the whole buffer
    PSTRBUFFTOGG(poutput, inputsize);
    
    return (inputsize);
}

void Print(PSINKMESSAGEPTR pmsg, uint8 printerport)
{
    uint8 index = 0;
    I2CBusLock();
        
    
    //CAREFUL WITH RETURNS AND RECURSIVE CALLS BETWEEN THE LOCK/UNLOCK CALLS THAT WOULD DEADLOCK!
        
    uint8 padspaces = 0x00;
    uint8 bufferlength = 0x00;
    
    //line feed 
    PRINTER_LINEFEED(printerport);
    
    ClearEepromBuffer();
    //Cleanup of the pump identifier in order to start the enumeration procedure
    LoadEepromPage(EEPROM_INSEPET_RESERVED_PAGE1);
    uint8 datamemory [8];
    uint8 asciidata[16];
    FOR(index = 0, index < 8, index++){
        datamemory[index] = GetEepromBuffer()[index];
        asciidata[2*(index)]   = ( (datamemory[index]>>4)  & 0x0F) + '0';
        asciidata[2*(index)+1] = ( (datamemory[index])     & 0x0F) + '0';
        if (asciidata[2*(index)] > '9') asciidata[2*(index)] += 7;
        if (asciidata[2*(index)+1] > '9') asciidata[2*(index)+1] += 7;
    }
    LoadEepromPage(EEPROM_CONFIGURATION_PAGE0);
    bufferlength = MeasurePrinterASCIIBufferLength(&GetEepromBuffer()[_EEPROM_PAGE_FIRST32START_]);
    padspaces = PadSpacesToCenter(bufferlength);
    //Pad spaces to center
    PRINTER_INDENTLINE(padspaces, printerport);
    //Here we start printing...
    //FIRST HEADING SECTION
    //output text
    WriteAuxPSoC(&GetEepromBuffer()[_EEPROM_PAGE_FIRST32START_], bufferlength, printerport);
    //line feed 
    PRINTER_LINEFEED(printerport);
    
    //SECOND HEADING SECTION
    bufferlength = MeasurePrinterASCIIBufferLength(&GetEepromBuffer()[_EEPROM_PAGE_SECOND32START_]);
    padspaces = PadSpacesToCenter(bufferlength);
    //Pad spaces to center
    PRINTER_INDENTLINE(padspaces, printerport);
    //output text
    WriteAuxPSoC(&GetEepromBuffer()[_EEPROM_PAGE_SECOND32START_], bufferlength, printerport);
    //line feed 
    PRINTER_LINEFEED(printerport);

    ClearEepromBuffer();
    //Cleanup of the pump identifier in order to start the enumeration procedure
    LoadEepromPage(EEPROM_CONFIGURATION_PAGE1);
    bufferlength = MeasurePrinterASCIIBufferLength(&GetEepromBuffer()[_EEPROM_PAGE_FIRST32START_]);
    padspaces = PadSpacesToCenter(bufferlength);
    //Here we start printing...
    //THIRD HEADING SECTION
    PRINTER_INDENTLINE(padspaces, printerport);
    //output text
    WriteAuxPSoC(&GetEepromBuffer()[_EEPROM_PAGE_FIRST32START_], bufferlength, printerport);
    //line feed 
    PRINTER_LINEFEED(printerport);
    
    //FOURTH HEADING SECTION
    bufferlength = MeasurePrinterASCIIBufferLength(&GetEepromBuffer()[_EEPROM_PAGE_SECOND32START_]);
    padspaces = PadSpacesToCenter(bufferlength);
    //Pad spaces to center
    PRINTER_INDENTLINE(padspaces, printerport);
    //output text
    WriteAuxPSoC(&GetEepromBuffer()[_EEPROM_PAGE_SECOND32START_], bufferlength, printerport);
    //line feed 
    PRINTER_LINEFEED(printerport);

    ClearEepromBuffer();
    //Cleanup of the pump identifier in order to start the enumeration procedure
    LoadEepromPage(EEPROM_CONFIGURATION_PAGE2);
    bufferlength = MeasurePrinterASCIIBufferLength(&GetEepromBuffer()[_EEPROM_PAGE_FIRST32START_]);
    padspaces = PadSpacesToCenter(bufferlength);
    //Here we start printing...
    //FIFTH HEADING SECTION
    //Pad spaces to center
    PRINTER_INDENTLINE(padspaces, printerport);
    //output text
    WriteAuxPSoC(&GetEepromBuffer()[_EEPROM_PAGE_FIRST32START_], bufferlength, printerport);
    //line separator
    PRINTER_SEPARATOR(printerport);
    
    //Contextual information comes next...
    #ifdef PRINTER_TYPE_PANEL
    PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
    #endif
   
    PSTR prefstr = GetPrinterTemplateLine(PRN_DATE);
    //output data left aligned
    WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
    //Here we start to print the transaction meaningful data...
    ReadRTC();
    
    index = 0;
    memset(_g_auxprinterbuffer, 0x00, _MAX_PRINTER_LINE_WIDTH_);
    _g_auxprinterbuffer[index++] = ((GetRtcBuffer()[RTC_DAY] >> 4) & 0x03) | 0x30;
    _g_auxprinterbuffer[index++] = (GetRtcBuffer()[RTC_DAY] & 0x0F) | 0x30;
    _g_auxprinterbuffer[index++] = _ASCII_SLASH_;
    _g_auxprinterbuffer[index++] = ((GetRtcBuffer()[RTC_MONTH] >> 4) & 0x01) | 0x30;
    _g_auxprinterbuffer[index++] = (GetRtcBuffer()[RTC_MONTH] & 0x0F) | 0x30;
    _g_auxprinterbuffer[index++] = _ASCII_SLASH_;
    _g_auxprinterbuffer[index++] = 0x32;//2 in ASCII
    _g_auxprinterbuffer[index++] = 0x30;//0 in ASCII (these values are enougth for a century of MUX History ;)
    _g_auxprinterbuffer[index++] = ((GetRtcBuffer()[RTC_YEAR] >> 4) & 0x0F) | 0x30;
    _g_auxprinterbuffer[index++] = (GetRtcBuffer()[RTC_YEAR] & 0x0F) | 0x30;

    //output data left aligned
    WriteAuxPSoC(_g_auxprinterbuffer, index, printerport);
    
    //line feed 
    PRINTER_LINEFEED(printerport);
    #ifdef PRINTER_TYPE_PANEL
    PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
    #endif

    prefstr = GetPrinterTemplateLine(PRN_HOUR);
    //output data left aligned
    WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
    
    index = 0x00;
    bool ampm = false;//This is to signal the time format (12/24)
    bool tf12 = false;//This is to signal the time format (12/24)
    if(GetRtcBuffer()[RTC_HOUR] & 0b01000000)
    {
        tf12 = true;//AM/PM
        _g_auxprinterbuffer[index++] = ((GetRtcBuffer()[RTC_HOUR] >> 4) & 0x01) | 0x30;
        _g_auxprinterbuffer[index++] = (GetRtcBuffer()[RTC_HOUR] & 0x0F) | 0x30;
        ampm = ((GetRtcBuffer()[RTC_HOUR] >> 4) & 0b00100000) | 0x30;
    }else{
        _g_auxprinterbuffer[index++] = ((GetRtcBuffer()[RTC_HOUR] >> 4) & 0x03) | 0x30;
        _g_auxprinterbuffer[index++] = (GetRtcBuffer()[RTC_HOUR] & 0x0F) | 0x30;
    }
    _g_auxprinterbuffer[index++] = _ASCII_COLON_;
    _g_auxprinterbuffer[index++] = ((GetRtcBuffer()[RTC_MINUTES] >> 4) & 0x07) | 0x30;
    _g_auxprinterbuffer[index++] = (GetRtcBuffer()[RTC_MINUTES] & 0x0F) | 0x30;
    _g_auxprinterbuffer[index++] = _ASCII_COLON_;
    _g_auxprinterbuffer[index++] = ((GetRtcBuffer()[RTC_SECONDS] >> 4) & 0x07) | 0x30;
    _g_auxprinterbuffer[index++] = (GetRtcBuffer()[RTC_SECONDS] & 0x0F) | 0x30;
    if(tf12)
    {
        _g_auxprinterbuffer[index++] = _ASCII_SPACE_;
        _g_auxprinterbuffer[index++] = (ampm)? 0x41: 0x50;
        _g_auxprinterbuffer[index++] = 0x4D;
    }
    
    //output data left aligned
    WriteAuxPSoC(_g_auxprinterbuffer, index, printerport);
    //line separator
    PRINTER_SEPARATOR(printerport);
    #ifdef PRINTER_TYPE_PANEL
    PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
    #endif
    
    prefstr = GetPrinterTemplateLine(PRN_PUMPPOS);
    //output data left aligned
    WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
    //PUMP POSITION COMES IN THE ARRAY INDEX 0
    memset(_g_auxprinterbuffer, 0x00, _MAX_PRINTER_LINE_WIDTH_);
    B78AD90CF552D9B30A(pmsg->_buffer[0x00] + _g_dispenserlayoutinfo._positionoffset, _g_auxprinterbuffer);
    PSTRBUFFTOGG(_g_auxprinterbuffer, strlen(PSTRCAST(_g_auxprinterbuffer)));
    WriteAuxPSoC((PNEAR_BYTE_PTR)_g_auxprinterbuffer, strlen(PSTRCAST(_g_auxprinterbuffer)), printerport);
    //line feed 
    PRINTER_LINEFEED(printerport);
    #ifdef PRINTER_TYPE_PANEL
    PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
    #endif

    prefstr = GetPrinterTemplateLine(PRN_INVNUMBER);
    //output data left aligned
    WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
    //INVOICE NUMBER
    WriteAuxPSoC((PNEAR_BYTE_PTR)&pmsg->_buffer[0x20], 0x0A, printerport);
    //line feed 
    PRINTER_LINEFEED(printerport);
    #ifdef PRINTER_TYPE_PANEL
    PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
    #endif
    
    prefstr = GetPrinterTemplateLine(PRN_PRODUCT);
    //output data left aligned
    WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
    //PRODUCT NAME
    memset(_g_auxprinterbuffer, 0x00, _MAX_PRINTER_LINE_WIDTH_);
    //HOSE ID COMES IN THE ARRAY INDEX #1
    GetProductNameFromPumpAndHose(pmsg->_buffer[0x00], pmsg->_buffer[0x01], _g_auxprinterbuffer);
    //output data
    WriteAuxPSoC(_g_auxprinterbuffer, 0x0A, printerport);
    //line feed 
    PRINTER_LINEFEED(printerport);
    #ifdef PRINTER_TYPE_PANEL
    PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
    #endif
    
    prefstr = GetPrinterTemplateLine(PRN_PRODPRICE);
    //output data left aligned
    WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
    //PRODUCT PRICE (PPU)
    index = 2;
    memset(_g_auxprinterbuffer, 0x00, _MAX_PRINTER_LINE_WIDTH_);
    //Byte #2 holds the length of the upcoming buffer that starts at position #3
    FormatNumberLSDFirst((PNEAR_BYTE_PTR)&pmsg->_buffer[index + 1], pmsg->_buffer[index], _g_auxprinterbuffer, 0);//_g_dispenserlayoutinfo._ppudecimals);
    //output data
    WriteAuxPSoC(&_g_auxprinterbuffer[0x00], (uint8)pmsg->_buffer[index], printerport);
    
    prefstr = GetPrinterPriceTemplateTag(_g_dispenserlayoutinfo._volumeunit);
    //output data left aligned
    WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
    //line feed 
    PRINTER_LINEFEED(printerport);
    #ifdef PRINTER_TYPE_PANEL
    PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
    #endif
    
    prefstr = GetPrinterTemplateLine(PRN_VOLUME);
    //output data left aligned
    WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
    //PRODUCT VOLUME
    index += (pmsg->_buffer[index] + 1);
    memset(_g_auxprinterbuffer, 0x00, _MAX_PRINTER_LINE_WIDTH_);
    FormatNumberLSDFirst((PNEAR_BYTE_PTR)&pmsg->_buffer[index + 1], pmsg->_buffer[index], _g_auxprinterbuffer, _g_dispenserlayoutinfo._volumedecimals);
    //output data
    WriteAuxPSoC(&_g_auxprinterbuffer[0x00], (uint8)pmsg->_buffer[index], printerport);
    
    prefstr = GetPrinterVolumeTemplateTag(_g_dispenserlayoutinfo._volumeunit);
    //output data left aligned
    WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
    //line feed 
    PRINTER_LINEFEED(printerport);
    #ifdef PRINTER_TYPE_PANEL
    PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
    #endif
    
    prefstr = GetPrinterTemplateLine(PRN_MONEY);
    //output data left aligned
    WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
    //PRODUCT TOTAL MONEY
    index += (pmsg->_buffer[index] + 1);
    memset(_g_auxprinterbuffer, 0x00, _MAX_PRINTER_LINE_WIDTH_);
    FormatNumberLSDFirst((PNEAR_BYTE_PTR)&pmsg->_buffer[index + 1], pmsg->_buffer[index], _g_auxprinterbuffer, 0);//_g_dispenserlayoutinfo._moneydecimals);
    //output data
    WriteAuxPSoC(&_g_auxprinterbuffer[0x00], (uint8)pmsg->_buffer[index], printerport);
    //line feed 
    PRINTER_LINEFEED(printerport);
    #ifdef PRINTER_TYPE_PANEL
    PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
    #endif
    
    ClearEepromBuffer();
    LoadEepromPage(EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE);
    if(GetEepromBuffer()[0x00] == pmsg->_buffer[0x00])
    {
        ClearEepromBuffer();
        if(pmsg->_buffer[0x1B] == 0x0D)
            LoadEepromPage(EEPROM_DISPENSER_PUMP11_TRANSACTION_PAGE);
        else
            LoadEepromPage(EEPROM_DISPENSER_PUMP13_TRANSACTION_PAGE);
    }
    else
    {
        ClearEepromBuffer();
        LoadEepromPage(EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE);
        if(GetEepromBuffer()[0x00] == pmsg->_buffer[0x00])
        {
            ClearEepromBuffer();
            if(pmsg->_buffer[0x1B] == 0x0D)
                LoadEepromPage(EEPROM_DISPENSER_PUMP21_TRANSACTION_PAGE);
            else
                LoadEepromPage(EEPROM_DISPENSER_PUMP23_TRANSACTION_PAGE);
        }
        else
        {
            ClearEepromBuffer();
            LoadEepromPage(EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE);
            if(GetEepromBuffer()[0x00] == pmsg->_buffer[0x00])
            {
                ClearEepromBuffer();
                if(pmsg->_buffer[0x1B] == 0x0D)
                    LoadEepromPage(EEPROM_DISPENSER_PUMP31_TRANSACTION_PAGE);
                else
                    LoadEepromPage(EEPROM_DISPENSER_PUMP33_TRANSACTION_PAGE);
                    
            }
            else
            {
                ClearEepromBuffer();
                LoadEepromPage(EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE);
                if(GetEepromBuffer()[0x00] == pmsg->_buffer[0x00])
                {
                    ClearEepromBuffer();
                    if(pmsg->_buffer[0x1B] == 0x0D)
                        LoadEepromPage(EEPROM_DISPENSER_PUMP41_TRANSACTION_PAGE);
                    else
                        LoadEepromPage(EEPROM_DISPENSER_PUMP43_TRANSACTION_PAGE);
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //This only for cash transactions since for credit transactions this information will be posted by the remote peer
    if(pmsg->_buffer[0x1B] == 0x0D)
    {
        //CAR'S PLATE
        uint8 bufflen = GetBufferLengthFromDisplayID(DISPLAY_DIGITE_PLACA);
        uint8 buffpos = GetBufferIndexFromDisplayID(DISPLAY_DIGITE_PLACA);
        
        memset(_g_auxprinterbuffer, 0x00, _MAX_PRINTER_LINE_WIDTH_);
        memcpy(_g_auxprinterbuffer, &GetEepromBuffer()[buffpos], bufflen);
        prefstr = GetPrinterTemplateLine(PRN_PLATE);
        //output data left aligned
        WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
        //output data
        WriteAuxPSoC(_g_auxprinterbuffer, index, printerport);
        //line feed 
        PRINTER_LINEFEED(printerport);
        #ifdef PRINTER_TYPE_PANEL
        PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
        #endif

        //CAR'S MILEAGE
        /*bufflen = GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE);
        buffpos = GetBufferIndexFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE);

        memset(_g_auxprinterbuffer, 0x00, _MAX_PRINTER_LINE_WIDTH_);
        memcpy(_g_auxprinterbuffer, &GetEepromBuffer()[buffpos], bufflen);
        prefstr = GetPrinterTemplateLine(PRN_MILLEAGE);
        //output data left aligned
        WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
        //output data
        WriteAuxPSoC(_g_auxprinterbuffer, index, printerport);*/
    }
    //In case of no printer output check the assigned value for the operation (Cash:0x0D / Credit:0x0E)
    else if(pmsg->_buffer[0x1B] == 0x0E)
    {
        //CAR'S MILEAGE
        uint8 bufflen = GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE);
        uint8 buffpos = GetBufferIndexFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE);

        memset(_g_auxprinterbuffer, 0x00, _MAX_PRINTER_LINE_WIDTH_);
        memcpy(_g_auxprinterbuffer, &GetEepromBuffer()[buffpos], bufflen);
        prefstr = GetPrinterTemplateLine(PRN_MILLEAGE);
        //output data left aligned
        WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
        //output data
        WriteAuxPSoC(_g_auxprinterbuffer, index, printerport);        

        //line separator
        PRINTER_SEPARATOR(printerport);
        #ifdef PRINTER_TYPE_PANEL
        PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
        #endif
        //////////////////////////////////////////////////////////////////////////////////        
        //Further lines for the Credit Transaction come here!!
        //////////////////////////////////////////////////////////////////////////////////        
        
        index = 0x20 + 0x0A;
        //Car's Plate (8 bytes)
        prefstr = GetPrinterTemplateLine(PRN_PLATE);
        //output data left aligned
        WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
        WriteAuxPSoC((PNEAR_BYTE_PTR)&pmsg->_buffer[index], 0x08, printerport);
        //line feed 
        PRINTER_LINEFEED(printerport);
        #ifdef PRINTER_TYPE_PANEL
        PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
        #endif
        index += 0x08;
        //Serial ibutton
        
        prefstr = GetPrinterTemplateLine(PRN_SERIAL);
        WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);        
        WriteAuxPSoC((PNEAR_BYTE_PTR)asciidata, 0x10, printerport);
        PRINTER_LINEFEED(printerport);
        #ifdef PRINTER_TYPE_PANEL
        PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
        #endif                        
        //index += 0x10;
        
        //Balance (10 bytes)
        prefstr = GetPrinterTemplateLine(PRN_BALANCE);
        //output data left aligned
        WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
        WriteAuxPSoC((PNEAR_BYTE_PTR)&pmsg->_buffer[index], 0x0A, printerport);
        //line feed 
        PRINTER_LINEFEED(printerport);
        #ifdef PRINTER_TYPE_PANEL
        PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
        #endif
        index += 0x0A;

        //Company (20 bytes)
        prefstr = GetPrinterTemplateLine(PRN_COMPANY);
        //output data left aligned
        WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
        WriteAuxPSoC((PNEAR_BYTE_PTR)&pmsg->_buffer[index], 0x14, printerport);
        //line feed 
        PRINTER_LINEFEED(printerport);
        #ifdef PRINTER_TYPE_PANEL
        PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
        #endif
        index += 0x14;

        //Account Name (20 bytes)
        prefstr = GetPrinterTemplateLine(PRN_ACCOUNT);
        //output data left aligned
        WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
        WriteAuxPSoC((PNEAR_BYTE_PTR)&pmsg->_buffer[index], 0x14, printerport);
        //line feed 
        PRINTER_LINEFEED(printerport);
        #ifdef PRINTER_TYPE_PANEL
        PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
        #endif
        index += 0x14;

        //Daily Visits (1 byte)
        prefstr = GetPrinterTemplateLine(PRN_VISITSDAY);
        //output data left aligned
        WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
        
        memset(_g_auxprinterbuffer, 0x00, _MAX_PRINTER_LINE_WIDTH_);
        B78AD90CF552D9B30A(pmsg->_buffer[index], _g_auxprinterbuffer);
        uint8 bufflength = strlen(PSTRCAST(_g_auxprinterbuffer));
        PSTRBUFFTOGG(_g_auxprinterbuffer, bufflength);
        WriteAuxPSoC(_g_auxprinterbuffer, bufflength, printerport);
        //line feed 
        PRINTER_LINEFEED(printerport);
        #ifdef PRINTER_TYPE_PANEL
        PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
        #endif
        index += 0x01;

        //Weekly Visits (1 byte)
        prefstr = GetPrinterTemplateLine(PRN_VISITSWEEK);
        //output data left aligned
        WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
        
        memset(_g_auxprinterbuffer, 0x00, _MAX_PRINTER_LINE_WIDTH_);
        B78AD90CF552D9B30A(pmsg->_buffer[index], _g_auxprinterbuffer);
        bufflength = strlen(PSTRCAST(_g_auxprinterbuffer));
        PSTRBUFFTOGG(_g_auxprinterbuffer, bufflength);
        WriteAuxPSoC(_g_auxprinterbuffer, bufflength, printerport);
        //line feed 
        PRINTER_LINEFEED(printerport);
        #ifdef PRINTER_TYPE_PANEL
        PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
        #endif
        index += 0x01;

        //Monthly Visits (1 byte)
        prefstr = GetPrinterTemplateLine(PRN_VISITSMONTH);
        //output data left aligned
        WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
        
        memset(_g_auxprinterbuffer, 0x00, _MAX_PRINTER_LINE_WIDTH_);
        B78AD90CF552D9B30A(pmsg->_buffer[index], _g_auxprinterbuffer);
        bufflength = strlen(PSTRCAST(_g_auxprinterbuffer));
        PSTRBUFFTOGG(_g_auxprinterbuffer, bufflength);
        WriteAuxPSoC(_g_auxprinterbuffer, bufflength, printerport);
        //line feed 
        PRINTER_LINEFEED(printerport);
        #ifdef PRINTER_TYPE_PANEL
        PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
        #endif
        index += 0x01;
        
        //Daily Volume (7 bytes)
        if(pmsg->_buffer[0x8B] == 0x01)
            prefstr = GetPrinterTemplateLine(PRN_VOLUMEDAY);
        else
            prefstr = GetPrinterTemplateLine(PRN_MONEYDAY);
        //output data left aligned
        WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
        WriteAuxPSoC((PNEAR_BYTE_PTR)&pmsg->_buffer[index], 0x07, printerport);
        //line feed 
        PRINTER_LINEFEED(printerport);
        #ifdef PRINTER_TYPE_PANEL
        PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
        #endif
        index += 0x07;

        //Weekly Volume (7 bytes)
        if(pmsg->_buffer[0x8B] == 0x01)
            prefstr = GetPrinterTemplateLine(PRN_VOLUMEWEEK);
        else
            prefstr = GetPrinterTemplateLine(PRN_MONEYWEEK);
            
        //output data left aligned
        WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
        WriteAuxPSoC((PNEAR_BYTE_PTR)&pmsg->_buffer[index], 0x07, printerport);
        //line feed 
        PRINTER_LINEFEED(printerport);
        #ifdef PRINTER_TYPE_PANEL
        PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
        #endif
        index += 0x07;

        //Monthly Volume (7 bytes)
        if(pmsg->_buffer[0x8B] == 0x01)
            prefstr = GetPrinterTemplateLine(PRN_VOLUMEMONTH);
        else
            prefstr = GetPrinterTemplateLine(PRN_MONEYMONTH);
        
        //output data left aligned
        WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
        WriteAuxPSoC((PNEAR_BYTE_PTR)&pmsg->_buffer[index], 0x07, printerport);
        //line feed 
        PRINTER_LINEFEED(printerport);
        #ifdef PRINTER_TYPE_PANEL
        PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
        #endif
        index += 0x07;

        //Account Type (16 bytes)
        prefstr = GetPrinterTemplateLine(PRN_ACCOUNTTYPE);
        //output data left aligned
        WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
        WriteAuxPSoC((PNEAR_BYTE_PTR)&pmsg->_buffer[index], 0x0F, printerport);
        //line feed 
        PRINTER_LINEFEED(printerport);
        #ifdef PRINTER_TYPE_PANEL
        PRINTER_INDENTLINE(_LEFT_MARGIN_, printerport);
        #endif
        index += 0x0F;                
        PRINTER_LINEFEED(printerport);        
        prefstr = GetPrinterTemplateLine(PRN_ID);
        WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
        PRINTER_LINEFEED(printerport);
        PRINTER_LINEFEED(printerport);
        PRINTER_LINEFEED(printerport);
        PRINTER_LINEFEED(printerport);
        prefstr = GetPrinterTemplateLine(PRN_SIGNATURE);
        WriteAuxPSoC((PNEAR_BYTE_PTR)prefstr, strlen(prefstr), printerport);
        PRINTER_LINEFEED(printerport);                    
    }
    
    //line separator
    PRINTER_SEPARATOR(printerport);
    
    //PRINT FOOTER1
    ClearEepromBuffer();
    //Cleanup of the pump identifier in order to start the enumeration procedure
    LoadEepromPage(EEPROM_CONFIGURATION_PAGE2);
    bufferlength = MeasurePrinterASCIIBufferLength(&GetEepromBuffer()[_EEPROM_PAGE_SECOND32START_]);
    padspaces = PadSpacesToCenter(bufferlength);
    //Pad spaces to center
    PRINTER_INDENTLINE(padspaces, printerport);
    //output text
    WriteAuxPSoC(&GetEepromBuffer()[_EEPROM_PAGE_SECOND32START_], bufferlength, printerport);
    //line feed 
    PRINTER_LINEFEED(printerport);

    ClearEepromBuffer();
    //Cleanup of the pump identifier in order to start the enumeration procedure
    LoadEepromPage(EEPROM_CONFIGURATION_PAGE3);
    bufferlength = MeasurePrinterASCIIBufferLength(&GetEepromBuffer()[_EEPROM_PAGE_FIRST32START_]);
    padspaces = PadSpacesToCenter(bufferlength);
    //Pad spaces to center
    PRINTER_INDENTLINE(padspaces, printerport);
    //output text
    WriteAuxPSoC(&GetEepromBuffer()[_EEPROM_PAGE_FIRST32START_], bufferlength, printerport);
    //line feed 
    PRINTER_LINEFEED(printerport);
    
    bufferlength = MeasurePrinterASCIIBufferLength(&GetEepromBuffer()[_EEPROM_PAGE_SECOND32START_]);
    padspaces = PadSpacesToCenter(bufferlength);
    //Pad spaces to center
    PRINTER_INDENTLINE(padspaces, printerport);
    //output text
    WriteAuxPSoC(&GetEepromBuffer()[_EEPROM_PAGE_SECOND32START_], bufferlength, printerport);
    //line feed 
    PRINTER_LINEFEED(printerport);
    //line feed 
    PRINTER_LINEFEED(printerport);
    //line feed 
    PRINTER_LINEFEED(printerport);
    //line feed 
    PRINTER_LINEFEED(printerport);
    
   //Espacio para el autocuter
        
    I2CBusUnlock();

}

void PrintGeneric(PSINKMESSAGEPTR pmsg, uint8 printerport)
{
    //line feed 
    PRINTER_LINEFEED(printerport);
    
    //CAREFUL WITH RETURNS AND RECURSIVE CALLS BETWEEN THE LOCK/UNLOCK CALLS THAT WOULD DEADLOCK!
    I2CBusLock();
    WriteAuxPSoC((PNEAR_BYTE_PTR)&pmsg->_buffer[0x01], pmsg->_buffersize, printerport);    
    
    #ifdef PRINTER_KIOSK_AUTOCUT
        //Auto cut command
        PRINTER_AUTOCUT(printerport);
        CyDelay(500);//To allow for the cutter to work
    #endif
        
    I2CBusUnlock();
}

/* [] END OF FILE */
