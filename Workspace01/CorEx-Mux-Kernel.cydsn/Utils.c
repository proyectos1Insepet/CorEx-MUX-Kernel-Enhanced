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

//@Created By: HIJH
//@Date: Septembre de 2016
uint64 BCDPositiveValueToUInt(uint8 *pbuffer, uint8 buffersize, uint8 mask)
{
    uint64 retval = 0;
    FOR(uint8 index = 0, index < buffersize, index++)
    {
        retval += pow10(index)*(pbuffer[index] & mask);
        //Not a BCD formatted value
        if((pbuffer[index] & mask) > 0x09)
        {
            retval = 0;
            break;
        }
    }
    return retval;
}

//@Created By: HIJH
//@Date: Septembre de 2016
void B78AD90CF552D9B30A(uint64 number, uint8 *pbuffer)
{
    uint8 index = 1;
    while(number != (number % (uint32)pow(10, index)))
    {
        pbuffer[index - 1] = (char8)(((number % (uint32)pow(10, index)) - (number % (uint32)pow(10, index - 1)))/(uint32)pow(10, index - 1)) | 0x30;
        index++;
    }
    pbuffer[index - 1] = (char8)(((number % (uint32)pow(10, index)) - (number % (uint32)pow(10, index - 1)))/(uint32)pow(10, index - 1)) | 0x30;
}

//@Created By: HIJH
//@Date: Septembre de 2016
void PSTRBUFFTOGG(uint8 *pbuffer, uint8 buffersize)
{
    uint8 valndx = 0x00;
    FOR(uint8 index0 = 0, index0 < (buffersize - 1), index0++)
    {
        valndx = pbuffer[buffersize - 1];
        FOR(int8 index1 = (buffersize - 2), index1 >= index0, index1--)
            pbuffer[index1 + 1] = pbuffer[index1];

        pbuffer[index0] = valndx;
    }
}

//@Created By: HIJH
//@Date: Septembre de 2016
double DLSDBCDBUFF2HEX(PNEAR_BYTE_PTR pbuffer, uint16 buffersize)
{
    double retval = 0.0;
    FOR(uint8 index = 0, index < buffersize, index++)
        retval += pow10(index)*(pbuffer[index] & 0x0F);
        
    return retval;
}

//@Created By: HIJH
//@Date: Septembre de 2016
uint32 LSDBCDBUFF2HEX(PNEAR_BYTE_PTR pbuffer, uint16 buffersize)
{
    uint32 retval = 0x00000000;
    FOR(uint8 index = 0, index < buffersize, index++)
        retval += pow10(index)*(pbuffer[index] & 0x0F);
        
    return retval;
}

//@Created By: HIJH
//@Date: Septembre de 2016
uint32 MSDBCDBUFF2HEX(PNEAR_BYTE_PTR pbuffer, uint16 buffersize)
{
    uint32 retval = 0x00000000;
    FOR(int8 index = (buffersize - 1), index >= 0, index--)
        retval += pow10(index)*(pbuffer[index] & 0x0F);
        
    return retval;
}

//@Created By: HIJH
//@Date: Septembre de 2016
bool ISVALIDBCD(PNEAR_BYTE_PTR pbuffer, uint16 buffersize)
{
    bool retval = true;
    FOR(uint8 index = 0, index < buffersize, index++)
    {
        if((pbuffer[index] & 0x0F) > 0x09 || (pbuffer[index] & 0x0F) < 0x00)
        {
            retval = false;
            break;
        }
    }
    return retval;
}

double Ceil(double val)
{
    int32 retval = (int32)val;
    return ((retval - val) < 0)? (retval + 1): retval;
}

uint16 RGB216BITS(uint8 r, uint8 g, uint8 b)
{
    uint16 code = 0x0000;
    
    code = (r & 0x1f) << 11;
    code |= (g & 0x3f) << 5;
    code |= (b & 0x1f) << 0;
    
    return code;
}

//@Created By: HIJH
//@Date: Septembre de 2016
bool ISASCIIPRNBLE(PNEAR_BYTE_PTR pbuffer, uint8 buffersize)
{
    bool retval = true;
    FOR(uint8 index = 0, index < buffersize, index++)
    {
        //This is approximately the range of printable ASCII characters that we require
        if(pbuffer[index] < 0x20)        
        {
            retval = false;
            break;
        }
    }
    return retval;
}
/* [] END OF FILE */
