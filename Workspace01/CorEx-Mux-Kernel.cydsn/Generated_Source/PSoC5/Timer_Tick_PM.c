/*******************************************************************************
* File Name: Timer_Tick_PM.c
* Version 2.70
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "Timer_Tick.h"

static Timer_Tick_backupStruct Timer_Tick_backup;


/*******************************************************************************
* Function Name: Timer_Tick_SaveConfig
********************************************************************************
*
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
*  Timer_Tick_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_Tick_SaveConfig(void) 
{
    #if (!Timer_Tick_UsingFixedFunction)
        Timer_Tick_backup.TimerUdb = Timer_Tick_ReadCounter();
        Timer_Tick_backup.InterruptMaskValue = Timer_Tick_STATUS_MASK;
        #if (Timer_Tick_UsingHWCaptureCounter)
            Timer_Tick_backup.TimerCaptureCounter = Timer_Tick_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_Tick_UDB_CONTROL_REG_REMOVED)
            Timer_Tick_backup.TimerControlRegister = Timer_Tick_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_Tick_RestoreConfig
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
*  Timer_Tick_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_Tick_RestoreConfig(void) 
{   
    #if (!Timer_Tick_UsingFixedFunction)

        Timer_Tick_WriteCounter(Timer_Tick_backup.TimerUdb);
        Timer_Tick_STATUS_MASK =Timer_Tick_backup.InterruptMaskValue;
        #if (Timer_Tick_UsingHWCaptureCounter)
            Timer_Tick_SetCaptureCount(Timer_Tick_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_Tick_UDB_CONTROL_REG_REMOVED)
            Timer_Tick_WriteControlRegister(Timer_Tick_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_Tick_Sleep
********************************************************************************
*
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
*  Timer_Tick_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_Tick_Sleep(void) 
{
    #if(!Timer_Tick_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_Tick_CTRL_ENABLE == (Timer_Tick_CONTROL & Timer_Tick_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_Tick_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_Tick_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_Tick_Stop();
    Timer_Tick_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_Tick_Wakeup
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
*  Timer_Tick_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_Tick_Wakeup(void) 
{
    Timer_Tick_RestoreConfig();
    #if(!Timer_Tick_UDB_CONTROL_REG_REMOVED)
        if(Timer_Tick_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_Tick_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
