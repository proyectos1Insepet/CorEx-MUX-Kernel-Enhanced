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
#include "GlobalDefinitions.h"
#include "UARTManager.h"
#include "SinkManager.h"
#include "SinkObservables.h"
#include "Configuration.h"
#include "Dispenser.h"
#include "Display.h"
#include "RFLink.h"
#include "iButton.h"
#include "Printer.h"
#include "Eeprom.h"
#include "Rtc.h"

uint8 _g_dispvalcodes[] = { 0x94, 0x7E, 0xFF };

void CastCreditVolumeValue(char8 *pbuffer, uint8 buffersize);
bool DisplayGenericValidator(LPVOID pparam);

bool _g_configured[] = { false, false };
//Screen #1 Handler (for further information on its initialization parameters, see its definition in "Display.h")
DisplayLayout _g_display1 = { DISPLAY1, DISPLAY1_MESSAGE, 0x00, 0x00, 0x00, 0x00, 0, 0, 0x00, 0, 0, true, &_g_displayflow[0], _g_displayflow, { 0x0000, 0x0000, 0x00, {} }, NULL, NULL, DisplayGenericValidator };
//Screen #2 Handler  (for further information on its initialization parameters, see its definition in "Display.h")
DisplayLayout _g_display2 = { DISPLAY2, DISPLAY2_MESSAGE, 0x00, 0x00, 0x00, 0x00, 0, 0, 0x00, 0, 0, true, &_g_displayflow[0], _g_displayflow, { 0x0000, 0x0000, 0x00, {} }, NULL, NULL, DisplayGenericValidator };

DisplayHomeAnimation _g_homeanimarray[] =
{
    { 0x00, true, 0x00, { 0x5C, 0x7C, 0x2F, 0x2D } },//Display #1
    { 0x00, true, 0x00, { 0x5C, 0x7C, 0x2F, 0x2D } },//Display #2
};

uint16 _g_productscreenpos[][2] =
{
    { 0x0115, 0x0095 },
    { 0x0125, 0x00E9 },
    { 0x0139, 0x0142 },
    { 0x014A, 0x0192 },
    { 0x0000, 0x0000 },
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// *********************************************  PURE BLL FUNCTIONS NEXT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void WorkShiftDecorator(void *pparam)
{
    FontData fontdata;
    fontdata._size = 0x03;
    fontdata._backcolor = 0xFFFF;
    fontdata._forecolor = 0x0000;
    fontdata._opaquebackground = DISP_TEXT_OPAQUE;
    //MOVE ALL OF THESE TEXTS TO THE EEPROM
    char8 *pmessage = "Validando turno, por favor espere...";
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    if(pdisplay->_dispid == DISPLAY1)
    {
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
        if(puartdisp)
        {
            puartdisp->_messagelength = DisplayOutputString(0x0020, 0x00EA, puartdisp->_messagetx, pmessage, strlen(pmessage), fontdata);
            puartdisp->_messagestate = PENDING;
        }
    }
    else
    {
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
        if(puartdisp)
        {
            puartdisp->_messagelength = DisplayOutputString(0x0020, 0x00EA, puartdisp->_messagetx, pmessage, strlen(pmessage), fontdata);
            puartdisp->_messagestate = PENDING;
        }
    }
}

void InitiateWorkShiftValidation(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay->_dispid == DISPLAY1)
    {
        if(((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SELECCIONE_POSICION)] == POSITIONA)
        {
            I2CBusLock();
            ClearEepromBuffer();
            LoadEepromPage(EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE);
            uint8 wsvalue = GetEepromBuffer()[0x05];
            I2CBusUnlock();
            if(wsvalue == 0x00)
                pdisplay->_currentstate = DISPLAY_OPEN_WORK_SHIFT;
            else
                pdisplay->_currentstate = DISPLAY_CLOSE_WORK_SHIFT;
        }
        else if(((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SELECCIONE_POSICION)] == POSITIONB)
        {
            I2CBusLock();
            ClearEepromBuffer();
            LoadEepromPage(EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE);
            uint8 wsvalue = GetEepromBuffer()[0x05];
            I2CBusUnlock();
            if(wsvalue == 0x00)
                pdisplay->_currentstate = DISPLAY_OPEN_WORK_SHIFT;
            else
                pdisplay->_currentstate = DISPLAY_CLOSE_WORK_SHIFT;
        }
    }
    else if(pdisplay->_dispid == DISPLAY2)
    {
        if(((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SELECCIONE_POSICION)] == POSITIONA)
        {
            I2CBusLock();
            ClearEepromBuffer();
            LoadEepromPage(EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE);
            uint8 wsvalue = GetEepromBuffer()[0x05];
            I2CBusUnlock();
            if(wsvalue == 0x00)
                pdisplay->_currentstate = DISPLAY_OPEN_WORK_SHIFT;
            else
                pdisplay->_currentstate = DISPLAY_CLOSE_WORK_SHIFT;
        }
        else if(((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SELECCIONE_POSICION)] == POSITIONB)
        {
            I2CBusLock();
            ClearEepromBuffer();
            LoadEepromPage(EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE);
            uint8 wsvalue = GetEepromBuffer()[0x05];
            I2CBusUnlock();
            if(wsvalue == 0x00)
                pdisplay->_currentstate = DISPLAY_OPEN_WORK_SHIFT;
            else
                pdisplay->_currentstate = DISPLAY_CLOSE_WORK_SHIFT;
        }
    }
}

void RequestWorkShiftOperAuthorization(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    //HERE WE ATTEMPT TO PUSH THE TRANSACTION INTO THE PUMP'S TRANSACTIONAL QUEUE.
    //THIS IS DONE ONLY IF THERE IS ROOM IN THE QUEUE, OTHERWISE THIS STATE WON'T
    //BE REPORTED TO THE REMOTE PEER.
    PNEAR_PUMPPTR ppumpptr = &_g_pumps[GetPumpIndexFromDisplay(pdisplay)];
    if(ppumpptr)
    {
        ppumpptr->PumpTransQueueLock(ppumpptr);
        PNEAR_PUMPTRANSACTIONALSTATEPTR pts = ppumpptr->PumpTransQueueAllocate(ppumpptr);
        ppumpptr->PumpTransQueueUnlock(ppumpptr);
        if(pts)
        {
            uint8 index = 0;
            pts->_transtate = RF_MUX_ISLAND_OPER_DATA_REQUEST;
            //Pump ID always comes in the first position (0x00)
            pts->_buffer[index++] = ppumpptr->_pumpid;
            
            memcpy(&pts->_buffer[index], &((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_INTRODUZCA_CEDULA)], GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_CEDULA));
            //PSTRBUFFTOGG((PNEAR_BYTE_PTR)&pts->_buffer[index], GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_CEDULA));
            index += GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_CEDULA);
            
            memcpy(&pts->_buffer[index], &((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_INGRESE_PASSWORD)], GetBufferLengthFromDisplayID(DISPLAY_INGRESE_PASSWORD));
            PSTRBUFFTOGG((PNEAR_BYTE_PTR)&pts->_buffer[index], strlen(&pts->_buffer[index]));
            index += GetBufferLengthFromDisplayID(DISPLAY_INGRESE_PASSWORD);
            
            pts->_buffersize = index;

            ppumpptr->PumpTransQueueLock(ppumpptr);
            ppumpptr->PumpTransQueueEnqueue(ppumpptr, pts);
            ppumpptr->PumpTransQueueUnlock(ppumpptr);
        }
    }
}

void DrawNumberOfCopies(void *pparam)
{
    char8 buffer[16];
    FontData fontdata;
    fontdata._size = 0x03;
    fontdata._backcolor = 0xFFFF;
    fontdata._forecolor = 0x0000;
    fontdata._opaquebackground = DISP_TEXT_TRANSP;
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    
    I2CBusLock();
    ClearEepromBuffer();
    LoadEepromPage(EEPROM_CONFIGURATION_PAGE7);
    uint8 numberofprintedcopies = GetEepromBuffer()[0x08];
    I2CBusUnlock();
    
    if(pdisplay->_dispid == DISPLAY1)
    {
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
        if(puartdisp)
        {
            memset(buffer, 0x00, sizeof(buffer));
            if(numberofprintedcopies == 0x01)
                sprintf(buffer, "(%d Copia)", numberofprintedcopies);
            else
                sprintf(buffer, "(%d Copias)", numberofprintedcopies);
                
            //MOVE ALL OF THESE TEXTS TO THE EEPROM
            puartdisp->_messagelength = DisplayOutputString(0x0F0, 0x12A, puartdisp->_messagetx, buffer, strlen(buffer), fontdata);
            puartdisp->_messagestate = PENDING;
        }
    }
    else
    {
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
        if(puartdisp)
        {
            memset(buffer, 0x00, sizeof(buffer));
            if(numberofprintedcopies == 0x01)
                sprintf(buffer, "(%d Copia)", numberofprintedcopies);
            else
                sprintf(buffer, "(%d Copias)", numberofprintedcopies);
                
            puartdisp->_messagelength = DisplayOutputString(0x0F0, 0x12A, puartdisp->_messagetx, buffer, strlen(buffer), fontdata);
            puartdisp->_messagestate = PENDING;
        }
    }
}

void CancelMessageDecorator(void *pparam)
{
    FontData fontdata;
    fontdata._size = 0x03;
    fontdata._backcolor = 0xFFFF;
    fontdata._forecolor = 0x0000;
    fontdata._opaquebackground = DISP_TEXT_OPAQUE;
    //MOVE ALL OF THESE TEXTS TO THE EEPROM
    char8 *pmessage = "**** OPERACION CANCELADA ****";    
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    if(pdisplay->_dispid == DISPLAY1)
    {
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
        if(puartdisp)
        {
            puartdisp->_messagelength = DisplayOutputString(0x0060, 0x00EA, puartdisp->_messagetx, pmessage, strlen(pmessage), fontdata);
            puartdisp->_messagestate = PENDING;
        }
    }
    else
    {
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
        if(puartdisp)
        {
            puartdisp->_messagelength = DisplayOutputString(0x0060, 0x00EA, puartdisp->_messagetx, pmessage, strlen(pmessage), fontdata);
            puartdisp->_messagestate = PENDING;
        }
    }
}

void MaskoutOptionDecorator(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay->_dispid == DISPLAY1)
    {
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
        if(puartdisp)
        {
            //SET FC BC COLORS (WHITE)
            uint8 buff[] = { 0xAA, 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
            memcpy(puartdisp->_messagetx, buff, sizeof(buff));
            puartdisp->_messagelength = sizeof(buff);
            puartdisp->_messagestate = PENDING;
        }
        
        puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
        if(puartdisp)
        {
            uint8 buff[] = { 0xAA, 0x5B, 0x01, 0x00, 0x01, 0x4A, 0x02, 0x70, 0x01, 0x90, 0xCC, 0x33, 0xC3, 0x3C };
            memcpy(puartdisp->_messagetx, buff, sizeof(buff));
            puartdisp->_messagelength = sizeof(buff);
            puartdisp->_messagestate = PENDING;
        }    
    }
    else if(pdisplay->_dispid == DISPLAY2)
    {
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
        if(puartdisp)
        {
            //SET FC BC COLORS (WHITE)
            uint8 buff[] = { 0xAA, 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
            memcpy(puartdisp->_messagetx, buff, sizeof(buff));
            puartdisp->_messagelength = sizeof(buff);
            puartdisp->_messagestate = PENDING;
        }
        
        puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
        if(puartdisp)
        {
            uint8 buff[] = { 0xAA, 0x5B, 0x01, 0x00, 0x01, 0x4A, 0x02, 0x70, 0x01, 0x90, 0xCC, 0x33, 0xC3, 0x3C };
            memcpy(puartdisp->_messagetx, buff, sizeof(buff));
            puartdisp->_messagelength = sizeof(buff);
            puartdisp->_messagestate = PENDING;
        }    
    }
}

void AuthorizationMessageDecorator(LPVOID pparam)
{
    FontData fontdata;
    fontdata._size = 0x03;
    fontdata._backcolor = 0xFFFF;
    fontdata._forecolor = 0x0000;
    fontdata._opaquebackground = DISP_TEXT_OPAQUE;
    //MOVE ALL OF THESE TEXTS TO THE EEPROM
    char8 *pmessage[] = 
    {
        "ENVIANDO LA SOLICITUD DE AUTORIZACION,",
        "                                      ",
        "<<<       POR FAVOR ESPERE ...     >>>",
    };
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    if(pdisplay->_dispid == DISPLAY1)
    {
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
        if(puartdisp)
        {
            uint8 index = 0;
            uint16 ypos = 0x00BA;
            puartdisp->_messagelength = 0;
            puartdisp->_messagelength += DisplayOutputString(0x001A, ypos, &puartdisp->_messagetx[puartdisp->_messagelength], pmessage[index], strlen(pmessage[index]), fontdata);
            ypos += 0x0020;index++;
            puartdisp->_messagelength += DisplayOutputString(0x001A, ypos, &puartdisp->_messagetx[puartdisp->_messagelength], pmessage[index], strlen(pmessage[index]), fontdata);
            ypos += 0x0020;index++;
            puartdisp->_messagelength += DisplayOutputString(0x001A, ypos, &puartdisp->_messagetx[puartdisp->_messagelength], pmessage[index], strlen(pmessage[index]), fontdata);
            ypos += 0x0020;index++;

            puartdisp->_messagestate = PENDING;
        }
    }
    else
    {
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
        if(puartdisp)
        {
            uint8 index = 0;
            uint16 ypos = 0x00BA;
            puartdisp->_messagelength = 0;
            puartdisp->_messagelength += DisplayOutputString(0x001A, ypos, &puartdisp->_messagetx[puartdisp->_messagelength], pmessage[index], strlen(pmessage[index]), fontdata);
            ypos += 0x0020;index++;
            puartdisp->_messagelength += DisplayOutputString(0x001A, ypos, &puartdisp->_messagetx[puartdisp->_messagelength], pmessage[index], strlen(pmessage[index]), fontdata);
            ypos += 0x0020;index++;
            puartdisp->_messagelength += DisplayOutputString(0x001A, ypos, &puartdisp->_messagetx[puartdisp->_messagelength], pmessage[index], strlen(pmessage[index]), fontdata);
            ypos += 0x0020;index++;
            
            puartdisp->_messagestate = PENDING;
        }
    }
}

void AcceptedAuthorizationMessageDecorator(LPVOID pparam)
{
    FontData fontdata;
    fontdata._size = 0x03;
    fontdata._backcolor = 0xFFFF;
    fontdata._forecolor = 0x0000;
    fontdata._opaquebackground = DISP_TEXT_OPAQUE;
    //MOVE ALL OF THESE TEXTS TO THE EEPROM
    char8 *pmessage[] = 
    {
        "  PETICION DE AUTORIZACION ACEPTADA!  ",
    };
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    if(pdisplay->_dispid == DISPLAY1)
    {
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
        if(puartdisp)
        {
            uint8 index = 0;
            uint16 ypos = 0x00BA;
            puartdisp->_messagelength = 0;
            puartdisp->_messagelength += DisplayOutputString(0x001A, ypos, &puartdisp->_messagetx[puartdisp->_messagelength], pmessage[index], strlen(pmessage[index]), fontdata);
            ypos += 0x0020;index++;

            puartdisp->_messagestate = PENDING;
        }
    }
    else
    {
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
        if(puartdisp)
        {
            uint8 index = 0;
            uint16 ypos = 0x00BA;
            puartdisp->_messagelength = 0;
            puartdisp->_messagelength += DisplayOutputString(0x001A, ypos, &puartdisp->_messagetx[puartdisp->_messagelength], pmessage[index], strlen(pmessage[index]), fontdata);
            ypos += 0x0020;index++;
            
            puartdisp->_messagestate = PENDING;
        }
    }
}

void ProductNameSelectionDecorator(LPVOID pparam)
{
    FontData fontdata;
    fontdata._size = 0x03;
    fontdata._backcolor = 0x0000;
    fontdata._forecolor = 0xFFFF;
    fontdata._opaquebackground = DISP_TEXT_TRANSP;
    uint8  numhoses = 0x00;
    uint16 eepromcardpage = 0x00;
    uint16 eepromprodpage = 0x00;
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    switch(GetPumpIndexFromDisplay(pdisplay))
    {
        case 0x00:
            numhoses = _g_dispenserlayoutinfo._hosesposition1;
            eepromcardpage = (uint16)EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE;
            eepromprodpage = (uint16)EEPROM_DISPENSER_PUMP1_PRODUCT_NAMES_PAGE;
            break;
        case 0x01:
            numhoses = _g_dispenserlayoutinfo._hosesposition2;
            eepromcardpage = (uint16)EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE;
            eepromprodpage = (uint16)EEPROM_DISPENSER_PUMP2_PRODUCT_NAMES_PAGE;
            break;
        case 0x02:
            numhoses = _g_dispenserlayoutinfo._hosesposition3;
            eepromcardpage = (uint16)EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE;
            eepromprodpage = (uint16)EEPROM_DISPENSER_PUMP3_PRODUCT_NAMES_PAGE;
            break;
        case 0x03:
            numhoses = _g_dispenserlayoutinfo._hosesposition4;
            eepromcardpage = (uint16)EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE;
            eepromprodpage = (uint16)EEPROM_DISPENSER_PUMP4_PRODUCT_NAMES_PAGE;
            break;
    }    
    if(pdisplay->_dispid == DISPLAY1)
    {
        uint8 cardinals[0x04];
        
        I2CBusLock();
        ClearEepromBuffer();
        LoadEepromPage(eepromcardpage);
        memcpy(cardinals, &(GetEepromBuffer()[0x01]), 0x04);
        I2CBusUnlock();
        
        I2CBusLock();
        ClearEepromBuffer();
        LoadEepromPage(eepromprodpage);
        FOR(uint8 index = 0, index < numhoses, index++)
        {
            switch(cardinals[index])
            {
                case 0x01:
                {
                    PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
                    if(puartdisp)
                    {
                        puartdisp->_messagelength = DisplayOutputString(_g_productscreenpos[index][0x00], _g_productscreenpos[index][0x01], puartdisp->_messagetx, PSTRCAST(&GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_]), _EEPROM_QUARTER_PAGE_SIZE_, fontdata);
                        puartdisp->_messagestate = PENDING;
                    }
                    break;
                }
                case 0x02:
                {
                    PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
                    if(puartdisp)
                    {
                        puartdisp->_messagelength = DisplayOutputString(_g_productscreenpos[index][0x00], _g_productscreenpos[index][0x01], puartdisp->_messagetx, PSTRCAST(&GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_]), _EEPROM_QUARTER_PAGE_SIZE_, fontdata);
                        puartdisp->_messagestate = PENDING;
                    }
                    break;
                }
                case 0x03:
                {
                    PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
                    if(puartdisp)
                    {
                        puartdisp->_messagelength = DisplayOutputString(_g_productscreenpos[index][0x00], _g_productscreenpos[index][0x01], puartdisp->_messagetx, PSTRCAST(&GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_]), _EEPROM_QUARTER_PAGE_SIZE_, fontdata);
                        puartdisp->_messagestate = PENDING;
                    }
                    break;
                }
                case 0x04:
                {
                    PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
                    if(puartdisp)
                    {
                        puartdisp->_messagelength = DisplayOutputString(_g_productscreenpos[index][0x00], _g_productscreenpos[index][0x01], puartdisp->_messagetx, PSTRCAST(&GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_]), _EEPROM_QUARTER_PAGE_SIZE_, fontdata);
                        puartdisp->_messagestate = PENDING;
                    }
                    break;
                }
            }
        }
        I2CBusUnlock();
    }
    else if(pdisplay->_dispid == DISPLAY2)
    {
        uint8 cardinals[0x04];
        
        I2CBusLock();
        ClearEepromBuffer();
        LoadEepromPage(eepromcardpage);
        memcpy(cardinals, &GetEepromBuffer()[0x01], 0x04);
        I2CBusUnlock();
        
        I2CBusLock();
        ClearEepromBuffer();
        LoadEepromPage(eepromprodpage);
        FOR(uint8 index = 0, index < numhoses, index++)
        {
            switch(cardinals[index])
            {
                case 0x01:
                {
                    PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
                    if(puartdisp)
                    {
                        puartdisp->_messagelength = DisplayOutputString(_g_productscreenpos[index][0x00], _g_productscreenpos[index][0x01], puartdisp->_messagetx, PSTRCAST(&GetEepromBuffer()[_EEPROM_PAGE_FIRST16START_]), _EEPROM_QUARTER_PAGE_SIZE_, fontdata);
                        puartdisp->_messagestate = PENDING;
                    }
                    break;
                }
                case 0x02:
                {
                    PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
                    if(puartdisp)
                    {
                        puartdisp->_messagelength = DisplayOutputString(_g_productscreenpos[index][0x00], _g_productscreenpos[index][0x01], puartdisp->_messagetx, PSTRCAST(&GetEepromBuffer()[_EEPROM_PAGE_SECOND16START_]), _EEPROM_QUARTER_PAGE_SIZE_, fontdata);
                        puartdisp->_messagestate = PENDING;
                    }
                    break;
                }
                case 0x03:
                {
                    PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
                    if(puartdisp)
                    {
                        puartdisp->_messagelength = DisplayOutputString(_g_productscreenpos[index][0x00], _g_productscreenpos[index][0x01], puartdisp->_messagetx, PSTRCAST(&GetEepromBuffer()[_EEPROM_PAGE_THIRD16START_]), _EEPROM_QUARTER_PAGE_SIZE_, fontdata);
                        puartdisp->_messagestate = PENDING;
                    }
                    break;
                }
                case 0x04:
                {
                    PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
                    if(puartdisp)
                    {
                        puartdisp->_messagelength = DisplayOutputString(_g_productscreenpos[index][0x00], _g_productscreenpos[index][0x01], puartdisp->_messagetx, PSTRCAST(&GetEepromBuffer()[_EEPROM_PAGE_FOURTH16START_]), _EEPROM_QUARTER_PAGE_SIZE_, fontdata);
                        puartdisp->_messagestate = PENDING;
                    }
                    break;
                }
            }
        }
        I2CBusUnlock();
    }
}

void RejectedAuthorizationMessageDecorator(LPVOID pparam)
{
    FontData fontdata;
    fontdata._size = 0x03;
    fontdata._backcolor = 0xFFFF;
    fontdata._forecolor = 0x0000;
    fontdata._opaquebackground = DISP_TEXT_OPAQUE;
    //MOVE ALL OF THESE TEXTS TO THE EEPROM
    char8 *pmessage[] = 
    {
        "  PETICION DE AUTORIZACION RECHAZADA! ",
    };
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    if(pdisplay->_dispid == DISPLAY1)
    {
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
        if(puartdisp)
        {
            uint8 index = 0;
            uint16 ypos = 0x00BA;
            puartdisp->_messagelength = 0;
            puartdisp->_messagelength += DisplayOutputString(0x001A, ypos, &puartdisp->_messagetx[puartdisp->_messagelength], pmessage[index], strlen(pmessage[index]), fontdata);
            ypos += 0x0020;index++;

            puartdisp->_messagestate = PENDING;
        }
    }
    else
    {
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
        if(puartdisp)
        {
            uint8 index = 0;
            uint16 ypos = 0x00BA;
            puartdisp->_messagelength = 0;
            puartdisp->_messagelength += DisplayOutputString(0x001A, ypos, &puartdisp->_messagetx[puartdisp->_messagelength], pmessage[index], strlen(pmessage[index]), fontdata);
            ypos += 0x0020;index++;
            
            puartdisp->_messagestate = PENDING;
        }
    }
}

void ValidateAuthorizationResponse4Credit(LPVOID pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    PNEAR_PUMPPTR ppumpptr = &_g_pumps[GetPumpIndexFromDisplay(pdisplay)];
    if(ppumpptr->_authorizationinfo._authorized)
    {
        //THE AUTHORIZATION INFORMATION IS READY IN THE "PUMPAUTHORIZATIONINFO" STRUCTURE
        //SO HERE WE ONLY NEED TO CONTINUE THE "DISPLAY FLOW" TO ITS END AND THEN POST THE
        //INFORMATION TO THE "PRIME FLOW".        
        ppumpptr->_authorizationinfo._authorized = false;
        //Select printing the default number of copies        
        memset(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_DESEA_IMPRIMIR_RECIBO)], 0x00, GetBufferLengthFromDisplayID(DISPLAY_DESEA_IMPRIMIR_RECIBO));
        ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_DESEA_IMPRIMIR_RECIBO)] = 0x39;//This value means printing request at the end
        ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_FORMA_PROGRAMACION)] = _PUMP_FILL_CREDIT_;//This value means printing request at the end
        
    }else//If no authorization has been acquired, then return to IDLE state
        ppumpptr->_pumprftransstate = RF_IDLE;
}

void SendAuthorizationRequest4Credit(LPVOID pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    //HERE WE ATTEMPT TO PUSH THE TRANSACTION INTO THE PUMP'S TRANSACTIONAL QUEUE.
    //THIS IS DONE ONLY IF THERE IS ROOM IN THE QUEUE, OTHERWISE THIS STATE WON'T
    //BE REPORTED TO THE REMOTE PEER.
    PNEAR_PUMPPTR ppumpptr = &_g_pumps[GetPumpIndexFromDisplay(pdisplay)];
    
    #ifndef _ALLOW_CREDIT_SALE_TESTING_
    ppumpptr->PumpTransQueueLock(ppumpptr);
    PNEAR_PUMPTRANSACTIONALSTATEPTR pts = ppumpptr->PumpTransQueueAllocate(ppumpptr);
    ppumpptr->PumpTransQueueUnlock(ppumpptr);
    if(pts)
    {
        
        uint8 index = 0;
        pts->_transtate = RF_MUX_CREDIT_SERIAL_AUTH_RESPONSE;
        //Pump ID always comes in the first position (0x00)
        pts->_buffer[index++] = ppumpptr->_pumpid;
        //This is the Hose ID
        pts->_buffer[index++] = ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SUBA_MANIJA)];
        //This is the Serial (iButton) data
        memcpy(&pts->_buffer[index], &((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_ESPERANDO_ID)], GetBufferLengthFromDisplayID(DISPLAY_ESPERANDO_ID));
        PSTRBUFFTOGG((PNEAR_BYTE_PTR)&pts->_buffer[index], GetBufferLengthFromDisplayID(DISPLAY_ESPERANDO_ID));
        index += GetBufferLengthFromDisplayID(DISPLAY_ESPERANDO_ID);
        //This is the milleage
        //PSTRBUFFTOGG((PNEAR_BYTE_PTR)&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE)], GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE));
        index += 0x01;//Since the allocated buffer space for the milleage is 10 bytes length
        memcpy(&pts->_buffer[index], &((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE)], GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE));
        PSTRBUFFTOGG((PNEAR_BYTE_PTR)&pts->_buffer[index], GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE));
        index += GetBufferLengthFromDisplayID(DISPLAY_INTRODUZCA_KILOMETRAJE);

        //Program mode (Money / Volume)
        uint8 value = ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_FORMA_PROGRAMACION)];
        switch(value)
        {
            case 0x0F:
            pts->_buffer[index++] = 0x02;//Money Preset
            break;
            case 0x10:
            pts->_buffer[index++] = 0x01;//Volume Preset
            break;
            case 0x43:
            pts->_buffer[index++] = 0x03;//Full Preset
            break;
        }
        
        //Value for the selected program mode
        memcpy(&pts->_buffer[index], &((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_INTRODUZCA_VALOR)], 0x08);
        if(value == 0x10)
            CastCreditVolumeValue(&pts->_buffer[index], 0x08);
        
        PSTRBUFFTOGG((PNEAR_BYTE_PTR)&pts->_buffer[index], 0x08);
        index += 0x08;

        memset(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_INTRODUZCA_VALOR)], 0x00, 0x08);
        
        pts->_buffersize = index;
        //This flag will be checked to inquiry about the authorization state.
        ppumpptr->_authorizationinfo._authorized = false;
        
        ppumpptr->PumpTransQueueLock(ppumpptr);
        ppumpptr->PumpTransQueueEnqueue(ppumpptr, pts);
        ppumpptr->PumpTransQueueUnlock(ppumpptr);
    }
    #endif
    
    #ifdef _ALLOW_CREDIT_SALE_TESTING_
    //Authorization information
    ppumpptr->_authorizationinfo._authorized = true;

    //See the protocol description file to understand the applied indexes 
    ppumpptr->_authorizationinfo._presettype = 0x02;
    ppumpptr->_authorizationinfo._presetamount  = 0x3DB8;
    ppumpptr->_authorizationinfo._hoseid = ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SUBA_MANIJA)];
    
    uint16 price = 0x3232;
    if(_g_dispenserlayoutinfo._moneydecimals == 0x03 && _g_dispenserlayoutinfo._ppudecimals == 0x02)
        ppumpptr->_authorizationinfo._price = price / 10;
    else
        ppumpptr->_authorizationinfo._price = price;
                    
    if(pdisplay)
    {
        if(ppumpptr->_authorizationinfo._authorized)
            pdisplay->_currentstate = DISPLAY_CREDIT_AUTHORIZATION_ACCEPTED;
    }
    #endif
}

void KillHosePositionStateValidation(LPVOID pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay->_pdata)
    {
        ((PSINKMESSAGEPTR)pdisplay->_pdata)->_selfkill = true;
        pdisplay->_pdata = PNEAR_NULLPTR;
    }
}

void DisplayStationIDDecorator(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay)
    {
        pdisplay->_bufferinfo._xpos = pdisplay->_pcurrflow->_inputboxinfo._xdefpos;
        pdisplay->_bufferinfo._ypos = pdisplay->_pcurrflow->_inputboxinfo._ydefpos;
        if(pdisplay->_dispid == DISPLAY1)
        {
            UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
            if(puartdisp)
            {
                puartdisp->_messagelength = DisplayOutputString(pdisplay->_bufferinfo._xpos, pdisplay->_bufferinfo._ypos, puartdisp->_messagetx, pdisplay->_bufferinfo._buffer, pdisplay->_pcurrflow->_inputboxinfo._maxchars, pdisplay->_pcurrflow->_inputboxinfo._font);
                puartdisp->_messagestate = PENDING;
            }
        }
        else
        {
            UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
            if(puartdisp)
            {
                puartdisp->_messagelength = DisplayOutputString(pdisplay->_bufferinfo._xpos, pdisplay->_bufferinfo._ypos, puartdisp->_messagetx, pdisplay->_bufferinfo._buffer, pdisplay->_pcurrflow->_inputboxinfo._maxchars, pdisplay->_pcurrflow->_inputboxinfo._font);
                puartdisp->_messagestate = PENDING;
            }
        }
    }
}

void DisplayStationIDStore(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay)
    {
        uint16 stationid = 0x0000;
        FOR(uint8 index = 0, index < pdisplay->_bufferinfo._bufferindex, index++)
            stationid += (uint16)(pow(10, pdisplay->_bufferinfo._bufferindex - index - 1)*(pdisplay->_bufferinfo._buffer[index] & 0x0F));
            
        //CAREFUL WITH RETURNS BETWEEN THE LOCK/UNLOCK CALLS
        //AND RECURSIVE CALLS THAT WOULD DEADLOCK!
        I2CBusLock();
        ClearEepromBuffer();
        
        LoadEepromPage(EEPROM_CONFIGURATION_PAGE5);
        GetEepromBuffer()[0x00] = (uint8)(stationid & 0x00FF);
        GetEepromBuffer()[0x01] = (uint8)((stationid >> 8) & 0x00FF);
        StoreEepromPage(EEPROM_CONFIGURATION_PAGE5);
        
        _g_stationidentifier = stationid;
        
        I2CBusUnlock();
        
    }
}

void DisplayStationIDLoad(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay)
    {
        //CAREFUL WITH RETURNS BETWEEN THE LOCK/UNLOCK CALLS
        //AND RECURSIVE CALLS THAT WOULD DEADLOCK!
        I2CBusLock();
        ClearEepromBuffer();
        
        LoadEepromPage(EEPROM_CONFIGURATION_PAGE5);
        uint16 stationid = (((uint16)GetEepromBuffer()[0x01] << 8) & 0xFF00) | ((uint16)GetEepromBuffer()[0x00]);
        I2CBusUnlock();
        
        memset(pdisplay->_bufferinfo._buffer, 0x00, _DISPLAY_BUFFER_SIZE_);
        B78AD90CF552D9B30A(stationid, PBYTECAST(pdisplay->_bufferinfo._buffer));
        pdisplay->_bufferinfo._bufferindex = strlen(pdisplay->_bufferinfo._buffer);
        PSTRBUFFTOGG(PBYTECAST(pdisplay->_bufferinfo._buffer), pdisplay->_bufferinfo._bufferindex);
    }
}

void DisplayPumpIDOffsetLoad(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay)
    {
        //CAREFUL WITH RETURNS BETWEEN THE LOCK/UNLOCK CALLS
        //AND RECURSIVE CALLS THAT WOULD DEADLOCK!
        I2CBusLock();
        ClearEepromBuffer();
        
        LoadEepromPage(EEPROM_CONFIGURATION_PAGE7);
        uint8 pumpoffset = ((uint8)GetEepromBuffer()[0x09]);
        I2CBusUnlock();
        
        memset(pdisplay->_bufferinfo._buffer, 0x00, _DISPLAY_BUFFER_SIZE_);
        B78AD90CF552D9B30A(pumpoffset, PBYTECAST(pdisplay->_bufferinfo._buffer));
        pdisplay->_bufferinfo._bufferindex = strlen(pdisplay->_bufferinfo._buffer);
        PSTRBUFFTOGG(PBYTECAST(pdisplay->_bufferinfo._buffer), pdisplay->_bufferinfo._bufferindex);
    }
}

void DisplayPumpIDOffsetDecorator(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay)
    {
        pdisplay->_bufferinfo._xpos = pdisplay->_pcurrflow->_inputboxinfo._xdefpos;
        pdisplay->_bufferinfo._ypos = pdisplay->_pcurrflow->_inputboxinfo._ydefpos;
        if(pdisplay->_dispid == DISPLAY1)
        {
            UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
            if(puartdisp)
            {
                puartdisp->_messagelength = DisplayOutputString(pdisplay->_bufferinfo._xpos, pdisplay->_bufferinfo._ypos, puartdisp->_messagetx, pdisplay->_bufferinfo._buffer, pdisplay->_pcurrflow->_inputboxinfo._maxchars, pdisplay->_pcurrflow->_inputboxinfo._font);
                puartdisp->_messagestate = PENDING;
            }
        }
        else
        {
            UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
            if(puartdisp)
            {
                puartdisp->_messagelength = DisplayOutputString(pdisplay->_bufferinfo._xpos, pdisplay->_bufferinfo._ypos, puartdisp->_messagetx, pdisplay->_bufferinfo._buffer, pdisplay->_pcurrflow->_inputboxinfo._maxchars, pdisplay->_pcurrflow->_inputboxinfo._font);
                puartdisp->_messagestate = PENDING;
            }
        }
    }
}

void DisplayPumpIDOffsetStore(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay)
    {
        //CAREFUL WITH RETURNS BETWEEN THE LOCK/UNLOCK CALLS
        //AND RECURSIVE CALLS THAT WOULD DEADLOCK!
        uint8 pumpidoffset = 0x00;
        FOR(uint8 index = 0, index < pdisplay->_bufferinfo._bufferindex, index++)
            pumpidoffset += (uint16)(pow(10, pdisplay->_bufferinfo._bufferindex - index - 1)*(pdisplay->_bufferinfo._buffer[index] & 0x0F));
            
        I2CBusLock();
        ClearEepromBuffer();
        
        LoadEepromPage(EEPROM_CONFIGURATION_PAGE7);
        GetEepromBuffer()[0x09] = (uint8)pumpidoffset;
        StoreEepromPage(EEPROM_CONFIGURATION_PAGE7);
        
        _g_dispenserlayoutinfo._positionoffset = (uint8)pumpidoffset;
        
        I2CBusUnlock();
        
    }
}

//@Created By: HIJH
//@Septembre de 2016
void iButtonScreenNotification(LPVOID pparam)
{
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pmsg->_pdata;
    if(pdisplay)
    {
        pdisplay->_timeoutscrid = DISPLAY_ID_RECONOCIDO;
        //Here we are storing the serial identifier read from the iButton device
        memcpy(pdisplay->_bufferinfo._buffer, pmsg->_buffer, _IBUTTON_PAYLOAD_LENGTH_);
        pdisplay->_bufferinfo._bufferindex = _IBUTTON_PAYLOAD_LENGTH_;
        
        _ALLOCATE_SINKMESSAGE_SLOT(psinkmsg);
        if(psinkmsg)
        {
            
            char8 pdummybuffer[] = { 0xAA, 0x00, 0x00, 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
            if(pdisplay->_dispid == DISPLAY1)
                psinkmsg->_messageid = DISPLAY1_RECEPTION;
            else
                psinkmsg->_messageid = DISPLAY2_RECEPTION;
            
            psinkmsg->_messagetype = FIRSTFOUND;
            memcpy(psinkmsg->_buffer, (const void*)pdummybuffer, sizeof(pdummybuffer));
            psinkmsg->_buffersize = sizeof(pdummybuffer);
            psinkmsg->_messagestate = SINK_BUSY;
            
        }
    }
}

//@Created By: HIJH
//@Septembre de 2016
void TerminateiButtonScan(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay->_pdata)
    {
        if((PSINKMESSAGEPTR)pdisplay->_pdata)
            ((PSINKMESSAGEPTR)pdisplay->_pdata)->_selfkill = true;
            
        pdisplay->_pdata = PNEAR_NULLPTR;
    }
}

//@Created By: HIJH
//@Septembre de 2016
void InitiateiButtonScan(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay->_dispid == DISPLAY1)
    {
        PSINKMESSAGEPTR psinkmsg = AllocateMessageSlot();
        if(psinkmsg)
        {
            pdisplay->_pdata = psinkmsg;
            psinkmsg->_pdata = pdisplay;
            psinkmsg->_messageid = IBUTTON1_SCAN;
            psinkmsg->_messagetimeoutlimit = _SINK_TIMEOUT_50MS_;
            psinkmsg->_messagedelay = _SINK_TIMEOUT_1S_;
            psinkmsg->Callback = iButtonScreenNotification;
            psinkmsg->_messagetype = FIRSTFOUNDFOREVER;
            psinkmsg->_messagestate = SINK_BUSY;
        }        
    }
    else if(pdisplay->_dispid == DISPLAY2)
    {
        PSINKMESSAGEPTR psinkmsg = AllocateMessageSlot();
        if(psinkmsg)
        {
            pdisplay->_pdata = psinkmsg;
            psinkmsg->_pdata = pdisplay;
            psinkmsg->_messageid = IBUTTON2_SCAN;
            psinkmsg->_messagetimeoutlimit = _SINK_TIMEOUT_50MS_;
            psinkmsg->_messagedelay = _SINK_TIMEOUT_1S_;
            psinkmsg->Callback = iButtonScreenNotification;
            psinkmsg->_messagetype = FIRSTFOUNDFOREVER;
            psinkmsg->_messagestate = SINK_BUSY;
        }
    }
}


void DrawDateTime(void *pparam)
{
    char8 buffer[8];
    FontData fontdata;
    fontdata._size = 0x04;
    fontdata._backcolor = 0xFFFF;
    fontdata._forecolor = 0x0000;
    fontdata._opaquebackground = DISP_TEXT_OPAQUE;
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    if(pdisplay->_dispid == DISPLAY1 && ((PDISPLAYFLOWPTR)pdisplay->_pcurrflow)->_scrid == DISPLAY_CONFIGURAR_FECHA_HORA)
    {
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
        if(puartdisp)
        {
            uint8 index = 0;
            
            //CAREFUL WITH RETURNS BETWEEN THE LOCK/UNLOCK CALLS
            //AND RECURSIVE CALLS THAT WOULD DEADLOCK!
            I2CBusLock();
            
            ClearRtcBuffer();
            ReadRTC();                           
            memset(buffer, 0x00, sizeof(buffer));            
            
            if(GetRtcBuffer()[RTC_HOUR] & 0b01000000){
                buffer[index++] = ((GetRtcBuffer()[RTC_HOUR] >> 4) & 0x01) | 0x30;
                buffer[index++] = (GetRtcBuffer()[RTC_HOUR] & 0x0F) | 0x30;
            }else{
                buffer[index++] = ((GetRtcBuffer()[RTC_HOUR] >> 4) & 0x03) | 0x30;
                buffer[index++] = (GetRtcBuffer()[RTC_HOUR] & 0x0F) | 0x30;
            }
            
            buffer[index++] = 0x3A;
            buffer[index++] = ((GetRtcBuffer()[RTC_MINUTES] >> 4) & 0x07) | 0x30;
            buffer[index++] = (GetRtcBuffer()[RTC_MINUTES] & 0x0F) | 0x30;
            buffer[index++] = 0x3A;
            buffer[index++] = ((GetRtcBuffer()[RTC_SECONDS] >> 4) & 0x07) | 0x30;
            buffer[index++] = (GetRtcBuffer()[RTC_SECONDS] & 0x0F) | 0x30;
            
            I2CBusUnlock();
            
            puartdisp->_messagelength = DisplayOutputString(0x0C9, 0x0BA, puartdisp->_messagetx, buffer, index, fontdata);
            puartdisp->_messagestate = PENDING;
        }
        
        puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
        if(puartdisp)
        {
            uint8 index = 0;
            
            //CAREFUL WITH RETURNS BETWEEN THE LOCK/UNLOCK CALLS
            //AND RECURSIVE CALLS THAT WOULD DEADLOCK!
            I2CBusLock();
            
            ClearRtcBuffer();
            ReadRTC();                           
            memset(buffer, 0x00, sizeof(buffer));            
            
            buffer[index++] = ((GetRtcBuffer()[RTC_DAY] >> 4) & 0x03) | 0x30;
            buffer[index++] = (GetRtcBuffer()[RTC_DAY] & 0x0F) | 0x30;
            buffer[index++] = 0x2F;
            buffer[index++] = ((GetRtcBuffer()[RTC_MONTH] >> 4) & 0x01) | 0x30;
            buffer[index++] = (GetRtcBuffer()[RTC_MONTH] & 0x0F) | 0x30;
            buffer[index++] = 0x2F;
            buffer[index++] = ((GetRtcBuffer()[RTC_YEAR] >> 4) & 0x0F) | 0x30;
            buffer[index++] = (GetRtcBuffer()[RTC_YEAR] & 0x0F) | 0x30;

            I2CBusUnlock();
            
            puartdisp->_messagelength = DisplayOutputString(0x0C9, 0x12A, puartdisp->_messagetx, buffer, index, fontdata);
            puartdisp->_messagestate = PENDING;
        }
        
    }
    else if(pdisplay->_dispid == DISPLAY2 && ((PDISPLAYFLOWPTR)pdisplay->_pcurrflow)->_scrid == DISPLAY_CONFIGURAR_FECHA_HORA)
    {
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
        if(puartdisp)
        {
            uint8 index = 0;
            
            //CAREFUL WITH RETURNS BETWEEN THE LOCK/UNLOCK CALLS
            //AND RECURSIVE CALLS THAT WOULD DEADLOCK!
            I2CBusLock();
            
            ClearRtcBuffer();
            ReadRTC();                           
            memset(buffer, 0x00, sizeof(buffer));            
            
            if(GetRtcBuffer()[RTC_HOUR] & 0b01000000){
                buffer[index++] = ((GetRtcBuffer()[RTC_HOUR] >> 4) & 0x01) | 0x30;
                buffer[index++] = (GetRtcBuffer()[RTC_HOUR] & 0x0F) | 0x30;
            }else{
                buffer[index++] = ((GetRtcBuffer()[RTC_HOUR] >> 4) & 0x03) | 0x30;
                buffer[index++] = (GetRtcBuffer()[RTC_HOUR] & 0x0F) | 0x30;
            }
            
            buffer[index++] = 0x3A;
            buffer[index++] = ((GetRtcBuffer()[RTC_MINUTES] >> 4) & 0x07) | 0x30;
            buffer[index++] = (GetRtcBuffer()[RTC_MINUTES] & 0x0F) | 0x30;
            buffer[index++] = 0x3A;
            buffer[index++] = ((GetRtcBuffer()[RTC_SECONDS] >> 4) & 0x07) | 0x30;
            buffer[index++] = (GetRtcBuffer()[RTC_SECONDS] & 0x0F) | 0x30;
            
            I2CBusUnlock();
            
            puartdisp->_messagelength = DisplayOutputString(0x0C9, 0x0BA, puartdisp->_messagetx, buffer, index, fontdata);
            puartdisp->_messagestate = PENDING;
        }
        
        puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
        if(puartdisp)
        {
            uint8 index = 0;
            
            //CAREFUL WITH RETURNS BETWEEN THE LOCK/UNLOCK CALLS
            //AND RECURSIVE CALLS THAT WOULD DEADLOCK!
            I2CBusLock();
            
            ClearRtcBuffer();
            ReadRTC();                           
            memset(buffer, 0x00, sizeof(buffer));            
            
            buffer[index++] = ((GetRtcBuffer()[RTC_DAY] >> 4) & 0x03) | 0x30;
            buffer[index++] = (GetRtcBuffer()[RTC_DAY] & 0x0F) | 0x30;
            buffer[index++] = 0x2F;
            buffer[index++] = ((GetRtcBuffer()[RTC_MONTH] >> 4) & 0x01) | 0x30;
            buffer[index++] = (GetRtcBuffer()[RTC_MONTH] & 0x0F) | 0x30;
            buffer[index++] = 0x2F;
            buffer[index++] = ((GetRtcBuffer()[RTC_YEAR] >> 4) & 0x0F) | 0x30;
            buffer[index++] = (GetRtcBuffer()[RTC_YEAR] & 0x0F) | 0x30;

            I2CBusUnlock();
            
            puartdisp->_messagelength = DisplayOutputString(0x0C9, 0x12A, puartdisp->_messagetx, buffer, index, fontdata);
            puartdisp->_messagestate = PENDING;
        }
    }
}

void DrawHomeDateTime(void *pparam)
{
    char8 buffer[8];
    FontData fontdata;
    fontdata._size = 0x02;
    fontdata._backcolor = 0xFFFF;
    fontdata._forecolor = 0x0000;
    fontdata._opaquebackground = DISP_TEXT_OPAQUE;
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    if(pdisplay->_dispid == DISPLAY1 && ((PDISPLAYFLOWPTR)pdisplay->_pcurrflow)->_scrid == DISPLAY_INICIO0)
    {
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
        if(puartdisp)
        {
            uint8 index = 0;
            
            //CAREFUL WITH RETURNS BETWEEN THE LOCK/UNLOCK CALLS
            //AND RECURSIVE CALLS THAT WOULD DEADLOCK!
            I2CBusLock();
            
            ClearRtcBuffer();
            ReadRTC();                           
            memset(buffer, 0x00, sizeof(buffer));            
            
            if(GetRtcBuffer()[RTC_HOUR] & 0b01000000){
                buffer[index++] = ((GetRtcBuffer()[RTC_HOUR] >> 4) & 0x01) | 0x30;
                buffer[index++] = (GetRtcBuffer()[RTC_HOUR] & 0x0F) | 0x30;
            }else{
                buffer[index++] = ((GetRtcBuffer()[RTC_HOUR] >> 4) & 0x03) | 0x30;
                buffer[index++] = (GetRtcBuffer()[RTC_HOUR] & 0x0F) | 0x30;
            }
            if(_g_homeanimarray[DISPLAY1]._colontoggler)
                buffer[index++] = 0x3A;
            else
                buffer[index++] = 0x20;
                
            buffer[index++] = ((GetRtcBuffer()[RTC_MINUTES] >> 4) & 0x07) | 0x30;
            buffer[index++] = (GetRtcBuffer()[RTC_MINUTES] & 0x0F) | 0x30;
            
            /*buffer[index++] = 0x3A;
            buffer[index++] = ((GetRtcBuffer()[RTC_SECONDS] >> 4) & 0x07) | 0x30;
            buffer[index++] = (GetRtcBuffer()[RTC_SECONDS] & 0x0F) | 0x30;
            */
            
            I2CBusUnlock();
            
            puartdisp->_messagelength = DisplayOutputString(0x010, 0x1BA, puartdisp->_messagetx, buffer, index, fontdata);
            puartdisp->_messagestate = PENDING;
        }
        
        puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
        if(puartdisp)
        {
            uint8 index = 0;
            
            //CAREFUL WITH RETURNS BETWEEN THE LOCK/UNLOCK CALLS
            //AND RECURSIVE CALLS THAT WOULD DEADLOCK!
            I2CBusLock();
            
            ClearRtcBuffer();
            ReadRTC();                           
            memset(buffer, 0x00, sizeof(buffer));            
            
            buffer[index++] = ((GetRtcBuffer()[RTC_DAY] >> 4) & 0x03) | 0x30;
            buffer[index++] = (GetRtcBuffer()[RTC_DAY] & 0x0F) | 0x30;
            buffer[index++] = 0x2F;
            buffer[index++] = ((GetRtcBuffer()[RTC_MONTH] >> 4) & 0x01) | 0x30;
            buffer[index++] = (GetRtcBuffer()[RTC_MONTH] & 0x0F) | 0x30;
            
            buffer[index++] = 0x2F;
            buffer[index++] = ((GetRtcBuffer()[RTC_YEAR] >> 4) & 0x0F) | 0x30;
            buffer[index++] = (GetRtcBuffer()[RTC_YEAR] & 0x0F) | 0x30;

            I2CBusUnlock();
            
            puartdisp->_messagelength = DisplayOutputString(0x210, 0x1BA, puartdisp->_messagetx, buffer, index, fontdata);
            puartdisp->_messagestate = PENDING;
        }
        
    }
    else if(pdisplay->_dispid == DISPLAY2 && ((PDISPLAYFLOWPTR)pdisplay->_pcurrflow)->_scrid == DISPLAY_INICIO0)
    {
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
        if(puartdisp)
        {
            uint8 index = 0;
            
            //CAREFUL WITH RETURNS BETWEEN THE LOCK/UNLOCK CALLS
            //AND RECURSIVE CALLS THAT WOULD DEADLOCK!
            I2CBusLock();
            
            ClearRtcBuffer();
            ReadRTC();                           
            memset(buffer, 0x00, sizeof(buffer));            
            
            if(GetRtcBuffer()[RTC_HOUR] & 0b01000000){
                buffer[index++] = ((GetRtcBuffer()[RTC_HOUR] >> 4) & 0x01) | 0x30;
                buffer[index++] = (GetRtcBuffer()[RTC_HOUR] & 0x0F) | 0x30;
            }else{
                buffer[index++] = ((GetRtcBuffer()[RTC_HOUR] >> 4) & 0x03) | 0x30;
                buffer[index++] = (GetRtcBuffer()[RTC_HOUR] & 0x0F) | 0x30;
            }
            if(_g_homeanimarray[DISPLAY2]._colontoggler)
                buffer[index++] = 0x3A;
            else
                buffer[index++] = 0x20;
                
            buffer[index++] = ((GetRtcBuffer()[RTC_MINUTES] >> 4) & 0x07) | 0x30;
            buffer[index++] = (GetRtcBuffer()[RTC_MINUTES] & 0x0F) | 0x30;
            
            /*buffer[index++] = 0x3A;
            buffer[index++] = ((GetRtcBuffer()[RTC_SECONDS] >> 4) & 0x07) | 0x30;
            buffer[index++] = (GetRtcBuffer()[RTC_SECONDS] & 0x0F) | 0x30;*/
            
            I2CBusUnlock();            
            
            puartdisp->_messagelength = DisplayOutputString(0x010, 0x1BA, puartdisp->_messagetx, buffer, index, fontdata);
            puartdisp->_messagestate = PENDING;
        }
        
        puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
        if(puartdisp)
        {
            uint8 index = 0;
            
            //CAREFUL WITH RETURNS BETWEEN THE LOCK/UNLOCK CALLS
            //AND RECURSIVE CALLS THAT WOULD DEADLOCK!
            I2CBusLock();
            
            ClearRtcBuffer();
            ReadRTC();                           
            memset(buffer, 0x00, sizeof(buffer));            
            
            buffer[index++] = ((GetRtcBuffer()[RTC_DAY] >> 4) & 0x03) | 0x30;
            buffer[index++] = (GetRtcBuffer()[RTC_DAY] & 0x0F) | 0x30;
            buffer[index++] = 0x2F;
            buffer[index++] = ((GetRtcBuffer()[RTC_MONTH] >> 4) & 0x01) | 0x30;
            buffer[index++] = (GetRtcBuffer()[RTC_MONTH] & 0x0F) | 0x30;
            buffer[index++] = 0x2F;
            buffer[index++] = ((GetRtcBuffer()[RTC_YEAR] >> 4) & 0x0F) | 0x30;
            buffer[index++] = (GetRtcBuffer()[RTC_YEAR] & 0x0F) | 0x30;

            I2CBusUnlock();            
            
            puartdisp->_messagelength = DisplayOutputString(0x210, 0x1BA, puartdisp->_messagetx, buffer, index, fontdata);
            puartdisp->_messagestate = PENDING;
        }
    }
}

void DisplayUpdateHomeAnimation(void *pparam)
{    
    char8 buffer[2];
    FontData fontdata;
    fontdata._size = 0x04;
    fontdata._backcolor = 0xFFFF;
    fontdata._forecolor = 0x0000;
    fontdata._opaquebackground = DISP_TEXT_OPAQUE;
    PSINKMESSAGEPTR psinkmsg = (PSINKMESSAGEPTR)pparam;
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)psinkmsg->_pdata;
    if(pdisplay->_dispid == DISPLAY1 && ((PDISPLAYFLOWPTR)pdisplay->_pcurrflow)->_scrid == DISPLAY_INICIO0)
    {
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
        if(puartdisp)
        {
            buffer[0] = _g_homeanimarray[DISPLAY1]._g_homeanimation[_g_homeanimarray[DISPLAY1]._currindex++];
            if(_g_homeanimarray[DISPLAY1]._currindex >= sizeof(_g_homeanimarray[DISPLAY1]._g_homeanimation))
                _g_homeanimarray[DISPLAY1]._currindex = 0x00;
            
            puartdisp->_messagelength = DisplayOutputString(0x250, 0x012, puartdisp->_messagetx, buffer, 1, fontdata);
            puartdisp->_messagestate = PENDING;
        }

        uint8 pumpavstates[0x02];
        memset(pumpavstates, 0x00, 0x02);
        GetPumpsAvailability(pdisplay, pumpavstates);

        bool pumpsprintstate[0x02];
        GetPumpsPrintState(pdisplay, pumpsprintstate);
        
        //On-Display witness to alert about pending or in-execution operations on the pump
        if(!pumpavstates[0x01])
        {
            puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
            if(puartdisp)
            {
                //SET FC BC COLORS (BLUE) IF PRINT HAS BEEN REQUESTED
                if(pumpsprintstate[0x01])
                {
                    uint8 buff[] = { 0xAA, 0x40, 0x00, 0x1F, 0xFF, 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
                    memcpy(puartdisp->_messagetx, buff, sizeof(buff));
                    puartdisp->_messagelength = sizeof(buff);
                    puartdisp->_messagestate = PENDING;
                }
                else
                {
                    uint8 buff[] = { 0xAA, 0x40, 0xF8, 0x00, 0xFF, 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
                    memcpy(puartdisp->_messagetx, buff, sizeof(buff));
                    puartdisp->_messagelength = sizeof(buff);
                    puartdisp->_messagestate = PENDING;
                }
            }
        }
        else
        {
            puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
            if(puartdisp)
            {
                //SET FC BC COLORS (WHITE)
                uint8 buff[] = { 0xAA, 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
                memcpy(puartdisp->_messagetx, buff, sizeof(buff));
                puartdisp->_messagelength = sizeof(buff);
                puartdisp->_messagestate = PENDING;
            }
        }
            
        puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
        if(puartdisp)
        {
            uint8 buff[] = { 0xAA, 0x5B, 0x02, 0x50, 0x01, 0x70, 0x02, 0x70, 0x01, 0x90, 0xCC, 0x33, 0xC3, 0x3C };
            memcpy(puartdisp->_messagetx, buff, sizeof(buff));
            puartdisp->_messagelength = sizeof(buff);
            puartdisp->_messagestate = PENDING;
        }

        //On-Display witness to alert about pending or in-execution operations on the pump
        if(!pumpavstates[0x00])
        {
            puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
            if(puartdisp)
            {
                //SET FC BC COLORS (BLUE) IF PRINT HAS BEEN REQUESTED
                if(pumpsprintstate[0x00])
                {
                    uint8 buff[] = { 0xAA, 0x40, 0x00, 0x1F, 0xFF, 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
                    memcpy(puartdisp->_messagetx, buff, sizeof(buff));
                    puartdisp->_messagelength = sizeof(buff);
                    puartdisp->_messagestate = PENDING;
                }
                else
                {
                    uint8 buff[] = { 0xAA, 0x40, 0xF8, 0x00, 0xFF, 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
                    memcpy(puartdisp->_messagetx, buff, sizeof(buff));
                    puartdisp->_messagelength = sizeof(buff);
                    puartdisp->_messagestate = PENDING;
                }
            }
        }
        else
        {
            puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
            if(puartdisp)
            {
                //SET FC BC COLORS (WHITE)
                uint8 buff[] = { 0xAA, 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
                memcpy(puartdisp->_messagetx, buff, sizeof(buff));
                puartdisp->_messagelength = sizeof(buff);
                puartdisp->_messagestate = PENDING;
            }
        }
        
        puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
        if(puartdisp)
        {
            uint8 buff[] = { 0xAA, 0x5B, 0x00, 0x10, 0x01, 0x70, 0x00, 0x30, 0x01, 0x90, 0xCC, 0x33, 0xC3, 0x3C };
            memcpy(puartdisp->_messagetx, buff, sizeof(buff));
            puartdisp->_messagelength = sizeof(buff);
            puartdisp->_messagestate = PENDING;
        }
        
        fontdata._backcolor = 0xFFFF;
        fontdata._forecolor = 0x0000;

        if((100*_g_homeanimarray[DISPLAY1]._timeoutmultiplier) > _SINK_TIMEOUT_2S_)
        {
            DrawHomeDateTime(pdisplay);
            _g_homeanimarray[DISPLAY1]._timeoutmultiplier = 0;
            _g_homeanimarray[DISPLAY1]._colontoggler = !_g_homeanimarray[DISPLAY1]._colontoggler;
            char8 *pmessage = "MUX Ver. 18.3";
            fontdata._size = 0x01;
            UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
            if(puartdisp)
    		{
    			puartdisp->_messagelength = DisplayOutputString(0x000F, 0x000E, puartdisp->_messagetx, pmessage, strlen(pmessage), fontdata);
    			puartdisp->_messagestate = PENDING;
    		}
        }else
            _g_homeanimarray[DISPLAY1]._timeoutmultiplier++;
            
        fontdata._size = 0x01;
        if(!_g_display1._starting)
        {
            if(FindSinkMessage(_g_printerlayout._printerportside1))
            {
                puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
                if(puartdisp)
                {
                    PSTR ptxtbuffer = "Imprimiendo, por favor espere...";
                    puartdisp->_messagelength = DisplayOutputString(0x0B8, 0x1D0, puartdisp->_messagetx, ptxtbuffer, strlen(ptxtbuffer), fontdata);
                    puartdisp->_messagestate = PENDING;
                }
            }
            else
            {
                puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
                if(puartdisp)
                {
                    uint8 ptxtbuffer[0x20];
                    memset(ptxtbuffer, 0x00, 0x20);
                    puartdisp->_messagelength = DisplayOutputString(0x0B8, 0x1D0, puartdisp->_messagetx, PSTRCAST(ptxtbuffer), 0x20, fontdata);
                    puartdisp->_messagestate = PENDING;
                }
            }
        }
        
        if(_g_dispenserlayoutinfo._inconfiguration)
        {
            puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
            if(puartdisp)
            {
                if(_g_dispenserlayoutinfo._currentloopopen)
                {
                    PSTR ptxtbuffer = "<<** LAZO DE CORRIENTE ABIERTO! **>>";
                    memcpy(puartdisp->_messagetx, ptxtbuffer, strlen(ptxtbuffer));
                    puartdisp->_messagelength = DisplayOutputString(0x0B8, 0x1D0, puartdisp->_messagetx, ptxtbuffer, strlen(ptxtbuffer), fontdata);
                }
                else
                {
                    PSTR ptxtbuffer = "Configurando, por favor espere.....";
                    memcpy(puartdisp->_messagetx, ptxtbuffer, strlen(ptxtbuffer));
                    puartdisp->_messagelength = DisplayOutputString(0x0B8, 0x1D0, puartdisp->_messagetx, ptxtbuffer, strlen(ptxtbuffer), fontdata);
                }
                puartdisp->_messagestate = PENDING;
            }
        }
        else if(!_g_configured[0x00])
        {
            puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
            if(puartdisp)
            {
                char8 txtbuffer[0x28];
                memset(txtbuffer, 0x20, 0x28);
                if(_g_dispenserlayoutinfo._numpositions == 0x04)
                    sprintf(txtbuffer, "Welcome to INSEPET GVR PRIME QUATTRO!  ");
                else
                    sprintf(txtbuffer, "Welcome to INSEPET GVR PRIME/ENCORE DUPLA!   ");
                    
                puartdisp->_messagelength = DisplayOutputString(0x0A0, 0x1D0, puartdisp->_messagetx, txtbuffer, strlen(txtbuffer), fontdata);
                puartdisp->_messagestate = PENDING;
            }
            _g_configured[0x00] = true;
            
        }
        
    }
    else if(pdisplay->_dispid == DISPLAY2 && ((PDISPLAYFLOWPTR)pdisplay->_pcurrflow)->_scrid == DISPLAY_INICIO0)
    {
        UARTMessage *puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
        if(puartdisp)
        {
            buffer[0] = _g_homeanimarray[DISPLAY2]._g_homeanimation[_g_homeanimarray[DISPLAY2]._currindex++];
            if(_g_homeanimarray[DISPLAY2]._currindex >= sizeof(_g_homeanimarray[DISPLAY2]._g_homeanimation))
                _g_homeanimarray[DISPLAY2]._currindex = 0x00;
            
            puartdisp->_messagelength = DisplayOutputString(0x250, 0x012, puartdisp->_messagetx, buffer, 1, fontdata);
            puartdisp->_messagestate = PENDING;
        }
        
        uint8 pumpavstates[0x02];
        memset(pumpavstates, 0x00, 0x02);
        GetPumpsAvailability(pdisplay, pumpavstates);

        bool pumpsprintstate[0x02];
        GetPumpsPrintState(pdisplay, pumpsprintstate);
        
        //On-Display witness to alert about pending or in-execution operations on the pump
        if(!pumpavstates[0x01])
        {
            puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
            if(puartdisp)
            {
                if(pumpsprintstate[0x01])
                {
                    //SET FC BC COLORS (BLUE) IF PRINT HAS BEEN REQUESTED
                    uint8 buff[] = { 0xAA, 0x40, 0x00, 0x1F, 0xFF, 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
                    memcpy(puartdisp->_messagetx, buff, sizeof(buff));
                    puartdisp->_messagelength = sizeof(buff);
                    puartdisp->_messagestate = PENDING;
                }
                else
                {
                    //SET FC BC COLORS (RED)
                    uint8 buff[] = { 0xAA, 0x40, 0xF8, 0x00, 0xFF, 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
                    memcpy(puartdisp->_messagetx, buff, sizeof(buff));
                    puartdisp->_messagelength = sizeof(buff);
                    puartdisp->_messagestate = PENDING;
                }
            }
        }
        else
        {
            puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
            if(puartdisp)
            {
                //SET FC BC COLORS (WHITE)
                uint8 buff[] = { 0xAA, 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
                memcpy(puartdisp->_messagetx, buff, sizeof(buff));
                puartdisp->_messagelength = sizeof(buff);
                puartdisp->_messagestate = PENDING;
            }
        }
        
        puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
        if(puartdisp)
        {
            uint8 buff[] = { 0xAA, 0x5B, 0x02, 0x50, 0x01, 0x70, 0x02, 0x70, 0x01, 0x90, 0xCC, 0x33, 0xC3, 0x3C };
            memcpy(puartdisp->_messagetx, buff, sizeof(buff));
            puartdisp->_messagelength = sizeof(buff);
            puartdisp->_messagestate = PENDING;
        }
        
        //On-Display witness to alert about pending or in-execution operations on the pump
        if(!pumpavstates[0x00])
        {
            puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
            if(puartdisp)
            {
                if(pumpsprintstate[0x00])
                {
                    //SET FC BC COLORS (BLUE) IF PRINT HAS BEEN REQUESTED
                    uint8 buff[] = { 0xAA, 0x40, 0x00, 0x1F, 0xFF, 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
                    memcpy(puartdisp->_messagetx, buff, sizeof(buff));
                    puartdisp->_messagelength = sizeof(buff);
                    puartdisp->_messagestate = PENDING;
                }
                else
                {
                    //SET FC BC COLORS (RED)
                    uint8 buff[] = { 0xAA, 0x40, 0xF8, 0x00, 0xFF, 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
                    memcpy(puartdisp->_messagetx, buff, sizeof(buff));
                    puartdisp->_messagelength = sizeof(buff);
                    puartdisp->_messagestate = PENDING;
                }
            }
        }
        else
        {
            puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
            if(puartdisp)
            {
                //SET FC BC COLORS (WHITE)
                uint8 buff[] = { 0xAA, 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
                memcpy(puartdisp->_messagetx, buff, sizeof(buff));
                puartdisp->_messagelength = sizeof(buff);
                puartdisp->_messagestate = PENDING;
            }
        }
        
        puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
        if(puartdisp)
        {
            uint8 buff[] = { 0xAA, 0x5B, 0x00, 0x10, 0x01, 0x70, 0x00, 0x30, 0x01, 0x90, 0xCC, 0x33, 0xC3, 0x3C };
            memcpy(puartdisp->_messagetx, buff, sizeof(buff));
            puartdisp->_messagelength = sizeof(buff);
            puartdisp->_messagestate = PENDING;
        }
        
        fontdata._backcolor = 0xFFFF;
        fontdata._forecolor = 0x0000;
        
        if((100*_g_homeanimarray[DISPLAY2]._timeoutmultiplier) > _SINK_TIMEOUT_2S_)
        {
            DrawHomeDateTime(pdisplay);
            _g_homeanimarray[DISPLAY2]._timeoutmultiplier = 0;
            _g_homeanimarray[DISPLAY2]._colontoggler = !_g_homeanimarray[DISPLAY2]._colontoggler;
        }else
            _g_homeanimarray[DISPLAY2]._timeoutmultiplier++;
            
        fontdata._size = 0x01;
        if(!_g_display2._starting)
        {
            if(FindSinkMessage(_g_printerlayout._printerportside2))
            {
                puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
                if(puartdisp)
                {
                    PSTR ptxtbuffer = "Imprimiendo, por favor espere...";
                    puartdisp->_messagelength = DisplayOutputString(0x0B8, 0x1D0, puartdisp->_messagetx, ptxtbuffer, strlen(ptxtbuffer), fontdata);
                    puartdisp->_messagestate = PENDING;
                }
            }
            else
            {
                puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
                if(puartdisp)
                {
                    uint8 ptxtbuffer[0x20];
                    memset(ptxtbuffer, 0x00, 0x20);
                    puartdisp->_messagelength = DisplayOutputString(0x0B8, 0x1D0, puartdisp->_messagetx, PSTRCAST(ptxtbuffer), 0x20, fontdata);
                    puartdisp->_messagestate = PENDING;
                }
            }
        }
        
        if(_g_dispenserlayoutinfo._inconfiguration)
        {
            puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
            if(puartdisp)
            {
                if(_g_dispenserlayoutinfo._currentloopopen)
                {
                    PSTR ptxtbuffer = "<<** LAZO DE CORRIENTE ABIERTO! **>>";
                    memcpy(puartdisp->_messagetx, ptxtbuffer, strlen(ptxtbuffer));
                    puartdisp->_messagelength = DisplayOutputString(0x0B8, 0x1D0, puartdisp->_messagetx, ptxtbuffer, strlen(ptxtbuffer), fontdata);
                }
                else
                {
                    PSTR ptxtbuffer = "Configurando, por favor espere.....";
                    memcpy(puartdisp->_messagetx, ptxtbuffer, strlen(ptxtbuffer));
                    puartdisp->_messagelength = DisplayOutputString(0x0B8, 0x1D0, puartdisp->_messagetx, ptxtbuffer, strlen(ptxtbuffer), fontdata);
                }
                puartdisp->_messagestate = PENDING;
            }
        }
        else if(!_g_configured[0x01])
        {
            puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
            if(puartdisp)
            {
                char8 txtbuffer[0x28];
                memset(txtbuffer, 0x20, 0x28);
                memcpy(puartdisp->_messagetx, txtbuffer, strlen(txtbuffer));
                if(_g_dispenserlayoutinfo._numpositions == 0x04)
                    sprintf(txtbuffer, "Welcome to INSEPET GVR PRIME QUATTRO!  ");
                else
                    sprintf(txtbuffer, "Welcome to INSEPET GVR PRIME/ENCORE DUPLA!   ");
                    
                puartdisp->_messagelength = DisplayOutputString(0x0A0, 0x1D0, puartdisp->_messagetx, txtbuffer, strlen(txtbuffer), fontdata);
                puartdisp->_messagestate = PENDING;
            }
            _g_configured[0x01] = true;
        }

    }
    
}

void InitiateUpdateDateTime(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay->_dispid == DISPLAY1)
    {
        PSINKMESSAGEPTR psinkmsg = AllocateMessageSlotConditional(DISPLAY1_UPDATE_DATETIME);
        if(psinkmsg)
        {
            pdisplay->_pdata = psinkmsg;
            psinkmsg->_pdata = pdisplay;
            psinkmsg->Callback = DrawDateTime;
            psinkmsg->_messageid = DISPLAY1_UPDATE_DATETIME;
            psinkmsg->_messagetype = FIRSTFOUNDFOREVER;
            psinkmsg->_messagetimeoutlimit = _SINK_TIMEOUT_1S_;
            psinkmsg->_messagestate = SINK_BUSY;
        }
    }
    else if(pdisplay->_dispid == DISPLAY2)
    {
        PSINKMESSAGEPTR psinkmsg = AllocateMessageSlotConditional(DISPLAY2_UPDATE_DATETIME);
        if(psinkmsg)
        {
            pdisplay->_pdata = psinkmsg;
            psinkmsg->_pdata = pdisplay;
            psinkmsg->Callback = DrawDateTime;
            psinkmsg->_messageid = DISPLAY2_UPDATE_DATETIME;
            psinkmsg->_messagetype = FIRSTFOUNDFOREVER;
            psinkmsg->_messagetimeoutlimit = _SINK_TIMEOUT_1S_;
            psinkmsg->_messagestate = SINK_BUSY;
        }
    }
}

void TerminateUpdateDateTime(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay->_dispid == DISPLAY1)
    {
        PSINKMESSAGEPTR psinkmsg = (PSINKMESSAGEPTR)pdisplay->_pdata;
        if(psinkmsg)
            psinkmsg->_selfkill = true;
    }
    else if(pdisplay->_dispid == DISPLAY2)
    {
        PSINKMESSAGEPTR psinkmsg = (PSINKMESSAGEPTR)pdisplay->_pdata;
        if(psinkmsg)
            psinkmsg->_selfkill = true;
    }
}

void InitiateHomeAnimation(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay->_dispid == DISPLAY1)
    {
        PSINKMESSAGEPTR psinkmsg = AllocateMessageSlotConditional(DISPLAY1_ANIMATE_HOME);
        if(psinkmsg)
        {
            _g_homeanimarray[DISPLAY1]._timeoutmultiplier = 0;
            
            pdisplay->_pdata = psinkmsg;
            psinkmsg->_pdata = pdisplay;
            psinkmsg->_messageid = DISPLAY1_ANIMATE_HOME;
            psinkmsg->_messagetype = FIRSTFOUNDFOREVER;
            psinkmsg->_messagetimeoutlimit = _SINK_TIMEOUT_100MS_;
            psinkmsg->_messagestate = SINK_BUSY;
            
        }
    }
    else if(pdisplay->_dispid == DISPLAY2)
    {
        PSINKMESSAGEPTR psinkmsg = AllocateMessageSlotConditional(DISPLAY2_ANIMATE_HOME);
        if(psinkmsg)
        {
            _g_homeanimarray[DISPLAY2]._timeoutmultiplier = 0;
            
            pdisplay->_pdata = psinkmsg;
            psinkmsg->_pdata = pdisplay;
            psinkmsg->_messageid = DISPLAY2_ANIMATE_HOME;
            psinkmsg->_messagetype = FIRSTFOUNDFOREVER;
            psinkmsg->_messagetimeoutlimit = _SINK_TIMEOUT_100MS_;
            psinkmsg->_messagestate = SINK_BUSY;

        }
    }
    //DrawHomeDateTime(pdisplay);
}

void DisplayUpdateDateTime(void *pparam)
{
    PSINKMESSAGEPTR psinkmsg = (PSINKMESSAGEPTR)pparam;
    if(psinkmsg->Callback)
        psinkmsg->Callback(psinkmsg->_pdata);
}

void TerminateHomeAnimation(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay->_dispid == DISPLAY1)
    {
        PSINKMESSAGEPTR psinkmsg = (PSINKMESSAGEPTR)pdisplay->_pdata;
        if(psinkmsg)
            psinkmsg->_selfkill = true;
    }
    else if(pdisplay->_dispid == DISPLAY2)
    {
        PSINKMESSAGEPTR psinkmsg = (PSINKMESSAGEPTR)pdisplay->_pdata;
        if(psinkmsg)
            psinkmsg->_selfkill = true;
    }
}

bool DisplayGenericValidator(LPVOID pparam)
{
    bool retval = false;
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    PSINKMESSAGEPTR psinkmsg = (PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink;

    uint8 pumpindex = GetPumpIndexFromDisplay(pparam);

    //If the current position isn't locked, then perform the check based on the RF link activity
    //to detect whether it is in "Autonomous Mode".
    if(!_g_pumps[pumpindex]._positionlocked)
    {
        //Means that there is no RF link so credit sales must be disabled
        #ifdef _ALLOW_CREDIT_SALE_TESTING_
        if(false)
        #endif
        #ifndef _ALLOW_CREDIT_SALE_TESTING_
        if(_g_rfautorun._state == RF_AUTONOMOUS)
        #endif
        {
            if(psinkmsg)
                if(psinkmsg->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SELECCIONE_POSICION)] != 0x00)
                {
                    uint8 paymodel = psinkmsg->_buffer[GetBufferIndexFromDisplayID(DISPLAY_FORMA_PAGO_DESEADA)];
                    if(paymodel == 0x0E || (pdisplay->_pcurrflow->_scrid == DISPLAY_FORMA_PAGO_DESEADA && pdisplay->_currscrcode == 0x0E))
                    {
                        retval = true;
                        pdisplay->_timeoutscrid = pdisplay->_statechangescrid = DISPLAY_NULL;
                        psinkmsg->_buffer[GetBufferIndexFromDisplayID(DISPLAY_FORMA_PAGO_DESEADA)] = 0x00;
                        psinkmsg->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SELECCIONE_POSICION)] = 0x00;
                        pdisplay->_nextscrid = DISPLAY_OPERACION_CANCELADA;
                    }
                }
        }
        
    }
    else
    {
        if(psinkmsg)
            if(psinkmsg->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SELECCIONE_POSICION)] != 0x00)
            {
                uint8 paymodel = psinkmsg->_buffer[GetBufferIndexFromDisplayID(DISPLAY_FORMA_PAGO_DESEADA)];
                if((paymodel != 0x45 && paymodel != 0x00) || (pdisplay->_pcurrflow->_scrid == DISPLAY_FORMA_PAGO_DESEADA && pdisplay->_currscrcode != 0x45))
                {
                    retval = _g_pumps[pumpindex]._positionlocked;
                    pdisplay->_timeoutscrid = pdisplay->_statechangescrid = DISPLAY_NULL;
                    psinkmsg->_buffer[GetBufferIndexFromDisplayID(DISPLAY_FORMA_PAGO_DESEADA)] = 0x00;
                    psinkmsg->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SELECCIONE_POSICION)] = 0x00;
                    pdisplay->_nextscrid = DISPLAY_CANCELADO_X_PC;
                }
            }
    }
        
    return retval;
    
}

void PrintersWorking(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    //Entering here means that both printers are ok
    if(pdisplay->_currscrcode == 0x39)
    {
        //This is the default printer port allocation
        _g_printerlayout._printerportside1 = PRINTER11_JOB;
        _g_printerlayout._printerportside2 = PRINTER21_JOB;
        
        I2CBusLock();
        ClearEepromBuffer();
        LoadEepromPage(EEPROM_CONFIGURATION_PAGE7);
        //Dispenser Pumps Offset is stored at position 0x09 in this memory area (see Eeprom.h)
        //Printer port #1
        GetEepromBuffer()[0x0B] = _g_printerlayout._printerportside1;
        //Printer port #2
        GetEepromBuffer()[0x0C] = _g_printerlayout._printerportside2;
        StoreEepromPage(EEPROM_CONFIGURATION_PAGE7);
        I2CBusUnlock();
    }
}

void EitherPrinterWorking(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    switch(pdisplay->_currscrcode)
    {
        case 0x8C://Printer #1 not working, then redirect printer jobs to printer #2
        {
            _g_printerlayout._printerportside1 = PRINTER21_JOB;
            _g_printerlayout._printerportside2 = PRINTER21_JOB;
            break;
        }
        case 0x8D://Printer #2 not working, then redirect printer jobs to printer #1
        {
            _g_printerlayout._printerportside1 = PRINTER11_JOB;
            _g_printerlayout._printerportside2 = PRINTER11_JOB;
            break;
        }
    }
    I2CBusLock();
    ClearEepromBuffer();
    LoadEepromPage(EEPROM_CONFIGURATION_PAGE7);
    //Dispenser Pumps Offset is stored at position 0x09 in this memory area (see Eeprom.h)
    //Printer port #1
    GetEepromBuffer()[0x0B] = _g_printerlayout._printerportside1;
    //Printer port #2
    GetEepromBuffer()[0x0C] = _g_printerlayout._printerportside2;
    StoreEepromPage(EEPROM_CONFIGURATION_PAGE7);
    I2CBusUnlock();
}

void CastCreditVolumeValue(char8 *pbuffer, uint8 buffersize)
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
    
    int8 shiftcount = (delimiterindex - _g_dispenserlayoutinfo._volumedecimals);
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

void SaveSelectedPosition(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    
    memset(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SELECCIONE_POSICION)], 0x00, GetBufferLengthFromDisplayID(DISPLAY_SELECCIONE_POSICION));
    ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SELECCIONE_POSICION)] = pdisplay->_currscrcode;
    ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SIDE_DUMMY_DISPLAY)] = pdisplay->_dispid;
}

//@Created By: HIJH
//@Septembre de 2016
void Display1Logic(void *pdata)
{
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pdata;
    
    ///////////////////////////////////////////////////////
    //TEMPORARILY DISABLED TO ALLOW FOR THE TESTS!!
    if(_g_dispenserlayoutinfo._inconfiguration)
        goto CLOSEDISPLAY1;
    ///////////////////////////////////////////////////////
    
    _g_display1._starting = false;

    _g_display1._currscrcode = ParseDisplayStream(pmsg->_buffer, pmsg->_buffersize);
    ParseDisplayFlow(&_g_display1);
    if(_g_display1._nextscrid != DISPLAY_NULL || 
        _g_display1._statechangescrid != DISPLAY_NULL ||
        _g_display1._timeoutscrid != DISPLAY_NULL)
    {
        //Here comes the validation for the state of the work shift 
        if(_g_display1.DisplayValidator)
            _g_display1.DisplayValidator(&_g_display1);
        
        bool validate = true;
        PNEAR_BYTE_PTR piterptr = &_g_dispvalcodes[0x00];
        while(*piterptr != 0xFF)
        {
            if(*piterptr == _g_display1._currscrcode)
            {
                validate = false;
                break;
            }
            piterptr++;
        }
        
        if(validate)
        {
            FPTRINPUTVALIDATOR fptrvalidator = DisplayFlowInputValidator(&_g_display1);
            if(fptrvalidator)
                if(!fptrvalidator(&_g_display1))
                {
                    _g_display1._timeoutscrid = _g_display1._statechangescrid = _g_display1._nextscrid = DISPLAY_NULL;
                    goto CLOSEDISPLAY1;
                }
                
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        //TODO: HERE WE NEED TO POST OR PROCESS THE INPUT BUFFER (CAPTURED THROUGH THE INPUT BOX IF EXISTS)
        //SINCE IT WILL BE CLEANED UP IN THE NEXT LINE
        //IT MUST BE POSTED THROUGH THE MESSAGE SINK TO THE BLL RELATED COMPONENT
        StoreTransactionData(&_g_display1);
        
        FPTRFINALIZER fptrfinalizer = DisplayFinalizer(&_g_display1);
        if(fptrfinalizer)
            fptrfinalizer(&_g_display1);
        
        PrepareDisplayStructure(&_g_display1);
        
        FPTRINITIALIZER fptrinitializer = DisplayInitializer(&_g_display1);
        if(fptrinitializer)
            fptrinitializer(&_g_display1);
        
        //This is a static structure for the display screen selection command
        char8 pbuffer[] = { 0xAA, 0x70, (_g_display1._pcurrflow->_scrid >> 8) & 0xFF, _g_display1._pcurrflow->_scrid & 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
        if(puartdisp)
        {
            puartdisp->_messagelength = sizeof(pbuffer) / sizeof(pbuffer[0]);
            memcpy(puartdisp->_messagetx, pbuffer, puartdisp->_messagelength);
            puartdisp->_messagestate = PENDING;
        }
        
        FPTRDECORATOR fptrdecorator = DisplayFlowDecorator(&_g_display1);
        if(fptrdecorator)
            fptrdecorator(&_g_display1);

    }
    else
    {
        //Input capture processing here for all of the displays that allow data input
        FPTRINPUTHANDLER fptrhandler = DisplayFlowInputHandler(&_g_display1);
        if(fptrhandler)
            fptrhandler(&_g_display1);
    }
    
    CLOSEDISPLAY1:
    Display1_ClearRxBuffer();
    Display1_Enable();
    return;
}
void PrintcopyReceipt(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    //HERE WE ATTEMPT TO PUSH THE TRANSACTION INTO THE PUMP'S TRANSACTIONAL QUEUE.
    //THIS IS DONE ONLY IF THERE IS ROOM IN THE QUEUE, OTHERWISE THIS STATE WON'T
    //BE REPORTED TO THE REMOTE PEER.
    PNEAR_PUMPPTR ppumpptr = &_g_pumps[GetPumpIndexFromDisplay(pdisplay)];
    if(ppumpptr)
    {
        ppumpptr->PumpTransQueueLock(ppumpptr);
        PNEAR_PUMPTRANSACTIONALSTATEPTR pts = ppumpptr->PumpTransQueueAllocate(ppumpptr);
        ppumpptr->PumpTransQueueUnlock(ppumpptr);
        if(pts)
        {
            uint8 index = 0;
            pts->_transtate = RF_MUX_COPY_REQUEST;
            //Pump ID always comes in the first position (0x00)
            pts->_buffer[index++] = ppumpptr->_pumpid;
      
            pts->_buffersize = index;

            ppumpptr->PumpTransQueueLock(ppumpptr);
            ppumpptr->PumpTransQueueEnqueue(ppumpptr, pts);
            ppumpptr->PumpTransQueueUnlock(ppumpptr);            
        }
    }        
}

//@Created By: HIJH
//@Septembre de 2016
void Display2Logic(void *pdata)
{
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pdata;

    ///////////////////////////////////////////////////////
    //TEMPORARILY DISABLED TO ALLOW TESTING OTHER FEATURES!!
    if(_g_dispenserlayoutinfo._inconfiguration)
        goto CLOSEDISPLAY2;
    ///////////////////////////////////////////////////////
    
    _g_display2._starting = false;
    
    _g_display2._currscrcode = ParseDisplayStream(pmsg->_buffer, pmsg->_buffersize);
    ParseDisplayFlow(&_g_display2);
    if(_g_display2._nextscrid != DISPLAY_NULL || 
        _g_display2._statechangescrid != DISPLAY_NULL ||
        _g_display2._timeoutscrid != DISPLAY_NULL)
    {
        //Here comes the validation for the state of the work shift 
        if(_g_display2.DisplayValidator)
            _g_display2.DisplayValidator(&_g_display2);
        
        bool validate = true;
        PNEAR_BYTE_PTR piterptr = &_g_dispvalcodes[0x00];
        while(*piterptr != 0xFF)
        {
            if(*piterptr == _g_display2._currscrcode)
            {
                validate = false;
                break;
            }
            piterptr++;
        }
        
        if(validate)
        {
            FPTRINPUTVALIDATOR fptrvalidator = DisplayFlowInputValidator(&_g_display2);
            if(fptrvalidator)
                if(!fptrvalidator(&_g_display2))
                {
                    _g_display2._timeoutscrid = _g_display2._statechangescrid = _g_display2._nextscrid = DISPLAY_NULL;
                    goto CLOSEDISPLAY2;
                }
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        //TODO: HERE WE NEED TO POST OR PROCESS THE INPUT BUFFER (CAPTURED THROUGH THE INPUT BOX IF EXISTS)
        //SINCE IT WILL BE CLEANED UP IN THE NEXT LINE
        //IT MUST BE POSTED THROUGH THE MESSAGE SINK TO THE BLL RELATED COMPONENT
        StoreTransactionData(&_g_display2);

        FPTRFINALIZER fptrfinalizer = DisplayFinalizer(&_g_display2);
        if(fptrfinalizer)
            fptrfinalizer(&_g_display2);
        
        PrepareDisplayStructure(&_g_display2);

        FPTRINITIALIZER fptrinitializer = DisplayInitializer(&_g_display2);
        if(fptrinitializer)
            fptrinitializer(&_g_display2);
        
        //This is a static structure for the display screen selection command
        char8 pbuffer[] = { 0xAA, 0x70, (_g_display2._pcurrflow->_scrid >> 8) & 0xFF, _g_display2._pcurrflow->_scrid & 0xFF, 0xCC, 0x33, 0xC3, 0x3C };
        PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
        if(puartdisp)
        {
            puartdisp->_messagelength = sizeof(pbuffer) / sizeof(pbuffer[0]);
            memcpy(puartdisp->_messagetx, pbuffer, puartdisp->_messagelength);
            puartdisp->_messagestate = PENDING;
        }
        
        FPTRDECORATOR fptrdecorator = DisplayFlowDecorator(&_g_display2);
        if(fptrdecorator)
            fptrdecorator(&_g_display2);

    }
    else
    {
        //Input capture processing here for all of the displays that allow data input
        FPTRINPUTHANDLER fptrhandler = DisplayFlowInputHandler(&_g_display2);
        if(fptrhandler)
            fptrhandler(&_g_display2);
    }
    CLOSEDISPLAY2:
    Display2_ClearRxBuffer();
    Display2_Enable();
    return;
}

/* [] END OF FILE */
