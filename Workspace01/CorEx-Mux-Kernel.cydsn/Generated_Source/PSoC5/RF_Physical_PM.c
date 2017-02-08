/*******************************************************************************
* File Name: RF_Physical_PM.c
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

#include "RF_Physical.h"


/***************************************
* Local data allocation
***************************************/

static RF_Physical_BACKUP_STRUCT  RF_Physical_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: RF_Physical_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the RF_Physical_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  RF_Physical_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void RF_Physical_SaveConfig(void)
{
    #if(RF_Physical_CONTROL_REG_REMOVED == 0u)
        RF_Physical_backup.cr = RF_Physical_CONTROL_REG;
    #endif /* End RF_Physical_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: RF_Physical_RestoreConfig
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
*  RF_Physical_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling RF_Physical_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void RF_Physical_RestoreConfig(void)
{
    #if(RF_Physical_CONTROL_REG_REMOVED == 0u)
        RF_Physical_CONTROL_REG = RF_Physical_backup.cr;
    #endif /* End RF_Physical_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: RF_Physical_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The RF_Physical_Sleep() API saves the current component state. Then it
*  calls the RF_Physical_Stop() function and calls 
*  RF_Physical_SaveConfig() to save the hardware configuration.
*  Call the RF_Physical_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  RF_Physical_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void RF_Physical_Sleep(void)
{
    #if(RF_Physical_RX_ENABLED || RF_Physical_HD_ENABLED)
        if((RF_Physical_RXSTATUS_ACTL_REG  & RF_Physical_INT_ENABLE) != 0u)
        {
            RF_Physical_backup.enableState = 1u;
        }
        else
        {
            RF_Physical_backup.enableState = 0u;
        }
    #else
        if((RF_Physical_TXSTATUS_ACTL_REG  & RF_Physical_INT_ENABLE) !=0u)
        {
            RF_Physical_backup.enableState = 1u;
        }
        else
        {
            RF_Physical_backup.enableState = 0u;
        }
    #endif /* End RF_Physical_RX_ENABLED || RF_Physical_HD_ENABLED*/

    RF_Physical_Stop();
    RF_Physical_SaveConfig();
}


/*******************************************************************************
* Function Name: RF_Physical_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  RF_Physical_Sleep() was called. The RF_Physical_Wakeup() function
*  calls the RF_Physical_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  RF_Physical_Sleep() function was called, the RF_Physical_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  RF_Physical_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void RF_Physical_Wakeup(void)
{
    RF_Physical_RestoreConfig();
    #if( (RF_Physical_RX_ENABLED) || (RF_Physical_HD_ENABLED) )
        RF_Physical_ClearRxBuffer();
    #endif /* End (RF_Physical_RX_ENABLED) || (RF_Physical_HD_ENABLED) */
    #if(RF_Physical_TX_ENABLED || RF_Physical_HD_ENABLED)
        RF_Physical_ClearTxBuffer();
    #endif /* End RF_Physical_TX_ENABLED || RF_Physical_HD_ENABLED */

    if(RF_Physical_backup.enableState != 0u)
    {
        RF_Physical_Enable();
    }
}


/* [] END OF FILE */
