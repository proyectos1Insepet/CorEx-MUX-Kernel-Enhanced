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

#define _CRC_CLOCK_PULSE_WIDTH_     1 //Pulse width in microseconds
#define _IBUTTON_PAYLOAD_LENGTH_    8 //8-BIT FAMILY CODE + 48-BIT SERIAL NUMBER + 8-BIT CRC CODE

#define CRC_CLOCK_PULSE             \
Clock_iButton_Write(true);          \
CyDelayUs(_CRC_CLOCK_PULSE_WIDTH_); \
Clock_iButton_Write(false);         \
CyDelayUs(_CRC_CLOCK_PULSE_WIDTH_)

#define _IBUTTON_READ_ROM_      0x33
//////////////////////////////////////////////////////////////////////////////////////////
//ALL OF THE DELAY VALUES ARE ACCORDING TO THE 1-WIRE PROTOCOL DOCUMENTATION
#define _1WIRE_WAIT_RSTL      CyDelayUs(500)//tRSTL
#define _1WIRE_WAIT_PDH       CyDelayUs(60) //tMSP
#define _1WIRE_WAIT_PDL       CyDelayUs(70) //tMSP
//SINGLE BIT WRITE TIME SLOTS FOR A MAXIMUM BIT WRITE OPERATION SLOT OF 75uS
#define _1WIRE_WRITE1_DELAY        CyDelayUs(6)
#define _1WIRE_WRITE0_DELAY        CyDelayUs(60)
#define _1WIRE_WRITE1_TIME_SLOT_   CyDelayUs(64)
#define _1WIRE_WRITE0_TIME_SLOT_   CyDelayUs(10)
//SINGLE BIT READ TIME SLOTS
#define _1WIRE_RL1_TIME                CyDelayUs(6)
#define _1WIRE_MSR0_TIME               CyDelayUs(9)
#define _1WIRE_WAIT_TIME_SLOT          CyDelayUs(55)
#define _1WIRE_RESET_WAIT_TIME_SLOT    CyDelayUs(100)
//////////////////////////////////////////////////////////////////////////////////////////

typedef enum
{
    _IBUTTON_PORT1_    = 0x01,
    _IBUTTON_PORT2_
    
} IButtonPort;

/* [] END OF FILE */
