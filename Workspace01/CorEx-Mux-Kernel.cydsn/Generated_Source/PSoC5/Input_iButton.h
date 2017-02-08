/*******************************************************************************
* File Name: Input_iButton.h  
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

#if !defined(CY_CONTROL_REG_Input_iButton_H) /* CY_CONTROL_REG_Input_iButton_H */
#define CY_CONTROL_REG_Input_iButton_H

#include "cytypes.h"

    
/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 controlState;

} Input_iButton_BACKUP_STRUCT;


/***************************************
*         Function Prototypes 
***************************************/

void    Input_iButton_Write(uint8 control) ;
uint8   Input_iButton_Read(void) ;

void Input_iButton_SaveConfig(void) ;
void Input_iButton_RestoreConfig(void) ;
void Input_iButton_Sleep(void) ; 
void Input_iButton_Wakeup(void) ;


/***************************************
*            Registers        
***************************************/

/* Control Register */
#define Input_iButton_Control        (* (reg8 *) Input_iButton_Sync_ctrl_reg__CONTROL_REG )
#define Input_iButton_Control_PTR    (  (reg8 *) Input_iButton_Sync_ctrl_reg__CONTROL_REG )

#endif /* End CY_CONTROL_REG_Input_iButton_H */


/* [] END OF FILE */
