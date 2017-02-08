/*******************************************************************************
* File Name: ShiftReg_iButtonCRC4LSB_PM.c
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

#include "ShiftReg_iButtonCRC4LSB.h"

static ShiftReg_iButtonCRC4LSB_BACKUP_STRUCT ShiftReg_iButtonCRC4LSB_backup =
{
    ShiftReg_iButtonCRC4LSB_DISABLED,

    ((uint8) ShiftReg_iButtonCRC4LSB_DEFAULT_A0),
    ((uint8) ShiftReg_iButtonCRC4LSB_DEFAULT_A1),

    #if(CY_UDB_V0)
        ((uint8) ShiftReg_iButtonCRC4LSB_INT_SRC),
    #endif /* (CY_UDB_V0) */
};


/*******************************************************************************
* Function Name: ShiftReg_iButtonCRC4LSB_SaveConfig
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
void ShiftReg_iButtonCRC4LSB_SaveConfig(void) 
{
    /* Store working registers A0 and A1 */
    ShiftReg_iButtonCRC4LSB_backup.saveSrA0Reg   = CY_GET_REG8(ShiftReg_iButtonCRC4LSB_SHIFT_REG_LSB_PTR);
    ShiftReg_iButtonCRC4LSB_backup.saveSrA1Reg   = CY_GET_REG8(ShiftReg_iButtonCRC4LSB_SHIFT_REG_VALUE_LSB_PTR);

    #if(CY_UDB_V0)
        ShiftReg_iButtonCRC4LSB_backup.saveSrIntMask = ShiftReg_iButtonCRC4LSB_SR_STATUS_MASK;
    #endif /* (CY_UDB_V0) */
}


/*******************************************************************************
* Function Name: ShiftReg_iButtonCRC4LSB_RestoreConfig
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
void ShiftReg_iButtonCRC4LSB_RestoreConfig(void) 
{
    /* Restore working registers A0 and A1 */
    CY_SET_REG8(ShiftReg_iButtonCRC4LSB_SHIFT_REG_LSB_PTR, ShiftReg_iButtonCRC4LSB_backup.saveSrA0Reg);
    CY_SET_REG8(ShiftReg_iButtonCRC4LSB_SHIFT_REG_VALUE_LSB_PTR, ShiftReg_iButtonCRC4LSB_backup.saveSrA1Reg);

    #if(CY_UDB_V0)
        ShiftReg_iButtonCRC4LSB_SR_STATUS_MASK = ((uint8) ShiftReg_iButtonCRC4LSB_backup.saveSrIntMask);
    #endif /* (CY_UDB_V0) */
}


/*******************************************************************************
* Function Name: ShiftReg_iButtonCRC4LSB_Sleep
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
void ShiftReg_iButtonCRC4LSB_Sleep(void) 
{
    ShiftReg_iButtonCRC4LSB_backup.enableState = ((uint8) ShiftReg_iButtonCRC4LSB_IS_ENABLED);

    ShiftReg_iButtonCRC4LSB_Stop();
    ShiftReg_iButtonCRC4LSB_SaveConfig();
}


/*******************************************************************************
* Function Name: ShiftReg_iButtonCRC4LSB_Wakeup
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
void ShiftReg_iButtonCRC4LSB_Wakeup(void) 
{
    ShiftReg_iButtonCRC4LSB_RestoreConfig();

    if(0u != ShiftReg_iButtonCRC4LSB_backup.enableState)
    {
        ShiftReg_iButtonCRC4LSB_Enable();
    }
}


/* [] END OF FILE */
