/*******************************************************************************
* File Name: CRC_Security_PM.c
* Version 2.40
*
* Description:
*  This file provides Sleep APIs for CRC component.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "CRC_Security.h"


/*******************************************************************************
* Function Name: CRC_Security_SaveConfig
********************************************************************************
*
* Summary:
*  Saves seed and polynomial registers.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global Variables:
*  CRC_Security_backup - modified when non-retention registers are saved.
*
*******************************************************************************/
void CRC_Security_SaveConfig(void) 
{       
    /* Save D0 and D1 registers are none-retention for Panther ES1 */
    #if (CY_UDB_V0)
    
        #if(CRC_Security_CRC_SIZE <= 32u)
            CRC_Security_backup.polynomial = CRC_Security_ReadPolynomial();
            
        #else
            CRC_Security_backup.polynomialUpper = CRC_Security_ReadPolynomialUpper();
            CRC_Security_backup.polynomialLower = CRC_Security_ReadPolynomialLower();
            
        #endif  /* End (CRC_Security_CRC_SIZE <= 32u) */
        
    #endif  /* End (CY_UDB_V0) */
}


/*******************************************************************************
* Function Name: CRC_Security_Sleep
********************************************************************************
*
* Summary:
*  Stops CRC computation and saves CRC configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global Variables:
*  CRC_Security_backup - modified when non-retention registers are saved.
*
*******************************************************************************/
void CRC_Security_Sleep(void) 
{
    /* Store CRC enable state */
    if(0u != (CRC_Security_CONTROL_REG & CRC_Security_CTRL_ENABLE))
    {
        CRC_Security_backup.enableState = 1u;
        CRC_Security_Stop();
    }
    else
    {
        CRC_Security_backup.enableState = 0u;
    }
    
    CRC_Security_SaveConfig();
}


/*******************************************************************************
* Function Name: CRC_Security_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores seed and polynomial registers.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void CRC_Security_RestoreConfig(void) 
{      
    /* Restore D0 and D1 registers are none-retention for Panther ES1 */
    #if (CY_UDB_V0)
    
        #if(CRC_Security_CRC_SIZE <= 32u)
            CRC_Security_WritePolynomial(CRC_Security_backup.polynomial);
            
        #else
            CRC_Security_WritePolynomialUpper(CRC_Security_backup.polynomialUpper);
            CRC_Security_WritePolynomialLower(CRC_Security_backup.polynomialLower);
            
        #endif  /* End (CY_UDB_V0) */
        
    #endif  /* End (CRC_Security_CRC_SIZE <= 32u) */
    
    #if (CRC_Security_CRC_SIZE <= 32u)
        CRC_Security_WriteSeed(CRC_Security_DEFAULT_SEED);                        
    #else
        CRC_Security_WriteSeedUpper(CRC_Security_DEFAULT_SEED_UPPER);
        CRC_Security_WriteSeedLower(CRC_Security_DEFAULT_SEED_LOWER); 
    #endif  /* End (CRC_Security_CRC_SIZE <= 32u) */
    
    #if (CRC_Security_CRC_SIZE <= 32u)
        CRC_Security_ResetSeedInit(CRC_Security_DEFAULT_SEED);                        
    #else
        CRC_Security_ResetSeedInitUpper(CRC_Security_DEFAULT_SEED_UPPER);
        CRC_Security_ResetSeedInitLower(CRC_Security_DEFAULT_SEED_LOWER); 
    #endif  /* End (CRC_Security_CRC_SIZE <= 32u) */
}


/*******************************************************************************
* Function Name: CRC_Security_Wakeup
********************************************************************************
*
* Summary:
*  Restores CRC configuration and starts CRC computation. 
*
* Parameters:
*  void
*
* Return:
*  void
* 
*******************************************************************************/
void CRC_Security_Wakeup(void) 
{
    CRC_Security_RestoreConfig();
    
    /* Restore CRC enable state */
    if (CRC_Security_backup.enableState != 0u)
    {
        CRC_Security_Enable();
    }
}


/* [] END OF FILE */
