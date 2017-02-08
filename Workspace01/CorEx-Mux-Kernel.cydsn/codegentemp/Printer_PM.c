/*******************************************************************************
* File Name: Printer_PM.c
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

#include "Printer.h"


/***************************************
* Local data allocation
***************************************/

static Printer_BACKUP_STRUCT  Printer_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: Printer_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the Printer_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Printer_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Printer_SaveConfig(void)
{
    #if(Printer_CONTROL_REG_REMOVED == 0u)
        Printer_backup.cr = Printer_CONTROL_REG;
    #endif /* End Printer_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Printer_RestoreConfig
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
*  Printer_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling Printer_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void Printer_RestoreConfig(void)
{
    #if(Printer_CONTROL_REG_REMOVED == 0u)
        Printer_CONTROL_REG = Printer_backup.cr;
    #endif /* End Printer_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Printer_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The Printer_Sleep() API saves the current component state. Then it
*  calls the Printer_Stop() function and calls 
*  Printer_SaveConfig() to save the hardware configuration.
*  Call the Printer_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Printer_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Printer_Sleep(void)
{
    #if(Printer_RX_ENABLED || Printer_HD_ENABLED)
        if((Printer_RXSTATUS_ACTL_REG  & Printer_INT_ENABLE) != 0u)
        {
            Printer_backup.enableState = 1u;
        }
        else
        {
            Printer_backup.enableState = 0u;
        }
    #else
        if((Printer_TXSTATUS_ACTL_REG  & Printer_INT_ENABLE) !=0u)
        {
            Printer_backup.enableState = 1u;
        }
        else
        {
            Printer_backup.enableState = 0u;
        }
    #endif /* End Printer_RX_ENABLED || Printer_HD_ENABLED*/

    Printer_Stop();
    Printer_SaveConfig();
}


/*******************************************************************************
* Function Name: Printer_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  Printer_Sleep() was called. The Printer_Wakeup() function
*  calls the Printer_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  Printer_Sleep() function was called, the Printer_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Printer_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Printer_Wakeup(void)
{
    Printer_RestoreConfig();
    #if( (Printer_RX_ENABLED) || (Printer_HD_ENABLED) )
        Printer_ClearRxBuffer();
    #endif /* End (Printer_RX_ENABLED) || (Printer_HD_ENABLED) */
    #if(Printer_TX_ENABLED || Printer_HD_ENABLED)
        Printer_ClearTxBuffer();
    #endif /* End Printer_TX_ENABLED || Printer_HD_ENABLED */

    if(Printer_backup.enableState != 0u)
    {
        Printer_Enable();
    }
}


/* [] END OF FILE */
