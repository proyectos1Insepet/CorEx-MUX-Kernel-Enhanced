/*******************************************************************************
* File Name: StatusReg_iButton5thCRCBit.h  
* Version 1.90
*
* Description:
*  This file containts Status Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_STATUS_REG_StatusReg_iButton5thCRCBit_H) /* CY_STATUS_REG_StatusReg_iButton5thCRCBit_H */
#define CY_STATUS_REG_StatusReg_iButton5thCRCBit_H

#include "cytypes.h"
#include "CyLib.h"

    
/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 statusState;

} StatusReg_iButton5thCRCBit_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

uint8 StatusReg_iButton5thCRCBit_Read(void) ;
void StatusReg_iButton5thCRCBit_InterruptEnable(void) ;
void StatusReg_iButton5thCRCBit_InterruptDisable(void) ;
void StatusReg_iButton5thCRCBit_WriteMask(uint8 mask) ;
uint8 StatusReg_iButton5thCRCBit_ReadMask(void) ;


/***************************************
*           API Constants
***************************************/

#define StatusReg_iButton5thCRCBit_STATUS_INTR_ENBL    0x10u


/***************************************
*         Parameter Constants
***************************************/

/* Status Register Inputs */
#define StatusReg_iButton5thCRCBit_INPUTS              1


/***************************************
*             Registers
***************************************/

/* Status Register */
#define StatusReg_iButton5thCRCBit_Status             (* (reg8 *) StatusReg_iButton5thCRCBit_sts_sts_reg__STATUS_REG )
#define StatusReg_iButton5thCRCBit_Status_PTR         (  (reg8 *) StatusReg_iButton5thCRCBit_sts_sts_reg__STATUS_REG )
#define StatusReg_iButton5thCRCBit_Status_Mask        (* (reg8 *) StatusReg_iButton5thCRCBit_sts_sts_reg__MASK_REG )
#define StatusReg_iButton5thCRCBit_Status_Aux_Ctrl    (* (reg8 *) StatusReg_iButton5thCRCBit_sts_sts_reg__STATUS_AUX_CTL_REG )

#endif /* End CY_STATUS_REG_StatusReg_iButton5thCRCBit_H */


/* [] END OF FILE */
