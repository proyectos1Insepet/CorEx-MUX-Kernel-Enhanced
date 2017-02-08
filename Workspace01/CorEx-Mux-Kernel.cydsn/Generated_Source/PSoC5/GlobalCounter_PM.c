/*******************************************************************************
* File Name: GlobalCounter_PM.c  
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

#include "GlobalCounter.h"

static GlobalCounter_backupStruct GlobalCounter_backup;


/*******************************************************************************
* Function Name: GlobalCounter_SaveConfig
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
*  GlobalCounter_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void GlobalCounter_SaveConfig(void) 
{
    #if (!GlobalCounter_UsingFixedFunction)

        GlobalCounter_backup.CounterUdb = GlobalCounter_ReadCounter();

        #if(!GlobalCounter_ControlRegRemoved)
            GlobalCounter_backup.CounterControlRegister = GlobalCounter_ReadControlRegister();
        #endif /* (!GlobalCounter_ControlRegRemoved) */

    #endif /* (!GlobalCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: GlobalCounter_RestoreConfig
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
*  GlobalCounter_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void GlobalCounter_RestoreConfig(void) 
{      
    #if (!GlobalCounter_UsingFixedFunction)

       GlobalCounter_WriteCounter(GlobalCounter_backup.CounterUdb);

        #if(!GlobalCounter_ControlRegRemoved)
            GlobalCounter_WriteControlRegister(GlobalCounter_backup.CounterControlRegister);
        #endif /* (!GlobalCounter_ControlRegRemoved) */

    #endif /* (!GlobalCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: GlobalCounter_Sleep
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
*  GlobalCounter_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void GlobalCounter_Sleep(void) 
{
    #if(!GlobalCounter_ControlRegRemoved)
        /* Save Counter's enable state */
        if(GlobalCounter_CTRL_ENABLE == (GlobalCounter_CONTROL & GlobalCounter_CTRL_ENABLE))
        {
            /* Counter is enabled */
            GlobalCounter_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            GlobalCounter_backup.CounterEnableState = 0u;
        }
    #else
        GlobalCounter_backup.CounterEnableState = 1u;
        if(GlobalCounter_backup.CounterEnableState != 0u)
        {
            GlobalCounter_backup.CounterEnableState = 0u;
        }
    #endif /* (!GlobalCounter_ControlRegRemoved) */
    
    GlobalCounter_Stop();
    GlobalCounter_SaveConfig();
}


/*******************************************************************************
* Function Name: GlobalCounter_Wakeup
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
*  GlobalCounter_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void GlobalCounter_Wakeup(void) 
{
    GlobalCounter_RestoreConfig();
    #if(!GlobalCounter_ControlRegRemoved)
        if(GlobalCounter_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            GlobalCounter_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!GlobalCounter_ControlRegRemoved) */
    
}


/* [] END OF FILE */
