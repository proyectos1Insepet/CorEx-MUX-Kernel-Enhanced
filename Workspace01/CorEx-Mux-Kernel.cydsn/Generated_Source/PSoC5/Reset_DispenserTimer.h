/*******************************************************************************
* File Name: Reset_DispenserTimer.h  
* Version 1.80
*
* Description:
*  This file containts Control Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CONTROL_REG_Reset_DispenserTimer_H) /* CY_CONTROL_REG_Reset_DispenserTimer_H */
#define CY_CONTROL_REG_Reset_DispenserTimer_H

#include "cytypes.h"

    
/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 controlState;

} Reset_DispenserTimer_BACKUP_STRUCT;


/***************************************
*         Function Prototypes 
***************************************/

void    Reset_DispenserTimer_Write(uint8 control) ;
uint8   Reset_DispenserTimer_Read(void) ;

void Reset_DispenserTimer_SaveConfig(void) ;
void Reset_DispenserTimer_RestoreConfig(void) ;
void Reset_DispenserTimer_Sleep(void) ; 
void Reset_DispenserTimer_Wakeup(void) ;


/***************************************
*            Registers        
***************************************/

/* Control Register */
#define Reset_DispenserTimer_Control        (* (reg8 *) Reset_DispenserTimer_Sync_ctrl_reg__CONTROL_REG )
#define Reset_DispenserTimer_Control_PTR    (  (reg8 *) Reset_DispenserTimer_Sync_ctrl_reg__CONTROL_REG )

#endif /* End CY_CONTROL_REG_Reset_DispenserTimer_H */


/* [] END OF FILE */
