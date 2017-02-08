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

#include "UARTManager.h"

bool RFUARTBufferResolver(uint8* pbuffer, uint16 buffersize);
bool Display1BufferResolver(uint8* pbuffer, uint16 buffersize);
bool Display2BufferResolver(uint8* pbuffer, uint16 buffersize);
bool DispenserBufferResolver(uint8* pbuffer, uint16 buffersize);

//@Created by: HIJH
//@Date: Septembre 3 de 2016
//This array of Resolvers handle the incoming data for each UART port. They parse the incoming data
//and decide when the data is complete to be pushed into the SinkManager.
UARTResolver _g_uartresolvers[] =  
{
    { UART_RF, RFUARTBufferResolver }, 
    { UART_PRINTER, NULL }, 
    { UART_DISPENSER, DispenserBufferResolver }, 
    { UART_DISPLAY1, Display1BufferResolver }, 
    { UART_DISPLAY2, Display2BufferResolver }, 
    { UART_NULL, NULL }
};

/*
@bool InvokeUARTResolver(uint8 identifier, char* pbuffer, uint16 buffersize)
@Created by: HIJH
@Date: Août 30 de 2016

This method handles the entire incoming stream and acts according to the status of the Networking Stack
and the block size.

Once a complete stream (block) has been received, this method checks for the length of the block and parses
its content.
*/
bool InvokeResolver(uint8 identifier, uint8* pbuffer, uint16 buffersize)
{
    bool retval = false;
    PNEAR_UARTRESOLVERPTR ptresolveriterator = &_g_uartresolvers[0];
    while(ptresolveriterator->_identifier != UART_NULL)
    {
        if(ptresolveriterator->_identifier == identifier)
        {
            retval = ptresolveriterator->PtrResolver(pbuffer, buffersize);
            break;
        }
        ptresolveriterator++;
    }
    return retval;
}

/* [] END OF FILE */
