/*******************************************************************************
* File Name: Reset_TimeoutTimer.c  
* Version 1.80
*
* Description:
*  This file contains API to enable firmware control of a Control Register.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "Reset_TimeoutTimer.h"

/* Check for removal by optimization */
#if !defined(Reset_TimeoutTimer_Sync_ctrl_reg__REMOVED)

    
/*******************************************************************************
* Function Name: Reset_TimeoutTimer_Write
********************************************************************************
*
* Summary:
*  Write a byte to the Control Register.
*
* Parameters:
*  control:  The value to be assigned to the Control Register.
*
* Return:
*  None.
*
*******************************************************************************/
void Reset_TimeoutTimer_Write(uint8 control) 
{
    Reset_TimeoutTimer_Control = control;
}


/*******************************************************************************
* Function Name: Reset_TimeoutTimer_Read
********************************************************************************
*
* Summary:
*  Reads the current value assigned to the Control Register.
*
* Parameters:
*  None.
*
* Return:
*  Returns the current value in the Control Register.
*
*******************************************************************************/
uint8 Reset_TimeoutTimer_Read(void) 
{
    return Reset_TimeoutTimer_Control;
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
