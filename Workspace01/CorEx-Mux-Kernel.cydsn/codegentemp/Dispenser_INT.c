/*******************************************************************************
* File Name: DispenserINT.c
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

#include "Dispenser.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

#include "UARTManager.h"

/* `#END` */

#if (Dispenser_RX_INTERRUPT_ENABLED && (Dispenser_RX_ENABLED || Dispenser_HD_ENABLED))
    /*******************************************************************************
    * Function Name: Dispenser_RXISR
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
    *  Dispenser_rxBuffer - RAM buffer pointer for save received data.
    *  Dispenser_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  Dispenser_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  Dispenser_rxBufferOverflow - software overflow flag. Set to one
    *     when Dispenser_rxBufferWrite index overtakes
    *     Dispenser_rxBufferRead index.
    *  Dispenser_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when Dispenser_rxBufferWrite is equal to
    *    Dispenser_rxBufferRead
    *  Dispenser_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  Dispenser_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(Dispenser_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef Dispenser_RXISR_ENTRY_CALLBACK
        Dispenser_RXISR_EntryCallback();
    #endif /* Dispenser_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START Dispenser_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = Dispenser_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in Dispenser_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (Dispenser_RX_STS_BREAK | 
                            Dispenser_RX_STS_PAR_ERROR |
                            Dispenser_RX_STS_STOP_ERROR | 
                            Dispenser_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                Dispenser_errorStatus |= readStatus & ( Dispenser_RX_STS_BREAK | 
                                                            Dispenser_RX_STS_PAR_ERROR | 
                                                            Dispenser_RX_STS_STOP_ERROR | 
                                                            Dispenser_RX_STS_OVERRUN);
                /* `#START Dispenser_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef Dispenser_RXISR_ERROR_CALLBACK
                Dispenser_RXISR_ERROR_Callback();
            #endif /* Dispenser_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & Dispenser_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = Dispenser_RXDATA_REG;
            #if (Dispenser_RXHW_ADDRESS_ENABLED)
                if(Dispenser_rxAddressMode == (uint8)Dispenser__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & Dispenser_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & Dispenser_RX_STS_ADDR_MATCH) != 0u)
                        {
                            Dispenser_rxAddressDetected = 1u;
                        }
                        else
                        {
                            Dispenser_rxAddressDetected = 0u;
                        }
                    }
                    if(Dispenser_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        Dispenser_rxBuffer[Dispenser_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    Dispenser_rxBuffer[Dispenser_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                Dispenser_rxBuffer[Dispenser_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (Dispenser_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(Dispenser_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        Dispenser_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    Dispenser_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(Dispenser_rxBufferWrite >= Dispenser_RX_BUFFER_SIZE)
                    {
                        Dispenser_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(Dispenser_rxBufferWrite == Dispenser_rxBufferRead)
                    {
                        Dispenser_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (Dispenser_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            Dispenser_RXSTATUS_MASK_REG  &= (uint8)~Dispenser_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(Dispenser_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (Dispenser_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & Dispenser_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START Dispenser_RXISR_END` */

        if(InvokeResolver(UART_DISPENSER, (char8*)Dispenser_rxBuffer, Dispenser_GetRxBufferSize()))
            Dispenser_ClearRxBuffer();
        
        /* `#END` */

    #ifdef Dispenser_RXISR_EXIT_CALLBACK
        Dispenser_RXISR_ExitCallback();
    #endif /* Dispenser_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (Dispenser_RX_INTERRUPT_ENABLED && (Dispenser_RX_ENABLED || Dispenser_HD_ENABLED)) */


#if (Dispenser_TX_INTERRUPT_ENABLED && Dispenser_TX_ENABLED)
    /*******************************************************************************
    * Function Name: Dispenser_TXISR
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
    *  Dispenser_txBuffer - RAM buffer pointer for transmit data from.
    *  Dispenser_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  Dispenser_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(Dispenser_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef Dispenser_TXISR_ENTRY_CALLBACK
        Dispenser_TXISR_EntryCallback();
    #endif /* Dispenser_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START Dispenser_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((Dispenser_txBufferRead != Dispenser_txBufferWrite) &&
             ((Dispenser_TXSTATUS_REG & Dispenser_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(Dispenser_txBufferRead >= Dispenser_TX_BUFFER_SIZE)
            {
                Dispenser_txBufferRead = 0u;
            }

            Dispenser_TXDATA_REG = Dispenser_txBuffer[Dispenser_txBufferRead];

            /* Set next pointer */
            Dispenser_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START Dispenser_TXISR_END` */

        /* `#END` */

    #ifdef Dispenser_TXISR_EXIT_CALLBACK
        Dispenser_TXISR_ExitCallback();
    #endif /* Dispenser_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (Dispenser_TX_INTERRUPT_ENABLED && Dispenser_TX_ENABLED) */


/* [] END OF FILE */
