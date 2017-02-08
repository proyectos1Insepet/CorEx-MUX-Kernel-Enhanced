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

This is the implementation of the 1-Wire protocol for the iButton communication

*/
#include <project.h>
#include "SinkManager.h"
#include "SinkObservables.h"
#include "GlobalDefinitions.h"
#include "iButton.h"

bool iButtonResetPresenceSequence(uint8 port)
{
    bool retval = false;
    switch(port)
    {
        case _IBUTTON_PORT1_:
        {
            iButton1_Write(LOW);
            _1WIRE_WAIT_RSTL;
            iButton1_Write(HIGH);
            _1WIRE_WAIT_PDL;
            retval = !iButton1_Read(); //Ack/Nack
            break;
        }
        case _IBUTTON_PORT2_:
        {
            iButton2_Write(LOW);
            _1WIRE_WAIT_RSTL;
            iButton2_Write(HIGH);
            _1WIRE_WAIT_PDL;
            retval = !iButton2_Read(); //Ack/Nack
            break;
        }
    }
    _1WIRE_RESET_WAIT_TIME_SLOT;
    return retval;
}

uint8 iButtonRead(uint8 port)
{
    uint8 retval = 0x00;
    switch(port)
    {
        case _IBUTTON_PORT1_:
        {
            FOR(uint8 index = 0, index < 8, index++)
            {
                iButton1_Write(LOW);
                _1WIRE_RL1_TIME;
                iButton1_Write(HIGH);
                _1WIRE_MSR0_TIME;
                uint8 bitdata = iButton1_Read();
                retval |= (bitdata << index);
                _1WIRE_WAIT_TIME_SLOT;
            }
            break;
        }
        case _IBUTTON_PORT2_:
        {
            FOR(uint8 index = 0, index < 8, index++)
            {
                iButton2_Write(LOW);
                _1WIRE_RL1_TIME;
                iButton2_Write(HIGH);
                _1WIRE_MSR0_TIME;
                uint8 bitdata = iButton2_Read();
                retval |= (bitdata << index);
                _1WIRE_WAIT_TIME_SLOT;
            }
            break;
        }
    }
    return retval;
}

void iButtonWrite(uint8 port, uint8 data)
{
    switch(port)
    {
        case _IBUTTON_PORT1_:
        {
            FOR(uint8 index = 0, index < 8, index++)
            {
                iButton1_Write(LOW);
                if((data >> index) & 0x01)
                {
                    _1WIRE_WRITE1_DELAY;
                    iButton1_Write(HIGH);
                    _1WIRE_WRITE1_TIME_SLOT_;
                }
                else
                {
                    _1WIRE_WRITE0_DELAY;
                    iButton1_Write(HIGH);
                    _1WIRE_WRITE0_TIME_SLOT_;

                }
            }
            break;
        }
        case _IBUTTON_PORT2_:
        {
            FOR(uint8 index = 0, index < 8, index++)
            {
                iButton2_Write(LOW);
                if((data >> index) & 0x01)
                {
                    _1WIRE_WRITE1_DELAY;
                    iButton2_Write(HIGH);
                    _1WIRE_WRITE1_TIME_SLOT_;
                }
                else
                {
                    _1WIRE_WRITE0_DELAY;
                    iButton2_Write(HIGH);
                    _1WIRE_WRITE0_TIME_SLOT_;
                }
            }
            break;
        }
    }
}


/* [] END OF FILE */
