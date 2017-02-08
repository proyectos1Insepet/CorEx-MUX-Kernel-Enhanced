/*******************************************************************************
* File Name: Clock_Scramble_PM.c
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

#include "Clock_Scramble.h"

/* Check for removal by optimization */
#if !defined(Clock_Scramble_Sync_ctrl_reg__REMOVED)

static Clock_Scramble_BACKUP_STRUCT  Clock_Scramble_backup = {0u};

    
/*******************************************************************************
* Function Name: Clock_Scramble_SaveConfig
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
void Clock_Scramble_SaveConfig(void) 
{
    Clock_Scramble_backup.controlState = Clock_Scramble_Control;
}


/*******************************************************************************
* Function Name: Clock_Scramble_RestoreConfig
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
void Clock_Scramble_RestoreConfig(void) 
{
     Clock_Scramble_Control = Clock_Scramble_backup.controlState;
}


/*******************************************************************************
* Function Name: Clock_Scramble_Sleep
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
void Clock_Scramble_Sleep(void) 
{
    Clock_Scramble_SaveConfig();
}


/*******************************************************************************
* Function Name: Clock_Scramble_Wakeup
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
void Clock_Scramble_Wakeup(void)  
{
    Clock_Scramble_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
