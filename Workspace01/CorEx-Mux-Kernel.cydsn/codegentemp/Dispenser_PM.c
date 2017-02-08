/*******************************************************************************
* File Name: Dispenser_PM.c
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

#include "Dispenser.h"


/***************************************
* Local data allocation
***************************************/

static Dispenser_BACKUP_STRUCT  Dispenser_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: Dispenser_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the Dispenser_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Dispenser_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Dispenser_SaveConfig(void)
{
    #if(Dispenser_CONTROL_REG_REMOVED == 0u)
        Dispenser_backup.cr = Dispenser_CONTROL_REG;
    #endif /* End Dispenser_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Dispenser_RestoreConfig
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
*  Dispenser_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling Dispenser_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void Dispenser_RestoreConfig(void)
{
    #if(Dispenser_CONTROL_REG_REMOVED == 0u)
        Dispenser_CONTROL_REG = Dispenser_backup.cr;
    #endif /* End Dispenser_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Dispenser_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The Dispenser_Sleep() API saves the current component state. Then it
*  calls the Dispenser_Stop() function and calls 
*  Dispenser_SaveConfig() to save the hardware configuration.
*  Call the Dispenser_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Dispenser_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Dispenser_Sleep(void)
{
    #if(Dispenser_RX_ENABLED || Dispenser_HD_ENABLED)
        if((Dispenser_RXSTATUS_ACTL_REG  & Dispenser_INT_ENABLE) != 0u)
        {
            Dispenser_backup.enableState = 1u;
        }
        else
        {
            Dispenser_backup.enableState = 0u;
        }
    #else
        if((Dispenser_TXSTATUS_ACTL_REG  & Dispenser_INT_ENABLE) !=0u)
        {
            Dispenser_backup.enableState = 1u;
        }
        else
        {
            Dispenser_backup.enableState = 0u;
        }
    #endif /* End Dispenser_RX_ENABLED || Dispenser_HD_ENABLED*/

    Dispenser_Stop();
    Dispenser_SaveConfig();
}


/*******************************************************************************
* Function Name: Dispenser_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  Dispenser_Sleep() was called. The Dispenser_Wakeup() function
*  calls the Dispenser_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  Dispenser_Sleep() function was called, the Dispenser_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Dispenser_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Dispenser_Wakeup(void)
{
    Dispenser_RestoreConfig();
    #if( (Dispenser_RX_ENABLED) || (Dispenser_HD_ENABLED) )
        Dispenser_ClearRxBuffer();
    #endif /* End (Dispenser_RX_ENABLED) || (Dispenser_HD_ENABLED) */
    #if(Dispenser_TX_ENABLED || Dispenser_HD_ENABLED)
        Dispenser_ClearTxBuffer();
    #endif /* End Dispenser_TX_ENABLED || Dispenser_HD_ENABLED */

    if(Dispenser_backup.enableState != 0u)
    {
        Dispenser_Enable();
    }
}


/* [] END OF FILE */
