/*******************************************************************************
* File Name: Clock_Unscramble_PM.c
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

#include "Clock_Unscramble.h"

/* Check for removal by optimization */
#if !defined(Clock_Unscramble_Sync_ctrl_reg__REMOVED)

static Clock_Unscramble_BACKUP_STRUCT  Clock_Unscramble_backup = {0u};

    
/*******************************************************************************
* Function Name: Clock_Unscramble_SaveConfig
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
void Clock_Unscramble_SaveConfig(void) 
{
    Clock_Unscramble_backup.controlState = Clock_Unscramble_Control;
}


/*******************************************************************************
* Function Name: Clock_Unscramble_RestoreConfig
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
void Clock_Unscramble_RestoreConfig(void) 
{
     Clock_Unscramble_Control = Clock_Unscramble_backup.controlState;
}


/*******************************************************************************
* Function Name: Clock_Unscramble_Sleep
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
void Clock_Unscramble_Sleep(void) 
{
    Clock_Unscramble_SaveConfig();
}


/*******************************************************************************
* Function Name: Clock_Unscramble_Wakeup
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
void Clock_Unscramble_Wakeup(void)  
{
    Clock_Unscramble_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
