/*******************************************************************************
* File Name: Display2_PM.c
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

#include "Display2.h"


/***************************************
* Local data allocation
***************************************/

static Display2_BACKUP_STRUCT  Display2_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: Display2_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the Display2_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Display2_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Display2_SaveConfig(void)
{
    #if(Display2_CONTROL_REG_REMOVED == 0u)
        Display2_backup.cr = Display2_CONTROL_REG;
    #endif /* End Display2_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Display2_RestoreConfig
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
*  Display2_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling Display2_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void Display2_RestoreConfig(void)
{
    #if(Display2_CONTROL_REG_REMOVED == 0u)
        Display2_CONTROL_REG = Display2_backup.cr;
    #endif /* End Display2_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Display2_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The Display2_Sleep() API saves the current component state. Then it
*  calls the Display2_Stop() function and calls 
*  Display2_SaveConfig() to save the hardware configuration.
*  Call the Display2_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Display2_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Display2_Sleep(void)
{
    #if(Display2_RX_ENABLED || Display2_HD_ENABLED)
        if((Display2_RXSTATUS_ACTL_REG  & Display2_INT_ENABLE) != 0u)
        {
            Display2_backup.enableState = 1u;
        }
        else
        {
            Display2_backup.enableState = 0u;
        }
    #else
        if((Display2_TXSTATUS_ACTL_REG  & Display2_INT_ENABLE) !=0u)
        {
            Display2_backup.enableState = 1u;
        }
        else
        {
            Display2_backup.enableState = 0u;
        }
    #endif /* End Display2_RX_ENABLED || Display2_HD_ENABLED*/

    Display2_Stop();
    Display2_SaveConfig();
}


/*******************************************************************************
* Function Name: Display2_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  Display2_Sleep() was called. The Display2_Wakeup() function
*  calls the Display2_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  Display2_Sleep() function was called, the Display2_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Display2_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Display2_Wakeup(void)
{
    Display2_RestoreConfig();
    #if( (Display2_RX_ENABLED) || (Display2_HD_ENABLED) )
        Display2_ClearRxBuffer();
    #endif /* End (Display2_RX_ENABLED) || (Display2_HD_ENABLED) */
    #if(Display2_TX_ENABLED || Display2_HD_ENABLED)
        Display2_ClearTxBuffer();
    #endif /* End Display2_TX_ENABLED || Display2_HD_ENABLED */

    if(Display2_backup.enableState != 0u)
    {
        Display2_Enable();
    }
}


/* [] END OF FILE */
