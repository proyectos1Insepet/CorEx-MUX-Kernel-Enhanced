/*******************************************************************************
* File Name: Display2INT.c
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

#include "Display2.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

#include "UARTManager.h"

/* `#END` */

#if (Display2_RX_INTERRUPT_ENABLED && (Display2_RX_ENABLED || Display2_HD_ENABLED))
    /*******************************************************************************
    * Function Name: Display2_RXISR
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
    *  Display2_rxBuffer - RAM buffer pointer for save received data.
    *  Display2_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  Display2_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  Display2_rxBufferOverflow - software overflow flag. Set to one
    *     when Display2_rxBufferWrite index overtakes
    *     Display2_rxBufferRead index.
    *  Display2_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when Display2_rxBufferWrite is equal to
    *    Display2_rxBufferRead
    *  Display2_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  Display2_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(Display2_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef Display2_RXISR_ENTRY_CALLBACK
        Display2_RXISR_EntryCallback();
    #endif /* Display2_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START Display2_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = Display2_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in Display2_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (Display2_RX_STS_BREAK | 
                            Display2_RX_STS_PAR_ERROR |
                            Display2_RX_STS_STOP_ERROR | 
                            Display2_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                Display2_errorStatus |= readStatus & ( Display2_RX_STS_BREAK | 
                                                            Display2_RX_STS_PAR_ERROR | 
                                                            Display2_RX_STS_STOP_ERROR | 
                                                            Display2_RX_STS_OVERRUN);
                /* `#START Display2_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef Display2_RXISR_ERROR_CALLBACK
                Display2_RXISR_ERROR_Callback();
            #endif /* Display2_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & Display2_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = Display2_RXDATA_REG;
            #if (Display2_RXHW_ADDRESS_ENABLED)
                if(Display2_rxAddressMode == (uint8)Display2__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & Display2_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & Display2_RX_STS_ADDR_MATCH) != 0u)
                        {
                            Display2_rxAddressDetected = 1u;
                        }
                        else
                        {
                            Display2_rxAddressDetected = 0u;
                        }
                    }
                    if(Display2_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        Display2_rxBuffer[Display2_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    Display2_rxBuffer[Display2_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                Display2_rxBuffer[Display2_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (Display2_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(Display2_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        Display2_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    Display2_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(Display2_rxBufferWrite >= Display2_RX_BUFFER_SIZE)
                    {
                        Display2_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(Display2_rxBufferWrite == Display2_rxBufferRead)
                    {
                        Display2_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (Display2_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            Display2_RXSTATUS_MASK_REG  &= (uint8)~Display2_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(Display2_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (Display2_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & Display2_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START Display2_RXISR_END` */

        if(InvokeResolver(UART_DISPLAY2, (char8*)Display2_rxBuffer, Display2_GetRxBufferSize()))
            Display2_ClearRxBuffer();
        
        /* `#END` */

    #ifdef Display2_RXISR_EXIT_CALLBACK
        Display2_RXISR_ExitCallback();
    #endif /* Display2_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (Display2_RX_INTERRUPT_ENABLED && (Display2_RX_ENABLED || Display2_HD_ENABLED)) */


#if (Display2_TX_INTERRUPT_ENABLED && Display2_TX_ENABLED)
    /*******************************************************************************
    * Function Name: Display2_TXISR
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
    *  Display2_txBuffer - RAM buffer pointer for transmit data from.
    *  Display2_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  Display2_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(Display2_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef Display2_TXISR_ENTRY_CALLBACK
        Display2_TXISR_EntryCallback();
    #endif /* Display2_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START Display2_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((Display2_txBufferRead != Display2_txBufferWrite) &&
             ((Display2_TXSTATUS_REG & Display2_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(Display2_txBufferRead >= Display2_TX_BUFFER_SIZE)
            {
                Display2_txBufferRead = 0u;
            }

            Display2_TXDATA_REG = Display2_txBuffer[Display2_txBufferRead];

            /* Set next pointer */
            Display2_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START Display2_TXISR_END` */

        /* `#END` */

    #ifdef Display2_TXISR_EXIT_CALLBACK
        Display2_TXISR_ExitCallback();
    #endif /* Display2_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (Display2_TX_INTERRUPT_ENABLED && Display2_TX_ENABLED) */


/* [] END OF FILE */
