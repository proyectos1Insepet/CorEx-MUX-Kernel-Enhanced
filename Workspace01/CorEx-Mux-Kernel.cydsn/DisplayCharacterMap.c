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
#include "GlobalDefinitions.h"
#include "Display.h"

//This LUT maps the incoming codes from the displays into their respective ASCII character
DisplayCharacterMap _g_charmaplut[] =
{
    { 0x01, 0x31 }, { 0x02, 0x32 }, { 0x03, 0x33 }, { 0x04, 0x34 }, { 0x05, 0x35 }, { 0x06, 0x36 }, { 0x07, 0x37 }, { 0x08, 0x38 }, { 0x09, 0x39 }, { 0x0A, 0x30 },
    { 0x1B, (char8)'Q' }, { 0x1C, (char8)'W'}, { 0x1D, (char8)'E'}, { 0x1E, (char8)'R'}, { 0x1F, (char8)'T'}, { 0x20, (char8)'Y'}, { 0x21, (char8)'U'}, { 0x22, (char8)'I'}, { 0x23, (char8)'O'}, { 0x24, (char8)'P'},
    { 0x25, (char8)'A'}, { 0x26, (char8)'S'}, { 0x27, (char8)'D'}, { 0x28, (char8)'F'}, { 0x29, (char8)'G'}, { 0x2A, (char8)'H'}, { 0x2B, (char8)'J'}, { 0x2C, (char8)'K'}, { 0x2D, (char8)'L'},
    { 0x2F, (char8)'Z'}, { 0x30, (char8)'X'}, { 0x31, (char8)'C'}, { 0x32, (char8)'V'}, { 0x42, (char8)'B'}, { 0x34, (char8)'N'}, { 0x35, (char8)'M'},
    { 0x6A, (char8)'-'}, { 0x68, (char8)' '}, { 0x51, (char8)','}, { 0x00, 0x00 }
};

//@Created By: HIJH
//@Septembre de 2016
char8 UnmapCode(uint8 code)
{
    char8 retval = 0x00;
    DisplayCharacterMap *pcharmap = _g_charmaplut;
    while(pcharmap->_refchar != 0x00)
    {
        if(pcharmap->_refchar == code)
        {
            retval = pcharmap->_mappedto;
            break;
        }
        pcharmap++;
    }
    return retval;
}

/* [] END OF FILE */
