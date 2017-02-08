/*******************************************************************************
* File Name: Dispenser.c
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

#include "Dispenser.h"
#if (Dispenser_INTERNAL_CLOCK_USED)
    #include "Dispenser_IntClock.h"
#endif /* End Dispenser_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 Dispenser_initVar = 0u;

#if (Dispenser_TX_INTERRUPT_ENABLED && Dispenser_TX_ENABLED)
    volatile uint8 Dispenser_txBuffer[Dispenser_TX_BUFFER_SIZE];
    volatile uint8 Dispenser_txBufferRead = 0u;
    uint8 Dispenser_txBufferWrite = 0u;
#endif /* (Dispenser_TX_INTERRUPT_ENABLED && Dispenser_TX_ENABLED) */

#if (Dispenser_RX_INTERRUPT_ENABLED && (Dispenser_RX_ENABLED || Dispenser_HD_ENABLED))
    uint8 Dispenser_errorStatus = 0u;
    volatile uint8 Dispenser_rxBuffer[Dispenser_RX_BUFFER_SIZE];
    volatile uint16 Dispenser_rxBufferRead  = 0u;
    volatile uint16 Dispenser_rxBufferWrite = 0u;
    volatile uint8 Dispenser_rxBufferLoopDetect = 0u;
    volatile uint8 Dispenser_rxBufferOverflow   = 0u;
    #if (Dispenser_RXHW_ADDRESS_ENABLED)
        volatile uint8 Dispenser_rxAddressMode = Dispenser_RX_ADDRESS_MODE;
        volatile uint8 Dispenser_rxAddressDetected = 0u;
    #endif /* (Dispenser_RXHW_ADDRESS_ENABLED) */
#endif /* (Dispenser_RX_INTERRUPT_ENABLED && (Dispenser_RX_ENABLED || Dispenser_HD_ENABLED)) */


/*******************************************************************************
* Function Name: Dispenser_Start
********************************************************************************
*
* Summary:
*  This is the preferred method to begin component operation.
*  Dispenser_Start() sets the initVar variable, calls the
*  Dispenser_Init() function, and then calls the
*  Dispenser_Enable() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The Dispenser_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time Dispenser_Start() is called. This
*  allows for component initialization without re-initialization in all
*  subsequent calls to the Dispenser_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Dispenser_Start(void) 
{
    /* If not initialized then initialize all required hardware and software */
    if(Dispenser_initVar == 0u)
    {
        Dispenser_Init();
        Dispenser_initVar = 1u;
    }

    Dispenser_Enable();
}


/*******************************************************************************
* Function Name: Dispenser_Init
********************************************************************************
*
* Summary:
*  Initializes or restores the component according to the customizer Configure
*  dialog settings. It is not necessary to call Dispenser_Init() because
*  the Dispenser_Start() API calls this function and is the preferred
*  method to begin component operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Dispenser_Init(void) 
{
    #if(Dispenser_RX_ENABLED || Dispenser_HD_ENABLED)

        #if (Dispenser_RX_INTERRUPT_ENABLED)
            /* Set RX interrupt vector and priority */
            (void) CyIntSetVector(Dispenser_RX_VECT_NUM, &Dispenser_RXISR);
            CyIntSetPriority(Dispenser_RX_VECT_NUM, Dispenser_RX_PRIOR_NUM);
            Dispenser_errorStatus = 0u;
        #endif /* (Dispenser_RX_INTERRUPT_ENABLED) */

        #if (Dispenser_RXHW_ADDRESS_ENABLED)
            Dispenser_SetRxAddressMode(Dispenser_RX_ADDRESS_MODE);
            Dispenser_SetRxAddress1(Dispenser_RX_HW_ADDRESS1);
            Dispenser_SetRxAddress2(Dispenser_RX_HW_ADDRESS2);
        #endif /* End Dispenser_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        Dispenser_RXBITCTR_PERIOD_REG = Dispenser_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        Dispenser_RXSTATUS_MASK_REG  = Dispenser_INIT_RX_INTERRUPTS_MASK;
    #endif /* End Dispenser_RX_ENABLED || Dispenser_HD_ENABLED*/

    #if(Dispenser_TX_ENABLED)
        #if (Dispenser_TX_INTERRUPT_ENABLED)
            /* Set TX interrupt vector and priority */
            (void) CyIntSetVector(Dispenser_TX_VECT_NUM, &Dispenser_TXISR);
            CyIntSetPriority(Dispenser_TX_VECT_NUM, Dispenser_TX_PRIOR_NUM);
        #endif /* (Dispenser_TX_INTERRUPT_ENABLED) */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if (Dispenser_TXCLKGEN_DP)
            Dispenser_TXBITCLKGEN_CTR_REG = Dispenser_BIT_CENTER;
            Dispenser_TXBITCLKTX_COMPLETE_REG = ((Dispenser_NUMBER_OF_DATA_BITS +
                        Dispenser_NUMBER_OF_START_BIT) * Dispenser_OVER_SAMPLE_COUNT) - 1u;
        #else
            Dispenser_TXBITCTR_PERIOD_REG = ((Dispenser_NUMBER_OF_DATA_BITS +
                        Dispenser_NUMBER_OF_START_BIT) * Dispenser_OVER_SAMPLE_8) - 1u;
        #endif /* End Dispenser_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if (Dispenser_TX_INTERRUPT_ENABLED)
            Dispenser_TXSTATUS_MASK_REG = Dispenser_TX_STS_FIFO_EMPTY;
        #else
            Dispenser_TXSTATUS_MASK_REG = Dispenser_INIT_TX_INTERRUPTS_MASK;
        #endif /*End Dispenser_TX_INTERRUPT_ENABLED*/

    #endif /* End Dispenser_TX_ENABLED */

    #if(Dispenser_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        Dispenser_WriteControlRegister( \
            (Dispenser_ReadControlRegister() & (uint8)~Dispenser_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(Dispenser_PARITY_TYPE << Dispenser_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End Dispenser_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: Dispenser_Enable
********************************************************************************
*
* Summary:
*  Activates the hardware and begins component operation. It is not necessary
*  to call Dispenser_Enable() because the Dispenser_Start() API
*  calls this function, which is the preferred method to begin component
*  operation.

* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Dispenser_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void Dispenser_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if (Dispenser_RX_ENABLED || Dispenser_HD_ENABLED)
        /* RX Counter (Count7) Enable */
        Dispenser_RXBITCTR_CONTROL_REG |= Dispenser_CNTR_ENABLE;

        /* Enable the RX Interrupt */
        Dispenser_RXSTATUS_ACTL_REG  |= Dispenser_INT_ENABLE;

        #if (Dispenser_RX_INTERRUPT_ENABLED)
            Dispenser_EnableRxInt();

            #if (Dispenser_RXHW_ADDRESS_ENABLED)
                Dispenser_rxAddressDetected = 0u;
            #endif /* (Dispenser_RXHW_ADDRESS_ENABLED) */
        #endif /* (Dispenser_RX_INTERRUPT_ENABLED) */
    #endif /* (Dispenser_RX_ENABLED || Dispenser_HD_ENABLED) */

    #if(Dispenser_TX_ENABLED)
        /* TX Counter (DP/Count7) Enable */
        #if(!Dispenser_TXCLKGEN_DP)
            Dispenser_TXBITCTR_CONTROL_REG |= Dispenser_CNTR_ENABLE;
        #endif /* End Dispenser_TXCLKGEN_DP */

        /* Enable the TX Interrupt */
        Dispenser_TXSTATUS_ACTL_REG |= Dispenser_INT_ENABLE;
        #if (Dispenser_TX_INTERRUPT_ENABLED)
            Dispenser_ClearPendingTxInt(); /* Clear history of TX_NOT_EMPTY */
            Dispenser_EnableTxInt();
        #endif /* (Dispenser_TX_INTERRUPT_ENABLED) */
     #endif /* (Dispenser_TX_INTERRUPT_ENABLED) */

    #if (Dispenser_INTERNAL_CLOCK_USED)
        Dispenser_IntClock_Start();  /* Enable the clock */
    #endif /* (Dispenser_INTERNAL_CLOCK_USED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Dispenser_Stop
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
void Dispenser_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if (Dispenser_RX_ENABLED || Dispenser_HD_ENABLED)
        Dispenser_RXBITCTR_CONTROL_REG &= (uint8) ~Dispenser_CNTR_ENABLE;
    #endif /* (Dispenser_RX_ENABLED || Dispenser_HD_ENABLED) */

    #if (Dispenser_TX_ENABLED)
        #if(!Dispenser_TXCLKGEN_DP)
            Dispenser_TXBITCTR_CONTROL_REG &= (uint8) ~Dispenser_CNTR_ENABLE;
        #endif /* (!Dispenser_TXCLKGEN_DP) */
    #endif /* (Dispenser_TX_ENABLED) */

    #if (Dispenser_INTERNAL_CLOCK_USED)
        Dispenser_IntClock_Stop();   /* Disable the clock */
    #endif /* (Dispenser_INTERNAL_CLOCK_USED) */

    /* Disable internal interrupt component */
    #if (Dispenser_RX_ENABLED || Dispenser_HD_ENABLED)
        Dispenser_RXSTATUS_ACTL_REG  &= (uint8) ~Dispenser_INT_ENABLE;

        #if (Dispenser_RX_INTERRUPT_ENABLED)
            Dispenser_DisableRxInt();
        #endif /* (Dispenser_RX_INTERRUPT_ENABLED) */
    #endif /* (Dispenser_RX_ENABLED || Dispenser_HD_ENABLED) */

    #if (Dispenser_TX_ENABLED)
        Dispenser_TXSTATUS_ACTL_REG &= (uint8) ~Dispenser_INT_ENABLE;

        #if (Dispenser_TX_INTERRUPT_ENABLED)
            Dispenser_DisableTxInt();
        #endif /* (Dispenser_TX_INTERRUPT_ENABLED) */
    #endif /* (Dispenser_TX_ENABLED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Dispenser_ReadControlRegister
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
uint8 Dispenser_ReadControlRegister(void) 
{
    #if (Dispenser_CONTROL_REG_REMOVED)
        return(0u);
    #else
        return(Dispenser_CONTROL_REG);
    #endif /* (Dispenser_CONTROL_REG_REMOVED) */
}


/*******************************************************************************
* Function Name: Dispenser_WriteControlRegister
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
void  Dispenser_WriteControlRegister(uint8 control) 
{
    #if (Dispenser_CONTROL_REG_REMOVED)
        if(0u != control)
        {
            /* Suppress compiler warning */
        }
    #else
       Dispenser_CONTROL_REG = control;
    #endif /* (Dispenser_CONTROL_REG_REMOVED) */
}


#if(Dispenser_RX_ENABLED || Dispenser_HD_ENABLED)
    /*******************************************************************************
    * Function Name: Dispenser_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the RX interrupt sources enabled.
    *
    * Parameters:
    *  IntSrc:  Bit field containing the RX interrupts to enable. Based on the 
    *  bit-field arrangement of the status register. This value must be a 
    *  combination of status register bit-masks shown below:
    *      Dispenser_RX_STS_FIFO_NOTEMPTY    Interrupt on byte received.
    *      Dispenser_RX_STS_PAR_ERROR        Interrupt on parity error.
    *      Dispenser_RX_STS_STOP_ERROR       Interrupt on stop error.
    *      Dispenser_RX_STS_BREAK            Interrupt on break.
    *      Dispenser_RX_STS_OVERRUN          Interrupt on overrun error.
    *      Dispenser_RX_STS_ADDR_MATCH       Interrupt on address match.
    *      Dispenser_RX_STS_MRKSPC           Interrupt on address detect.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void Dispenser_SetRxInterruptMode(uint8 intSrc) 
    {
        Dispenser_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: Dispenser_ReadRxData
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
    *  Dispenser_rxBuffer - RAM buffer pointer for save received data.
    *  Dispenser_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  Dispenser_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  Dispenser_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 Dispenser_ReadRxData(void) 
    {
        uint8 rxData;

    #if (Dispenser_RX_INTERRUPT_ENABLED)

        uint16 locRxBufferRead;
        uint16 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        Dispenser_DisableRxInt();

        locRxBufferRead  = Dispenser_rxBufferRead;
        locRxBufferWrite = Dispenser_rxBufferWrite;

        if( (Dispenser_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = Dispenser_rxBuffer[locRxBufferRead];
            locRxBufferRead++;

            if(locRxBufferRead >= Dispenser_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            Dispenser_rxBufferRead = locRxBufferRead;

            if(Dispenser_rxBufferLoopDetect != 0u)
            {
                Dispenser_rxBufferLoopDetect = 0u;
                #if ((Dispenser_RX_INTERRUPT_ENABLED) && (Dispenser_FLOW_CONTROL != 0u))
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( Dispenser_HD_ENABLED )
                        if((Dispenser_CONTROL_REG & Dispenser_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only in RX
                            *  configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            Dispenser_RXSTATUS_MASK_REG  |= Dispenser_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        Dispenser_RXSTATUS_MASK_REG  |= Dispenser_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end Dispenser_HD_ENABLED */
                #endif /* ((Dispenser_RX_INTERRUPT_ENABLED) && (Dispenser_FLOW_CONTROL != 0u)) */
            }
        }
        else
        {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
            rxData = Dispenser_RXDATA_REG;
        }

        Dispenser_EnableRxInt();

    #else

        /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
        rxData = Dispenser_RXDATA_REG;

    #endif /* (Dispenser_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: Dispenser_ReadRxStatus
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
    *  Dispenser_RX_STS_FIFO_NOTEMPTY.
    *  Dispenser_RX_STS_FIFO_NOTEMPTY clears immediately after RX data
    *  register read.
    *
    * Global Variables:
    *  Dispenser_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn't free space in
    *   Dispenser_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   Dispenser_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 Dispenser_ReadRxStatus(void) 
    {
        uint8 status;

        status = Dispenser_RXSTATUS_REG & Dispenser_RX_HW_MASK;

    #if (Dispenser_RX_INTERRUPT_ENABLED)
        if(Dispenser_rxBufferOverflow != 0u)
        {
            status |= Dispenser_RX_STS_SOFT_BUFF_OVER;
            Dispenser_rxBufferOverflow = 0u;
        }
    #endif /* (Dispenser_RX_INTERRUPT_ENABLED) */

        return(status);
    }


    /*******************************************************************************
    * Function Name: Dispenser_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Returns the last received byte of data. Dispenser_GetChar() is
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
    *  Dispenser_rxBuffer - RAM buffer pointer for save received data.
    *  Dispenser_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  Dispenser_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  Dispenser_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 Dispenser_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

    #if (Dispenser_RX_INTERRUPT_ENABLED)
        uint16 locRxBufferRead;
        uint16 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        Dispenser_DisableRxInt();

        locRxBufferRead  = Dispenser_rxBufferRead;
        locRxBufferWrite = Dispenser_rxBufferWrite;

        if( (Dispenser_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = Dispenser_rxBuffer[locRxBufferRead];
            locRxBufferRead++;
            if(locRxBufferRead >= Dispenser_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            Dispenser_rxBufferRead = locRxBufferRead;

            if(Dispenser_rxBufferLoopDetect != 0u)
            {
                Dispenser_rxBufferLoopDetect = 0u;
                #if( (Dispenser_RX_INTERRUPT_ENABLED) && (Dispenser_FLOW_CONTROL != 0u) )
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( Dispenser_HD_ENABLED )
                        if((Dispenser_CONTROL_REG & Dispenser_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only if
                            *  RX configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            Dispenser_RXSTATUS_MASK_REG |= Dispenser_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        Dispenser_RXSTATUS_MASK_REG |= Dispenser_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end Dispenser_HD_ENABLED */
                #endif /* Dispenser_RX_INTERRUPT_ENABLED and Hardware flow control*/
            }

        }
        else
        {   rxStatus = Dispenser_RXSTATUS_REG;
            if((rxStatus & Dispenser_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO */
                rxData = Dispenser_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (Dispenser_RX_STS_BREAK | Dispenser_RX_STS_PAR_ERROR |
                                Dispenser_RX_STS_STOP_ERROR | Dispenser_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        }

        Dispenser_EnableRxInt();

    #else

        rxStatus =Dispenser_RXSTATUS_REG;
        if((rxStatus & Dispenser_RX_STS_FIFO_NOTEMPTY) != 0u)
        {
            /* Read received data from FIFO */
            rxData = Dispenser_RXDATA_REG;

            /*Check status on error*/
            if((rxStatus & (Dispenser_RX_STS_BREAK | Dispenser_RX_STS_PAR_ERROR |
                            Dispenser_RX_STS_STOP_ERROR | Dispenser_RX_STS_OVERRUN)) != 0u)
            {
                rxData = 0u;
            }
        }
    #endif /* (Dispenser_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: Dispenser_GetByte
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
    uint16 Dispenser_GetByte(void) 
    {
        
    #if (Dispenser_RX_INTERRUPT_ENABLED)
        uint16 locErrorStatus;
        /* Protect variables that could change on interrupt */
        Dispenser_DisableRxInt();
        locErrorStatus = (uint16)Dispenser_errorStatus;
        Dispenser_errorStatus = 0u;
        Dispenser_EnableRxInt();
        return ( (uint16)(locErrorStatus << 8u) | Dispenser_ReadRxData() );
    #else
        return ( ((uint16)Dispenser_ReadRxStatus() << 8u) | Dispenser_ReadRxData() );
    #endif /* Dispenser_RX_INTERRUPT_ENABLED */
        
    }


    /*******************************************************************************
    * Function Name: Dispenser_GetRxBufferSize
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
    *  uint16: Number of bytes in the RX buffer. 
    *    Return value type depends on RX Buffer Size parameter.
    *
    * Global Variables:
    *  Dispenser_rxBufferWrite - used to calculate left bytes.
    *  Dispenser_rxBufferRead - used to calculate left bytes.
    *  Dispenser_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint16 Dispenser_GetRxBufferSize(void)
                                                            
    {
        uint16 size;

    #if (Dispenser_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt */
        Dispenser_DisableRxInt();

        if(Dispenser_rxBufferRead == Dispenser_rxBufferWrite)
        {
            if(Dispenser_rxBufferLoopDetect != 0u)
            {
                size = Dispenser_RX_BUFFER_SIZE;
            }
            else
            {
                size = 0u;
            }
        }
        else if(Dispenser_rxBufferRead < Dispenser_rxBufferWrite)
        {
            size = (Dispenser_rxBufferWrite - Dispenser_rxBufferRead);
        }
        else
        {
            size = (Dispenser_RX_BUFFER_SIZE - Dispenser_rxBufferRead) + Dispenser_rxBufferWrite;
        }

        Dispenser_EnableRxInt();

    #else

        /* We can only know if there is data in the fifo. */
        size = ((Dispenser_RXSTATUS_REG & Dispenser_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

    #endif /* (Dispenser_RX_INTERRUPT_ENABLED) */

        return(size);
    }


    /*******************************************************************************
    * Function Name: Dispenser_ClearRxBuffer
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
    *  Dispenser_rxBufferWrite - cleared to zero.
    *  Dispenser_rxBufferRead - cleared to zero.
    *  Dispenser_rxBufferLoopDetect - cleared to zero.
    *  Dispenser_rxBufferOverflow - cleared to zero.
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
    void Dispenser_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Clear the HW FIFO */
        enableInterrupts = CyEnterCriticalSection();
        Dispenser_RXDATA_AUX_CTL_REG |= (uint8)  Dispenser_RX_FIFO_CLR;
        Dispenser_RXDATA_AUX_CTL_REG &= (uint8) ~Dispenser_RX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (Dispenser_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        Dispenser_DisableRxInt();

        Dispenser_rxBufferRead = 0u;
        Dispenser_rxBufferWrite = 0u;
        Dispenser_rxBufferLoopDetect = 0u;
        Dispenser_rxBufferOverflow = 0u;

        Dispenser_EnableRxInt();

    #endif /* (Dispenser_RX_INTERRUPT_ENABLED) */

    }


    /*******************************************************************************
    * Function Name: Dispenser_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Sets the software controlled Addressing mode used by the RX portion of the
    *  UART.
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  Dispenser__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  Dispenser__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  Dispenser__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  Dispenser__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  Dispenser__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Dispenser_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  Dispenser_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void Dispenser_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(Dispenser_RXHW_ADDRESS_ENABLED)
            #if(Dispenser_CONTROL_REG_REMOVED)
                if(0u != addressMode)
                {
                    /* Suppress compiler warning */
                }
            #else /* Dispenser_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = Dispenser_CONTROL_REG & (uint8)~Dispenser_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << Dispenser_CTRL_RXADDR_MODE0_SHIFT);
                Dispenser_CONTROL_REG = tmpCtrl;

                #if(Dispenser_RX_INTERRUPT_ENABLED && \
                   (Dispenser_RXBUFFERSIZE > Dispenser_FIFO_LENGTH) )
                    Dispenser_rxAddressMode = addressMode;
                    Dispenser_rxAddressDetected = 0u;
                #endif /* End Dispenser_RXBUFFERSIZE > Dispenser_FIFO_LENGTH*/
            #endif /* End Dispenser_CONTROL_REG_REMOVED */
        #else /* Dispenser_RXHW_ADDRESS_ENABLED */
            if(0u != addressMode)
            {
                /* Suppress compiler warning */
            }
        #endif /* End Dispenser_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: Dispenser_SetRxAddress1
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
    void Dispenser_SetRxAddress1(uint8 address) 
    {
        Dispenser_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: Dispenser_SetRxAddress2
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
    void Dispenser_SetRxAddress2(uint8 address) 
    {
        Dispenser_RXADDRESS2_REG = address;
    }

#endif  /* Dispenser_RX_ENABLED || Dispenser_HD_ENABLED*/


#if( (Dispenser_TX_ENABLED) || (Dispenser_HD_ENABLED) )
    /*******************************************************************************
    * Function Name: Dispenser_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the TX interrupt sources to be enabled, but does not enable the
    *  interrupt.
    *
    * Parameters:
    *  intSrc: Bit field containing the TX interrupt sources to enable
    *   Dispenser_TX_STS_COMPLETE        Interrupt on TX byte complete
    *   Dispenser_TX_STS_FIFO_EMPTY      Interrupt when TX FIFO is empty
    *   Dispenser_TX_STS_FIFO_FULL       Interrupt when TX FIFO is full
    *   Dispenser_TX_STS_FIFO_NOT_FULL   Interrupt when TX FIFO is not full
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void Dispenser_SetTxInterruptMode(uint8 intSrc) 
    {
        Dispenser_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: Dispenser_WriteTxData
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
    *  Dispenser_txBuffer - RAM buffer pointer for save data for transmission
    *  Dispenser_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  Dispenser_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  Dispenser_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void Dispenser_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(Dispenser_initVar != 0u)
        {
        #if (Dispenser_TX_INTERRUPT_ENABLED)

            /* Protect variables that could change on interrupt. */
            Dispenser_DisableTxInt();

            if( (Dispenser_txBufferRead == Dispenser_txBufferWrite) &&
                ((Dispenser_TXSTATUS_REG & Dispenser_TX_STS_FIFO_FULL) == 0u) )
            {
                /* Add directly to the FIFO. */
                Dispenser_TXDATA_REG = txDataByte;
            }
            else
            {
                if(Dispenser_txBufferWrite >= Dispenser_TX_BUFFER_SIZE)
                {
                    Dispenser_txBufferWrite = 0u;
                }

                Dispenser_txBuffer[Dispenser_txBufferWrite] = txDataByte;

                /* Add to the software buffer. */
                Dispenser_txBufferWrite++;
            }

            Dispenser_EnableTxInt();

        #else

            /* Add directly to the FIFO. */
            Dispenser_TXDATA_REG = txDataByte;

        #endif /*(Dispenser_TX_INTERRUPT_ENABLED) */
        }
    }


    /*******************************************************************************
    * Function Name: Dispenser_ReadTxStatus
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
    uint8 Dispenser_ReadTxStatus(void) 
    {
        return(Dispenser_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: Dispenser_PutChar
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
    *  Dispenser_txBuffer - RAM buffer pointer for save data for transmission
    *  Dispenser_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  Dispenser_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  Dispenser_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void Dispenser_PutChar(uint8 txDataByte) 
    {
    #if (Dispenser_TX_INTERRUPT_ENABLED)
        /* The temporary output pointer is used since it takes two instructions
        *  to increment with a wrap, and we can't risk doing that with the real
        *  pointer and getting an interrupt in between instructions.
        */
        uint8 locTxBufferWrite;
        uint8 locTxBufferRead;

        do
        { /* Block if software buffer is full, so we don't overwrite. */

        #if ((Dispenser_TX_BUFFER_SIZE > Dispenser_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Disable TX interrupt to protect variables from modification */
            Dispenser_DisableTxInt();
        #endif /* (Dispenser_TX_BUFFER_SIZE > Dispenser_MAX_BYTE_VALUE) && (CY_PSOC3) */

            locTxBufferWrite = Dispenser_txBufferWrite;
            locTxBufferRead  = Dispenser_txBufferRead;

        #if ((Dispenser_TX_BUFFER_SIZE > Dispenser_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Enable interrupt to continue transmission */
            Dispenser_EnableTxInt();
        #endif /* (Dispenser_TX_BUFFER_SIZE > Dispenser_MAX_BYTE_VALUE) && (CY_PSOC3) */
        }
        while( (locTxBufferWrite < locTxBufferRead) ? (locTxBufferWrite == (locTxBufferRead - 1u)) :
                                ((locTxBufferWrite - locTxBufferRead) ==
                                (uint8)(Dispenser_TX_BUFFER_SIZE - 1u)) );

        if( (locTxBufferRead == locTxBufferWrite) &&
            ((Dispenser_TXSTATUS_REG & Dispenser_TX_STS_FIFO_FULL) == 0u) )
        {
            /* Add directly to the FIFO */
            Dispenser_TXDATA_REG = txDataByte;
        }
        else
        {
            if(locTxBufferWrite >= Dispenser_TX_BUFFER_SIZE)
            {
                locTxBufferWrite = 0u;
            }
            /* Add to the software buffer. */
            Dispenser_txBuffer[locTxBufferWrite] = txDataByte;
            locTxBufferWrite++;

            /* Finally, update the real output pointer */
        #if ((Dispenser_TX_BUFFER_SIZE > Dispenser_MAX_BYTE_VALUE) && (CY_PSOC3))
            Dispenser_DisableTxInt();
        #endif /* (Dispenser_TX_BUFFER_SIZE > Dispenser_MAX_BYTE_VALUE) && (CY_PSOC3) */

            Dispenser_txBufferWrite = locTxBufferWrite;

        #if ((Dispenser_TX_BUFFER_SIZE > Dispenser_MAX_BYTE_VALUE) && (CY_PSOC3))
            Dispenser_EnableTxInt();
        #endif /* (Dispenser_TX_BUFFER_SIZE > Dispenser_MAX_BYTE_VALUE) && (CY_PSOC3) */

            if(0u != (Dispenser_TXSTATUS_REG & Dispenser_TX_STS_FIFO_EMPTY))
            {
                /* Trigger TX interrupt to send software buffer */
                Dispenser_SetPendingTxInt();
            }
        }

    #else

        while((Dispenser_TXSTATUS_REG & Dispenser_TX_STS_FIFO_FULL) != 0u)
        {
            /* Wait for room in the FIFO */
        }

        /* Add directly to the FIFO */
        Dispenser_TXDATA_REG = txDataByte;

    #endif /* Dispenser_TX_INTERRUPT_ENABLED */
    }


    /*******************************************************************************
    * Function Name: Dispenser_PutString
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
    *  Dispenser_initVar - checked to identify that the component has been
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
    void Dispenser_PutString(const char8 string[]) 
    {
        uint16 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(Dispenser_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent */
            while(string[bufIndex] != (char8) 0)
            {
                Dispenser_PutChar((uint8)string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: Dispenser_PutArray
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
    *  Dispenser_initVar - checked to identify that the component has been
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
    void Dispenser_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(Dispenser_initVar != 0u)
        {
            while(bufIndex < byteCount)
            {
                Dispenser_PutChar(string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: Dispenser_PutCRLF
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
    *  Dispenser_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void Dispenser_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function */
        if(Dispenser_initVar != 0u)
        {
            Dispenser_PutChar(txDataByte);
            Dispenser_PutChar(0x0Du);
            Dispenser_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: Dispenser_GetTxBufferSize
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
    *  Dispenser_txBufferWrite - used to calculate left space.
    *  Dispenser_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 Dispenser_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (Dispenser_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        Dispenser_DisableTxInt();

        if(Dispenser_txBufferRead == Dispenser_txBufferWrite)
        {
            size = 0u;
        }
        else if(Dispenser_txBufferRead < Dispenser_txBufferWrite)
        {
            size = (Dispenser_txBufferWrite - Dispenser_txBufferRead);
        }
        else
        {
            size = (Dispenser_TX_BUFFER_SIZE - Dispenser_txBufferRead) +
                    Dispenser_txBufferWrite;
        }

        Dispenser_EnableTxInt();

    #else

        size = Dispenser_TXSTATUS_REG;

        /* Is the fifo is full. */
        if((size & Dispenser_TX_STS_FIFO_FULL) != 0u)
        {
            size = Dispenser_FIFO_LENGTH;
        }
        else if((size & Dispenser_TX_STS_FIFO_EMPTY) != 0u)
        {
            size = 0u;
        }
        else
        {
            /* We only know there is data in the fifo. */
            size = 1u;
        }

    #endif /* (Dispenser_TX_INTERRUPT_ENABLED) */

    return(size);
    }


    /*******************************************************************************
    * Function Name: Dispenser_ClearTxBuffer
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
    *  Dispenser_txBufferWrite - cleared to zero.
    *  Dispenser_txBufferRead - cleared to zero.
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
    void Dispenser_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        enableInterrupts = CyEnterCriticalSection();
        /* Clear the HW FIFO */
        Dispenser_TXDATA_AUX_CTL_REG |= (uint8)  Dispenser_TX_FIFO_CLR;
        Dispenser_TXDATA_AUX_CTL_REG &= (uint8) ~Dispenser_TX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (Dispenser_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        Dispenser_DisableTxInt();

        Dispenser_txBufferRead = 0u;
        Dispenser_txBufferWrite = 0u;

        /* Enable Tx interrupt. */
        Dispenser_EnableTxInt();

    #endif /* (Dispenser_TX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: Dispenser_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Transmits a break signal on the bus.
    *
    * Parameters:
    *  uint8 retMode:  Send Break return mode. See the following table for options.
    *   Dispenser_SEND_BREAK - Initialize registers for break, send the Break
    *       signal and return immediately.
    *   Dispenser_WAIT_FOR_COMPLETE_REINIT - Wait until break transmission is
    *       complete, reinitialize registers to normal transmission mode then return
    *   Dispenser_REINIT - Reinitialize registers to normal transmission mode
    *       then return.
    *   Dispenser_SEND_WAIT_REINIT - Performs both options: 
    *      Dispenser_SEND_BREAK and Dispenser_WAIT_FOR_COMPLETE_REINIT.
    *      This option is recommended for most cases.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Dispenser_initVar - checked to identify that the component has been
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
    *     When interrupt appear with Dispenser_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *  The Dispenser_SendBreak() function initializes registers to send a
    *  break signal.
    *  Break signal length depends on the break signal bits configuration.
    *  The register configuration should be reinitialized before normal 8-bit
    *  communication can continue.
    *
    *******************************************************************************/
    void Dispenser_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(Dispenser_initVar != 0u)
        {
            /* Set the Counter to 13-bits and transmit a 00 byte */
            /* When that is done then reset the counter value back */
            uint8 tmpStat;

        #if(Dispenser_HD_ENABLED) /* Half Duplex mode*/

            if( (retMode == Dispenser_SEND_BREAK) ||
                (retMode == Dispenser_SEND_WAIT_REINIT ) )
            {
                /* CTRL_HD_SEND_BREAK - sends break bits in HD mode */
                Dispenser_WriteControlRegister(Dispenser_ReadControlRegister() |
                                                      Dispenser_CTRL_HD_SEND_BREAK);
                /* Send zeros */
                Dispenser_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = Dispenser_TXSTATUS_REG;
                }
                while((tmpStat & Dispenser_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == Dispenser_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Dispenser_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = Dispenser_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & Dispenser_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == Dispenser_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Dispenser_REINIT) ||
                (retMode == Dispenser_SEND_WAIT_REINIT) )
            {
                Dispenser_WriteControlRegister(Dispenser_ReadControlRegister() &
                                              (uint8)~Dispenser_CTRL_HD_SEND_BREAK);
            }

        #else /* Dispenser_HD_ENABLED Full Duplex mode */

            static uint8 txPeriod;

            if( (retMode == Dispenser_SEND_BREAK) ||
                (retMode == Dispenser_SEND_WAIT_REINIT) )
            {
                /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode */
                #if( (Dispenser_PARITY_TYPE != Dispenser__B_UART__NONE_REVB) || \
                                    (Dispenser_PARITY_TYPE_SW != 0u) )
                    Dispenser_WriteControlRegister(Dispenser_ReadControlRegister() |
                                                          Dispenser_CTRL_HD_SEND_BREAK);
                #endif /* End Dispenser_PARITY_TYPE != Dispenser__B_UART__NONE_REVB  */

                #if(Dispenser_TXCLKGEN_DP)
                    txPeriod = Dispenser_TXBITCLKTX_COMPLETE_REG;
                    Dispenser_TXBITCLKTX_COMPLETE_REG = Dispenser_TXBITCTR_BREAKBITS;
                #else
                    txPeriod = Dispenser_TXBITCTR_PERIOD_REG;
                    Dispenser_TXBITCTR_PERIOD_REG = Dispenser_TXBITCTR_BREAKBITS8X;
                #endif /* End Dispenser_TXCLKGEN_DP */

                /* Send zeros */
                Dispenser_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = Dispenser_TXSTATUS_REG;
                }
                while((tmpStat & Dispenser_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == Dispenser_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Dispenser_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = Dispenser_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & Dispenser_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == Dispenser_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Dispenser_REINIT) ||
                (retMode == Dispenser_SEND_WAIT_REINIT) )
            {

            #if(Dispenser_TXCLKGEN_DP)
                Dispenser_TXBITCLKTX_COMPLETE_REG = txPeriod;
            #else
                Dispenser_TXBITCTR_PERIOD_REG = txPeriod;
            #endif /* End Dispenser_TXCLKGEN_DP */

            #if( (Dispenser_PARITY_TYPE != Dispenser__B_UART__NONE_REVB) || \
                 (Dispenser_PARITY_TYPE_SW != 0u) )
                Dispenser_WriteControlRegister(Dispenser_ReadControlRegister() &
                                                      (uint8) ~Dispenser_CTRL_HD_SEND_BREAK);
            #endif /* End Dispenser_PARITY_TYPE != NONE */
            }
        #endif    /* End Dispenser_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: Dispenser_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the transmitter to signal the next bytes is address or data.
    *
    * Parameters:
    *  addressMode: 
    *       Dispenser_SET_SPACE - Configure the transmitter to send the next
    *                                    byte as a data.
    *       Dispenser_SET_MARK  - Configure the transmitter to send the next
    *                                    byte as an address.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  This function sets and clears Dispenser_CTRL_MARK bit in the Control
    *  register.
    *
    *******************************************************************************/
    void Dispenser_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable */
        if(addressMode != 0u)
        {
        #if( Dispenser_CONTROL_REG_REMOVED == 0u )
            Dispenser_WriteControlRegister(Dispenser_ReadControlRegister() |
                                                  Dispenser_CTRL_MARK);
        #endif /* End Dispenser_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
        #if( Dispenser_CONTROL_REG_REMOVED == 0u )
            Dispenser_WriteControlRegister(Dispenser_ReadControlRegister() &
                                                  (uint8) ~Dispenser_CTRL_MARK);
        #endif /* End Dispenser_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndDispenser_TX_ENABLED */

#if(Dispenser_HD_ENABLED)


    /*******************************************************************************
    * Function Name: Dispenser_LoadRxConfig
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
    void Dispenser_LoadRxConfig(void) 
    {
        Dispenser_WriteControlRegister(Dispenser_ReadControlRegister() &
                                                (uint8)~Dispenser_CTRL_HD_SEND);
        Dispenser_RXBITCTR_PERIOD_REG = Dispenser_HD_RXBITCTR_INIT;

    #if (Dispenser_RX_INTERRUPT_ENABLED)
        /* Enable RX interrupt after set RX configuration */
        Dispenser_SetRxInterruptMode(Dispenser_INIT_RX_INTERRUPTS_MASK);
    #endif /* (Dispenser_RX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: Dispenser_LoadTxConfig
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
    void Dispenser_LoadTxConfig(void) 
    {
    #if (Dispenser_RX_INTERRUPT_ENABLED)
        /* Disable RX interrupts before set TX configuration */
        Dispenser_SetRxInterruptMode(0u);
    #endif /* (Dispenser_RX_INTERRUPT_ENABLED) */

        Dispenser_WriteControlRegister(Dispenser_ReadControlRegister() | Dispenser_CTRL_HD_SEND);
        Dispenser_RXBITCTR_PERIOD_REG = Dispenser_HD_TXBITCTR_INIT;
    }

#endif  /* Dispenser_HD_ENABLED */


/* [] END OF FILE */
