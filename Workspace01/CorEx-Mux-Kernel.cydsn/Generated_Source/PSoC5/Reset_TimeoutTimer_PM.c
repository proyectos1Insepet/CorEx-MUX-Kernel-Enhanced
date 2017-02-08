/*******************************************************************************
* File Name: Reset_TimeoutTimer_PM.c
* Version 1.80
*
* Description:
*  This file contains the setup, control, and status commands to support 
*  the component operation in the low power mode. 
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "Reset_TimeoutTimer.h"

/* Check for removal by optimization */
#if !defined(Reset_TimeoutTimer_Sync_ctrl_reg__REMOVED)

static Reset_TimeoutTimer_BACKUP_STRUCT  Reset_TimeoutTimer_backup = {0u};

    
/*******************************************************************************
* Function Name: Reset_TimeoutTimer_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Reset_TimeoutTimer_SaveConfig(void) 
{
    Reset_TimeoutTimer_backup.controlState = Reset_TimeoutTimer_Control;
}


/*******************************************************************************
* Function Name: Reset_TimeoutTimer_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*
*******************************************************************************/
void Reset_TimeoutTimer_RestoreConfig(void) 
{
     Reset_TimeoutTimer_Control = Reset_TimeoutTimer_backup.controlState;
}


/*******************************************************************************
* Function Name: Reset_TimeoutTimer_Sleep
********************************************************************************
*
* Summary:
*  Prepares the component for entering the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Reset_TimeoutTimer_Sleep(void) 
{
    Reset_TimeoutTimer_SaveConfig();
}


/*******************************************************************************
* Function Name: Reset_TimeoutTimer_Wakeup
********************************************************************************
*
* Summary:
*  Restores the component after waking up from the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Reset_TimeoutTimer_Wakeup(void)  
{
    Reset_TimeoutTimer_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
