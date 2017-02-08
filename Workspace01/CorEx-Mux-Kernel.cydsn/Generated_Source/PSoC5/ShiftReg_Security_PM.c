/*******************************************************************************
* File Name: ShiftReg_Security_PM.c
* Version 2.30
*
* Description:
*  This file provides the API source code for sleep mode support for Shift
*  Register component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "ShiftReg_Security.h"

static ShiftReg_Security_BACKUP_STRUCT ShiftReg_Security_backup =
{
    ShiftReg_Security_DISABLED,

    ((uint8) ShiftReg_Security_DEFAULT_A0),
    ((uint8) ShiftReg_Security_DEFAULT_A1),

    #if(CY_UDB_V0)
        ((uint8) ShiftReg_Security_INT_SRC),
    #endif /* (CY_UDB_V0) */
};


/*******************************************************************************
* Function Name: ShiftReg_Security_SaveConfig
********************************************************************************
*
* Summary:
*  Saves Shift Register configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void ShiftReg_Security_SaveConfig(void) 
{
    /* Store working registers A0 and A1 */
    ShiftReg_Security_backup.saveSrA0Reg   = CY_GET_REG8(ShiftReg_Security_SHIFT_REG_LSB_PTR);
    ShiftReg_Security_backup.saveSrA1Reg   = CY_GET_REG8(ShiftReg_Security_SHIFT_REG_VALUE_LSB_PTR);

    #if(CY_UDB_V0)
        ShiftReg_Security_backup.saveSrIntMask = ShiftReg_Security_SR_STATUS_MASK;
    #endif /* (CY_UDB_V0) */
}


/*******************************************************************************
* Function Name: ShiftReg_Security_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores Shift Register configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void ShiftReg_Security_RestoreConfig(void) 
{
    /* Restore working registers A0 and A1 */
    CY_SET_REG8(ShiftReg_Security_SHIFT_REG_LSB_PTR, ShiftReg_Security_backup.saveSrA0Reg);
    CY_SET_REG8(ShiftReg_Security_SHIFT_REG_VALUE_LSB_PTR, ShiftReg_Security_backup.saveSrA1Reg);

    #if(CY_UDB_V0)
        ShiftReg_Security_SR_STATUS_MASK = ((uint8) ShiftReg_Security_backup.saveSrIntMask);
    #endif /* (CY_UDB_V0) */
}


/*******************************************************************************
* Function Name: ShiftReg_Security_Sleep
********************************************************************************
*
* Summary:
*  Prepare the component to enter a Sleep mode.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void ShiftReg_Security_Sleep(void) 
{
    ShiftReg_Security_backup.enableState = ((uint8) ShiftReg_Security_IS_ENABLED);

    ShiftReg_Security_Stop();
    ShiftReg_Security_SaveConfig();
}


/*******************************************************************************
* Function Name: ShiftReg_Security_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void ShiftReg_Security_Wakeup(void) 
{
    ShiftReg_Security_RestoreConfig();

    if(0u != ShiftReg_Security_backup.enableState)
    {
        ShiftReg_Security_Enable();
    }
}


/* [] END OF FILE */
