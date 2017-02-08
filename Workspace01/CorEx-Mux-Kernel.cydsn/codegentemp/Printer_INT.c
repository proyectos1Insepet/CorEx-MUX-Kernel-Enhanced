/*******************************************************************************
* File Name: PrinterINT.c
* Version 2.50
*
* Description:
*  This file provides all Interrupt Service functionality of the UART component
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Printer.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

#include "UARTManager.h"

/* `#END` */

#if (Printer_RX_INTERRUPT_ENABLED && (Printer_RX_ENABLED || Printer_HD_ENABLED))
    /*******************************************************************************
    * Function Name: Printer_RXISR
    ********************************************************************************
    *
    * Summary:
    *  Interrupt Service Routine for RX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Printer_rxBuffer - RAM buffer pointer for save received data.
    *  Printer_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  Printer_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  Printer_rxBufferOverflow - software overflow flag. Set to one
    *     when Printer_rxBufferWrite index overtakes
    *     Printer_rxBufferRead index.
    *  Printer_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when Printer_rxBufferWrite is equal to
    *    Printer_rxBufferRead
    *  Printer_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  Printer_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(Printer_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef Printer_RXISR_ENTRY_CALLBACK
        Printer_RXISR_EntryCallback();
    #endif /* Printer_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START Printer_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = Printer_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in Printer_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (Printer_RX_STS_BREAK | 
                            Printer_RX_STS_PAR_ERROR |
                            Printer_RX_STS_STOP_ERROR | 
                            Printer_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                Printer_errorStatus |= readStatus & ( Printer_RX_STS_BREAK | 
                                                            Printer_RX_STS_PAR_ERROR | 
                                                            Printer_RX_STS_STOP_ERROR | 
                                                            Printer_RX_STS_OVERRUN);
                /* `#START Printer_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef Printer_RXISR_ERROR_CALLBACK
                Printer_RXISR_ERROR_Callback();
            #endif /* Printer_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & Printer_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = Printer_RXDATA_REG;
            #if (Printer_RXHW_ADDRESS_ENABLED)
                if(Printer_rxAddressMode == (uint8)Printer__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & Printer_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & Printer_RX_STS_ADDR_MATCH) != 0u)
                        {
                            Printer_rxAddressDetected = 1u;
                        }
                        else
                        {
                            Printer_rxAddressDetected = 0u;
                        }
                    }
                    if(Printer_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        Printer_rxBuffer[Printer_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    Printer_rxBuffer[Printer_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                Printer_rxBuffer[Printer_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (Printer_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(Printer_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        Printer_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    Printer_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(Printer_rxBufferWrite >= Printer_RX_BUFFER_SIZE)
                    {
                        Printer_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(Printer_rxBufferWrite == Printer_rxBufferRead)
                    {
                        Printer_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (Printer_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            Printer_RXSTATUS_MASK_REG  &= (uint8)~Printer_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(Printer_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (Printer_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & Printer_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START Printer_RXISR_END` */

        /* `#END` */

    #ifdef Printer_RXISR_EXIT_CALLBACK
        Printer_RXISR_ExitCallback();
    #endif /* Printer_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (Printer_RX_INTERRUPT_ENABLED && (Printer_RX_ENABLED || Printer_HD_ENABLED)) */


#if (Printer_TX_INTERRUPT_ENABLED && Printer_TX_ENABLED)
    /*******************************************************************************
    * Function Name: Printer_TXISR
    ********************************************************************************
    *
    * Summary:
    * Interrupt Service Routine for the TX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Printer_txBuffer - RAM buffer pointer for transmit data from.
    *  Printer_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  Printer_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(Printer_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef Printer_TXISR_ENTRY_CALLBACK
        Printer_TXISR_EntryCallback();
    #endif /* Printer_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START Printer_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((Printer_txBufferRead != Printer_txBufferWrite) &&
             ((Printer_TXSTATUS_REG & Printer_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(Printer_txBufferRead >= Printer_TX_BUFFER_SIZE)
            {
                Printer_txBufferRead = 0u;
            }

            Printer_TXDATA_REG = Printer_txBuffer[Printer_txBufferRead];

            /* Set next pointer */
            Printer_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START Printer_TXISR_END` */

        /* `#END` */

    #ifdef Printer_TXISR_EXIT_CALLBACK
        Printer_TXISR_ExitCallback();
    #endif /* Printer_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (Printer_TX_INTERRUPT_ENABLED && Printer_TX_ENABLED) */


/* [] END OF FILE */
