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

//*************************************************************************************************************
//ADD HERE ALL OF THE HEADER FILES FOR THE AVAILABLE UART PORTS TO BE ACCESSED THROUGH THE UART MANAGER
#include "RF_Physical.h"
#include "Printer.h"
#include "Display1.h"
#include "Display2.h"
#include "Dispenser.h"


//*************************************************************************************************************    
//@Created by: HIJH
//@Date: Septembre 1 de 2016    
//THIS IS ARRAY CONFIGURES ALL OF THE AVAILABLE UART PORTS TO BE USED BY THE COMMUNICATIONS STACK.
//TO AVOID USING MAGIC NUMBERS TO ADDRESS THE APPROPRIATE SET OF POINTER, AGGREGATE A SUITABLE
//"SET-IDENTIFIER" IN THE MANAGERS ENUMERATION LOCATED IN THE "UARTManager.h" FILE
UARTManager _g_uartmanagers[] = 
{
    //Wireless Link entry
    { UART_RF, GlobalUARTSend, RF_Physical_Start, RF_Physical_Stop, RF_Physical_ReadControlRegister, RF_Physical_WriteControlRegister, RF_Physical_ReadRxData, RF_Physical_ReadRxStatus, RF_Physical_GetChar, RF_Physical_GetByte, RF_Physical_GetRxBufferSize, RF_Physical_ClearRxBuffer, RF_Physical_WriteTxData, RF_Physical_ReadTxStatus, RF_Physical_PutChar, RF_Physical_PutString, RF_Physical_PutArray, RF_Physical_PutCRLF, RF_Physical_ClearTxBuffer },
    //Printer entry
    { UART_PRINTER, NULL, Printer_Start, Printer_Stop, Printer_ReadControlRegister, Printer_WriteControlRegister, Printer_ReadRxData, Printer_ReadRxStatus, Printer_GetChar, Printer_GetByte, Printer_GetRxBufferSize, Printer_ClearRxBuffer, Printer_WriteTxData, Printer_ReadTxStatus, Printer_PutChar, Printer_PutString, Printer_PutArray, Printer_PutCRLF, Printer_ClearTxBuffer },
    //Dispenser/Pump entry
    { UART_DISPENSER, DispenserUARTSend, Dispenser_Start, Dispenser_Stop, Dispenser_ReadControlRegister, Dispenser_WriteControlRegister, Dispenser_ReadRxData, Dispenser_ReadRxStatus, Dispenser_GetChar, Dispenser_GetByte, Dispenser_GetRxBufferSize, Dispenser_ClearRxBuffer, Dispenser_WriteTxData, Dispenser_ReadTxStatus, Dispenser_PutChar, Dispenser_PutString, Dispenser_PutArray, Dispenser_PutCRLF, Dispenser_ClearTxBuffer },
    //Display/LCD 1 entry
    { UART_DISPLAY1, NULL, Display1_Start, Display1_Stop, Display1_ReadControlRegister, Display1_WriteControlRegister, Display1_ReadRxData, Display1_ReadRxStatus, Display1_GetChar, Display1_GetByte, Display1_GetRxBufferSize, Display1_ClearRxBuffer, Display1_WriteTxData, Display1_ReadTxStatus, Display1_PutChar, Display1_PutString, Display1_PutArray, Display1_PutCRLF, Display1_ClearTxBuffer },
    //Display/LCD 2 entry
    { UART_DISPLAY2, NULL, Display2_Start, Display2_Stop, Display2_ReadControlRegister, Display2_WriteControlRegister, Display2_ReadRxData, Display2_ReadRxStatus, Display2_GetChar, Display2_GetByte, Display2_GetRxBufferSize, Display2_ClearRxBuffer, Display2_WriteTxData, Display2_ReadTxStatus, Display2_PutChar, Display2_PutString, Display2_PutArray, Display2_PutCRLF, Display2_ClearTxBuffer },
    //NULL entry
    { UART_NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

/* [] END OF FILE */
