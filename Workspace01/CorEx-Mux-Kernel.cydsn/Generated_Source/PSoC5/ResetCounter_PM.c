/*******************************************************************************
* File Name: ResetCounter_PM.c
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

#include "ResetCounter.h"

/* Check for removal by optimization */
#if !defined(ResetCounter_Sync_ctrl_reg__REMOVED)

static ResetCounter_BACKUP_STRUCT  ResetCounter_backup = {0u};

    
/*******************************************************************************
* Function Name: ResetCounter_SaveConfig
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
void ResetCounter_SaveConfig(void) 
{
    ResetCounter_backup.controlState = ResetCounter_Control;
}


/*******************************************************************************
* Function Name: ResetCounter_RestoreConfig
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
void ResetCounter_RestoreConfig(void) 
{
     ResetCounter_Control = ResetCounter_backup.controlState;
}


/*******************************************************************************
* Function Name: ResetCounter_Sleep
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
void ResetCounter_Sleep(void) 
{
    ResetCounter_SaveConfig();
}


/*******************************************************************************
* Function Name: ResetCounter_Wakeup
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
void ResetCounter_Wakeup(void)  
{
    ResetCounter_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
