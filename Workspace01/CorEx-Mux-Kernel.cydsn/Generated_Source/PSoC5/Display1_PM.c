/*******************************************************************************
* File Name: Display1_PM.c
* Version 2.50
*
* Description:
*  This file provides Sleep/WakeUp APIs functionality.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Display1.h"


/***************************************
* Local data allocation
***************************************/

static Display1_BACKUP_STRUCT  Display1_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: Display1_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the Display1_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Display1_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Display1_SaveConfig(void)
{
    #if(Display1_CONTROL_REG_REMOVED == 0u)
        Display1_backup.cr = Display1_CONTROL_REG;
    #endif /* End Display1_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Display1_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the nonretention control register except FIFO.
*  Does not restore the FIFO which is a set of nonretention registers.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Display1_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling Display1_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void Display1_RestoreConfig(void)
{
    #if(Display1_CONTROL_REG_REMOVED == 0u)
        Display1_CONTROL_REG = Display1_backup.cr;
    #endif /* End Display1_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Display1_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The Display1_Sleep() API saves the current component state. Then it
*  calls the Display1_Stop() function and calls 
*  Display1_SaveConfig() to save the hardware configuration.
*  Call the Display1_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Display1_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Display1_Sleep(void)
{
    #if(Display1_RX_ENABLED || Display1_HD_ENABLED)
        if((Display1_RXSTATUS_ACTL_REG  & Display1_INT_ENABLE) != 0u)
        {
            Display1_backup.enableState = 1u;
        }
        else
        {
            Display1_backup.enableState = 0u;
        }
    #else
        if((Display1_TXSTATUS_ACTL_REG  & Display1_INT_ENABLE) !=0u)
        {
            Display1_backup.enableState = 1u;
        }
        else
        {
            Display1_backup.enableState = 0u;
        }
    #endif /* End Display1_RX_ENABLED || Display1_HD_ENABLED*/

    Display1_Stop();
    Display1_SaveConfig();
}


/*******************************************************************************
* Function Name: Display1_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  Display1_Sleep() was called. The Display1_Wakeup() function
*  calls the Display1_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  Display1_Sleep() function was called, the Display1_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Display1_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Display1_Wakeup(void)
{
    Display1_RestoreConfig();
    #if( (Display1_RX_ENABLED) || (Display1_HD_ENABLED) )
        Display1_ClearRxBuffer();
    #endif /* End (Display1_RX_ENABLED) || (Display1_HD_ENABLED) */
    #if(Display1_TX_ENABLED || Display1_HD_ENABLED)
        Display1_ClearTxBuffer();
    #endif /* End Display1_TX_ENABLED || Display1_HD_ENABLED */

    if(Display1_backup.enableState != 0u)
    {
        Display1_Enable();
    }
}


/* [] END OF FILE */
