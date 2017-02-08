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
#include "SinkManager.h"
#include "SinkObservables.h"
#include "UARTManager.h"

void ReleaseMessage(UARTMessage *pmessage);

void RFUARTSend(void *ptr);
void Dislay1UARTSend(void *ptr);
void Dislay2UARTSend(void *ptr);
void DispenserUARTSend(void *param);
void PrinterUARTSend(void *ptr);

//*************************************************************************************************************    
//@Created by: HIJH
//@Date: Septembre 1 de 2016    
//THIS IS ARRAY CONFIGURES ALL OF THE AVAILABLE UART PORTS TO BE USED BY THE COMMUNICATIONS STACK.
//TO AVOID USING MAGIC NUMBERS TO ADDRESS THE APPROPRIATE SET OF POINTERS, AGGREGATE A SUITABLE
//"SET-IDENTIFIER" IN THE MANAGER ENUMERATION LOCATED IN THE "UARTManager.h" FILE

UARTManager _g_uartmanagers[] = 
{
    //Wireless Link entry
    { UART_RF, RFUARTSend, RF_Physical_Start, RF_Physical_Stop, RF_Physical_ReadControlRegister, RF_Physical_WriteControlRegister, RF_Physical_ReadRxData, RF_Physical_ReadRxStatus, RF_Physical_GetChar, RF_Physical_GetByte, RF_Physical_GetRxBufferSize, RF_Physical_ClearRxBuffer, RF_Physical_WriteTxData, RF_Physical_ReadTxStatus, RF_Physical_PutChar, RF_Physical_PutString, RF_Physical_PutArray, RF_Physical_PutCRLF, RF_Physical_ClearTxBuffer },
    //Printer entry
    { UART_PRINTER, PrinterUARTSend, Printer_Start, Printer_Stop, Printer_ReadControlRegister, Printer_WriteControlRegister, Printer_ReadRxData, Printer_ReadRxStatus, Printer_GetChar, Printer_GetByte, (uint16 (*)(void))Printer_GetRxBufferSize, Printer_ClearRxBuffer, Printer_WriteTxData, Printer_ReadTxStatus, Printer_PutChar, Printer_PutString, Printer_PutArray, Printer_PutCRLF, Printer_ClearTxBuffer },
    //Dispenser/Pump entry
    { UART_DISPENSER, DispenserUARTSend, Dispenser_Start, Dispenser_Stop, Dispenser_ReadControlRegister, Dispenser_WriteControlRegister, Dispenser_ReadRxData, Dispenser_ReadRxStatus, Dispenser_GetChar, Dispenser_GetByte, Dispenser_GetRxBufferSize, Dispenser_ClearRxBuffer, Dispenser_WriteTxData, Dispenser_ReadTxStatus, Dispenser_PutChar, Dispenser_PutString, Dispenser_PutArray, Dispenser_PutCRLF, Dispenser_ClearTxBuffer },
    //Display/LCD 1 entry
    { UART_DISPLAY1, Dislay1UARTSend, Display1_Start, Display1_Stop, Display1_ReadControlRegister, Display1_WriteControlRegister, Display1_ReadRxData, Display1_ReadRxStatus, Display1_GetChar, Display1_GetByte, (uint16 (*)(void))Display1_GetRxBufferSize, Display1_ClearRxBuffer, Display1_WriteTxData, Display1_ReadTxStatus, Display1_PutChar, Display1_PutString, Display1_PutArray, Display1_PutCRLF, Display1_ClearTxBuffer },
    //Display/LCD 2 entry
    { UART_DISPLAY2, Dislay1UARTSend, Display2_Start, Display2_Stop, Display2_ReadControlRegister, Display2_WriteControlRegister, Display2_ReadRxData, Display2_ReadRxStatus, Display2_GetChar, Display2_GetByte, (uint16 (*)(void))Display2_GetRxBufferSize, Display2_ClearRxBuffer, Display2_WriteTxData, Display2_ReadTxStatus, Display2_PutChar, Display2_PutString, Display2_PutArray, Display2_PutCRLF, Display2_ClearTxBuffer },
    //NULL entry
    { UART_NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};


/*
*/
//The maximum number of message that could be sent at a time and awaiting for a response
UARTMessage _g_uartmessages[_MAX_MESSAGES_];

///@************USAGE EXAMPLE OF THE UART MANAGER MODEL************
///@TO ACQUIRE A MESSAGE SLOT, INVOKE THE "UARTMessage* GetUARTMessageSlot()" METHOD.
///@FILL THE ACQUIRED MESSAGE WITH THE APPROPRIATE PARAMETERS AS FOLLOW:
/*
        ptrmessage->_puartmanager = GetUARTManager(UART_RF);

        //FILL THE TRANSMISSION BUFFER WITH THE DATA TO BE TRANSMITTED FOLLOWING THE NEXT CONVENTION:

        ptrmessage->_messagetx <= { _COMMAND_IDENTIFIER, CUSTOM DATA };

        //THE COMMAND IDENTIFIER ALLOWS FOR THE RESPONSE MAPPING TO THE AWAITING MESSAGE.
        //THIS MUST BE REPLIED BACK FOLLOWING THE SAME MESSAGE FORMAT SO THE RESPECTIVE
        //ISR CAN EXTRACT THE IDENTIFIER AND PUSH THE RESPONSE BUFFER INTO THE MESSAGE BUFFER

        ptrmessage->_messagerx <= ISR_UART_BUFFER;
*/
void UARTGlobalHandler()
{
    FOR(uint8 index = 0, index < _MAX_MESSAGES_, index++)
    {
        PUARTMESSAGEPTR pmessage = &_g_uartmessages[index];
        //A PENDING message is one that has been acquired recently but 
        //has not been posted yet to its destination
        if(pmessage->_messagestate == PENDING)
        {
            pmessage->_puartmanager->CustomUARTSend(pmessage);
            ReleaseMessage(pmessage);
        }
    }
}

///@UARTManager* GetUARTManager(uint8 manager)
///@Created by: HIJH
///@Date: Septembre 1 de 2016
///Returns the requested UART manager identified by the appropriate identifier
PUARTMANAGERPTR GetUARTManager(uint8 manager)
{
    PUARTMANAGERPTR pretvaliterator = _g_uartmanagers;
    while(pretvaliterator->_identifier != UART_NULL)
    {
        if(pretvaliterator->_identifier == manager)
            break;
        
        pretvaliterator++;
    }
    return pretvaliterator;
}

///@UARTMessage* GetUARTMessageSlot()
///@Created by: HIJH
///@Date: Septembre 1 de 2016
///This function search for an available slot in the array of messages and returns a pointer
///to the first availabe. If there isn't any available slot, then returns NULL.
PUARTMESSAGEPTR GetUARTMessageSlot(uint8 manager)
{
    PUARTMESSAGEPTR pretval = NULL;
    FOR(uint8 index = 0, index < _MAX_MESSAGES_, index++)
    {
        if(_g_uartmessages[index]._messagestate == FREE)
        {
            _g_uartmessages[index]._messagestate = ALLOCATED;                        
            _g_uartmessages[index]._messagelength = 0x0000;
            _g_uartmessages[index]._puartmanager = GetUARTManager(manager);
            memset(_g_uartmessages[index]._messagetx, 0x00, _MAX_MESSAGE_SIZE_);
            
            pretval = &_g_uartmessages[index];
            break;
        }
    }
    return pretval;
}

void InitUARTManager()
{
    FOR(uint8 index = 0, index < _MAX_MESSAGES_, index++)
    {
        _g_uartmessages[index]._messagestate = FREE;        
        _g_uartmessages[index]._messagelength = 0x0000;
        memset(_g_uartmessages[index]._messagetx, 0x00, _MAX_MESSAGE_SIZE_);
        _g_uartmessages[index]._puartmanager = NULL;
    }
        
    GLOBAL_LOOP_PUSH(UARTGlobalHandler);
}

/*
@RFUARTSend standard method used for the UART Manager
@Created by: HIJH
@Date: Août 30 de 2016
Since all of the communications are posted through the same channel, the state machine carry all of the
flags required to notify through the layers.

*/

void ReleaseMessage(UARTMessage *pmessage)
{
    pmessage->_messagestate = FREE;
    pmessage->_messagelength = 0x0000;
    memset(pmessage->_messagetx, 0x00, _MAX_MESSAGE_SIZE_);
    pmessage->_puartmanager = NULL;
}

/* [] END OF FILE */
