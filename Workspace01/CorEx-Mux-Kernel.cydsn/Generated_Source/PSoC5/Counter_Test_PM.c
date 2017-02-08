/*******************************************************************************
* File Name: Counter_Test_PM.c  
* Version 3.0
*
*  Description:
*    This file provides the power management source code to API for the
*    Counter.  
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

#include "Counter_Test.h"

static Counter_Test_backupStruct Counter_Test_backup;


/*******************************************************************************
* Function Name: Counter_Test_SaveConfig
********************************************************************************
* Summary:
*     Save the current user configuration
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  Counter_Test_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void Counter_Test_SaveConfig(void) 
{
    #if (!Counter_Test_UsingFixedFunction)

        Counter_Test_backup.CounterUdb = Counter_Test_ReadCounter();

        #if(!Counter_Test_ControlRegRemoved)
            Counter_Test_backup.CounterControlRegister = Counter_Test_ReadControlRegister();
        #endif /* (!Counter_Test_ControlRegRemoved) */

    #endif /* (!Counter_Test_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Counter_Test_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  Counter_Test_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Counter_Test_RestoreConfig(void) 
{      
    #if (!Counter_Test_UsingFixedFunction)

       Counter_Test_WriteCounter(Counter_Test_backup.CounterUdb);

        #if(!Counter_Test_ControlRegRemoved)
            Counter_Test_WriteControlRegister(Counter_Test_backup.CounterControlRegister);
        #endif /* (!Counter_Test_ControlRegRemoved) */

    #endif /* (!Counter_Test_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Counter_Test_Sleep
********************************************************************************
* Summary:
*     Stop and Save the user configuration
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  Counter_Test_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void Counter_Test_Sleep(void) 
{
    #if(!Counter_Test_ControlRegRemoved)
        /* Save Counter's enable state */
        if(Counter_Test_CTRL_ENABLE == (Counter_Test_CONTROL & Counter_Test_CTRL_ENABLE))
        {
            /* Counter is enabled */
            Counter_Test_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            Counter_Test_backup.CounterEnableState = 0u;
        }
    #else
        Counter_Test_backup.CounterEnableState = 1u;
        if(Counter_Test_backup.CounterEnableState != 0u)
        {
            Counter_Test_backup.CounterEnableState = 0u;
        }
    #endif /* (!Counter_Test_ControlRegRemoved) */
    
    Counter_Test_Stop();
    Counter_Test_SaveConfig();
}


/*******************************************************************************
* Function Name: Counter_Test_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*  
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  Counter_Test_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Counter_Test_Wakeup(void) 
{
    Counter_Test_RestoreConfig();
    #if(!Counter_Test_ControlRegRemoved)
        if(Counter_Test_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            Counter_Test_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!Counter_Test_ControlRegRemoved) */
    
}


/* [] END OF FILE */
