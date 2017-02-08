/*******************************************************************************
* File Name: GlobalCounter.c  
* Version 3.0
*
*  Description:
*     The Counter component consists of a 8, 16, 24 or 32-bit counter with
*     a selectable period between 2 and 2^Width - 1.  
*
*   Note:
*     None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "GlobalCounter.h"

uint8 GlobalCounter_initVar = 0u;


/*******************************************************************************
* Function Name: GlobalCounter_Init
********************************************************************************
* Summary:
*     Initialize to the schematic state
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
*******************************************************************************/
void GlobalCounter_Init(void) 
{
        #if (!GlobalCounter_UsingFixedFunction && !GlobalCounter_ControlRegRemoved)
            uint8 ctrl;
        #endif /* (!GlobalCounter_UsingFixedFunction && !GlobalCounter_ControlRegRemoved) */
        
        #if(!GlobalCounter_UsingFixedFunction) 
            /* Interrupt State Backup for Critical Region*/
            uint8 GlobalCounter_interruptState;
        #endif /* (!GlobalCounter_UsingFixedFunction) */
        
        #if (GlobalCounter_UsingFixedFunction)
            /* Clear all bits but the enable bit (if it's already set for Timer operation */
            GlobalCounter_CONTROL &= GlobalCounter_CTRL_ENABLE;
            
            /* Clear the mode bits for continuous run mode */
            #if (CY_PSOC5A)
                GlobalCounter_CONTROL2 &= ((uint8)(~GlobalCounter_CTRL_MODE_MASK));
            #endif /* (CY_PSOC5A) */
            #if (CY_PSOC3 || CY_PSOC5LP)
                GlobalCounter_CONTROL3 &= ((uint8)(~GlobalCounter_CTRL_MODE_MASK));                
            #endif /* (CY_PSOC3 || CY_PSOC5LP) */
            /* Check if One Shot mode is enabled i.e. RunMode !=0*/
            #if (GlobalCounter_RunModeUsed != 0x0u)
                /* Set 3rd bit of Control register to enable one shot mode */
                GlobalCounter_CONTROL |= GlobalCounter_ONESHOT;
            #endif /* (GlobalCounter_RunModeUsed != 0x0u) */
            
            /* Set the IRQ to use the status register interrupts */
            GlobalCounter_CONTROL2 |= GlobalCounter_CTRL2_IRQ_SEL;
            
            /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
            GlobalCounter_RT1 &= ((uint8)(~GlobalCounter_RT1_MASK));
            GlobalCounter_RT1 |= GlobalCounter_SYNC;     
                    
            /*Enable DSI Sync all all inputs of the Timer*/
            GlobalCounter_RT1 &= ((uint8)(~GlobalCounter_SYNCDSI_MASK));
            GlobalCounter_RT1 |= GlobalCounter_SYNCDSI_EN;

        #else
            #if(!GlobalCounter_ControlRegRemoved)
            /* Set the default compare mode defined in the parameter */
            ctrl = GlobalCounter_CONTROL & ((uint8)(~GlobalCounter_CTRL_CMPMODE_MASK));
            GlobalCounter_CONTROL = ctrl | GlobalCounter_DEFAULT_COMPARE_MODE;
            
            /* Set the default capture mode defined in the parameter */
            ctrl = GlobalCounter_CONTROL & ((uint8)(~GlobalCounter_CTRL_CAPMODE_MASK));
            
            #if( 0 != GlobalCounter_CAPTURE_MODE_CONF)
                GlobalCounter_CONTROL = ctrl | GlobalCounter_DEFAULT_CAPTURE_MODE;
            #else
                GlobalCounter_CONTROL = ctrl;
            #endif /* 0 != GlobalCounter_CAPTURE_MODE */ 
            
            #endif /* (!GlobalCounter_ControlRegRemoved) */
        #endif /* (GlobalCounter_UsingFixedFunction) */
        
        /* Clear all data in the FIFO's */
        #if (!GlobalCounter_UsingFixedFunction)
            GlobalCounter_ClearFIFO();
        #endif /* (!GlobalCounter_UsingFixedFunction) */
        
        /* Set Initial values from Configuration */
        GlobalCounter_WritePeriod(GlobalCounter_INIT_PERIOD_VALUE);
        #if (!(GlobalCounter_UsingFixedFunction && (CY_PSOC5A)))
            GlobalCounter_WriteCounter(GlobalCounter_INIT_COUNTER_VALUE);
        #endif /* (!(GlobalCounter_UsingFixedFunction && (CY_PSOC5A))) */
        GlobalCounter_SetInterruptMode(GlobalCounter_INIT_INTERRUPTS_MASK);
        
        #if (!GlobalCounter_UsingFixedFunction)
            /* Read the status register to clear the unwanted interrupts */
            (void)GlobalCounter_ReadStatusRegister();
            /* Set the compare value (only available to non-fixed function implementation */
            GlobalCounter_WriteCompare(GlobalCounter_INIT_COMPARE_VALUE);
            /* Use the interrupt output of the status register for IRQ output */
            
            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            GlobalCounter_interruptState = CyEnterCriticalSection();
            
            GlobalCounter_STATUS_AUX_CTRL |= GlobalCounter_STATUS_ACTL_INT_EN_MASK;
            
            /* Exit Critical Region*/
            CyExitCriticalSection(GlobalCounter_interruptState);
            
        #endif /* (!GlobalCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: GlobalCounter_Enable
********************************************************************************
* Summary:
*     Enable the Counter
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Side Effects: 
*   If the Enable mode is set to Hardware only then this function has no effect 
*   on the operation of the counter.
*
*******************************************************************************/
void GlobalCounter_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (GlobalCounter_UsingFixedFunction)
        GlobalCounter_GLOBAL_ENABLE |= GlobalCounter_BLOCK_EN_MASK;
        GlobalCounter_GLOBAL_STBY_ENABLE |= GlobalCounter_BLOCK_STBY_EN_MASK;
    #endif /* (GlobalCounter_UsingFixedFunction) */  
        
    /* Enable the counter from the control register  */
    /* If Fixed Function then make sure Mode is set correctly */
    /* else make sure reset is clear */
    #if(!GlobalCounter_ControlRegRemoved || GlobalCounter_UsingFixedFunction)
        GlobalCounter_CONTROL |= GlobalCounter_CTRL_ENABLE;                
    #endif /* (!GlobalCounter_ControlRegRemoved || GlobalCounter_UsingFixedFunction) */
    
}


/*******************************************************************************
* Function Name: GlobalCounter_Start
********************************************************************************
* Summary:
*  Enables the counter for operation 
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Global variables:
*  GlobalCounter_initVar: Is modified when this function is called for the  
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void GlobalCounter_Start(void) 
{
    if(GlobalCounter_initVar == 0u)
    {
        GlobalCounter_Init();
        
        GlobalCounter_initVar = 1u; /* Clear this bit for Initialization */        
    }
    
    /* Enable the Counter */
    GlobalCounter_Enable();        
}


/*******************************************************************************
* Function Name: GlobalCounter_Stop
********************************************************************************
* Summary:
* Halts the counter, but does not change any modes or disable interrupts.
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Side Effects: If the Enable mode is set to Hardware only then this function
*               has no effect on the operation of the counter.
*
*******************************************************************************/
void GlobalCounter_Stop(void) 
{
    /* Disable Counter */
    #if(!GlobalCounter_ControlRegRemoved || GlobalCounter_UsingFixedFunction)
        GlobalCounter_CONTROL &= ((uint8)(~GlobalCounter_CTRL_ENABLE));        
    #endif /* (!GlobalCounter_ControlRegRemoved || GlobalCounter_UsingFixedFunction) */
    
    /* Globally disable the Fixed Function Block chosen */
    #if (GlobalCounter_UsingFixedFunction)
        GlobalCounter_GLOBAL_ENABLE &= ((uint8)(~GlobalCounter_BLOCK_EN_MASK));
        GlobalCounter_GLOBAL_STBY_ENABLE &= ((uint8)(~GlobalCounter_BLOCK_STBY_EN_MASK));
    #endif /* (GlobalCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: GlobalCounter_SetInterruptMode
********************************************************************************
* Summary:
* Configures which interrupt sources are enabled to generate the final interrupt
*
* Parameters:  
*  InterruptsMask: This parameter is an or'd collection of the status bits
*                   which will be allowed to generate the counters interrupt.   
*
* Return: 
*  void
*
*******************************************************************************/
void GlobalCounter_SetInterruptMode(uint8 interruptsMask) 
{
    GlobalCounter_STATUS_MASK = interruptsMask;
}


/*******************************************************************************
* Function Name: GlobalCounter_ReadStatusRegister
********************************************************************************
* Summary:
*   Reads the status register and returns it's state. This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the status register
*
* Side Effects:
*   Status register bits may be clear on read. 
*
*******************************************************************************/
uint8   GlobalCounter_ReadStatusRegister(void) 
{
    return GlobalCounter_STATUS;
}


#if(!GlobalCounter_ControlRegRemoved)
/*******************************************************************************
* Function Name: GlobalCounter_ReadControlRegister
********************************************************************************
* Summary:
*   Reads the control register and returns it's state. This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the control register
*
*******************************************************************************/
uint8   GlobalCounter_ReadControlRegister(void) 
{
    return GlobalCounter_CONTROL;
}


/*******************************************************************************
* Function Name: GlobalCounter_WriteControlRegister
********************************************************************************
* Summary:
*   Sets the bit-field of the control register.  This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the control register
*
*******************************************************************************/
void    GlobalCounter_WriteControlRegister(uint8 control) 
{
    GlobalCounter_CONTROL = control;
}

#endif  /* (!GlobalCounter_ControlRegRemoved) */


#if (!(GlobalCounter_UsingFixedFunction && (CY_PSOC5A)))
/*******************************************************************************
* Function Name: GlobalCounter_WriteCounter
********************************************************************************
* Summary:
*   This funtion is used to set the counter to a specific value
*
* Parameters:  
*  counter:  New counter value. 
*
* Return: 
*  void 
*
*******************************************************************************/
void GlobalCounter_WriteCounter(uint16 counter) \
                                   
{
    #if(GlobalCounter_UsingFixedFunction)
        /* assert if block is already enabled */
        CYASSERT (0u == (GlobalCounter_GLOBAL_ENABLE & GlobalCounter_BLOCK_EN_MASK));
        /* If block is disabled, enable it and then write the counter */
        GlobalCounter_GLOBAL_ENABLE |= GlobalCounter_BLOCK_EN_MASK;
        CY_SET_REG16(GlobalCounter_COUNTER_LSB_PTR, (uint16)counter);
        GlobalCounter_GLOBAL_ENABLE &= ((uint8)(~GlobalCounter_BLOCK_EN_MASK));
    #else
        CY_SET_REG16(GlobalCounter_COUNTER_LSB_PTR, counter);
    #endif /* (GlobalCounter_UsingFixedFunction) */
}
#endif /* (!(GlobalCounter_UsingFixedFunction && (CY_PSOC5A))) */


/*******************************************************************************
* Function Name: GlobalCounter_ReadCounter
********************************************************************************
* Summary:
* Returns the current value of the counter.  It doesn't matter
* if the counter is enabled or running.
*
* Parameters:  
*  void:  
*
* Return: 
*  (uint16) The present value of the counter.
*
*******************************************************************************/
uint16 GlobalCounter_ReadCounter(void) 
{
    /* Force capture by reading Accumulator */
    /* Must first do a software capture to be able to read the counter */
    /* It is up to the user code to make sure there isn't already captured data in the FIFO */
    #if(GlobalCounter_UsingFixedFunction)
		(void)CY_GET_REG16(GlobalCounter_COUNTER_LSB_PTR);
	#else
		(void)CY_GET_REG8(GlobalCounter_COUNTER_LSB_PTR_8BIT);
	#endif/* (GlobalCounter_UsingFixedFunction) */
    
    /* Read the data from the FIFO (or capture register for Fixed Function)*/
    #if(GlobalCounter_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(GlobalCounter_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG16(GlobalCounter_STATICCOUNT_LSB_PTR));
    #endif /* (GlobalCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: GlobalCounter_ReadCapture
********************************************************************************
* Summary:
*   This function returns the last value captured.
*
* Parameters:  
*  void
*
* Return: 
*  (uint16) Present Capture value.
*
*******************************************************************************/
uint16 GlobalCounter_ReadCapture(void) 
{
    #if(GlobalCounter_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(GlobalCounter_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG16(GlobalCounter_STATICCOUNT_LSB_PTR));
    #endif /* (GlobalCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: GlobalCounter_WritePeriod
********************************************************************************
* Summary:
* Changes the period of the counter.  The new period 
* will be loaded the next time terminal count is detected.
*
* Parameters:  
*  period: (uint16) A value of 0 will result in
*         the counter remaining at zero.  
*
* Return: 
*  void
*
*******************************************************************************/
void GlobalCounter_WritePeriod(uint16 period) 
{
    #if(GlobalCounter_UsingFixedFunction)
        CY_SET_REG16(GlobalCounter_PERIOD_LSB_PTR,(uint16)period);
    #else
        CY_SET_REG16(GlobalCounter_PERIOD_LSB_PTR, period);
    #endif /* (GlobalCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: GlobalCounter_ReadPeriod
********************************************************************************
* Summary:
* Reads the current period value without affecting counter operation.
*
* Parameters:  
*  void:  
*
* Return: 
*  (uint16) Present period value.
*
*******************************************************************************/
uint16 GlobalCounter_ReadPeriod(void) 
{
    #if(GlobalCounter_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(GlobalCounter_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG16(GlobalCounter_PERIOD_LSB_PTR));
    #endif /* (GlobalCounter_UsingFixedFunction) */
}


#if (!GlobalCounter_UsingFixedFunction)
/*******************************************************************************
* Function Name: GlobalCounter_WriteCompare
********************************************************************************
* Summary:
* Changes the compare value.  The compare output will 
* reflect the new value on the next UDB clock.  The compare output will be 
* driven high when the present counter value compares true based on the 
* configured compare mode setting. 
*
* Parameters:  
*  Compare:  New compare value. 
*
* Return: 
*  void
*
*******************************************************************************/
void GlobalCounter_WriteCompare(uint16 compare) \
                                   
{
    #if(GlobalCounter_UsingFixedFunction)
        CY_SET_REG16(GlobalCounter_COMPARE_LSB_PTR, (uint16)compare);
    #else
        CY_SET_REG16(GlobalCounter_COMPARE_LSB_PTR, compare);
    #endif /* (GlobalCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: GlobalCounter_ReadCompare
********************************************************************************
* Summary:
* Returns the compare value.
*
* Parameters:  
*  void:
*
* Return: 
*  (uint16) Present compare value.
*
*******************************************************************************/
uint16 GlobalCounter_ReadCompare(void) 
{
    return (CY_GET_REG16(GlobalCounter_COMPARE_LSB_PTR));
}


#if (GlobalCounter_COMPARE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: GlobalCounter_SetCompareMode
********************************************************************************
* Summary:
*  Sets the software controlled Compare Mode.
*
* Parameters:
*  compareMode:  Compare Mode Enumerated Type.
*
* Return:
*  void
*
*******************************************************************************/
void GlobalCounter_SetCompareMode(uint8 compareMode) 
{
    /* Clear the compare mode bits in the control register */
    GlobalCounter_CONTROL &= ((uint8)(~GlobalCounter_CTRL_CMPMODE_MASK));
    
    /* Write the new setting */
    GlobalCounter_CONTROL |= compareMode;
}
#endif  /* (GlobalCounter_COMPARE_MODE_SOFTWARE) */


#if (GlobalCounter_CAPTURE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: GlobalCounter_SetCaptureMode
********************************************************************************
* Summary:
*  Sets the software controlled Capture Mode.
*
* Parameters:
*  captureMode:  Capture Mode Enumerated Type.
*
* Return:
*  void
*
*******************************************************************************/
void GlobalCounter_SetCaptureMode(uint8 captureMode) 
{
    /* Clear the capture mode bits in the control register */
    GlobalCounter_CONTROL &= ((uint8)(~GlobalCounter_CTRL_CAPMODE_MASK));
    
    /* Write the new setting */
    GlobalCounter_CONTROL |= ((uint8)((uint8)captureMode << GlobalCounter_CTRL_CAPMODE0_SHIFT));
}
#endif  /* (GlobalCounter_CAPTURE_MODE_SOFTWARE) */


/*******************************************************************************
* Function Name: GlobalCounter_ClearFIFO
********************************************************************************
* Summary:
*   This function clears all capture data from the capture FIFO
*
* Parameters:  
*  void:
*
* Return: 
*  None
*
*******************************************************************************/
void GlobalCounter_ClearFIFO(void) 
{

    while(0u != (GlobalCounter_ReadStatusRegister() & GlobalCounter_STATUS_FIFONEMP))
    {
        (void)GlobalCounter_ReadCapture();
    }

}
#endif  /* (!GlobalCounter_UsingFixedFunction) */


/* [] END OF FILE */

