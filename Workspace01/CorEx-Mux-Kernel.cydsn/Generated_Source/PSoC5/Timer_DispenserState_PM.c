/*******************************************************************************
* File Name: Timer_DispenserState_PM.c
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

#include "Timer_DispenserState.h"

static Timer_DispenserState_backupStruct Timer_DispenserState_backup;


/*******************************************************************************
* Function Name: Timer_DispenserState_SaveConfig
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
*  Timer_DispenserState_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_DispenserState_SaveConfig(void) 
{
    #if (!Timer_DispenserState_UsingFixedFunction)
        Timer_DispenserState_backup.TimerUdb = Timer_DispenserState_ReadCounter();
        Timer_DispenserState_backup.InterruptMaskValue = Timer_DispenserState_STATUS_MASK;
        #if (Timer_DispenserState_UsingHWCaptureCounter)
            Timer_DispenserState_backup.TimerCaptureCounter = Timer_DispenserState_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_DispenserState_UDB_CONTROL_REG_REMOVED)
            Timer_DispenserState_backup.TimerControlRegister = Timer_DispenserState_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_DispenserState_RestoreConfig
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
*  Timer_DispenserState_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_DispenserState_RestoreConfig(void) 
{   
    #if (!Timer_DispenserState_UsingFixedFunction)

        Timer_DispenserState_WriteCounter(Timer_DispenserState_backup.TimerUdb);
        Timer_DispenserState_STATUS_MASK =Timer_DispenserState_backup.InterruptMaskValue;
        #if (Timer_DispenserState_UsingHWCaptureCounter)
            Timer_DispenserState_SetCaptureCount(Timer_DispenserState_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_DispenserState_UDB_CONTROL_REG_REMOVED)
            Timer_DispenserState_WriteControlRegister(Timer_DispenserState_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_DispenserState_Sleep
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
*  Timer_DispenserState_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_DispenserState_Sleep(void) 
{
    #if(!Timer_DispenserState_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_DispenserState_CTRL_ENABLE == (Timer_DispenserState_CONTROL & Timer_DispenserState_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_DispenserState_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_DispenserState_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_DispenserState_Stop();
    Timer_DispenserState_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_DispenserState_Wakeup
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
*  Timer_DispenserState_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_DispenserState_Wakeup(void) 
{
    Timer_DispenserState_RestoreConfig();
    #if(!Timer_DispenserState_UDB_CONTROL_REG_REMOVED)
        if(Timer_DispenserState_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_DispenserState_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
