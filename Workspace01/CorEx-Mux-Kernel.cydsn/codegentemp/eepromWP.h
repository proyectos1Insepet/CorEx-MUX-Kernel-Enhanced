/*******************************************************************************
* File Name: eepromWP.h  
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

#if !defined(CY_PINS_eepromWP_H) /* Pins eepromWP_H */
#define CY_PINS_eepromWP_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "eepromWP_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 eepromWP__PORT == 15 && ((eepromWP__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    eepromWP_Write(uint8 value);
void    eepromWP_SetDriveMode(uint8 mode);
uint8   eepromWP_ReadDataReg(void);
uint8   eepromWP_Read(void);
void    eepromWP_SetInterruptMode(uint16 position, uint16 mode);
uint8   eepromWP_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the eepromWP_SetDriveMode() function.
     *  @{
     */
        #define eepromWP_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define eepromWP_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define eepromWP_DM_RES_UP          PIN_DM_RES_UP
        #define eepromWP_DM_RES_DWN         PIN_DM_RES_DWN
        #define eepromWP_DM_OD_LO           PIN_DM_OD_LO
        #define eepromWP_DM_OD_HI           PIN_DM_OD_HI
        #define eepromWP_DM_STRONG          PIN_DM_STRONG
        #define eepromWP_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define eepromWP_MASK               eepromWP__MASK
#define eepromWP_SHIFT              eepromWP__SHIFT
#define eepromWP_WIDTH              1u

/* Interrupt constants */
#if defined(eepromWP__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in eepromWP_SetInterruptMode() function.
     *  @{
     */
        #define eepromWP_INTR_NONE      (uint16)(0x0000u)
        #define eepromWP_INTR_RISING    (uint16)(0x0001u)
        #define eepromWP_INTR_FALLING   (uint16)(0x0002u)
        #define eepromWP_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define eepromWP_INTR_MASK      (0x01u) 
#endif /* (eepromWP__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define eepromWP_PS                     (* (reg8 *) eepromWP__PS)
/* Data Register */
#define eepromWP_DR                     (* (reg8 *) eepromWP__DR)
/* Port Number */
#define eepromWP_PRT_NUM                (* (reg8 *) eepromWP__PRT) 
/* Connect to Analog Globals */                                                  
#define eepromWP_AG                     (* (reg8 *) eepromWP__AG)                       
/* Analog MUX bux enable */
#define eepromWP_AMUX                   (* (reg8 *) eepromWP__AMUX) 
/* Bidirectional Enable */                                                        
#define eepromWP_BIE                    (* (reg8 *) eepromWP__BIE)
/* Bit-mask for Aliased Register Access */
#define eepromWP_BIT_MASK               (* (reg8 *) eepromWP__BIT_MASK)
/* Bypass Enable */
#define eepromWP_BYP                    (* (reg8 *) eepromWP__BYP)
/* Port wide control signals */                                                   
#define eepromWP_CTL                    (* (reg8 *) eepromWP__CTL)
/* Drive Modes */
#define eepromWP_DM0                    (* (reg8 *) eepromWP__DM0) 
#define eepromWP_DM1                    (* (reg8 *) eepromWP__DM1)
#define eepromWP_DM2                    (* (reg8 *) eepromWP__DM2) 
/* Input Buffer Disable Override */
#define eepromWP_INP_DIS                (* (reg8 *) eepromWP__INP_DIS)
/* LCD Common or Segment Drive */
#define eepromWP_LCD_COM_SEG            (* (reg8 *) eepromWP__LCD_COM_SEG)
/* Enable Segment LCD */
#define eepromWP_LCD_EN                 (* (reg8 *) eepromWP__LCD_EN)
/* Slew Rate Control */
#define eepromWP_SLW                    (* (reg8 *) eepromWP__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define eepromWP_PRTDSI__CAPS_SEL       (* (reg8 *) eepromWP__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define eepromWP_PRTDSI__DBL_SYNC_IN    (* (reg8 *) eepromWP__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define eepromWP_PRTDSI__OE_SEL0        (* (reg8 *) eepromWP__PRTDSI__OE_SEL0) 
#define eepromWP_PRTDSI__OE_SEL1        (* (reg8 *) eepromWP__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define eepromWP_PRTDSI__OUT_SEL0       (* (reg8 *) eepromWP__PRTDSI__OUT_SEL0) 
#define eepromWP_PRTDSI__OUT_SEL1       (* (reg8 *) eepromWP__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define eepromWP_PRTDSI__SYNC_OUT       (* (reg8 *) eepromWP__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(eepromWP__SIO_CFG)
    #define eepromWP_SIO_HYST_EN        (* (reg8 *) eepromWP__SIO_HYST_EN)
    #define eepromWP_SIO_REG_HIFREQ     (* (reg8 *) eepromWP__SIO_REG_HIFREQ)
    #define eepromWP_SIO_CFG            (* (reg8 *) eepromWP__SIO_CFG)
    #define eepromWP_SIO_DIFF           (* (reg8 *) eepromWP__SIO_DIFF)
#endif /* (eepromWP__SIO_CFG) */

/* Interrupt Registers */
#if defined(eepromWP__INTSTAT)
    #define eepromWP_INTSTAT            (* (reg8 *) eepromWP__INTSTAT)
    #define eepromWP_SNAP               (* (reg8 *) eepromWP__SNAP)
    
	#define eepromWP_0_INTTYPE_REG 		(* (reg8 *) eepromWP__0__INTTYPE)
#endif /* (eepromWP__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_eepromWP_H */


/* [] END OF FILE */
