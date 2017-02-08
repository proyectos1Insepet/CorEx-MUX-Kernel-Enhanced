/*******************************************************************************
* File Name: Reset_iButton_PM.c
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

#include "Reset_iButton.h"

/* Check for removal by optimization */
#if !defined(Reset_iButton_Sync_ctrl_reg__REMOVED)

static Reset_iButton_BACKUP_STRUCT  Reset_iButton_backup = {0u};

    
/*******************************************************************************
* Function Name: Reset_iButton_SaveConfig
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
void Reset_iButton_SaveConfig(void) 
{
    Reset_iButton_backup.controlState = Reset_iButton_Control;
}


/*******************************************************************************
* Function Name: Reset_iButton_RestoreConfig
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
void Reset_iButton_RestoreConfig(void) 
{
     Reset_iButton_Control = Reset_iButton_backup.controlState;
}


/*******************************************************************************
* Function Name: Reset_iButton_Sleep
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
void Reset_iButton_Sleep(void) 
{
    Reset_iButton_SaveConfig();
}


/*******************************************************************************
* Function Name: Reset_iButton_Wakeup
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
void Reset_iButton_Wakeup(void)  
{
    Reset_iButton_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
