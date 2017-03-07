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
@Created by: HIJH
@Date: Septembre 2 de 2016
*/

#include <project.h>
#include "GlobalDefinitions.h"

void ClockCRC();

//@Created By: HIJH
//@Septembre de 2016
void StartSecurityLayer()
{
    //ShiftReg_Scramble_Start();
    ShiftReg_Security_Start();
    CRC_Security_Start();
}

//*******************************************************************************************
//@Created by: HIJH
//@Septembre 1 de 2016
//@Comments: RESERVED FOR FUTURE USE
//This set of numbers have been discretionarily but carefully chosen for the Scramble/Unscramble procedure.
//This procedure has the purpose of giving some sort of encryption OTA.
//uint8 _primenumberset[] = { 0x0b, 0x07, 0x11, 0x17, 0x03 };
//*******************************************************************************************

void ClockCRC()
{
    Clock_CRC_Write(1u);
    CyDelay(1u);
    Clock_CRC_Write(0u);
    CyDelay(1u);
}

//@Created By: HIJH
//@Septembre de 2016
char8 CalcCRC(char8 *pbuffer, uint8 buffersize)
{
    uint8 index = 0;
    uint8 byteindex = 0;
    char8 retval = 0x00;
    
    Reset_CRC_Write(1u);
    CyDelay(1u);
    ClockCRC();
    Reset_CRC_Write(0u);
    for(;index < buffersize; index++)
    {
        ShiftReg_Security_WriteRegValue(pbuffer[index]);
        //Shifting current byte bits from the Shift Register 
        //into the CRC UDB to apply the polynomial
        //This should count from 0 to 7 (meaning 8 bits = byte)
        for(; byteindex < 8; byteindex++)
            ClockCRC();

    }
    retval = CRC_Security_ReadCRC();
    
    return retval;
}

uint8 CRCCheck(char8 crcseed, uint8 data)
{
    uint8 i;
    crcseed = crcseed ^ data;
    for (i = 0; i < 8; i++)
    {
        if (crcseed & 0x01){
            crcseed = (crcseed >> 1) ^ 0x8C;
		}	
        else{
            crcseed >>= 1;
		}	
    }
    return crcseed;
}

//@Created By: HIJH
//@Septembre de 2016
uint8 RawCRCCheck(char8 *pdata, uint16 buffersize)
{
    uint8 crcseed = 0x00;
    FOR(uint16 index = 0, index < buffersize, index++)
        crcseed = CRCCheck(crcseed, pdata[index]);
        
    return crcseed;
}


/* [] END OF FILE */
