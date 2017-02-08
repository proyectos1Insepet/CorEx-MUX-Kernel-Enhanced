/*******************************************************************************
* File Name: StatusReg_iButton5thCRCBit.c  
* Version 1.90
*
* Description:
*  This file contains API to enable firmware to read the value of a Status 
*  Register.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "StatusReg_iButton5thCRCBit.h"

#if !defined(StatusReg_iButton5thCRCBit_sts_sts_reg__REMOVED) /* Check for removal by optimization */


/*******************************************************************************
* Function Name: StatusReg_iButton5thCRCBit_Read
********************************************************************************
*
* Summary:
*  Reads the current value assigned to the Status Register.
*
* Parameters:
*  None.
*
* Return:
*  The current value in the Status Register.
*
*******************************************************************************/
uint8 StatusReg_iButton5thCRCBit_Read(void) 
{ 
    return StatusReg_iButton5thCRCBit_Status;
}


/*******************************************************************************
* Function Name: StatusReg_iButton5thCRCBit_InterruptEnable
********************************************************************************
*
* Summary:
*  Enables the Status Register interrupt.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void StatusReg_iButton5thCRCBit_InterruptEnable(void) 
{
    uint8 interruptState;
    interruptState = CyEnterCriticalSection();
    StatusReg_iButton5thCRCBit_Status_Aux_Ctrl |= StatusReg_iButton5thCRCBit_STATUS_INTR_ENBL;
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: StatusReg_iButton5thCRCBit_InterruptDisable
********************************************************************************
*
* Summary:
*  Disables the Status Register interrupt.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void StatusReg_iButton5thCRCBit_InterruptDisable(void) 
{
    uint8 interruptState;
    interruptState = CyEnterCriticalSection();
    StatusReg_iButton5thCRCBit_Status_Aux_Ctrl &= (uint8)(~StatusReg_iButton5thCRCBit_STATUS_INTR_ENBL);
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: StatusReg_iButton5thCRCBit_WriteMask
********************************************************************************
*
* Summary:
*  Writes the current mask value assigned to the Status Register.
*
* Parameters:
*  mask:  Value to write into the mask register.
*
* Return:
*  None.
*
*******************************************************************************/
void StatusReg_iButton5thCRCBit_WriteMask(uint8 mask) 
{
    #if(StatusReg_iButton5thCRCBit_INPUTS < 8u)
    	mask &= ((uint8)(1u << StatusReg_iButton5thCRCBit_INPUTS) - 1u);
	#endif /* End StatusReg_iButton5thCRCBit_INPUTS < 8u */
    StatusReg_iButton5thCRCBit_Status_Mask = mask;
}


/*******************************************************************************
* Function Name: StatusReg_iButton5thCRCBit_ReadMask
********************************************************************************
*
* Summary:
*  Reads the current interrupt mask assigned to the Status Register.
*
* Parameters:
*  None.
*
* Return:
*  The value of the interrupt mask of the Status Register.
*
*******************************************************************************/
uint8 StatusReg_iButton5thCRCBit_ReadMask(void) 
{
    return StatusReg_iButton5thCRCBit_Status_Mask;
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
