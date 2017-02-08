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
#include "Dispenser.h"
#include "UARTManager.h"
#include "SinkManager.h"
#include "SinkObservables.h"
#include "Printer.h"
#include "Display.h"
#include "Eeprom.h"
#include "Rtc.h"

bool _g_displaystarting = true;

DisplayCharacterMap _g_productbuttonmap[] = 
{
    { 0x80, 0x00 },
    { 0x81, 0x01 },
    { 0x7F, 0x02 },
    { 0xB8, 0x03 },
    { 0x00, 0x00 }
};

//@Created By: HIJH
//@Septembre de 2016
uint16 DisplayOutputString(uint16 xpos, uint16 ypos, char *pbuffer, char8 *pstring, uint16 buffersize, FontData fontdata)
{
    uint16 retval = 0x0000;
    if(pbuffer)
    {
        char8 pframedelimiters[] = { 0xAA, 0x98, 0xCC, 0x33, 0xC3, 0x3C };
        //Frame header + Command ID delimiters
        memcpy(&pbuffer[retval], &pframedelimiters[0], 2);
        retval += 2;
        
        //X position of the string MSB
        pbuffer[retval++] = (xpos >> 8) & 0xFF;
        //X position of the string LSB
        pbuffer[retval++] = xpos & 0xFF;
        //Y position of the string MSB
        pbuffer[retval++] = (ypos >> 8) & 0xFF;
        //Y position of the string LSB
        pbuffer[retval++] = ypos & 0xFF;
        
        //Transparent Background: 0x85
        //Opaque Background: 0xC5
        char8 punk[]  = { 0x20 | (fontdata._size & 0x0F), fontdata._opaquebackground, 0x02, (fontdata._forecolor >> 8) & 0xFF , fontdata._forecolor & 0xFF, (fontdata._backcolor >> 8) & 0xFF, fontdata._backcolor & 0xFF };
        memcpy(&pbuffer[retval], punk, sizeof(punk));
        retval += sizeof(punk);

        //Copying the string into the buffer
        memcpy(&pbuffer[retval], pstring, buffersize);
        retval += buffersize;
        //Frame foot delimiter
        memcpy(&pbuffer[retval], &pframedelimiters[2], 4);
        retval += 4;
    }
    return retval;
}

//@Created by: HIJH
//@Date: Septembre de 2016
//Parses the display flow matrix and determines which screen is next
//depending on the predefined conditions.
//It also checks on the current screen for either display, this parser determines whether the current 
//screen needs to change as consequence of an external state change. For instance, if a PUMP state changes
//maybe it requires that the display shows another different screen.
void ParseDisplayFlow(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    PDISPLAYFLOWPTR pdispflow = pdisplay->_preferenceflow;
    while(pdispflow->_scrid != DISPLAY_NULL)
    {        
        if(pdispflow->_scrid == pdisplay->_pcurrflow->_scrid && 
            pdispflow->_parentscrid == pdisplay->_pcurrflow->_parentscrid && 
            pdispflow->_parentcode == pdisplay->_pcurrflow->_parentcode)
        {
            FOR(uint8 index = 0, index < _MAX_DISPLAY_PATHS_, index++)
            {
                if(pdispflow->_pathcodes[index] == pdisplay->_currscrcode)
                {
                    pdisplay->_nextscrid = pdispflow->_pathnodes[index];
                    break;
                }
                if(pdispflow->_statecodes[index] == pdisplay->_currentstate)
                {
                    pdisplay->_statechangescrid = pdispflow->_statenodes[index];
                    break;
                }
            }
            break;
        }
        pdispflow++;
    }
}

//@Created by: HIJH
//@Date: Septembre de 2016
//This validator checks whether data has been entered into the InpupBox
FPTRINPUTVALIDATOR DisplayFlowInputValidator(void *pparam)
{
    FPTRINPUTVALIDATOR pretval = NULL;
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    PDISPLAYFLOWPTR pdispflow = pdisplay->_preferenceflow;
    while(pdispflow->_scrid != DISPLAY_NULL)
    {
        if(pdispflow->_scrid == pdisplay->_pcurrflow->_scrid && 
            pdispflow->_parentscrid == pdisplay->_pcurrflow->_parentscrid && 
            pdispflow->_parentcode == pdisplay->_pcurrflow->_parentcode)
        {
            pretval = pdispflow->_inputvalidator;
            break;
        }
        pdispflow++;
    }
    return pretval;
}

//@Created by: HIJH
//@Date: Septembre de 2016
//This is the core handler for the input processing. It manages all the incoming codes from the screen
//and map them to the valid ASCII. It also validates the inputbox size and manages the cleanup.
FPTRINPUTHANDLER DisplayFlowInputHandler(void *pparam)
{
    FPTRINPUTHANDLER pretval = NULL;
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    PDISPLAYFLOWPTR pdispflow = pdisplay->_preferenceflow;    
    while(pdispflow->_scrid != DISPLAY_NULL)
    {
        if(pdispflow->_scrid == pdisplay->_pcurrflow->_scrid && 
            pdispflow->_parentscrid == pdisplay->_pcurrflow->_parentscrid && 
            pdispflow->_parentcode == pdisplay->_pcurrflow->_parentcode)
        {
            pretval = pdispflow->_inputhandler;
            break;
        }
        pdispflow++;
    }
    return pretval;
}

//@Created by: HIJH
//@Date: Septembre de 2016
//Retrieves the Decorator for the currently selected screen
FPTRDECORATOR DisplayFlowDecorator(void *pparam)
{
    FPTRDECORATOR pretval = NULL;
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    PDISPLAYFLOWPTR pdispflow = pdisplay->_preferenceflow;
    while(pdispflow->_scrid != DISPLAY_NULL)
    {
        if(pdispflow->_scrid == pdisplay->_pcurrflow->_scrid && 
            pdispflow->_parentscrid == pdisplay->_pcurrflow->_parentscrid && 
            pdispflow->_parentcode == pdisplay->_pcurrflow->_parentcode)
        {
            pretval = pdispflow->_decorator;
            break;
        }
        pdispflow++;
    }
    return pretval;
}

//@Created by: HIJH
//@Date: Septembre de 2016
//Retrieves the Initializer for the currently selected screen
FPTRINITIALIZER DisplayInitializer(void *pparam)
{
    FPTRINITIALIZER pretval = NULL;
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    PDISPLAYFLOWPTR pdispflow = pdisplay->_preferenceflow;
    while(pdispflow->_scrid != DISPLAY_NULL)
    {
        if(pdispflow->_scrid == pdisplay->_pcurrflow->_scrid && 
            pdispflow->_parentscrid == pdisplay->_pcurrflow->_parentscrid && 
            pdispflow->_parentcode == pdisplay->_pcurrflow->_parentcode)
        {
            pretval = pdispflow->_initializer;
            break;
        }
        pdispflow++;
    }
    return pretval;
}

//@Created by: HIJH
//@Date: Septembre de 2016
//Retrieves the Finalizer for the currently selected screen
FPTRFINALIZER DisplayFinalizer(void *pparam)
{
    FPTRFINALIZER pretval = NULL;
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    PDISPLAYFLOWPTR pdispflow = pdisplay->_preferenceflow;
    while(pdispflow->_scrid != DISPLAY_NULL)
    {
        if(pdispflow->_scrid == pdisplay->_pcurrflow->_scrid && 
            pdispflow->_parentscrid == pdisplay->_pcurrflow->_parentscrid && 
            pdispflow->_parentcode == pdisplay->_pcurrflow->_parentcode)
        {
            pretval = pdispflow->_finalizer;
            break;
        }
        pdispflow++;
    }
    return pretval;
}

char8 ParseDisplayStream(char* pbuffer, uint8 buffersize)
{
    char8 retval = 0x00;
    char8 pdelimiter[] = { 0xCC, 0x33, 0xC3, 0x3C };
    uint16 delimitersize = sizeof(pdelimiter);
    
    if((pbuffer[buffersize - 1] == pdelimiter[delimitersize - 1]) && 
        (pbuffer[buffersize - 2] == pdelimiter[delimitersize - 2]) &&
        (pbuffer[buffersize - 3] == pdelimiter[delimitersize - 3]) && 
        (pbuffer[buffersize - 4] == pdelimiter[delimitersize - 4]))
        retval = pbuffer[buffersize - 5];
    
    return retval;
}

//@Created by: HIJH
//@Date: Septembre de 2016
//Retrieves the selected DisplayFlow node from the DisplayFlow Matrix matching the correct parent
DisplayFlow *GetDisplayFlowNode(void *pparam)
{
    PDISPLAYFLOWPTR retval = NULL;
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    PDISPLAYFLOWPTR pdispflow = pdisplay->_preferenceflow;
    
    uint8 scrid = (pdisplay->_nextscrid == 0xFF)?  ((pdisplay->_statechangescrid == 0xFF)? pdisplay->_timeoutscrid: pdisplay->_statechangescrid) : pdisplay->_nextscrid;
    uint8 parcode = pdisplay->_currscrcode;
    uint8 parscrid = pdisplay->_pcurrflow->_scrid;
    
    while(pdispflow->_scrid != DISPLAY_NULL)
    {
        //Retrieves the requested screen by matching the parent. If the screen has no parent defined (0xFF) means
        //means that the requested screen must be the root screen or has no parent rule applied
        if((scrid == pdispflow->_scrid) && 
            ((parscrid == pdispflow->_parentscrid) || (0xFF == pdispflow->_parentscrid)) && 
            ((parcode == pdispflow->_parentcode) || (0xFF == pdispflow->_parentcode) || (0xFF == pdispflow->_parentscrid)))
        {
            retval = pdispflow;
            break;
        }
        pdispflow++;
    }
    return retval;
}

//@Created by: HIJH
//@Date: Septembre de 2016
//Further cleanup before initiation the display flow
void PrepareDisplayStructure(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    
    pdisplay->_pcurrflow = GetDisplayFlowNode(pdisplay);
    pdisplay->_nextscrid = DISPLAY_NULL;
    pdisplay->_statechangescrid  = DISPLAY_NULL;
    pdisplay->_currentstate  = 0x00;
    pdisplay->_timeoutscrid = DISPLAY_NULL;
    pdisplay->_currscrcode = 0x00;
    pdisplay->_timeoutmultiplier = 0;
    memset(pdisplay->_bufferinfo._buffer, 0x00, _DISPLAY_BUFFER_SIZE_);
    //This setting only applies if the DisplayFlow item has been defined to timeout
    pdisplay->_timeoutreftick = Timer_Tick_ReadCounter();
    pdisplay->_bufferinfo._bufferindex = 0;
    memset(pdisplay->_bufferinfo._buffer, 0x00, _DISPLAY_BUFFER_SIZE_);

}

//@Created by: HIJH
//@Date: Septembre de 2016
//This method validates that the capture buffer for the current display
//has received the required information and is not empty
bool NonEmptyDataValidator(void *pparam)
{
    bool retval = false;
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    if(pdisplay->_bufferinfo._bufferindex > 0)
        retval = true;
    
    return retval;
}

//@Created by: HIJH
//@Date: Septembre de 2016
//This method validates that the capture buffer for the current display
//has received the required information and is not empty
bool SecureConfigurationValidator(void *pparam)
{
    bool retval = false;
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    if(pdisplay->_bufferinfo._bufferindex > 0)
    {
        retval = true;
        const char* StringPass = "1234";
        ClearEepromBuffer();
        LoadEepromPage(EEPROM_CONFIGURATION_PAGE7);
        //This EEPROM's page location is used to store the four digit ASCII password
        if(!strncmp(pdisplay->_bufferinfo._buffer, StringPass, 0x04))
        {
            if(pdisplay->_bufferinfo._buffer[0x04] == 0x2C)
            {
                pdisplay->_currscrcode = 0x1E;
                pdisplay->_nextscrid = DISPLAY_NUEVO_PASSWORD;
            }
            else
            {
                pdisplay->_currscrcode = 0x1D;
                pdisplay->_nextscrid = DISPLAY_PASSWORD_VALIDO;
            }
        }
    }
    
    return retval;
}

//@Created by: HIJH
//@Date: Septembre de 2016
void SaveNewPassword(void *pparam)
{
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    if(pdisplay->_bufferinfo._bufferindex > 0)
    {
        ClearEepromBuffer();
        LoadEepromPage(EEPROM_CONFIGURATION_PAGE7);
        memcpy(&GetEepromBuffer()[0x0D], pdisplay->_bufferinfo._buffer, 0x04);
        StoreEepromPage(EEPROM_CONFIGURATION_PAGE7);
    }
}

//@Created by: HIJH
//@Date: Septembre de 2016
//This method validates that the capture buffer for the current display
//has received the required information and is not empty
bool NonEmptyMoneyValidator(void *pparam)
{
    bool retval = false;
    char8 tempbuffer[_DISPLAY_BUFFER_SIZE_];
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    
    if(pdisplay->_bufferinfo._bufferindex > 0)
        retval = true;
    else
        retval = false;    
    
    if(retval)
    {
        uint8 remplaces = _DISPLAY_BUFFER_SIZE_ - pdisplay->_bufferinfo._bufferindex;
        memcpy(tempbuffer, pdisplay->_bufferinfo._buffer, _DISPLAY_BUFFER_SIZE_);
        FOR(int8 index = pdisplay->_bufferinfo._bufferindex, index < _DISPLAY_BUFFER_SIZE_, index++)
            tempbuffer[index] = 0x00;
        
        FOR(index = (pdisplay->_bufferinfo._bufferindex - 1), index >= 0, index--)
        {
            tempbuffer[index + remplaces] = tempbuffer[index] & 0x0F;
            tempbuffer[index] = 0x00;
        }    
        CastMoneyValue(tempbuffer, _DISPLAY_BUFFER_SIZE_);
        PSTRBUFFTOGG((PNEAR_BYTE_PTR)tempbuffer, _DISPLAY_BUFFER_SIZE_);
        uint32 datavalue = LSDBCDBUFF2HEX((PNEAR_BYTE_PTR)tempbuffer, _DISPLAY_BUFFER_SIZE_);
        
        I2CBusLock();
        ClearEepromBuffer();
        LoadEepromPage(EEPROM_CONFIGURATION_PAGE5);
        uint32 refvalue = LSDBCDBUFF2HEX((PNEAR_BYTE_PTR)&GetEepromBuffer()[0x02], 0x10);                
        I2CBusUnlock();
        
        if(datavalue < refvalue)
            retval = false;
    }
        
    return retval;
}

//@Created by: HIJH
//@Date: Septembre de 2016
//This method validates that the capture buffer for the current display
//has received the required information and is not empty
bool NonEmptyVolumeValidator(void *pparam)
{
    bool retval = false;
    char8 tempbuffer[_DISPLAY_BUFFER_SIZE_];
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    
    if(pdisplay->_bufferinfo._bufferindex > 0)
        retval = true;
    else
        retval = false;    
    
    if(retval)
    {
        uint8 remplaces = _DISPLAY_BUFFER_SIZE_ - pdisplay->_bufferinfo._bufferindex;
        memcpy(tempbuffer, pdisplay->_bufferinfo._buffer, _DISPLAY_BUFFER_SIZE_);
        FOR(int8 index = pdisplay->_bufferinfo._bufferindex, index < _DISPLAY_BUFFER_SIZE_, index++)
            tempbuffer[index] = 0x00;
        
        FOR(index = (pdisplay->_bufferinfo._bufferindex - 1), index >= 0, index--)
        {
            tempbuffer[index + remplaces] = tempbuffer[index] & 0x0F;
            tempbuffer[index] = 0x00;
        }    
        CastVolumeValue(tempbuffer, _DISPLAY_BUFFER_SIZE_);
        PSTRBUFFTOGG((PNEAR_BYTE_PTR)tempbuffer, _DISPLAY_BUFFER_SIZE_);
        uint32 datavalue = LSDBCDBUFF2HEX((PNEAR_BYTE_PTR)tempbuffer, _DISPLAY_BUFFER_SIZE_);
        
        I2CBusLock();
        ClearEepromBuffer();
        LoadEepromPage(EEPROM_CONFIGURATION_PAGE6);
        uint32 refvalue = LSDBCDBUFF2HEX((PNEAR_BYTE_PTR)&GetEepromBuffer()[0x00], 0x10);                
        I2CBusUnlock();
        
        if(datavalue < refvalue)
            retval = false;
    }
    
    return retval;
}

//@Created by: HIJH
//@Date: Septembre de 2016
//This method validates that the capture buffer for the current display
//has received a fifteen modulo value
bool NonEmpty15ModuloValidator(void *pparam)
{
    bool retval = false;
    char8 tempbuffer[_DISPLAY_BUFFER_SIZE_];
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    
    if(pdisplay->_bufferinfo._bufferindex > 0)
        retval = true;
    else
        retval = false;    
    
    if(retval)
    {
        uint8 remplaces = _DISPLAY_BUFFER_SIZE_ - pdisplay->_bufferinfo._bufferindex;
        memcpy(tempbuffer, pdisplay->_bufferinfo._buffer, _DISPLAY_BUFFER_SIZE_);
        FOR(int8 index = pdisplay->_bufferinfo._bufferindex, index < _DISPLAY_BUFFER_SIZE_, index++)
            tempbuffer[index] = 0x00;
        
        FOR(index = (pdisplay->_bufferinfo._bufferindex - 1), index >= 0, index--)
        {
            tempbuffer[index + remplaces] = tempbuffer[index] & 0x0F;
            tempbuffer[index] = 0x00;
        }    
        PSTRBUFFTOGG((PNEAR_BYTE_PTR)tempbuffer, _DISPLAY_BUFFER_SIZE_);
        uint32 datavalue = LSDBCDBUFF2HEX((PNEAR_BYTE_PTR)tempbuffer, _DISPLAY_BUFFER_SIZE_);
                
        if((datavalue % 15) != 0)
            retval = false;
    }
        
    return retval;
}

bool ValidatePumpAvailability4Positions(void *pparam)
{
    bool retval = true;
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    if(_g_dispenserlayoutinfo._numpositions == 0x04)
    {
        if(pdisplay->_dispid == DISPLAY1)
        {
            switch(pdisplay->_currscrcode)
            {
                case POSITIONA://PUMP #1
                retval = (((_g_pumps[0x00]._pumpstate == PUMP_IDLE) || (_g_pumps[0x00]._pumpstate == PUMP_CALLING)) && !_g_pumps[0x00]._pumplocked);
                break;
                case POSITIONB://PUMP #3
                retval = (((_g_pumps[0x02]._pumpstate == PUMP_IDLE) || (_g_pumps[0x02]._pumpstate == PUMP_CALLING)) && !_g_pumps[0x02]._pumplocked);
                break;
            }
            
            retval &= !FindSinkMessage(_g_printerlayout._printerportside1);
        }
        else if(pdisplay->_dispid == DISPLAY2)
        {
            switch(pdisplay->_currscrcode)
            {
                case POSITIONA://PUMP #4
                retval = (((_g_pumps[0x03]._pumpstate == PUMP_IDLE) || (_g_pumps[0x03]._pumpstate == PUMP_CALLING)) && !_g_pumps[0x03]._pumplocked);
                break;
                case POSITIONB://PUMP #2
                retval = (((_g_pumps[0x01]._pumpstate == PUMP_IDLE) || (_g_pumps[0x01]._pumpstate == PUMP_CALLING)) && !_g_pumps[0x01]._pumplocked);
                break;
            }
            
            retval &= !FindSinkMessage(_g_printerlayout._printerportside2);
        }
    }
    
    return retval;
}

bool ValidatePumpAvailability21Positions(void *pparam)
{
    bool retval = true;
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    if(_g_dispenserlayoutinfo._numpositions == 0x02)
    {
        if(pdisplay->_dispid == DISPLAY1)
        {
            retval = (((_g_pumps[0x00]._pumpstate == PUMP_IDLE) || (_g_pumps[0x00]._pumpstate == PUMP_CALLING)) && !_g_pumps[0x00]._pumplocked);
            retval &= !FindSinkMessage(_g_printerlayout._printerportside1);
        }
        else if(pdisplay->_dispid == DISPLAY2)
        {
            retval = (((_g_pumps[0x01]._pumpstate == PUMP_IDLE) || (_g_pumps[0x01]._pumpstate == PUMP_CALLING)) && !_g_pumps[0x01]._pumplocked);
            retval &= !FindSinkMessage(_g_printerlayout._printerportside2);
        }
    }
    else if(_g_dispenserlayoutinfo._numpositions == 0x01)
    {
        if(pdisplay->_dispid == DISPLAY1)
        {
            retval = (((_g_pumps[0x00]._pumpstate == PUMP_IDLE) || (_g_pumps[0x00]._pumpstate == PUMP_CALLING)) && !_g_pumps[0x00]._pumplocked);
            retval &= !FindSinkMessage(_g_printerlayout._printerportside1);
        }
    }
    
    return retval;
}

void GetPumpsAvailability(void *pparam, uint8 *pvalues)
{
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    if(_g_dispenserlayoutinfo._numpositions == 0x04)
    {
        if(pdisplay->_dispid == DISPLAY1)
        {
            pvalues[0x00] = (((_g_pumps[0x00]._pumpstate == PUMP_IDLE) || (_g_pumps[0x00]._pumpstate == PUMP_CALLING)) && !_g_pumps[0x00]._pumplocked);
            pvalues[0x01] = (((_g_pumps[0x02]._pumpstate == PUMP_IDLE) || (_g_pumps[0x02]._pumpstate == PUMP_CALLING)) && !_g_pumps[0x02]._pumplocked);
            
        }
        else if(pdisplay->_dispid == DISPLAY2)
        {
            pvalues[0x00] = (((_g_pumps[0x03]._pumpstate == PUMP_IDLE) || (_g_pumps[0x03]._pumpstate == PUMP_CALLING)) && !_g_pumps[0x03]._pumplocked);
            pvalues[0x01] = (((_g_pumps[0x01]._pumpstate == PUMP_IDLE) || (_g_pumps[0x01]._pumpstate == PUMP_CALLING)) && !_g_pumps[0x01]._pumplocked);
            
        }
    }
    else if(_g_dispenserlayoutinfo._numpositions == 0x02)
    {
        if(pdisplay->_dispid == DISPLAY1)
        {
            pvalues[0x00] = (((_g_pumps[0x00]._pumpstate == PUMP_IDLE) || (_g_pumps[0x00]._pumpstate == PUMP_CALLING)) && !_g_pumps[0x00]._pumplocked);
            pvalues[0x01] = (((_g_pumps[0x00]._pumpstate == PUMP_IDLE) || (_g_pumps[0x00]._pumpstate == PUMP_CALLING)) && !_g_pumps[0x00]._pumplocked);
            
        }
        else if(pdisplay->_dispid == DISPLAY2)
        {
            pvalues[0x00] = (((_g_pumps[0x01]._pumpstate == PUMP_IDLE) || (_g_pumps[0x01]._pumpstate == PUMP_CALLING)) && !_g_pumps[0x01]._pumplocked);
            pvalues[0x01] = (((_g_pumps[0x01]._pumpstate == PUMP_IDLE) || (_g_pumps[0x01]._pumpstate == PUMP_CALLING)) && !_g_pumps[0x01]._pumplocked);
            
        }
    }
    else if(_g_dispenserlayoutinfo._numpositions == 0x01)
    {
        if(pdisplay->_dispid == DISPLAY1)
        {
            pvalues[0x00] = (((_g_pumps[0x00]._pumpstate == PUMP_IDLE) || (_g_pumps[0x00]._pumpstate == PUMP_CALLING)) && !_g_pumps[0x00]._pumplocked);
            pvalues[0x01] = (((_g_pumps[0x00]._pumpstate == PUMP_IDLE) || (_g_pumps[0x00]._pumpstate == PUMP_CALLING)) && !_g_pumps[0x00]._pumplocked);
            
        }
    }
}

void GetPumpsPrintState(void *pparam, bool *pvalues)
{
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    if(_g_dispenserlayoutinfo._numpositions == 0x04)
    {
        if(pdisplay->_dispid == DISPLAY1)
        {
            pvalues[0x00] = _g_pumps[0x00]._print;
            pvalues[0x01] = _g_pumps[0x02]._print;
            
        }
        else if(pdisplay->_dispid == DISPLAY2)
        {
            pvalues[0x00] = _g_pumps[0x03]._print;
            pvalues[0x01] = _g_pumps[0x01]._print;
            
        }
    }
    else if(_g_dispenserlayoutinfo._numpositions == 0x02)
    {
        if(pdisplay->_dispid == DISPLAY1)
        {
            pvalues[0x00] = _g_pumps[0x00]._print;
            pvalues[0x01] = _g_pumps[0x00]._print;
            
        }
        else if(pdisplay->_dispid == DISPLAY2)
        {
            pvalues[0x00] = _g_pumps[0x01]._print;
            pvalues[0x01] = _g_pumps[0x01]._print;
            
        }
    }
    else if(_g_dispenserlayoutinfo._numpositions == 0x01)
    {
        if(pdisplay->_dispid == DISPLAY1)
        {
            pvalues[0x00] = _g_pumps[0x00]._print;
            pvalues[0x01] = _g_pumps[0x00]._print;
            
        }
    }
}

//@Created by: HIJH
//@Date: Septembre de 2016
//This method handles the selected product on a credit sale
void DisplayProductSelectionInputHandler(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay)
    {
        uint8  numhoses = 0x00;
        PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
        switch(GetPumpIndexFromDisplay(pdisplay))
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
        DisplayCharacterMap *pdispcharmap = _g_productbuttonmap;
        while(pdispcharmap->_refchar != 0x00)
        {
            if((pdispcharmap->_refchar == pdisplay->_currscrcode) && ((pdispcharmap->_mappedto + 1) <= numhoses))
            {
                ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SUBA_MANIJA)] = pdispcharmap->_mappedto;
                pdisplay->_currentstate = DISPLAY_HOSE_ACTIVATED;
                break;
            }
            pdispcharmap++;
        }
    }
}

//@Created by: HIJH
//@Date: Septembre de 2016
//This method handles the input data typed 
//in the screen
void DisplayInputHandler(void *pparam)
{
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    if(pdisplay)
    {
        char8 mappedchar = 0x00;
        //This is since 0x0B has been defined as "Cancel" in the keypad
        if(pdisplay->_currscrcode == 0x0B)
            pdisplay->_bufferinfo._bufferindex = 0;
        else
            mappedchar = UnmapCode(pdisplay->_currscrcode);
        
        if(pdisplay->_bufferinfo._bufferindex == 0)
        {
            pdisplay->_bufferinfo._xpos = pdisplay->_pcurrflow->_inputboxinfo._xdefpos;
            pdisplay->_bufferinfo._ypos = pdisplay->_pcurrflow->_inputboxinfo._ydefpos;
            memset(pdisplay->_bufferinfo._buffer, ' ', _DISPLAY_BUFFER_SIZE_);
        }
        //The InputBox is already filled up!
        if(pdisplay->_bufferinfo._bufferindex >= pdisplay->_pcurrflow->_inputboxinfo._maxchars)
            return;
        
        //There's nothing new to show up since the unmapper didn't find anything to map to
        if(mappedchar != 0x00)
            pdisplay->_bufferinfo._buffer[pdisplay->_bufferinfo._bufferindex++] = mappedchar;
        
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

void DisplayIDInputHandler(void *pparam)
{
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    if(pdisplay)
    {
        char8 mappedchar = 0x00;
        //This is since 0x0B has been defined as "Cancel" in the keypad
        if(pdisplay->_currscrcode == 0x0B)
            pdisplay->_bufferinfo._bufferindex = 0;
        else
        {
            mappedchar = UnmapCode(pdisplay->_currscrcode);
            
            //No zeros allowed as left most significant digits
            if((mappedchar < 0x30 || mappedchar > 0x39) && (pdisplay->_bufferinfo._bufferindex == 0))
                return;
        }
        
        if(pdisplay->_bufferinfo._bufferindex == 0)
        {
            pdisplay->_bufferinfo._xpos = pdisplay->_pcurrflow->_inputboxinfo._xdefpos;
            pdisplay->_bufferinfo._ypos = pdisplay->_pcurrflow->_inputboxinfo._ydefpos;
            memset(pdisplay->_bufferinfo._buffer, ' ', _DISPLAY_BUFFER_SIZE_);
        }
        //The InputBox is already filled up!
        if(pdisplay->_bufferinfo._bufferindex >= pdisplay->_pcurrflow->_inputboxinfo._maxchars)
            return;
        
        //There's nothing new to show up since the unmapper didn't find anything to map to
        if(mappedchar != 0x00)
            pdisplay->_bufferinfo._buffer[pdisplay->_bufferinfo._bufferindex++] = mappedchar;
        
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

//@Created by: HIJH
//@Date: Septembre de 2016
//This method handles numeric input data typed in a InputBox
void DisplayNumericInputHandler(void *pparam)
{
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    if(pdisplay)
    {
        char8 mappedchar = 0x00;
        //This is since 0x0B has been defined as "Cancel" in the keypad
        if(pdisplay->_currscrcode == 0x0B)
        {
            pdisplay->_bufferinfo._bufferindex = 0;
            memset(pdisplay->_bufferinfo._buffer, 0x00, _DISPLAY_BUFFER_SIZE_);
        }
        else
        {
            mappedchar = UnmapCode(pdisplay->_currscrcode);
            
            //No zeros allowed as left most significant digits
            if((mappedchar <= 0x30 || mappedchar > 0x39) && (pdisplay->_bufferinfo._bufferindex == 0))
                return;
            
            //Just one decimal delimiter allowed
            if(strchr(pdisplay->_bufferinfo._buffer, (char8)',') && (mappedchar == (char8)','))
                return;

            //Just _MAX_DECIMALS_ decimals allowed
            char tempbuffer[16];
            memset(tempbuffer, 0x00, 16);
            memcpy(tempbuffer, pdisplay->_bufferinfo._buffer, pdisplay->_bufferinfo._bufferindex);
            char *ptrpos = strchr(tempbuffer, (char8)',');
            if(ptrpos)
                if(strlen(ptrpos) > _MAX_DECIMALS_)
                    return;
        }
        
        if(pdisplay->_bufferinfo._bufferindex == 0)
        {
            pdisplay->_bufferinfo._xpos = pdisplay->_pcurrflow->_inputboxinfo._xdefpos;
            pdisplay->_bufferinfo._ypos = pdisplay->_pcurrflow->_inputboxinfo._ydefpos;
            memset(pdisplay->_bufferinfo._buffer, ' ', _DISPLAY_BUFFER_SIZE_);
        }
        //The InputBox is already filled up!
        if(pdisplay->_bufferinfo._bufferindex >= pdisplay->_pcurrflow->_inputboxinfo._maxchars)
            return;
        
        //There's nothing new to show up since the unmapper didn't find anything to map to
        if(mappedchar != 0x00)
            pdisplay->_bufferinfo._buffer[pdisplay->_bufferinfo._bufferindex++] = mappedchar;
        
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

//@Created by: HIJH
//@Date: Septembre de 2016
//This method handles numeric input data typed in a InputBox
void DisplayIntegerInputHandler(void *pparam)
{
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    if(pdisplay)
    {
        char8 mappedchar = 0x00;
        //This is since 0x0B has been defined as "Cancel" in the keypad
        if(pdisplay->_currscrcode == 0x0B)
            pdisplay->_bufferinfo._bufferindex = 0;
        else
        {
            mappedchar = UnmapCode(pdisplay->_currscrcode);
            
            //No zeros allowed as left most significant digits
            if((mappedchar <= 0x30 || mappedchar > 0x39) && (pdisplay->_bufferinfo._bufferindex == 0))
                return;
            
            //Just one decimal delimiter allowed
            if(mappedchar == (char8)',')
                return;

        }
        
        if(pdisplay->_bufferinfo._bufferindex == 0)
        {
            pdisplay->_bufferinfo._xpos = pdisplay->_pcurrflow->_inputboxinfo._xdefpos;
            pdisplay->_bufferinfo._ypos = pdisplay->_pcurrflow->_inputboxinfo._ydefpos;
            memset(pdisplay->_bufferinfo._buffer, ' ', _DISPLAY_BUFFER_SIZE_);
        }
        //The InputBox is already filled up!
        if(pdisplay->_bufferinfo._bufferindex >= pdisplay->_pcurrflow->_inputboxinfo._maxchars)
            return;
        
        //There's nothing new to show up since the unmapper didn't find anything to map to
        if(mappedchar != 0x00)
            pdisplay->_bufferinfo._buffer[pdisplay->_bufferinfo._bufferindex++] = mappedchar;
        
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

//@Created by: HIJH
//@Date: Septembre de 2016
//This method handles numeric input data typed in a InputBox
void DisplayMoneyInputHandler(void *pparam)
{
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    if(pdisplay)
    {
        char8 mappedchar = 0x00;
        //This is since 0x0B has been defined as "Cancel" in the keypad
        if(pdisplay->_currscrcode == 0x0B)
        {
            pdisplay->_bufferinfo._bufferindex = 0;
            memset(pdisplay->_bufferinfo._buffer, 0x00, _DISPLAY_BUFFER_SIZE_);
        }
        else
        {
            mappedchar = UnmapCode(pdisplay->_currscrcode);
            
            //No zeros allowed as left most significant digits
            if((mappedchar <= 0x30 || mappedchar > 0x39) && (pdisplay->_bufferinfo._bufferindex == 0))
                return;
            
            //Just one decimal delimiter allowed
            if(strchr(pdisplay->_bufferinfo._buffer, (char8)',') && (mappedchar == (char8)','))
                return;

            //If no decimals are allowed, just return
            if((_g_dispenserlayoutinfo._moneydecimals == 0) && (mappedchar == (char8)','))
                return;
                        
            char tempbuffer[16];
            memset(tempbuffer, 0x00, 16);
            memcpy(tempbuffer, pdisplay->_bufferinfo._buffer, pdisplay->_bufferinfo._bufferindex);
            char *ptrpos = strchr(tempbuffer, (char8)',');
            if(ptrpos)
                if(strlen(ptrpos) > 0)//_g_dispenserlayoutinfo._moneydecimals)
                    return;
        }
        
        if(pdisplay->_bufferinfo._bufferindex == 0)
        {
            pdisplay->_bufferinfo._xpos = pdisplay->_pcurrflow->_inputboxinfo._xdefpos;
            pdisplay->_bufferinfo._ypos = pdisplay->_pcurrflow->_inputboxinfo._ydefpos;
            memset(pdisplay->_bufferinfo._buffer, ' ', _DISPLAY_BUFFER_SIZE_);
        }
        //The InputBox is already filled up!
        if(pdisplay->_bufferinfo._bufferindex >= pdisplay->_pcurrflow->_inputboxinfo._maxchars || pdisplay->_bufferinfo._bufferindex >= _g_dispenserlayoutinfo._displaydigitsmode)
            return;
        
        //There's nothing new to show up since the unmapper didn't find anything to map to
        if(mappedchar != 0x00)
            pdisplay->_bufferinfo._buffer[pdisplay->_bufferinfo._bufferindex++] = mappedchar;
        
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

//@Created by: HIJH
//@Date: Septembre de 2016
//This method handles numeric input data typed in a InputBox
void DisplayVolumeInputHandler(void *pparam)
{
    DisplayLayout *pdisplay = (DisplayLayout*)pparam;
    if(pdisplay)
    {
        char8 mappedchar = 0x00;
        //This is since 0x0B has been defined as "Cancel" in the keypad
        if(pdisplay->_currscrcode == 0x0B)
        {
            pdisplay->_bufferinfo._bufferindex = 0;
            memset(pdisplay->_bufferinfo._buffer, 0x00, _DISPLAY_BUFFER_SIZE_);
        }
        else
        {
            mappedchar = UnmapCode(pdisplay->_currscrcode);
            
            //No zeros allowed as left most significant digits
            if((mappedchar < 0x30 || mappedchar > 0x39) && (pdisplay->_bufferinfo._bufferindex == 0))
                return;
            
            //Just one decimal delimiter allowed
            if(strchr(pdisplay->_bufferinfo._buffer, (char8)',') && (mappedchar == (char8)','))
                return;

            //Just one zero before the decimal delimiter allowed
            if(pdisplay->_bufferinfo._buffer[0] == 0x30 && (mappedchar == (char8)',' && pdisplay->_bufferinfo._buffer[0x01] == (char8)','))
                return;
            
            //If no decimals allowed just return
            if((_g_dispenserlayoutinfo._volumedecimals == 0) && (mappedchar == (char8)','))
                return;
            
            char tempbuffer[16];
            memset(tempbuffer, 0x00, 16);
            memcpy(tempbuffer, pdisplay->_bufferinfo._buffer, pdisplay->_bufferinfo._bufferindex);
            char *ptrpos = strchr(tempbuffer, (char8)',');
            if(ptrpos)
                if(strlen(ptrpos) > ((_g_dispenserlayoutinfo._displaydigitsmode == 0x06)?_g_dispenserlayoutinfo._volumedecimals - 1: _g_dispenserlayoutinfo._volumedecimals))
                    return;
            
            
        }
        
        if(pdisplay->_bufferinfo._bufferindex == 0)
        {
            pdisplay->_bufferinfo._xpos = pdisplay->_pcurrflow->_inputboxinfo._xdefpos;
            pdisplay->_bufferinfo._ypos = pdisplay->_pcurrflow->_inputboxinfo._ydefpos;
            memset(pdisplay->_bufferinfo._buffer, ' ', _DISPLAY_BUFFER_SIZE_);
        }
        //The InputBox is already filled up!
        if(pdisplay->_bufferinfo._bufferindex >= pdisplay->_pcurrflow->_inputboxinfo._maxchars || pdisplay->_bufferinfo._bufferindex >= (_g_dispenserlayoutinfo._displaydigitsmode - 1))
            return;
        
        //There's nothing new to show up since the unmapper didn't find anything to map to
        if(mappedchar != 0x00)
            pdisplay->_bufferinfo._buffer[pdisplay->_bufferinfo._bufferindex++] = mappedchar;
        
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

//@Created by: HIJH
//@Date: Septembre de 2016
//This method handles the input data typed in the inputbox. 
//It masks the output so it allows for writting passwords.
void DisplayInputPasswordHandler(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay)
    {
        char8 mappedchar = 0x00;
        //This is since 0x0B has been defined as "Cancel" in the keypad
        if(pdisplay->_currscrcode == 0x0B)
            pdisplay->_bufferinfo._bufferindex = 0;
        else
            mappedchar = UnmapCode(pdisplay->_currscrcode);
        
        if(pdisplay->_bufferinfo._bufferindex == 0)
        {
            pdisplay->_bufferinfo._xpos = pdisplay->_pcurrflow->_inputboxinfo._xdefpos;
            pdisplay->_bufferinfo._ypos = pdisplay->_pcurrflow->_inputboxinfo._ydefpos;
            memset(pdisplay->_bufferinfo._buffer, ' ', _DISPLAY_BUFFER_SIZE_);
        }
        //The InputBox is already filled up!
        if(pdisplay->_bufferinfo._bufferindex >= pdisplay->_pcurrflow->_inputboxinfo._maxchars)
            return;
        
        //There's nothing new to show up since the unmapper didn't find anything to map to
        if(mappedchar != 0x00)
            pdisplay->_bufferinfo._buffer[pdisplay->_bufferinfo._bufferindex++] = mappedchar;
        
        if(pdisplay->_dispid == DISPLAY1)
        {
            PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPLAY1);
            if(puartdisp)
            {
                char8 maskedinput[16];
                memset(maskedinput, 0x00, sizeof(maskedinput));
                memset(maskedinput, '*', pdisplay->_bufferinfo._bufferindex);
                puartdisp->_messagelength = DisplayOutputString(pdisplay->_bufferinfo._xpos, pdisplay->_bufferinfo._ypos, puartdisp->_messagetx, maskedinput, pdisplay->_pcurrflow->_inputboxinfo._maxchars, pdisplay->_pcurrflow->_inputboxinfo._font);
                puartdisp->_messagestate = PENDING;
            }
        }
        else
        {
            PUARTMESSAGEPTR puartdisp = GetUARTMessageSlot(UART_DISPLAY2);
            if(puartdisp)
            {
                char8 maskedinput[16];
                memset(maskedinput, 0x00, sizeof(maskedinput));
                memset(maskedinput, '*', pdisplay->_bufferinfo._bufferindex);
                puartdisp->_messagelength = DisplayOutputString(pdisplay->_bufferinfo._xpos, pdisplay->_bufferinfo._ypos, puartdisp->_messagetx, maskedinput, pdisplay->_pcurrflow->_inputboxinfo._maxchars, pdisplay->_pcurrflow->_inputboxinfo._font);
                puartdisp->_messagestate = PENDING;
            }
        }
    }
}

//@Created by: HIJH
//@Date: Septembre de 2016
//Allocates and stores a sink message to store the transacion data
void SequesterMessageSink(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay)
    {
        if(pdisplay->_psequesteredmessagesink)
            ReleaseSinkMessage((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink);

        pdisplay->_psequesteredmessagesink = AllocateMessageSlot();
        if(pdisplay->_dispid == DISPLAY1)
            ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_messageid = DISPLAY1_MESSAGE;
        else
            ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_messageid = DISPLAY2_MESSAGE;            
            
        ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_messagetype = FIRSTFOUND;
        
        memset(pdisplay->_bufferinfo._buffer, 0x00, _DISPLAY_BUFFER_SIZE_);
        
        if(_g_dispenserlayoutinfo._numpositions < 0x04)
        {
            DisplayScreenBufferMap *ptr = _g_buffscrlut;
            while(ptr->_refscrid != 0x00)
            {
                if(pdisplay->_pcurrflow->_scrid == ptr->_refscrid)
                {
                    switch(ptr->_refscrid)
                    {
                        case DISPLAY_SELECCIONE_POSICION:
                        {
                            if(pdisplay->_dispid == DISPLAY1)
                            {
                                memset(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos], 0x00, ptr->_bufferlength);
                                ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos] = POSITIONA;
                                ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SIDE_DUMMY_DISPLAY)] = pdisplay->_dispid;
                            }
                            else if(pdisplay->_dispid == DISPLAY2)
                            {
                                memset(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos], 0x00, ptr->_bufferlength);
                                ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos] = POSITIONB;
                                ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SIDE_DUMMY_DISPLAY)] = pdisplay->_dispid;
                            }
                            break;
                        }
                    }
                    break;
                }
                ptr++;
            }
            pdisplay->_currentstate = DISPLAY_LESS_THAN_FOUR_POSITIONS;
        }
    }
}

//@Created by: HIJH
//@Date: Septembre de 2016
//Cleans the sink message that has been allocated to store the transacion data
void CleanMessageSink(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay)
    {
        if(pdisplay->_psequesteredmessagesink)
            ReleaseSinkMessage((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink);
        
        memset(pdisplay->_bufferinfo._buffer, 0x00, _DISPLAY_BUFFER_SIZE_);
    }
}

//@Created by: HIJH
//@Date: Septembre de 2016
//Sets the SinkMessage in "BUSY" state so the whole buffered data 
//from the display flow will be send to the pump transaction model
void ReleaseMessageSink(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay)
    {
        PSINKMESSAGEPTR psinkmsg = (PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink;
        if(psinkmsg)
        {
            //CAREFUL WITH RETURNS BETWEEN THE LOCK/UNLOCK CALLS
            //AND RECURSIVE CALLS THAT WOULD DEADLOCK!
            I2CBusLock();
            
            ClearEepromBuffer();
            //Here we store into EEPROM, the whole captured data for the current transaction
            //on either display and position
            if(pdisplay->_dispid == DISPLAY1)
            {
                switch(psinkmsg->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SELECCIONE_POSICION)])
                {
                    case POSITIONA:
                    {
                        ClearEepromBuffer();
                        memcpy(GetEepromBuffer(), ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer, _EEPROM_FULL_PAGE_SIZE_);
                        if(psinkmsg->_buffer[GetBufferIndexFromDisplayID(DISPLAY_FORMA_PAGO_DESEADA)] == 0x0D)
                            StoreEepromPage(EEPROM_DISPENSER_PUMP11_TRANSACTION_PAGE);
                        else
                            StoreEepromPage(EEPROM_DISPENSER_PUMP13_TRANSACTION_PAGE);
                            
                        break;
                    }
                    case POSITIONB:
                    {
                        ClearEepromBuffer();
                        memcpy(GetEepromBuffer(), ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer, _EEPROM_FULL_PAGE_SIZE_);
                        if(psinkmsg->_buffer[GetBufferIndexFromDisplayID(DISPLAY_FORMA_PAGO_DESEADA)] == 0x0D)
                            StoreEepromPage(EEPROM_DISPENSER_PUMP31_TRANSACTION_PAGE);
                        else
                            StoreEepromPage(EEPROM_DISPENSER_PUMP33_TRANSACTION_PAGE);
                            
                        break;
                    }
                }
            }
            else
            {
                switch(psinkmsg->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SELECCIONE_POSICION)])
                {
                    case POSITIONA:
                        memcpy(GetEepromBuffer(), ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer, _EEPROM_FULL_PAGE_SIZE_);
                        if(psinkmsg->_buffer[GetBufferIndexFromDisplayID(DISPLAY_FORMA_PAGO_DESEADA)] == 0x0D)
                            StoreEepromPage(EEPROM_DISPENSER_PUMP41_TRANSACTION_PAGE);
                        else
                            StoreEepromPage(EEPROM_DISPENSER_PUMP43_TRANSACTION_PAGE);
                            
                        break;
                    case POSITIONB:
                        memcpy(GetEepromBuffer(), ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer, _EEPROM_FULL_PAGE_SIZE_);
                        if(psinkmsg->_buffer[GetBufferIndexFromDisplayID(DISPLAY_FORMA_PAGO_DESEADA)] == 0x0D)
                            StoreEepromPage(EEPROM_DISPENSER_PUMP21_TRANSACTION_PAGE);
                        else
                            StoreEepromPage(EEPROM_DISPENSER_PUMP23_TRANSACTION_PAGE);
                            
                        break;
                }
            }
            I2CBusUnlock();
            
            //////////////////////////////////////////////////////////////////////////////////
            //THIS MUST BE SET TO "SINK_BUSY" TO ALLOW FOR ITS PROCESSING!!! KEEP THIS!!!
            psinkmsg->_messagestate = SINK_BUSY;
            //////////////////////////////////////////////////////////////////////////////////
            pdisplay->_psequesteredmessagesink = NULL;
        }
    }
}

//@Created by: HIJH
//@Date: Septembre de 2016
//Buffers the captured data from the display flow and makes a predefined layout for the
//upcoming transaction on the selected pump.
void StoreTransactionData(void *pparam)
{
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay)
        if(pdisplay->_psequesteredmessagesink != NULL)
        {
            DisplayScreenBufferMap *ptr = _g_buffscrlut;
            while(ptr->_refscrid != 0x00)
            {
                if(pdisplay->_pcurrflow->_scrid == ptr->_refscrid)
                {
                    switch(ptr->_refscrid)
                    {
                        case DISPLAY_FORMA_PAGO_DESEADA:
                        {
                            memset(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos], 0x00, ptr->_bufferlength);
                            ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos] = pdisplay->_currscrcode;
                            break;
                        }
                        case DISPLAY_SELECCIONE_POSICION:
                        {
                            if(_g_dispenserlayoutinfo._numpositions == 0x04)
                            {
                                memset(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos], 0x00, ptr->_bufferlength);
                                ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos] = pdisplay->_currscrcode;
                                ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SIDE_DUMMY_DISPLAY)] = pdisplay->_dispid;
                            }
                            break;
                        }
                        case DISPLAY_FORMA_PROGRAMACION:
                        {
                            memset(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos], 0x00, ptr->_bufferlength);
                            ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos] = pdisplay->_currscrcode;
                            break;
                        }
                        case DISPLAY_INTRODUZCA_VALOR:
                        {
                            memset(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos], 0x00, ptr->_bufferlength);
                            //FOR NOW 8 DEFINED INTO THE BUFFER COPY SINCE THIS IS THE MAXIMUM NUMBER OF DIGITS ALLOWED BY THE PUMP'S PRESET
                            memcpy(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos + (8 - pdisplay->_bufferinfo._bufferindex)], pdisplay->_bufferinfo._buffer, pdisplay->_bufferinfo._bufferindex);
                            break;
                        }
                        case DISPLAY_INTRODUZCA_VOLUMEN:
                        {
                            memset(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos], 0x00, ptr->_bufferlength);
                            //FOR NOW 8 DEFINED INTO THE BUFFER COPY SINCE THIS IS THE MAXIMUM NUMBER OF DIGITS ALLOWED BY THE PUMP'S PRESET
                            memcpy(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos + (8 - pdisplay->_bufferinfo._bufferindex)], pdisplay->_bufferinfo._buffer, pdisplay->_bufferinfo._bufferindex);
                            break;
                        }
                        case DISPLAY_DIGITE_PLACA:
                        {
                            memset(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos], 0x00, ptr->_bufferlength);
                            memcpy(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos], pdisplay->_bufferinfo._buffer, pdisplay->_bufferinfo._bufferindex);
                            break;
                        }
                        case DISPLAY_DESEA_IMPRIMIR_RECIBO:
                        {
                            memset(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos], 0x00, ptr->_bufferlength);
                            ((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos] = pdisplay->_currscrcode;
                            break;
                        }
                        case DISPLAY_INTRODUZCA_KILOMETRAJE:
                        {
                            memset(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos], 0x00, ptr->_bufferlength);
                            memcpy(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos], pdisplay->_bufferinfo._buffer, pdisplay->_bufferinfo._bufferindex);
                            break;
                        }
                        case DISPLAY_INTRODUZCA_CEDULA:
                        {
                            memset(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos], 0x00, ptr->_bufferlength);
                            memcpy(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos], pdisplay->_bufferinfo._buffer, pdisplay->_bufferinfo._bufferindex);
                            break;
                        }
                        case DISPLAY_INGRESE_PASSWORD:
                        {
                            memset(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos], 0x00, ptr->_bufferlength);
                            memcpy(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos], pdisplay->_bufferinfo._buffer, pdisplay->_bufferinfo._bufferindex);
                            break;
                        }
                        case DISPLAY_ESPERANDO_ID:
                        {
                            memset(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos], 0x00, ptr->_bufferlength);
                            memcpy(&((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[ptr->_bufferpos], pdisplay->_bufferinfo._buffer, pdisplay->_bufferinfo._bufferindex);
                            break;
                        }
                    }
                    break;
                }
                ptr++;
            }
        }

}

//@Created by: HIJH
//@Date: Octobre de 2016
//Retrieves the right display depending on the pump identifier. This is done by taking into account the
//dispenser layaou (NORMAL / QUATTRO)
PDISPLAYLAYOUTPTR GetDisplayFromPumpID(uint8 pumpid)
{
    PDISPLAYLAYOUTPTR retval = PNEAR_NULLPTR;
    switch(_g_dispenserlayoutinfo._numpositions)
    {
        case 0x02:
        {
            if(_g_pumps[0x00]._pumpid == pumpid)
                retval = &_g_display1;
            else if(_g_pumps[0x01]._pumpid == pumpid)
                retval = &_g_display2;
            
            break;
        }
        case 0x04:
        {
            if(_g_pumps[0x00]._pumpid == pumpid)
                retval = &_g_display1;
            else if(_g_pumps[0x01]._pumpid == pumpid)
                retval = &_g_display2;
            else if(_g_pumps[0x02]._pumpid == pumpid)
                retval = &_g_display1;
            else if(_g_pumps[0x03]._pumpid == pumpid)
                retval = &_g_display2;
            
            break;
        }
    }
    return retval;
}

/* [] END OF FILE */
