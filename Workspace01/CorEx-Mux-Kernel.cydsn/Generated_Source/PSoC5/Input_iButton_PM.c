/*******************************************************************************
* File Name: Input_iButton_PM.c
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

#include "Input_iButton.h"

/* Check for removal by optimization */
#if !defined(Input_iButton_Sync_ctrl_reg__REMOVED)

static Input_iButton_BACKUP_STRUCT  Input_iButton_backup = {0u};

    
/*******************************************************************************
* Function Name: Input_iButton_SaveConfig
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
void Input_iButton_SaveConfig(void) 
{
    Input_iButton_backup.controlState = Input_iButton_Control;
}


/*******************************************************************************
* Function Name: Input_iButton_RestoreConfig
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
void Input_iButton_RestoreConfig(void) 
{
     Input_iButton_Control = Input_iButton_backup.controlState;
}


/*******************************************************************************
* Function Name: Input_iButton_Sleep
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
void Input_iButton_Sleep(void) 
{
    Input_iButton_SaveConfig();
}


/*******************************************************************************
* Function Name: Input_iButton_Wakeup
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
void Input_iButton_Wakeup(void)  
{
    Input_iButton_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
