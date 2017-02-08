/*******************************************************************************
* File Name: Timer_DataLink_PM.c
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

#include "Timer_DataLink.h"

static Timer_DataLink_backupStruct Timer_DataLink_backup;


/*******************************************************************************
* Function Name: Timer_DataLink_SaveConfig
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
*  Timer_DataLink_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_DataLink_SaveConfig(void) 
{
    #if (!Timer_DataLink_UsingFixedFunction)
        Timer_DataLink_backup.TimerUdb = Timer_DataLink_ReadCounter();
        Timer_DataLink_backup.InterruptMaskValue = Timer_DataLink_STATUS_MASK;
        #if (Timer_DataLink_UsingHWCaptureCounter)
            Timer_DataLink_backup.TimerCaptureCounter = Timer_DataLink_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_DataLink_UDB_CONTROL_REG_REMOVED)
            Timer_DataLink_backup.TimerControlRegister = Timer_DataLink_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_DataLink_RestoreConfig
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
*  Timer_DataLink_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_DataLink_RestoreConfig(void) 
{   
    #if (!Timer_DataLink_UsingFixedFunction)

        Timer_DataLink_WriteCounter(Timer_DataLink_backup.TimerUdb);
        Timer_DataLink_STATUS_MASK =Timer_DataLink_backup.InterruptMaskValue;
        #if (Timer_DataLink_UsingHWCaptureCounter)
            Timer_DataLink_SetCaptureCount(Timer_DataLink_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_DataLink_UDB_CONTROL_REG_REMOVED)
            Timer_DataLink_WriteControlRegister(Timer_DataLink_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_DataLink_Sleep
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
*  Timer_DataLink_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_DataLink_Sleep(void) 
{
    #if(!Timer_DataLink_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_DataLink_CTRL_ENABLE == (Timer_DataLink_CONTROL & Timer_DataLink_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_DataLink_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_DataLink_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_DataLink_Stop();
    Timer_DataLink_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_DataLink_Wakeup
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
*  Timer_DataLink_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_DataLink_Wakeup(void) 
{
    Timer_DataLink_RestoreConfig();
    #if(!Timer_DataLink_UDB_CONTROL_REG_REMOVED)
        if(Timer_DataLink_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_DataLink_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
