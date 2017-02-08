/*******************************************************************************
* File Name: iButton1.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_iButton1_H) /* Pins iButton1_H */
#define CY_PINS_iButton1_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "iButton1_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 iButton1__PORT == 15 && ((iButton1__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    iButton1_Write(uint8 value);
void    iButton1_SetDriveMode(uint8 mode);
uint8   iButton1_ReadDataReg(void);
uint8   iButton1_Read(void);
void    iButton1_SetInterruptMode(uint16 position, uint16 mode);
uint8   iButton1_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the iButton1_SetDriveMode() function.
     *  @{
     */
        #define iButton1_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define iButton1_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define iButton1_DM_RES_UP          PIN_DM_RES_UP
        #define iButton1_DM_RES_DWN         PIN_DM_RES_DWN
        #define iButton1_DM_OD_LO           PIN_DM_OD_LO
        #define iButton1_DM_OD_HI           PIN_DM_OD_HI
        #define iButton1_DM_STRONG          PIN_DM_STRONG
        #define iButton1_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define iButton1_MASK               iButton1__MASK
#define iButton1_SHIFT              iButton1__SHIFT
#define iButton1_WIDTH              1u

/* Interrupt constants */
#if defined(iButton1__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in iButton1_SetInterruptMode() function.
     *  @{
     */
        #define iButton1_INTR_NONE      (uint16)(0x0000u)
        #define iButton1_INTR_RISING    (uint16)(0x0001u)
        #define iButton1_INTR_FALLING   (uint16)(0x0002u)
        #define iButton1_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define iButton1_INTR_MASK      (0x01u) 
#endif /* (iButton1__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define iButton1_PS                     (* (reg8 *) iButton1__PS)
/* Data Register */
#define iButton1_DR                     (* (reg8 *) iButton1__DR)
/* Port Number */
#define iButton1_PRT_NUM                (* (reg8 *) iButton1__PRT) 
/* Connect to Analog Globals */                                                  
#define iButton1_AG                     (* (reg8 *) iButton1__AG)                       
/* Analog MUX bux enable */
#define iButton1_AMUX                   (* (reg8 *) iButton1__AMUX) 
/* Bidirectional Enable */                                                        
#define iButton1_BIE                    (* (reg8 *) iButton1__BIE)
/* Bit-mask for Aliased Register Access */
#define iButton1_BIT_MASK               (* (reg8 *) iButton1__BIT_MASK)
/* Bypass Enable */
#define iButton1_BYP                    (* (reg8 *) iButton1__BYP)
/* Port wide control signals */                                                   
#define iButton1_CTL                    (* (reg8 *) iButton1__CTL)
/* Drive Modes */
#define iButton1_DM0                    (* (reg8 *) iButton1__DM0) 
#define iButton1_DM1                    (* (reg8 *) iButton1__DM1)
#define iButton1_DM2                    (* (reg8 *) iButton1__DM2) 
/* Input Buffer Disable Override */
#define iButton1_INP_DIS                (* (reg8 *) iButton1__INP_DIS)
/* LCD Common or Segment Drive */
#define iButton1_LCD_COM_SEG            (* (reg8 *) iButton1__LCD_COM_SEG)
/* Enable Segment LCD */
#define iButton1_LCD_EN                 (* (reg8 *) iButton1__LCD_EN)
/* Slew Rate Control */
#define iButton1_SLW                    (* (reg8 *) iButton1__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define iButton1_PRTDSI__CAPS_SEL       (* (reg8 *) iButton1__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define iButton1_PRTDSI__DBL_SYNC_IN    (* (reg8 *) iButton1__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define iButton1_PRTDSI__OE_SEL0        (* (reg8 *) iButton1__PRTDSI__OE_SEL0) 
#define iButton1_PRTDSI__OE_SEL1        (* (reg8 *) iButton1__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define iButton1_PRTDSI__OUT_SEL0       (* (reg8 *) iButton1__PRTDSI__OUT_SEL0) 
#define iButton1_PRTDSI__OUT_SEL1       (* (reg8 *) iButton1__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define iButton1_PRTDSI__SYNC_OUT       (* (reg8 *) iButton1__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(iButton1__SIO_CFG)
    #define iButton1_SIO_HYST_EN        (* (reg8 *) iButton1__SIO_HYST_EN)
    #define iButton1_SIO_REG_HIFREQ     (* (reg8 *) iButton1__SIO_REG_HIFREQ)
    #define iButton1_SIO_CFG            (* (reg8 *) iButton1__SIO_CFG)
    #define iButton1_SIO_DIFF           (* (reg8 *) iButton1__SIO_DIFF)
#endif /* (iButton1__SIO_CFG) */

/* Interrupt Registers */
#if defined(iButton1__INTSTAT)
    #define iButton1_INTSTAT            (* (reg8 *) iButton1__INTSTAT)
    #define iButton1_SNAP               (* (reg8 *) iButton1__SNAP)
    
	#define iButton1_0_INTTYPE_REG 		(* (reg8 *) iButton1__0__INTTYPE)
#endif /* (iButton1__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_iButton1_H */


/* [] END OF FILE */
