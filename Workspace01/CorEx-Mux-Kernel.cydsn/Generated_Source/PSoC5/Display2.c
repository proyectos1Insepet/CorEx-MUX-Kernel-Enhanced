/*******************************************************************************
* File Name: Display2.c
* Version 2.50
*
* Description:
*  This file provides all API functionality of the UART component
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Display2.h"
#if (Display2_INTERNAL_CLOCK_USED)
    #include "Display2_IntClock.h"
#endif /* End Display2_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 Display2_initVar = 0u;

#if (Display2_TX_INTERRUPT_ENABLED && Display2_TX_ENABLED)
    volatile uint8 Display2_txBuffer[Display2_TX_BUFFER_SIZE];
    volatile uint8 Display2_txBufferRead = 0u;
    uint8 Display2_txBufferWrite = 0u;
#endif /* (Display2_TX_INTERRUPT_ENABLED && Display2_TX_ENABLED) */

#if (Display2_RX_INTERRUPT_ENABLED && (Display2_RX_ENABLED || Display2_HD_ENABLED))
    uint8 Display2_errorStatus = 0u;
    volatile uint8 Display2_rxBuffer[Display2_RX_BUFFER_SIZE];
    volatile uint8 Display2_rxBufferRead  = 0u;
    volatile uint8 Display2_rxBufferWrite = 0u;
    volatile uint8 Display2_rxBufferLoopDetect = 0u;
    volatile uint8 Display2_rxBufferOverflow   = 0u;
    #if (Display2_RXHW_ADDRESS_ENABLED)
        volatile uint8 Display2_rxAddressMode = Display2_RX_ADDRESS_MODE;
        volatile uint8 Display2_rxAddressDetected = 0u;
    #endif /* (Display2_RXHW_ADDRESS_ENABLED) */
#endif /* (Display2_RX_INTERRUPT_ENABLED && (Display2_RX_ENABLED || Display2_HD_ENABLED)) */


/*******************************************************************************
* Function Name: Display2_Start
********************************************************************************
*
* Summary:
*  This is the preferred method to begin component operation.
*  Display2_Start() sets the initVar variable, calls the
*  Display2_Init() function, and then calls the
*  Display2_Enable() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The Display2_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time Display2_Start() is called. This
*  allows for component initialization without re-initialization in all
*  subsequent calls to the Display2_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Display2_Start(void) 
{
    /* If not initialized then initialize all required hardware and software */
    if(Display2_initVar == 0u)
    {
        Display2_Init();
        Display2_initVar = 1u;
    }

    Display2_Enable();
}


/*******************************************************************************
* Function Name: Display2_Init
********************************************************************************
*
* Summary:
*  Initializes or restores the component according to the customizer Configure
*  dialog settings. It is not necessary to call Display2_Init() because
*  the Display2_Start() API calls this function and is the preferred
*  method to begin component operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Display2_Init(void) 
{
    #if(Display2_RX_ENABLED || Display2_HD_ENABLED)

        #if (Display2_RX_INTERRUPT_ENABLED)
            /* Set RX interrupt vector and priority */
            (void) CyIntSetVector(Display2_RX_VECT_NUM, &Display2_RXISR);
            CyIntSetPriority(Display2_RX_VECT_NUM, Display2_RX_PRIOR_NUM);
            Display2_errorStatus = 0u;
        #endif /* (Display2_RX_INTERRUPT_ENABLED) */

        #if (Display2_RXHW_ADDRESS_ENABLED)
            Display2_SetRxAddressMode(Display2_RX_ADDRESS_MODE);
            Display2_SetRxAddress1(Display2_RX_HW_ADDRESS1);
            Display2_SetRxAddress2(Display2_RX_HW_ADDRESS2);
        #endif /* End Display2_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        Display2_RXBITCTR_PERIOD_REG = Display2_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        Display2_RXSTATUS_MASK_REG  = Display2_INIT_RX_INTERRUPTS_MASK;
    #endif /* End Display2_RX_ENABLED || Display2_HD_ENABLED*/

    #if(Display2_TX_ENABLED)
        #if (Display2_TX_INTERRUPT_ENABLED)
            /* Set TX interrupt vector and priority */
            (void) CyIntSetVector(Display2_TX_VECT_NUM, &Display2_TXISR);
            CyIntSetPriority(Display2_TX_VECT_NUM, Display2_TX_PRIOR_NUM);
        #endif /* (Display2_TX_INTERRUPT_ENABLED) */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if (Display2_TXCLKGEN_DP)
            Display2_TXBITCLKGEN_CTR_REG = Display2_BIT_CENTER;
            Display2_TXBITCLKTX_COMPLETE_REG = ((Display2_NUMBER_OF_DATA_BITS +
                        Display2_NUMBER_OF_START_BIT) * Display2_OVER_SAMPLE_COUNT) - 1u;
        #else
            Display2_TXBITCTR_PERIOD_REG = ((Display2_NUMBER_OF_DATA_BITS +
                        Display2_NUMBER_OF_START_BIT) * Display2_OVER_SAMPLE_8) - 1u;
        #endif /* End Display2_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if (Display2_TX_INTERRUPT_ENABLED)
            Display2_TXSTATUS_MASK_REG = Display2_TX_STS_FIFO_EMPTY;
        #else
            Display2_TXSTATUS_MASK_REG = Display2_INIT_TX_INTERRUPTS_MASK;
        #endif /*End Display2_TX_INTERRUPT_ENABLED*/

    #endif /* End Display2_TX_ENABLED */

    #if(Display2_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        Display2_WriteControlRegister( \
            (Display2_ReadControlRegister() & (uint8)~Display2_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(Display2_PARITY_TYPE << Display2_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End Display2_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: Display2_Enable
********************************************************************************
*
* Summary:
*  Activates the hardware and begins component operation. It is not necessary
*  to call Display2_Enable() because the Display2_Start() API
*  calls this function, which is the preferred method to begin component
*  operation.

* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Display2_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void Display2_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if (Display2_RX_ENABLED || Display2_HD_ENABLED)
        /* RX Counter (Count7) Enable */
        Display2_RXBITCTR_CONTROL_REG |= Display2_CNTR_ENABLE;

        /* Enable the RX Interrupt */
        Display2_RXSTATUS_ACTL_REG  |= Display2_INT_ENABLE;

        #if (Display2_RX_INTERRUPT_ENABLED)
            Display2_EnableRxInt();

            #if (Display2_RXHW_ADDRESS_ENABLED)
                Display2_rxAddressDetected = 0u;
            #endif /* (Display2_RXHW_ADDRESS_ENABLED) */
        #endif /* (Display2_RX_INTERRUPT_ENABLED) */
    #endif /* (Display2_RX_ENABLED || Display2_HD_ENABLED) */

    #if(Display2_TX_ENABLED)
        /* TX Counter (DP/Count7) Enable */
        #if(!Display2_TXCLKGEN_DP)
            Display2_TXBITCTR_CONTROL_REG |= Display2_CNTR_ENABLE;
        #endif /* End Display2_TXCLKGEN_DP */

        /* Enable the TX Interrupt */
        Display2_TXSTATUS_ACTL_REG |= Display2_INT_ENABLE;
        #if (Display2_TX_INTERRUPT_ENABLED)
            Display2_ClearPendingTxInt(); /* Clear history of TX_NOT_EMPTY */
            Display2_EnableTxInt();
        #endif /* (Display2_TX_INTERRUPT_ENABLED) */
     #endif /* (Display2_TX_INTERRUPT_ENABLED) */

    #if (Display2_INTERNAL_CLOCK_USED)
        Display2_IntClock_Start();  /* Enable the clock */
    #endif /* (Display2_INTERNAL_CLOCK_USED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Display2_Stop
********************************************************************************
*
* Summary:
*  Disables the UART operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Display2_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if (Display2_RX_ENABLED || Display2_HD_ENABLED)
        Display2_RXBITCTR_CONTROL_REG &= (uint8) ~Display2_CNTR_ENABLE;
    #endif /* (Display2_RX_ENABLED || Display2_HD_ENABLED) */

    #if (Display2_TX_ENABLED)
        #if(!Display2_TXCLKGEN_DP)
            Display2_TXBITCTR_CONTROL_REG &= (uint8) ~Display2_CNTR_ENABLE;
        #endif /* (!Display2_TXCLKGEN_DP) */
    #endif /* (Display2_TX_ENABLED) */

    #if (Display2_INTERNAL_CLOCK_USED)
        Display2_IntClock_Stop();   /* Disable the clock */
    #endif /* (Display2_INTERNAL_CLOCK_USED) */

    /* Disable internal interrupt component */
    #if (Display2_RX_ENABLED || Display2_HD_ENABLED)
        Display2_RXSTATUS_ACTL_REG  &= (uint8) ~Display2_INT_ENABLE;

        #if (Display2_RX_INTERRUPT_ENABLED)
            Display2_DisableRxInt();
        #endif /* (Display2_RX_INTERRUPT_ENABLED) */
    #endif /* (Display2_RX_ENABLED || Display2_HD_ENABLED) */

    #if (Display2_TX_ENABLED)
        Display2_TXSTATUS_ACTL_REG &= (uint8) ~Display2_INT_ENABLE;

        #if (Display2_TX_INTERRUPT_ENABLED)
            Display2_DisableTxInt();
        #endif /* (Display2_TX_INTERRUPT_ENABLED) */
    #endif /* (Display2_TX_ENABLED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Display2_ReadControlRegister
********************************************************************************
*
* Summary:
*  Returns the current value of the control register.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the control register.
*
*******************************************************************************/
uint8 Display2_ReadControlRegister(void) 
{
    #if (Display2_CONTROL_REG_REMOVED)
        return(0u);
    #else
        return(Display2_CONTROL_REG);
    #endif /* (Display2_CONTROL_REG_REMOVED) */
}


/*******************************************************************************
* Function Name: Display2_WriteControlRegister
********************************************************************************
*
* Summary:
*  Writes an 8-bit value into the control register
*
* Parameters:
*  control:  control register value
*
* Return:
*  None.
*
*******************************************************************************/
void  Display2_WriteControlRegister(uint8 control) 
{
    #if (Display2_CONTROL_REG_REMOVED)
        if(0u != control)
        {
            /* Suppress compiler warning */
        }
    #else
       Display2_CONTROL_REG = control;
    #endif /* (Display2_CONTROL_REG_REMOVED) */
}


#if(Display2_RX_ENABLED || Display2_HD_ENABLED)
    /*******************************************************************************
    * Function Name: Display2_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the RX interrupt sources enabled.
    *
    * Parameters:
    *  IntSrc:  Bit field containing the RX interrupts to enable. Based on the 
    *  bit-field arrangement of the status register. This value must be a 
    *  combination of status register bit-masks shown below:
    *      Display2_RX_STS_FIFO_NOTEMPTY    Interrupt on byte received.
    *      Display2_RX_STS_PAR_ERROR        Interrupt on parity error.
    *      Display2_RX_STS_STOP_ERROR       Interrupt on stop error.
    *      Display2_RX_STS_BREAK            Interrupt on break.
    *      Display2_RX_STS_OVERRUN          Interrupt on overrun error.
    *      Display2_RX_STS_ADDR_MATCH       Interrupt on address match.
    *      Display2_RX_STS_MRKSPC           Interrupt on address detect.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void Display2_SetRxInterruptMode(uint8 intSrc) 
    {
        Display2_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: Display2_ReadRxData
    ********************************************************************************
    *
    * Summary:
    *  Returns the next byte of received data. This function returns data without
    *  checking the status. You must check the status separately.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Received data from RX register
    *
    * Global Variables:
    *  Display2_rxBuffer - RAM buffer pointer for save received data.
    *  Display2_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  Display2_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  Display2_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 Display2_ReadRxData(void) 
    {
        uint8 rxData;

    #if (Display2_RX_INTERRUPT_ENABLED)

        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        Display2_DisableRxInt();

        locRxBufferRead  = Display2_rxBufferRead;
        locRxBufferWrite = Display2_rxBufferWrite;

        if( (Display2_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = Display2_rxBuffer[locRxBufferRead];
            locRxBufferRead++;

            if(locRxBufferRead >= Display2_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            Display2_rxBufferRead = locRxBufferRead;

            if(Display2_rxBufferLoopDetect != 0u)
            {
                Display2_rxBufferLoopDetect = 0u;
                #if ((Display2_RX_INTERRUPT_ENABLED) && (Display2_FLOW_CONTROL != 0u))
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( Display2_HD_ENABLED )
                        if((Display2_CONTROL_REG & Display2_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only in RX
                            *  configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            Display2_RXSTATUS_MASK_REG  |= Display2_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        Display2_RXSTATUS_MASK_REG  |= Display2_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end Display2_HD_ENABLED */
                #endif /* ((Display2_RX_INTERRUPT_ENABLED) && (Display2_FLOW_CONTROL != 0u)) */
            }
        }
        else
        {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
            rxData = Display2_RXDATA_REG;
        }

        Display2_EnableRxInt();

    #else

        /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
        rxData = Display2_RXDATA_REG;

    #endif /* (Display2_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: Display2_ReadRxStatus
    ********************************************************************************
    *
    * Summary:
    *  Returns the current state of the receiver status register and the software
    *  buffer overflow status.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Current state of the status register.
    *
    * Side Effect:
    *  All status register bits are clear-on-read except
    *  Display2_RX_STS_FIFO_NOTEMPTY.
    *  Display2_RX_STS_FIFO_NOTEMPTY clears immediately after RX data
    *  register read.
    *
    * Global Variables:
    *  Display2_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn't free space in
    *   Display2_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   Display2_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 Display2_ReadRxStatus(void) 
    {
        uint8 status;

        status = Display2_RXSTATUS_REG & Display2_RX_HW_MASK;

    #if (Display2_RX_INTERRUPT_ENABLED)
        if(Display2_rxBufferOverflow != 0u)
        {
            status |= Display2_RX_STS_SOFT_BUFF_OVER;
            Display2_rxBufferOverflow = 0u;
        }
    #endif /* (Display2_RX_INTERRUPT_ENABLED) */

        return(status);
    }


    /*******************************************************************************
    * Function Name: Display2_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Returns the last received byte of data. Display2_GetChar() is
    *  designed for ASCII characters and returns a uint8 where 1 to 255 are values
    *  for valid characters and 0 indicates an error occurred or no data is present.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Character read from UART RX buffer. ASCII characters from 1 to 255 are valid.
    *  A returned zero signifies an error condition or no data available.
    *
    * Global Variables:
    *  Display2_rxBuffer - RAM buffer pointer for save received data.
    *  Display2_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  Display2_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  Display2_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 Display2_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

    #if (Display2_RX_INTERRUPT_ENABLED)
        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        Display2_DisableRxInt();

        locRxBufferRead  = Display2_rxBufferRead;
        locRxBufferWrite = Display2_rxBufferWrite;

        if( (Display2_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = Display2_rxBuffer[locRxBufferRead];
            locRxBufferRead++;
            if(locRxBufferRead >= Display2_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            Display2_rxBufferRead = locRxBufferRead;

            if(Display2_rxBufferLoopDetect != 0u)
            {
                Display2_rxBufferLoopDetect = 0u;
                #if( (Display2_RX_INTERRUPT_ENABLED) && (Display2_FLOW_CONTROL != 0u) )
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( Display2_HD_ENABLED )
                        if((Display2_CONTROL_REG & Display2_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only if
                            *  RX configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            Display2_RXSTATUS_MASK_REG |= Display2_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        Display2_RXSTATUS_MASK_REG |= Display2_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end Display2_HD_ENABLED */
                #endif /* Display2_RX_INTERRUPT_ENABLED and Hardware flow control*/
            }

        }
        else
        {   rxStatus = Display2_RXSTATUS_REG;
            if((rxStatus & Display2_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO */
                rxData = Display2_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (Display2_RX_STS_BREAK | Display2_RX_STS_PAR_ERROR |
                                Display2_RX_STS_STOP_ERROR | Display2_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        }

        Display2_EnableRxInt();

    #else

        rxStatus =Display2_RXSTATUS_REG;
        if((rxStatus & Display2_RX_STS_FIFO_NOTEMPTY) != 0u)
        {
            /* Read received data from FIFO */
            rxData = Display2_RXDATA_REG;

            /*Check status on error*/
            if((rxStatus & (Display2_RX_STS_BREAK | Display2_RX_STS_PAR_ERROR |
                            Display2_RX_STS_STOP_ERROR | Display2_RX_STS_OVERRUN)) != 0u)
            {
                rxData = 0u;
            }
        }
    #endif /* (Display2_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: Display2_GetByte
    ********************************************************************************
    *
    * Summary:
    *  Reads UART RX buffer immediately, returns received character and error
    *  condition.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  MSB contains status and LSB contains UART RX data. If the MSB is nonzero,
    *  an error has occurred.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint16 Display2_GetByte(void) 
    {
        
    #if (Display2_RX_INTERRUPT_ENABLED)
        uint16 locErrorStatus;
        /* Protect variables that could change on interrupt */
        Display2_DisableRxInt();
        locErrorStatus = (uint16)Display2_errorStatus;
        Display2_errorStatus = 0u;
        Display2_EnableRxInt();
        return ( (uint16)(locErrorStatus << 8u) | Display2_ReadRxData() );
    #else
        return ( ((uint16)Display2_ReadRxStatus() << 8u) | Display2_ReadRxData() );
    #endif /* Display2_RX_INTERRUPT_ENABLED */
        
    }


    /*******************************************************************************
    * Function Name: Display2_GetRxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of received bytes available in the RX buffer.
    *  * RX software buffer is disabled (RX Buffer Size parameter is equal to 4): 
    *    returns 0 for empty RX FIFO or 1 for not empty RX FIFO.
    *  * RX software buffer is enabled: returns the number of bytes available in 
    *    the RX software buffer. Bytes available in the RX FIFO do not take to 
    *    account.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  uint8: Number of bytes in the RX buffer. 
    *    Return value type depends on RX Buffer Size parameter.
    *
    * Global Variables:
    *  Display2_rxBufferWrite - used to calculate left bytes.
    *  Display2_rxBufferRead - used to calculate left bytes.
    *  Display2_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint8 Display2_GetRxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (Display2_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt */
        Display2_DisableRxInt();

        if(Display2_rxBufferRead == Display2_rxBufferWrite)
        {
            if(Display2_rxBufferLoopDetect != 0u)
            {
                size = Display2_RX_BUFFER_SIZE;
            }
            else
            {
                size = 0u;
            }
        }
        else if(Display2_rxBufferRead < Display2_rxBufferWrite)
        {
            size = (Display2_rxBufferWrite - Display2_rxBufferRead);
        }
        else
        {
            size = (Display2_RX_BUFFER_SIZE - Display2_rxBufferRead) + Display2_rxBufferWrite;
        }

        Display2_EnableRxInt();

    #else

        /* We can only know if there is data in the fifo. */
        size = ((Display2_RXSTATUS_REG & Display2_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

    #endif /* (Display2_RX_INTERRUPT_ENABLED) */

        return(size);
    }


    /*******************************************************************************
    * Function Name: Display2_ClearRxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears the receiver memory buffer and hardware RX FIFO of all received data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Display2_rxBufferWrite - cleared to zero.
    *  Display2_rxBufferRead - cleared to zero.
    *  Display2_rxBufferLoopDetect - cleared to zero.
    *  Display2_rxBufferOverflow - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may
    *  have remained in the RAM.
    *
    * Side Effects:
    *  Any received data not read from the RAM or FIFO buffer will be lost.
    *
    *******************************************************************************/
    void Display2_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Clear the HW FIFO */
        enableInterrupts = CyEnterCriticalSection();
        Display2_RXDATA_AUX_CTL_REG |= (uint8)  Display2_RX_FIFO_CLR;
        Display2_RXDATA_AUX_CTL_REG &= (uint8) ~Display2_RX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (Display2_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        Display2_DisableRxInt();

        Display2_rxBufferRead = 0u;
        Display2_rxBufferWrite = 0u;
        Display2_rxBufferLoopDetect = 0u;
        Display2_rxBufferOverflow = 0u;

        Display2_EnableRxInt();

    #endif /* (Display2_RX_INTERRUPT_ENABLED) */

    }


    /*******************************************************************************
    * Function Name: Display2_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Sets the software controlled Addressing mode used by the RX portion of the
    *  UART.
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  Display2__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  Display2__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  Display2__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  Display2__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  Display2__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Display2_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  Display2_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void Display2_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(Display2_RXHW_ADDRESS_ENABLED)
            #if(Display2_CONTROL_REG_REMOVED)
                if(0u != addressMode)
                {
                    /* Suppress compiler warning */
                }
            #else /* Display2_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = Display2_CONTROL_REG & (uint8)~Display2_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << Display2_CTRL_RXADDR_MODE0_SHIFT);
                Display2_CONTROL_REG = tmpCtrl;

                #if(Display2_RX_INTERRUPT_ENABLED && \
                   (Display2_RXBUFFERSIZE > Display2_FIFO_LENGTH) )
                    Display2_rxAddressMode = addressMode;
                    Display2_rxAddressDetected = 0u;
                #endif /* End Display2_RXBUFFERSIZE > Display2_FIFO_LENGTH*/
            #endif /* End Display2_CONTROL_REG_REMOVED */
        #else /* Display2_RXHW_ADDRESS_ENABLED */
            if(0u != addressMode)
            {
                /* Suppress compiler warning */
            }
        #endif /* End Display2_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: Display2_SetRxAddress1
    ********************************************************************************
    *
    * Summary:
    *  Sets the first of two hardware-detectable receiver addresses.
    *
    * Parameters:
    *  address: Address #1 for hardware address detection.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void Display2_SetRxAddress1(uint8 address) 
    {
        Display2_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: Display2_SetRxAddress2
    ********************************************************************************
    *
    * Summary:
    *  Sets the second of two hardware-detectable receiver addresses.
    *
    * Parameters:
    *  address: Address #2 for hardware address detection.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void Display2_SetRxAddress2(uint8 address) 
    {
        Display2_RXADDRESS2_REG = address;
    }

#endif  /* Display2_RX_ENABLED || Display2_HD_ENABLED*/


#if( (Display2_TX_ENABLED) || (Display2_HD_ENABLED) )
    /*******************************************************************************
    * Function Name: Display2_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the TX interrupt sources to be enabled, but does not enable the
    *  interrupt.
    *
    * Parameters:
    *  intSrc: Bit field containing the TX interrupt sources to enable
    *   Display2_TX_STS_COMPLETE        Interrupt on TX byte complete
    *   Display2_TX_STS_FIFO_EMPTY      Interrupt when TX FIFO is empty
    *   Display2_TX_STS_FIFO_FULL       Interrupt when TX FIFO is full
    *   Display2_TX_STS_FIFO_NOT_FULL   Interrupt when TX FIFO is not full
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void Display2_SetTxInterruptMode(uint8 intSrc) 
    {
        Display2_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: Display2_WriteTxData
    ********************************************************************************
    *
    * Summary:
    *  Places a byte of data into the transmit buffer to be sent when the bus is
    *  available without checking the TX status register. You must check status
    *  separately.
    *
    * Parameters:
    *  txDataByte: data byte
    *
    * Return:
    * None.
    *
    * Global Variables:
    *  Display2_txBuffer - RAM buffer pointer for save data for transmission
    *  Display2_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  Display2_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  Display2_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void Display2_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(Display2_initVar != 0u)
        {
        #if (Display2_TX_INTERRUPT_ENABLED)

            /* Protect variables that could change on interrupt. */
            Display2_DisableTxInt();

            if( (Display2_txBufferRead == Display2_txBufferWrite) &&
                ((Display2_TXSTATUS_REG & Display2_TX_STS_FIFO_FULL) == 0u) )
            {
                /* Add directly to the FIFO. */
                Display2_TXDATA_REG = txDataByte;
            }
            else
            {
                if(Display2_txBufferWrite >= Display2_TX_BUFFER_SIZE)
                {
                    Display2_txBufferWrite = 0u;
                }

                Display2_txBuffer[Display2_txBufferWrite] = txDataByte;

                /* Add to the software buffer. */
                Display2_txBufferWrite++;
            }

            Display2_EnableTxInt();

        #else

            /* Add directly to the FIFO. */
            Display2_TXDATA_REG = txDataByte;

        #endif /*(Display2_TX_INTERRUPT_ENABLED) */
        }
    }


    /*******************************************************************************
    * Function Name: Display2_ReadTxStatus
    ********************************************************************************
    *
    * Summary:
    *  Reads the status register for the TX portion of the UART.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Contents of the status register
    *
    * Theory:
    *  This function reads the TX status register, which is cleared on read.
    *  It is up to the user to handle all bits in this return value accordingly,
    *  even if the bit was not enabled as an interrupt source the event happened
    *  and must be handled accordingly.
    *
    *******************************************************************************/
    uint8 Display2_ReadTxStatus(void) 
    {
        return(Display2_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: Display2_PutChar
    ********************************************************************************
    *
    * Summary:
    *  Puts a byte of data into the transmit buffer to be sent when the bus is
    *  available. This is a blocking API that waits until the TX buffer has room to
    *  hold the data.
    *
    * Parameters:
    *  txDataByte: Byte containing the data to transmit
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Display2_txBuffer - RAM buffer pointer for save data for transmission
    *  Display2_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  Display2_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  Display2_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void Display2_PutChar(uint8 txDataByte) 
    {
    #if (Display2_TX_INTERRUPT_ENABLED)
        /* The temporary output pointer is used since it takes two instructions
        *  to increment with a wrap, and we can't risk doing that with the real
        *  pointer and getting an interrupt in between instructions.
        */
        uint8 locTxBufferWrite;
        uint8 locTxBufferRead;

        do
        { /* Block if software buffer is full, so we don't overwrite. */

        #if ((Display2_TX_BUFFER_SIZE > Display2_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Disable TX interrupt to protect variables from modification */
            Display2_DisableTxInt();
        #endif /* (Display2_TX_BUFFER_SIZE > Display2_MAX_BYTE_VALUE) && (CY_PSOC3) */

            locTxBufferWrite = Display2_txBufferWrite;
            locTxBufferRead  = Display2_txBufferRead;

        #if ((Display2_TX_BUFFER_SIZE > Display2_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Enable interrupt to continue transmission */
            Display2_EnableTxInt();
        #endif /* (Display2_TX_BUFFER_SIZE > Display2_MAX_BYTE_VALUE) && (CY_PSOC3) */
        }
        while( (locTxBufferWrite < locTxBufferRead) ? (locTxBufferWrite == (locTxBufferRead - 1u)) :
                                ((locTxBufferWrite - locTxBufferRead) ==
                                (uint8)(Display2_TX_BUFFER_SIZE - 1u)) );

        if( (locTxBufferRead == locTxBufferWrite) &&
            ((Display2_TXSTATUS_REG & Display2_TX_STS_FIFO_FULL) == 0u) )
        {
            /* Add directly to the FIFO */
            Display2_TXDATA_REG = txDataByte;
        }
        else
        {
            if(locTxBufferWrite >= Display2_TX_BUFFER_SIZE)
            {
                locTxBufferWrite = 0u;
            }
            /* Add to the software buffer. */
            Display2_txBuffer[locTxBufferWrite] = txDataByte;
            locTxBufferWrite++;

            /* Finally, update the real output pointer */
        #if ((Display2_TX_BUFFER_SIZE > Display2_MAX_BYTE_VALUE) && (CY_PSOC3))
            Display2_DisableTxInt();
        #endif /* (Display2_TX_BUFFER_SIZE > Display2_MAX_BYTE_VALUE) && (CY_PSOC3) */

            Display2_txBufferWrite = locTxBufferWrite;

        #if ((Display2_TX_BUFFER_SIZE > Display2_MAX_BYTE_VALUE) && (CY_PSOC3))
            Display2_EnableTxInt();
        #endif /* (Display2_TX_BUFFER_SIZE > Display2_MAX_BYTE_VALUE) && (CY_PSOC3) */

            if(0u != (Display2_TXSTATUS_REG & Display2_TX_STS_FIFO_EMPTY))
            {
                /* Trigger TX interrupt to send software buffer */
                Display2_SetPendingTxInt();
            }
        }

    #else

        while((Display2_TXSTATUS_REG & Display2_TX_STS_FIFO_FULL) != 0u)
        {
            /* Wait for room in the FIFO */
        }

        /* Add directly to the FIFO */
        Display2_TXDATA_REG = txDataByte;

    #endif /* Display2_TX_INTERRUPT_ENABLED */
    }


    /*******************************************************************************
    * Function Name: Display2_PutString
    ********************************************************************************
    *
    * Summary:
    *  Sends a NULL terminated string to the TX buffer for transmission.
    *
    * Parameters:
    *  string[]: Pointer to the null terminated string array residing in RAM or ROM
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Display2_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  If there is not enough memory in the TX buffer for the entire string, this
    *  function blocks until the last character of the string is loaded into the
    *  TX buffer.
    *
    *******************************************************************************/
    void Display2_PutString(const char8 string[]) 
    {
        uint16 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(Display2_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent */
            while(string[bufIndex] != (char8) 0)
            {
                Display2_PutChar((uint8)string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: Display2_PutArray
    ********************************************************************************
    *
    * Summary:
    *  Places N bytes of data from a memory array into the TX buffer for
    *  transmission.
    *
    * Parameters:
    *  string[]: Address of the memory array residing in RAM or ROM.
    *  byteCount: Number of bytes to be transmitted. The type depends on TX Buffer
    *             Size parameter.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Display2_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  If there is not enough memory in the TX buffer for the entire string, this
    *  function blocks until the last character of the string is loaded into the
    *  TX buffer.
    *
    *******************************************************************************/
    void Display2_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(Display2_initVar != 0u)
        {
            while(bufIndex < byteCount)
            {
                Display2_PutChar(string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: Display2_PutCRLF
    ********************************************************************************
    *
    * Summary:
    *  Writes a byte of data followed by a carriage return (0x0D) and line feed
    *  (0x0A) to the transmit buffer.
    *
    * Parameters:
    *  txDataByte: Data byte to transmit before the carriage return and line feed.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Display2_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void Display2_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function */
        if(Display2_initVar != 0u)
        {
            Display2_PutChar(txDataByte);
            Display2_PutChar(0x0Du);
            Display2_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: Display2_GetTxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of bytes in the TX buffer which are waiting to be 
    *  transmitted.
    *  * TX software buffer is disabled (TX Buffer Size parameter is equal to 4): 
    *    returns 0 for empty TX FIFO, 1 for not full TX FIFO or 4 for full TX FIFO.
    *  * TX software buffer is enabled: returns the number of bytes in the TX 
    *    software buffer which are waiting to be transmitted. Bytes available in the
    *    TX FIFO do not count.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Number of bytes used in the TX buffer. Return value type depends on the TX 
    *  Buffer Size parameter.
    *
    * Global Variables:
    *  Display2_txBufferWrite - used to calculate left space.
    *  Display2_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 Display2_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (Display2_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        Display2_DisableTxInt();

        if(Display2_txBufferRead == Display2_txBufferWrite)
        {
            size = 0u;
        }
        else if(Display2_txBufferRead < Display2_txBufferWrite)
        {
            size = (Display2_txBufferWrite - Display2_txBufferRead);
        }
        else
        {
            size = (Display2_TX_BUFFER_SIZE - Display2_txBufferRead) +
                    Display2_txBufferWrite;
        }

        Display2_EnableTxInt();

    #else

        size = Display2_TXSTATUS_REG;

        /* Is the fifo is full. */
        if((size & Display2_TX_STS_FIFO_FULL) != 0u)
        {
            size = Display2_FIFO_LENGTH;
        }
        else if((size & Display2_TX_STS_FIFO_EMPTY) != 0u)
        {
            size = 0u;
        }
        else
        {
            /* We only know there is data in the fifo. */
            size = 1u;
        }

    #endif /* (Display2_TX_INTERRUPT_ENABLED) */

    return(size);
    }


    /*******************************************************************************
    * Function Name: Display2_ClearTxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears all data from the TX buffer and hardware TX FIFO.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Display2_txBufferWrite - cleared to zero.
    *  Display2_txBufferRead - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may have
    *  remained in the RAM.
    *
    * Side Effects:
    *  Data waiting in the transmit buffer is not sent; a byte that is currently
    *  transmitting finishes transmitting.
    *
    *******************************************************************************/
    void Display2_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        enableInterrupts = CyEnterCriticalSection();
        /* Clear the HW FIFO */
        Display2_TXDATA_AUX_CTL_REG |= (uint8)  Display2_TX_FIFO_CLR;
        Display2_TXDATA_AUX_CTL_REG &= (uint8) ~Display2_TX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (Display2_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        Display2_DisableTxInt();

        Display2_txBufferRead = 0u;
        Display2_txBufferWrite = 0u;

        /* Enable Tx interrupt. */
        Display2_EnableTxInt();

    #endif /* (Display2_TX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: Display2_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Transmits a break signal on the bus.
    *
    * Parameters:
    *  uint8 retMode:  Send Break return mode. See the following table for options.
    *   Display2_SEND_BREAK - Initialize registers for break, send the Break
    *       signal and return immediately.
    *   Display2_WAIT_FOR_COMPLETE_REINIT - Wait until break transmission is
    *       complete, reinitialize registers to normal transmission mode then return
    *   Display2_REINIT - Reinitialize registers to normal transmission mode
    *       then return.
    *   Display2_SEND_WAIT_REINIT - Performs both options: 
    *      Display2_SEND_BREAK and Display2_WAIT_FOR_COMPLETE_REINIT.
    *      This option is recommended for most cases.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Display2_initVar - checked to identify that the component has been
    *     initialized.
    *  txPeriod - static variable, used for keeping TX period configuration.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  SendBreak function initializes registers to send 13-bit break signal. It is
    *  important to return the registers configuration to normal for continue 8-bit
    *  operation.
    *  There are 3 variants for this API usage:
    *  1) SendBreak(3) - function will send the Break signal and take care on the
    *     configuration returning. Function will block CPU until transmission
    *     complete.
    *  2) User may want to use blocking time if UART configured to the low speed
    *     operation
    *     Example for this case:
    *     SendBreak(0);     - initialize Break signal transmission
    *         Add your code here to use CPU time
    *     SendBreak(1);     - complete Break operation
    *  3) Same to 2) but user may want to initialize and use the interrupt to
    *     complete break operation.
    *     Example for this case:
    *     Initialize TX interrupt with "TX - On TX Complete" parameter
    *     SendBreak(0);     - initialize Break signal transmission
    *         Add your code here to use CPU time
    *     When interrupt appear with Display2_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *  The Display2_SendBreak() function initializes registers to send a
    *  break signal.
    *  Break signal length depends on the break signal bits configuration.
    *  The register configuration should be reinitialized before normal 8-bit
    *  communication can continue.
    *
    *******************************************************************************/
    void Display2_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(Display2_initVar != 0u)
        {
            /* Set the Counter to 13-bits and transmit a 00 byte */
            /* When that is done then reset the counter value back */
            uint8 tmpStat;

        #if(Display2_HD_ENABLED) /* Half Duplex mode*/

            if( (retMode == Display2_SEND_BREAK) ||
                (retMode == Display2_SEND_WAIT_REINIT ) )
            {
                /* CTRL_HD_SEND_BREAK - sends break bits in HD mode */
                Display2_WriteControlRegister(Display2_ReadControlRegister() |
                                                      Display2_CTRL_HD_SEND_BREAK);
                /* Send zeros */
                Display2_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = Display2_TXSTATUS_REG;
                }
                while((tmpStat & Display2_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == Display2_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Display2_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = Display2_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & Display2_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == Display2_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Display2_REINIT) ||
                (retMode == Display2_SEND_WAIT_REINIT) )
            {
                Display2_WriteControlRegister(Display2_ReadControlRegister() &
                                              (uint8)~Display2_CTRL_HD_SEND_BREAK);
            }

        #else /* Display2_HD_ENABLED Full Duplex mode */

            static uint8 txPeriod;

            if( (retMode == Display2_SEND_BREAK) ||
                (retMode == Display2_SEND_WAIT_REINIT) )
            {
                /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode */
                #if( (Display2_PARITY_TYPE != Display2__B_UART__NONE_REVB) || \
                                    (Display2_PARITY_TYPE_SW != 0u) )
                    Display2_WriteControlRegister(Display2_ReadControlRegister() |
                                                          Display2_CTRL_HD_SEND_BREAK);
                #endif /* End Display2_PARITY_TYPE != Display2__B_UART__NONE_REVB  */

                #if(Display2_TXCLKGEN_DP)
                    txPeriod = Display2_TXBITCLKTX_COMPLETE_REG;
                    Display2_TXBITCLKTX_COMPLETE_REG = Display2_TXBITCTR_BREAKBITS;
                #else
                    txPeriod = Display2_TXBITCTR_PERIOD_REG;
                    Display2_TXBITCTR_PERIOD_REG = Display2_TXBITCTR_BREAKBITS8X;
                #endif /* End Display2_TXCLKGEN_DP */

                /* Send zeros */
                Display2_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = Display2_TXSTATUS_REG;
                }
                while((tmpStat & Display2_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == Display2_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Display2_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = Display2_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & Display2_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == Display2_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Display2_REINIT) ||
                (retMode == Display2_SEND_WAIT_REINIT) )
            {

            #if(Display2_TXCLKGEN_DP)
                Display2_TXBITCLKTX_COMPLETE_REG = txPeriod;
            #else
                Display2_TXBITCTR_PERIOD_REG = txPeriod;
            #endif /* End Display2_TXCLKGEN_DP */

            #if( (Display2_PARITY_TYPE != Display2__B_UART__NONE_REVB) || \
                 (Display2_PARITY_TYPE_SW != 0u) )
                Display2_WriteControlRegister(Display2_ReadControlRegister() &
                                                      (uint8) ~Display2_CTRL_HD_SEND_BREAK);
            #endif /* End Display2_PARITY_TYPE != NONE */
            }
        #endif    /* End Display2_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: Display2_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the transmitter to signal the next bytes is address or data.
    *
    * Parameters:
    *  addressMode: 
    *       Display2_SET_SPACE - Configure the transmitter to send the next
    *                                    byte as a data.
    *       Display2_SET_MARK  - Configure the transmitter to send the next
    *                                    byte as an address.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  This function sets and clears Display2_CTRL_MARK bit in the Control
    *  register.
    *
    *******************************************************************************/
    void Display2_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable */
        if(addressMode != 0u)
        {
        #if( Display2_CONTROL_REG_REMOVED == 0u )
            Display2_WriteControlRegister(Display2_ReadControlRegister() |
                                                  Display2_CTRL_MARK);
        #endif /* End Display2_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
        #if( Display2_CONTROL_REG_REMOVED == 0u )
            Display2_WriteControlRegister(Display2_ReadControlRegister() &
                                                  (uint8) ~Display2_CTRL_MARK);
        #endif /* End Display2_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndDisplay2_TX_ENABLED */

#if(Display2_HD_ENABLED)


    /*******************************************************************************
    * Function Name: Display2_LoadRxConfig
    ********************************************************************************
    *
    * Summary:
    *  Loads the receiver configuration in half duplex mode. After calling this
    *  function, the UART is ready to receive data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  Valid only in half duplex mode. You must make sure that the previous
    *  transaction is complete and it is safe to unload the transmitter
    *  configuration.
    *
    *******************************************************************************/
    void Display2_LoadRxConfig(void) 
    {
        Display2_WriteControlRegister(Display2_ReadControlRegister() &
                                                (uint8)~Display2_CTRL_HD_SEND);
        Display2_RXBITCTR_PERIOD_REG = Display2_HD_RXBITCTR_INIT;

    #if (Display2_RX_INTERRUPT_ENABLED)
        /* Enable RX interrupt after set RX configuration */
        Display2_SetRxInterruptMode(Display2_INIT_RX_INTERRUPTS_MASK);
    #endif /* (Display2_RX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: Display2_LoadTxConfig
    ********************************************************************************
    *
    * Summary:
    *  Loads the transmitter configuration in half duplex mode. After calling this
    *  function, the UART is ready to transmit data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  Valid only in half duplex mode. You must make sure that the previous
    *  transaction is complete and it is safe to unload the receiver configuration.
    *
    *******************************************************************************/
    void Display2_LoadTxConfig(void) 
    {
    #if (Display2_RX_INTERRUPT_ENABLED)
        /* Disable RX interrupts before set TX configuration */
        Display2_SetRxInterruptMode(0u);
    #endif /* (Display2_RX_INTERRUPT_ENABLED) */

        Display2_WriteControlRegister(Display2_ReadControlRegister() | Display2_CTRL_HD_SEND);
        Display2_RXBITCTR_PERIOD_REG = Display2_HD_TXBITCTR_INIT;
    }

#endif  /* Display2_HD_ENABLED */


/* [] END OF FILE */
