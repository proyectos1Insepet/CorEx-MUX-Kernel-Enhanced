/*******************************************************************************
* File Name: Load_Unscramble.h  
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

#if !defined(CY_CONTROL_REG_Load_Unscramble_H) /* CY_CONTROL_REG_Load_Unscramble_H */
#define CY_CONTROL_REG_Load_Unscramble_H

#include "cytypes.h"

    
/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 controlState;

} Load_Unscramble_BACKUP_STRUCT;


/***************************************
*         Function Prototypes 
***************************************/

void    Load_Unscramble_Write(uint8 control) ;
uint8   Load_Unscramble_Read(void) ;

void Load_Unscramble_SaveConfig(void) ;
void Load_Unscramble_RestoreConfig(void) ;
void Load_Unscramble_Sleep(void) ; 
void Load_Unscramble_Wakeup(void) ;


/***************************************
*            Registers        
***************************************/

/* Control Register */
#define Load_Unscramble_Control        (* (reg8 *) Load_Unscramble_Sync_ctrl_reg__CONTROL_REG )
#define Load_Unscramble_Control_PTR    (  (reg8 *) Load_Unscramble_Sync_ctrl_reg__CONTROL_REG )

#endif /* End CY_CONTROL_REG_Load_Unscramble_H */


/* [] END OF FILE */
