/*******************************************************************************
* File Name: Display1INT.c
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

#include "Display1.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

#include "UARTManager.h"

/* `#END` */

#if (Display1_RX_INTERRUPT_ENABLED && (Display1_RX_ENABLED || Display1_HD_ENABLED))
    /*******************************************************************************
    * Function Name: Display1_RXISR
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
    *  Display1_rxBuffer - RAM buffer pointer for save received data.
    *  Display1_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  Display1_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  Display1_rxBufferOverflow - software overflow flag. Set to one
    *     when Display1_rxBufferWrite index overtakes
    *     Display1_rxBufferRead index.
    *  Display1_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when Display1_rxBufferWrite is equal to
    *    Display1_rxBufferRead
    *  Display1_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  Display1_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(Display1_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef Display1_RXISR_ENTRY_CALLBACK
        Display1_RXISR_EntryCallback();
    #endif /* Display1_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START Display1_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = Display1_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in Display1_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (Display1_RX_STS_BREAK | 
                            Display1_RX_STS_PAR_ERROR |
                            Display1_RX_STS_STOP_ERROR | 
                            Display1_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                Display1_errorStatus |= readStatus & ( Display1_RX_STS_BREAK | 
                                                            Display1_RX_STS_PAR_ERROR | 
                                                            Display1_RX_STS_STOP_ERROR | 
                                                            Display1_RX_STS_OVERRUN);
                /* `#START Display1_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef Display1_RXISR_ERROR_CALLBACK
                Display1_RXISR_ERROR_Callback();
            #endif /* Display1_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & Display1_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = Display1_RXDATA_REG;
            #if (Display1_RXHW_ADDRESS_ENABLED)
                if(Display1_rxAddressMode == (uint8)Display1__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & Display1_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & Display1_RX_STS_ADDR_MATCH) != 0u)
                        {
                            Display1_rxAddressDetected = 1u;
                        }
                        else
                        {
                            Display1_rxAddressDetected = 0u;
                        }
                    }
                    if(Display1_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        Display1_rxBuffer[Display1_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    Display1_rxBuffer[Display1_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                Display1_rxBuffer[Display1_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (Display1_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(Display1_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        Display1_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    Display1_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(Display1_rxBufferWrite >= Display1_RX_BUFFER_SIZE)
                    {
                        Display1_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(Display1_rxBufferWrite == Display1_rxBufferRead)
                    {
                        Display1_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (Display1_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            Display1_RXSTATUS_MASK_REG  &= (uint8)~Display1_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(Display1_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (Display1_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & Display1_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START Display1_RXISR_END` */

        if(InvokeResolver(UART_DISPLAY1, (char8*)Display1_rxBuffer, Display1_GetRxBufferSize()))
            Display1_ClearRxBuffer();
        
        /* `#END` */

    #ifdef Display1_RXISR_EXIT_CALLBACK
        Display1_RXISR_ExitCallback();
    #endif /* Display1_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (Display1_RX_INTERRUPT_ENABLED && (Display1_RX_ENABLED || Display1_HD_ENABLED)) */


#if (Display1_TX_INTERRUPT_ENABLED && Display1_TX_ENABLED)
    /*******************************************************************************
    * Function Name: Display1_TXISR
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
    *  Display1_txBuffer - RAM buffer pointer for transmit data from.
    *  Display1_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  Display1_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(Display1_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef Display1_TXISR_ENTRY_CALLBACK
        Display1_TXISR_EntryCallback();
    #endif /* Display1_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START Display1_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((Display1_txBufferRead != Display1_txBufferWrite) &&
             ((Display1_TXSTATUS_REG & Display1_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(Display1_txBufferRead >= Display1_TX_BUFFER_SIZE)
            {
                Display1_txBufferRead = 0u;
            }

            Display1_TXDATA_REG = Display1_txBuffer[Display1_txBufferRead];

            /* Set next pointer */
            Display1_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START Display1_TXISR_END` */

        /* `#END` */

    #ifdef Display1_TXISR_EXIT_CALLBACK
        Display1_TXISR_ExitCallback();
    #endif /* Display1_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (Display1_TX_INTERRUPT_ENABLED && Display1_TX_ENABLED) */


/* [] END OF FILE */
