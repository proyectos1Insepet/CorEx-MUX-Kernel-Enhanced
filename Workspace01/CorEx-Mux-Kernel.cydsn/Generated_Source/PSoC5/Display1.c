/*******************************************************************************
* File Name: Display1.c
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

#include "Display1.h"
#if (Display1_INTERNAL_CLOCK_USED)
    #include "Display1_IntClock.h"
#endif /* End Display1_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 Display1_initVar = 0u;

#if (Display1_TX_INTERRUPT_ENABLED && Display1_TX_ENABLED)
    volatile uint8 Display1_txBuffer[Display1_TX_BUFFER_SIZE];
    volatile uint8 Display1_txBufferRead = 0u;
    uint8 Display1_txBufferWrite = 0u;
#endif /* (Display1_TX_INTERRUPT_ENABLED && Display1_TX_ENABLED) */

#if (Display1_RX_INTERRUPT_ENABLED && (Display1_RX_ENABLED || Display1_HD_ENABLED))
    uint8 Display1_errorStatus = 0u;
    volatile uint8 Display1_rxBuffer[Display1_RX_BUFFER_SIZE];
    volatile uint8 Display1_rxBufferRead  = 0u;
    volatile uint8 Display1_rxBufferWrite = 0u;
    volatile uint8 Display1_rxBufferLoopDetect = 0u;
    volatile uint8 Display1_rxBufferOverflow   = 0u;
    #if (Display1_RXHW_ADDRESS_ENABLED)
        volatile uint8 Display1_rxAddressMode = Display1_RX_ADDRESS_MODE;
        volatile uint8 Display1_rxAddressDetected = 0u;
    #endif /* (Display1_RXHW_ADDRESS_ENABLED) */
#endif /* (Display1_RX_INTERRUPT_ENABLED && (Display1_RX_ENABLED || Display1_HD_ENABLED)) */


/*******************************************************************************
* Function Name: Display1_Start
********************************************************************************
*
* Summary:
*  This is the preferred method to begin component operation.
*  Display1_Start() sets the initVar variable, calls the
*  Display1_Init() function, and then calls the
*  Display1_Enable() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The Display1_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time Display1_Start() is called. This
*  allows for component initialization without re-initialization in all
*  subsequent calls to the Display1_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Display1_Start(void) 
{
    /* If not initialized then initialize all required hardware and software */
    if(Display1_initVar == 0u)
    {
        Display1_Init();
        Display1_initVar = 1u;
    }

    Display1_Enable();
}


/*******************************************************************************
* Function Name: Display1_Init
********************************************************************************
*
* Summary:
*  Initializes or restores the component according to the customizer Configure
*  dialog settings. It is not necessary to call Display1_Init() because
*  the Display1_Start() API calls this function and is the preferred
*  method to begin component operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Display1_Init(void) 
{
    #if(Display1_RX_ENABLED || Display1_HD_ENABLED)

        #if (Display1_RX_INTERRUPT_ENABLED)
            /* Set RX interrupt vector and priority */
            (void) CyIntSetVector(Display1_RX_VECT_NUM, &Display1_RXISR);
            CyIntSetPriority(Display1_RX_VECT_NUM, Display1_RX_PRIOR_NUM);
            Display1_errorStatus = 0u;
        #endif /* (Display1_RX_INTERRUPT_ENABLED) */

        #if (Display1_RXHW_ADDRESS_ENABLED)
            Display1_SetRxAddressMode(Display1_RX_ADDRESS_MODE);
            Display1_SetRxAddress1(Display1_RX_HW_ADDRESS1);
            Display1_SetRxAddress2(Display1_RX_HW_ADDRESS2);
        #endif /* End Display1_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        Display1_RXBITCTR_PERIOD_REG = Display1_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        Display1_RXSTATUS_MASK_REG  = Display1_INIT_RX_INTERRUPTS_MASK;
    #endif /* End Display1_RX_ENABLED || Display1_HD_ENABLED*/

    #if(Display1_TX_ENABLED)
        #if (Display1_TX_INTERRUPT_ENABLED)
            /* Set TX interrupt vector and priority */
            (void) CyIntSetVector(Display1_TX_VECT_NUM, &Display1_TXISR);
            CyIntSetPriority(Display1_TX_VECT_NUM, Display1_TX_PRIOR_NUM);
        #endif /* (Display1_TX_INTERRUPT_ENABLED) */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if (Display1_TXCLKGEN_DP)
            Display1_TXBITCLKGEN_CTR_REG = Display1_BIT_CENTER;
            Display1_TXBITCLKTX_COMPLETE_REG = ((Display1_NUMBER_OF_DATA_BITS +
                        Display1_NUMBER_OF_START_BIT) * Display1_OVER_SAMPLE_COUNT) - 1u;
        #else
            Display1_TXBITCTR_PERIOD_REG = ((Display1_NUMBER_OF_DATA_BITS +
                        Display1_NUMBER_OF_START_BIT) * Display1_OVER_SAMPLE_8) - 1u;
        #endif /* End Display1_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if (Display1_TX_INTERRUPT_ENABLED)
            Display1_TXSTATUS_MASK_REG = Display1_TX_STS_FIFO_EMPTY;
        #else
            Display1_TXSTATUS_MASK_REG = Display1_INIT_TX_INTERRUPTS_MASK;
        #endif /*End Display1_TX_INTERRUPT_ENABLED*/

    #endif /* End Display1_TX_ENABLED */

    #if(Display1_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        Display1_WriteControlRegister( \
            (Display1_ReadControlRegister() & (uint8)~Display1_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(Display1_PARITY_TYPE << Display1_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End Display1_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: Display1_Enable
********************************************************************************
*
* Summary:
*  Activates the hardware and begins component operation. It is not necessary
*  to call Display1_Enable() because the Display1_Start() API
*  calls this function, which is the preferred method to begin component
*  operation.

* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Display1_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void Display1_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if (Display1_RX_ENABLED || Display1_HD_ENABLED)
        /* RX Counter (Count7) Enable */
        Display1_RXBITCTR_CONTROL_REG |= Display1_CNTR_ENABLE;

        /* Enable the RX Interrupt */
        Display1_RXSTATUS_ACTL_REG  |= Display1_INT_ENABLE;

        #if (Display1_RX_INTERRUPT_ENABLED)
            Display1_EnableRxInt();

            #if (Display1_RXHW_ADDRESS_ENABLED)
                Display1_rxAddressDetected = 0u;
            #endif /* (Display1_RXHW_ADDRESS_ENABLED) */
        #endif /* (Display1_RX_INTERRUPT_ENABLED) */
    #endif /* (Display1_RX_ENABLED || Display1_HD_ENABLED) */

    #if(Display1_TX_ENABLED)
        /* TX Counter (DP/Count7) Enable */
        #if(!Display1_TXCLKGEN_DP)
            Display1_TXBITCTR_CONTROL_REG |= Display1_CNTR_ENABLE;
        #endif /* End Display1_TXCLKGEN_DP */

        /* Enable the TX Interrupt */
        Display1_TXSTATUS_ACTL_REG |= Display1_INT_ENABLE;
        #if (Display1_TX_INTERRUPT_ENABLED)
            Display1_ClearPendingTxInt(); /* Clear history of TX_NOT_EMPTY */
            Display1_EnableTxInt();
        #endif /* (Display1_TX_INTERRUPT_ENABLED) */
     #endif /* (Display1_TX_INTERRUPT_ENABLED) */

    #if (Display1_INTERNAL_CLOCK_USED)
        Display1_IntClock_Start();  /* Enable the clock */
    #endif /* (Display1_INTERNAL_CLOCK_USED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Display1_Stop
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
void Display1_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if (Display1_RX_ENABLED || Display1_HD_ENABLED)
        Display1_RXBITCTR_CONTROL_REG &= (uint8) ~Display1_CNTR_ENABLE;
    #endif /* (Display1_RX_ENABLED || Display1_HD_ENABLED) */

    #if (Display1_TX_ENABLED)
        #if(!Display1_TXCLKGEN_DP)
            Display1_TXBITCTR_CONTROL_REG &= (uint8) ~Display1_CNTR_ENABLE;
        #endif /* (!Display1_TXCLKGEN_DP) */
    #endif /* (Display1_TX_ENABLED) */

    #if (Display1_INTERNAL_CLOCK_USED)
        Display1_IntClock_Stop();   /* Disable the clock */
    #endif /* (Display1_INTERNAL_CLOCK_USED) */

    /* Disable internal interrupt component */
    #if (Display1_RX_ENABLED || Display1_HD_ENABLED)
        Display1_RXSTATUS_ACTL_REG  &= (uint8) ~Display1_INT_ENABLE;

        #if (Display1_RX_INTERRUPT_ENABLED)
            Display1_DisableRxInt();
        #endif /* (Display1_RX_INTERRUPT_ENABLED) */
    #endif /* (Display1_RX_ENABLED || Display1_HD_ENABLED) */

    #if (Display1_TX_ENABLED)
        Display1_TXSTATUS_ACTL_REG &= (uint8) ~Display1_INT_ENABLE;

        #if (Display1_TX_INTERRUPT_ENABLED)
            Display1_DisableTxInt();
        #endif /* (Display1_TX_INTERRUPT_ENABLED) */
    #endif /* (Display1_TX_ENABLED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Display1_ReadControlRegister
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
uint8 Display1_ReadControlRegister(void) 
{
    #if (Display1_CONTROL_REG_REMOVED)
        return(0u);
    #else
        return(Display1_CONTROL_REG);
    #endif /* (Display1_CONTROL_REG_REMOVED) */
}


/*******************************************************************************
* Function Name: Display1_WriteControlRegister
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
void  Display1_WriteControlRegister(uint8 control) 
{
    #if (Display1_CONTROL_REG_REMOVED)
        if(0u != control)
        {
            /* Suppress compiler warning */
        }
    #else
       Display1_CONTROL_REG = control;
    #endif /* (Display1_CONTROL_REG_REMOVED) */
}


#if(Display1_RX_ENABLED || Display1_HD_ENABLED)
    /*******************************************************************************
    * Function Name: Display1_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the RX interrupt sources enabled.
    *
    * Parameters:
    *  IntSrc:  Bit field containing the RX interrupts to enable. Based on the 
    *  bit-field arrangement of the status register. This value must be a 
    *  combination of status register bit-masks shown below:
    *      Display1_RX_STS_FIFO_NOTEMPTY    Interrupt on byte received.
    *      Display1_RX_STS_PAR_ERROR        Interrupt on parity error.
    *      Display1_RX_STS_STOP_ERROR       Interrupt on stop error.
    *      Display1_RX_STS_BREAK            Interrupt on break.
    *      Display1_RX_STS_OVERRUN          Interrupt on overrun error.
    *      Display1_RX_STS_ADDR_MATCH       Interrupt on address match.
    *      Display1_RX_STS_MRKSPC           Interrupt on address detect.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void Display1_SetRxInterruptMode(uint8 intSrc) 
    {
        Display1_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: Display1_ReadRxData
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
    *  Display1_rxBuffer - RAM buffer pointer for save received data.
    *  Display1_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  Display1_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  Display1_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 Display1_ReadRxData(void) 
    {
        uint8 rxData;

    #if (Display1_RX_INTERRUPT_ENABLED)

        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        Display1_DisableRxInt();

        locRxBufferRead  = Display1_rxBufferRead;
        locRxBufferWrite = Display1_rxBufferWrite;

        if( (Display1_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = Display1_rxBuffer[locRxBufferRead];
            locRxBufferRead++;

            if(locRxBufferRead >= Display1_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            Display1_rxBufferRead = locRxBufferRead;

            if(Display1_rxBufferLoopDetect != 0u)
            {
                Display1_rxBufferLoopDetect = 0u;
                #if ((Display1_RX_INTERRUPT_ENABLED) && (Display1_FLOW_CONTROL != 0u))
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( Display1_HD_ENABLED )
                        if((Display1_CONTROL_REG & Display1_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only in RX
                            *  configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            Display1_RXSTATUS_MASK_REG  |= Display1_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        Display1_RXSTATUS_MASK_REG  |= Display1_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end Display1_HD_ENABLED */
                #endif /* ((Display1_RX_INTERRUPT_ENABLED) && (Display1_FLOW_CONTROL != 0u)) */
            }
        }
        else
        {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
            rxData = Display1_RXDATA_REG;
        }

        Display1_EnableRxInt();

    #else

        /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
        rxData = Display1_RXDATA_REG;

    #endif /* (Display1_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: Display1_ReadRxStatus
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
    *  Display1_RX_STS_FIFO_NOTEMPTY.
    *  Display1_RX_STS_FIFO_NOTEMPTY clears immediately after RX data
    *  register read.
    *
    * Global Variables:
    *  Display1_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn't free space in
    *   Display1_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   Display1_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 Display1_ReadRxStatus(void) 
    {
        uint8 status;

        status = Display1_RXSTATUS_REG & Display1_RX_HW_MASK;

    #if (Display1_RX_INTERRUPT_ENABLED)
        if(Display1_rxBufferOverflow != 0u)
        {
            status |= Display1_RX_STS_SOFT_BUFF_OVER;
            Display1_rxBufferOverflow = 0u;
        }
    #endif /* (Display1_RX_INTERRUPT_ENABLED) */

        return(status);
    }


    /*******************************************************************************
    * Function Name: Display1_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Returns the last received byte of data. Display1_GetChar() is
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
    *  Display1_rxBuffer - RAM buffer pointer for save received data.
    *  Display1_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  Display1_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  Display1_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 Display1_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

    #if (Display1_RX_INTERRUPT_ENABLED)
        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        Display1_DisableRxInt();

        locRxBufferRead  = Display1_rxBufferRead;
        locRxBufferWrite = Display1_rxBufferWrite;

        if( (Display1_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = Display1_rxBuffer[locRxBufferRead];
            locRxBufferRead++;
            if(locRxBufferRead >= Display1_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            Display1_rxBufferRead = locRxBufferRead;

            if(Display1_rxBufferLoopDetect != 0u)
            {
                Display1_rxBufferLoopDetect = 0u;
                #if( (Display1_RX_INTERRUPT_ENABLED) && (Display1_FLOW_CONTROL != 0u) )
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( Display1_HD_ENABLED )
                        if((Display1_CONTROL_REG & Display1_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only if
                            *  RX configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            Display1_RXSTATUS_MASK_REG |= Display1_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        Display1_RXSTATUS_MASK_REG |= Display1_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end Display1_HD_ENABLED */
                #endif /* Display1_RX_INTERRUPT_ENABLED and Hardware flow control*/
            }

        }
        else
        {   rxStatus = Display1_RXSTATUS_REG;
            if((rxStatus & Display1_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO */
                rxData = Display1_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (Display1_RX_STS_BREAK | Display1_RX_STS_PAR_ERROR |
                                Display1_RX_STS_STOP_ERROR | Display1_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        }

        Display1_EnableRxInt();

    #else

        rxStatus =Display1_RXSTATUS_REG;
        if((rxStatus & Display1_RX_STS_FIFO_NOTEMPTY) != 0u)
        {
            /* Read received data from FIFO */
            rxData = Display1_RXDATA_REG;

            /*Check status on error*/
            if((rxStatus & (Display1_RX_STS_BREAK | Display1_RX_STS_PAR_ERROR |
                            Display1_RX_STS_STOP_ERROR | Display1_RX_STS_OVERRUN)) != 0u)
            {
                rxData = 0u;
            }
        }
    #endif /* (Display1_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: Display1_GetByte
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
    uint16 Display1_GetByte(void) 
    {
        
    #if (Display1_RX_INTERRUPT_ENABLED)
        uint16 locErrorStatus;
        /* Protect variables that could change on interrupt */
        Display1_DisableRxInt();
        locErrorStatus = (uint16)Display1_errorStatus;
        Display1_errorStatus = 0u;
        Display1_EnableRxInt();
        return ( (uint16)(locErrorStatus << 8u) | Display1_ReadRxData() );
    #else
        return ( ((uint16)Display1_ReadRxStatus() << 8u) | Display1_ReadRxData() );
    #endif /* Display1_RX_INTERRUPT_ENABLED */
        
    }


    /*******************************************************************************
    * Function Name: Display1_GetRxBufferSize
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
    *  Display1_rxBufferWrite - used to calculate left bytes.
    *  Display1_rxBufferRead - used to calculate left bytes.
    *  Display1_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint8 Display1_GetRxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (Display1_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt */
        Display1_DisableRxInt();

        if(Display1_rxBufferRead == Display1_rxBufferWrite)
        {
            if(Display1_rxBufferLoopDetect != 0u)
            {
                size = Display1_RX_BUFFER_SIZE;
            }
            else
            {
                size = 0u;
            }
        }
        else if(Display1_rxBufferRead < Display1_rxBufferWrite)
        {
            size = (Display1_rxBufferWrite - Display1_rxBufferRead);
        }
        else
        {
            size = (Display1_RX_BUFFER_SIZE - Display1_rxBufferRead) + Display1_rxBufferWrite;
        }

        Display1_EnableRxInt();

    #else

        /* We can only know if there is data in the fifo. */
        size = ((Display1_RXSTATUS_REG & Display1_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

    #endif /* (Display1_RX_INTERRUPT_ENABLED) */

        return(size);
    }


    /*******************************************************************************
    * Function Name: Display1_ClearRxBuffer
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
    *  Display1_rxBufferWrite - cleared to zero.
    *  Display1_rxBufferRead - cleared to zero.
    *  Display1_rxBufferLoopDetect - cleared to zero.
    *  Display1_rxBufferOverflow - cleared to zero.
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
    void Display1_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Clear the HW FIFO */
        enableInterrupts = CyEnterCriticalSection();
        Display1_RXDATA_AUX_CTL_REG |= (uint8)  Display1_RX_FIFO_CLR;
        Display1_RXDATA_AUX_CTL_REG &= (uint8) ~Display1_RX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (Display1_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        Display1_DisableRxInt();

        Display1_rxBufferRead = 0u;
        Display1_rxBufferWrite = 0u;
        Display1_rxBufferLoopDetect = 0u;
        Display1_rxBufferOverflow = 0u;

        Display1_EnableRxInt();

    #endif /* (Display1_RX_INTERRUPT_ENABLED) */

    }


    /*******************************************************************************
    * Function Name: Display1_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Sets the software controlled Addressing mode used by the RX portion of the
    *  UART.
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  Display1__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  Display1__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  Display1__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  Display1__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  Display1__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Display1_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  Display1_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void Display1_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(Display1_RXHW_ADDRESS_ENABLED)
            #if(Display1_CONTROL_REG_REMOVED)
                if(0u != addressMode)
                {
                    /* Suppress compiler warning */
                }
            #else /* Display1_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = Display1_CONTROL_REG & (uint8)~Display1_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << Display1_CTRL_RXADDR_MODE0_SHIFT);
                Display1_CONTROL_REG = tmpCtrl;

                #if(Display1_RX_INTERRUPT_ENABLED && \
                   (Display1_RXBUFFERSIZE > Display1_FIFO_LENGTH) )
                    Display1_rxAddressMode = addressMode;
                    Display1_rxAddressDetected = 0u;
                #endif /* End Display1_RXBUFFERSIZE > Display1_FIFO_LENGTH*/
            #endif /* End Display1_CONTROL_REG_REMOVED */
        #else /* Display1_RXHW_ADDRESS_ENABLED */
            if(0u != addressMode)
            {
                /* Suppress compiler warning */
            }
        #endif /* End Display1_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: Display1_SetRxAddress1
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
    void Display1_SetRxAddress1(uint8 address) 
    {
        Display1_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: Display1_SetRxAddress2
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
    void Display1_SetRxAddress2(uint8 address) 
    {
        Display1_RXADDRESS2_REG = address;
    }

#endif  /* Display1_RX_ENABLED || Display1_HD_ENABLED*/


#if( (Display1_TX_ENABLED) || (Display1_HD_ENABLED) )
    /*******************************************************************************
    * Function Name: Display1_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the TX interrupt sources to be enabled, but does not enable the
    *  interrupt.
    *
    * Parameters:
    *  intSrc: Bit field containing the TX interrupt sources to enable
    *   Display1_TX_STS_COMPLETE        Interrupt on TX byte complete
    *   Display1_TX_STS_FIFO_EMPTY      Interrupt when TX FIFO is empty
    *   Display1_TX_STS_FIFO_FULL       Interrupt when TX FIFO is full
    *   Display1_TX_STS_FIFO_NOT_FULL   Interrupt when TX FIFO is not full
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void Display1_SetTxInterruptMode(uint8 intSrc) 
    {
        Display1_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: Display1_WriteTxData
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
    *  Display1_txBuffer - RAM buffer pointer for save data for transmission
    *  Display1_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  Display1_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  Display1_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void Display1_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(Display1_initVar != 0u)
        {
        #if (Display1_TX_INTERRUPT_ENABLED)

            /* Protect variables that could change on interrupt. */
            Display1_DisableTxInt();

            if( (Display1_txBufferRead == Display1_txBufferWrite) &&
                ((Display1_TXSTATUS_REG & Display1_TX_STS_FIFO_FULL) == 0u) )
            {
                /* Add directly to the FIFO. */
                Display1_TXDATA_REG = txDataByte;
            }
            else
            {
                if(Display1_txBufferWrite >= Display1_TX_BUFFER_SIZE)
                {
                    Display1_txBufferWrite = 0u;
                }

                Display1_txBuffer[Display1_txBufferWrite] = txDataByte;

                /* Add to the software buffer. */
                Display1_txBufferWrite++;
            }

            Display1_EnableTxInt();

        #else

            /* Add directly to the FIFO. */
            Display1_TXDATA_REG = txDataByte;

        #endif /*(Display1_TX_INTERRUPT_ENABLED) */
        }
    }


    /*******************************************************************************
    * Function Name: Display1_ReadTxStatus
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
    uint8 Display1_ReadTxStatus(void) 
    {
        return(Display1_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: Display1_PutChar
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
    *  Display1_txBuffer - RAM buffer pointer for save data for transmission
    *  Display1_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  Display1_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  Display1_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void Display1_PutChar(uint8 txDataByte) 
    {
    #if (Display1_TX_INTERRUPT_ENABLED)
        /* The temporary output pointer is used since it takes two instructions
        *  to increment with a wrap, and we can't risk doing that with the real
        *  pointer and getting an interrupt in between instructions.
        */
        uint8 locTxBufferWrite;
        uint8 locTxBufferRead;

        do
        { /* Block if software buffer is full, so we don't overwrite. */

        #if ((Display1_TX_BUFFER_SIZE > Display1_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Disable TX interrupt to protect variables from modification */
            Display1_DisableTxInt();
        #endif /* (Display1_TX_BUFFER_SIZE > Display1_MAX_BYTE_VALUE) && (CY_PSOC3) */

            locTxBufferWrite = Display1_txBufferWrite;
            locTxBufferRead  = Display1_txBufferRead;

        #if ((Display1_TX_BUFFER_SIZE > Display1_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Enable interrupt to continue transmission */
            Display1_EnableTxInt();
        #endif /* (Display1_TX_BUFFER_SIZE > Display1_MAX_BYTE_VALUE) && (CY_PSOC3) */
        }
        while( (locTxBufferWrite < locTxBufferRead) ? (locTxBufferWrite == (locTxBufferRead - 1u)) :
                                ((locTxBufferWrite - locTxBufferRead) ==
                                (uint8)(Display1_TX_BUFFER_SIZE - 1u)) );

        if( (locTxBufferRead == locTxBufferWrite) &&
            ((Display1_TXSTATUS_REG & Display1_TX_STS_FIFO_FULL) == 0u) )
        {
            /* Add directly to the FIFO */
            Display1_TXDATA_REG = txDataByte;
        }
        else
        {
            if(locTxBufferWrite >= Display1_TX_BUFFER_SIZE)
            {
                locTxBufferWrite = 0u;
            }
            /* Add to the software buffer. */
            Display1_txBuffer[locTxBufferWrite] = txDataByte;
            locTxBufferWrite++;

            /* Finally, update the real output pointer */
        #if ((Display1_TX_BUFFER_SIZE > Display1_MAX_BYTE_VALUE) && (CY_PSOC3))
            Display1_DisableTxInt();
        #endif /* (Display1_TX_BUFFER_SIZE > Display1_MAX_BYTE_VALUE) && (CY_PSOC3) */

            Display1_txBufferWrite = locTxBufferWrite;

        #if ((Display1_TX_BUFFER_SIZE > Display1_MAX_BYTE_VALUE) && (CY_PSOC3))
            Display1_EnableTxInt();
        #endif /* (Display1_TX_BUFFER_SIZE > Display1_MAX_BYTE_VALUE) && (CY_PSOC3) */

            if(0u != (Display1_TXSTATUS_REG & Display1_TX_STS_FIFO_EMPTY))
            {
                /* Trigger TX interrupt to send software buffer */
                Display1_SetPendingTxInt();
            }
        }

    #else

        while((Display1_TXSTATUS_REG & Display1_TX_STS_FIFO_FULL) != 0u)
        {
            /* Wait for room in the FIFO */
        }

        /* Add directly to the FIFO */
        Display1_TXDATA_REG = txDataByte;

    #endif /* Display1_TX_INTERRUPT_ENABLED */
    }


    /*******************************************************************************
    * Function Name: Display1_PutString
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
    *  Display1_initVar - checked to identify that the component has been
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
    void Display1_PutString(const char8 string[]) 
    {
        uint16 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(Display1_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent */
            while(string[bufIndex] != (char8) 0)
            {
                Display1_PutChar((uint8)string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: Display1_PutArray
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
    *  Display1_initVar - checked to identify that the component has been
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
    void Display1_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(Display1_initVar != 0u)
        {
            while(bufIndex < byteCount)
            {
                Display1_PutChar(string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: Display1_PutCRLF
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
    *  Display1_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void Display1_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function */
        if(Display1_initVar != 0u)
        {
            Display1_PutChar(txDataByte);
            Display1_PutChar(0x0Du);
            Display1_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: Display1_GetTxBufferSize
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
    *  Display1_txBufferWrite - used to calculate left space.
    *  Display1_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 Display1_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (Display1_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        Display1_DisableTxInt();

        if(Display1_txBufferRead == Display1_txBufferWrite)
        {
            size = 0u;
        }
        else if(Display1_txBufferRead < Display1_txBufferWrite)
        {
            size = (Display1_txBufferWrite - Display1_txBufferRead);
        }
        else
        {
            size = (Display1_TX_BUFFER_SIZE - Display1_txBufferRead) +
                    Display1_txBufferWrite;
        }

        Display1_EnableTxInt();

    #else

        size = Display1_TXSTATUS_REG;

        /* Is the fifo is full. */
        if((size & Display1_TX_STS_FIFO_FULL) != 0u)
        {
            size = Display1_FIFO_LENGTH;
        }
        else if((size & Display1_TX_STS_FIFO_EMPTY) != 0u)
        {
            size = 0u;
        }
        else
        {
            /* We only know there is data in the fifo. */
            size = 1u;
        }

    #endif /* (Display1_TX_INTERRUPT_ENABLED) */

    return(size);
    }


    /*******************************************************************************
    * Function Name: Display1_ClearTxBuffer
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
    *  Display1_txBufferWrite - cleared to zero.
    *  Display1_txBufferRead - cleared to zero.
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
    void Display1_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        enableInterrupts = CyEnterCriticalSection();
        /* Clear the HW FIFO */
        Display1_TXDATA_AUX_CTL_REG |= (uint8)  Display1_TX_FIFO_CLR;
        Display1_TXDATA_AUX_CTL_REG &= (uint8) ~Display1_TX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (Display1_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        Display1_DisableTxInt();

        Display1_txBufferRead = 0u;
        Display1_txBufferWrite = 0u;

        /* Enable Tx interrupt. */
        Display1_EnableTxInt();

    #endif /* (Display1_TX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: Display1_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Transmits a break signal on the bus.
    *
    * Parameters:
    *  uint8 retMode:  Send Break return mode. See the following table for options.
    *   Display1_SEND_BREAK - Initialize registers for break, send the Break
    *       signal and return immediately.
    *   Display1_WAIT_FOR_COMPLETE_REINIT - Wait until break transmission is
    *       complete, reinitialize registers to normal transmission mode then return
    *   Display1_REINIT - Reinitialize registers to normal transmission mode
    *       then return.
    *   Display1_SEND_WAIT_REINIT - Performs both options: 
    *      Display1_SEND_BREAK and Display1_WAIT_FOR_COMPLETE_REINIT.
    *      This option is recommended for most cases.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Display1_initVar - checked to identify that the component has been
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
    *     When interrupt appear with Display1_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *  The Display1_SendBreak() function initializes registers to send a
    *  break signal.
    *  Break signal length depends on the break signal bits configuration.
    *  The register configuration should be reinitialized before normal 8-bit
    *  communication can continue.
    *
    *******************************************************************************/
    void Display1_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(Display1_initVar != 0u)
        {
            /* Set the Counter to 13-bits and transmit a 00 byte */
            /* When that is done then reset the counter value back */
            uint8 tmpStat;

        #if(Display1_HD_ENABLED) /* Half Duplex mode*/

            if( (retMode == Display1_SEND_BREAK) ||
                (retMode == Display1_SEND_WAIT_REINIT ) )
            {
                /* CTRL_HD_SEND_BREAK - sends break bits in HD mode */
                Display1_WriteControlRegister(Display1_ReadControlRegister() |
                                                      Display1_CTRL_HD_SEND_BREAK);
                /* Send zeros */
                Display1_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = Display1_TXSTATUS_REG;
                }
                while((tmpStat & Display1_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == Display1_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Display1_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = Display1_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & Display1_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == Display1_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Display1_REINIT) ||
                (retMode == Display1_SEND_WAIT_REINIT) )
            {
                Display1_WriteControlRegister(Display1_ReadControlRegister() &
                                              (uint8)~Display1_CTRL_HD_SEND_BREAK);
            }

        #else /* Display1_HD_ENABLED Full Duplex mode */

            static uint8 txPeriod;

            if( (retMode == Display1_SEND_BREAK) ||
                (retMode == Display1_SEND_WAIT_REINIT) )
            {
                /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode */
                #if( (Display1_PARITY_TYPE != Display1__B_UART__NONE_REVB) || \
                                    (Display1_PARITY_TYPE_SW != 0u) )
                    Display1_WriteControlRegister(Display1_ReadControlRegister() |
                                                          Display1_CTRL_HD_SEND_BREAK);
                #endif /* End Display1_PARITY_TYPE != Display1__B_UART__NONE_REVB  */

                #if(Display1_TXCLKGEN_DP)
                    txPeriod = Display1_TXBITCLKTX_COMPLETE_REG;
                    Display1_TXBITCLKTX_COMPLETE_REG = Display1_TXBITCTR_BREAKBITS;
                #else
                    txPeriod = Display1_TXBITCTR_PERIOD_REG;
                    Display1_TXBITCTR_PERIOD_REG = Display1_TXBITCTR_BREAKBITS8X;
                #endif /* End Display1_TXCLKGEN_DP */

                /* Send zeros */
                Display1_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = Display1_TXSTATUS_REG;
                }
                while((tmpStat & Display1_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == Display1_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Display1_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = Display1_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & Display1_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == Display1_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Display1_REINIT) ||
                (retMode == Display1_SEND_WAIT_REINIT) )
            {

            #if(Display1_TXCLKGEN_DP)
                Display1_TXBITCLKTX_COMPLETE_REG = txPeriod;
            #else
                Display1_TXBITCTR_PERIOD_REG = txPeriod;
            #endif /* End Display1_TXCLKGEN_DP */

            #if( (Display1_PARITY_TYPE != Display1__B_UART__NONE_REVB) || \
                 (Display1_PARITY_TYPE_SW != 0u) )
                Display1_WriteControlRegister(Display1_ReadControlRegister() &
                                                      (uint8) ~Display1_CTRL_HD_SEND_BREAK);
            #endif /* End Display1_PARITY_TYPE != NONE */
            }
        #endif    /* End Display1_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: Display1_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the transmitter to signal the next bytes is address or data.
    *
    * Parameters:
    *  addressMode: 
    *       Display1_SET_SPACE - Configure the transmitter to send the next
    *                                    byte as a data.
    *       Display1_SET_MARK  - Configure the transmitter to send the next
    *                                    byte as an address.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  This function sets and clears Display1_CTRL_MARK bit in the Control
    *  register.
    *
    *******************************************************************************/
    void Display1_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable */
        if(addressMode != 0u)
        {
        #if( Display1_CONTROL_REG_REMOVED == 0u )
            Display1_WriteControlRegister(Display1_ReadControlRegister() |
                                                  Display1_CTRL_MARK);
        #endif /* End Display1_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
        #if( Display1_CONTROL_REG_REMOVED == 0u )
            Display1_WriteControlRegister(Display1_ReadControlRegister() &
                                                  (uint8) ~Display1_CTRL_MARK);
        #endif /* End Display1_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndDisplay1_TX_ENABLED */

#if(Display1_HD_ENABLED)


    /*******************************************************************************
    * Function Name: Display1_LoadRxConfig
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
    void Display1_LoadRxConfig(void) 
    {
        Display1_WriteControlRegister(Display1_ReadControlRegister() &
                                                (uint8)~Display1_CTRL_HD_SEND);
        Display1_RXBITCTR_PERIOD_REG = Display1_HD_RXBITCTR_INIT;

    #if (Display1_RX_INTERRUPT_ENABLED)
        /* Enable RX interrupt after set RX configuration */
        Display1_SetRxInterruptMode(Display1_INIT_RX_INTERRUPTS_MASK);
    #endif /* (Display1_RX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: Display1_LoadTxConfig
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
    void Display1_LoadTxConfig(void) 
    {
    #if (Display1_RX_INTERRUPT_ENABLED)
        /* Disable RX interrupts before set TX configuration */
        Display1_SetRxInterruptMode(0u);
    #endif /* (Display1_RX_INTERRUPT_ENABLED) */

        Display1_WriteControlRegister(Display1_ReadControlRegister() | Display1_CTRL_HD_SEND);
        Display1_RXBITCTR_PERIOD_REG = Display1_HD_TXBITCTR_INIT;
    }

#endif  /* Display1_HD_ENABLED */


/* [] END OF FILE */
