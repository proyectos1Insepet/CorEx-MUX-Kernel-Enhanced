/*******************************************************************************
* File Name: Pin_ResetShiftCRC.h  
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

#if !defined(CY_PINS_Pin_ResetShiftCRC_H) /* Pins Pin_ResetShiftCRC_H */
#define CY_PINS_Pin_ResetShiftCRC_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Pin_ResetShiftCRC_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Pin_ResetShiftCRC__PORT == 15 && ((Pin_ResetShiftCRC__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Pin_ResetShiftCRC_Write(uint8 value);
void    Pin_ResetShiftCRC_SetDriveMode(uint8 mode);
uint8   Pin_ResetShiftCRC_ReadDataReg(void);
uint8   Pin_ResetShiftCRC_Read(void);
void    Pin_ResetShiftCRC_SetInterruptMode(uint16 position, uint16 mode);
uint8   Pin_ResetShiftCRC_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Pin_ResetShiftCRC_SetDriveMode() function.
     *  @{
     */
        #define Pin_ResetShiftCRC_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Pin_ResetShiftCRC_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Pin_ResetShiftCRC_DM_RES_UP          PIN_DM_RES_UP
        #define Pin_ResetShiftCRC_DM_RES_DWN         PIN_DM_RES_DWN
        #define Pin_ResetShiftCRC_DM_OD_LO           PIN_DM_OD_LO
        #define Pin_ResetShiftCRC_DM_OD_HI           PIN_DM_OD_HI
        #define Pin_ResetShiftCRC_DM_STRONG          PIN_DM_STRONG
        #define Pin_ResetShiftCRC_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Pin_ResetShiftCRC_MASK               Pin_ResetShiftCRC__MASK
#define Pin_ResetShiftCRC_SHIFT              Pin_ResetShiftCRC__SHIFT
#define Pin_ResetShiftCRC_WIDTH              1u

/* Interrupt constants */
#if defined(Pin_ResetShiftCRC__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Pin_ResetShiftCRC_SetInterruptMode() function.
     *  @{
     */
        #define Pin_ResetShiftCRC_INTR_NONE      (uint16)(0x0000u)
        #define Pin_ResetShiftCRC_INTR_RISING    (uint16)(0x0001u)
        #define Pin_ResetShiftCRC_INTR_FALLING   (uint16)(0x0002u)
        #define Pin_ResetShiftCRC_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Pin_ResetShiftCRC_INTR_MASK      (0x01u) 
#endif /* (Pin_ResetShiftCRC__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Pin_ResetShiftCRC_PS                     (* (reg8 *) Pin_ResetShiftCRC__PS)
/* Data Register */
#define Pin_ResetShiftCRC_DR                     (* (reg8 *) Pin_ResetShiftCRC__DR)
/* Port Number */
#define Pin_ResetShiftCRC_PRT_NUM                (* (reg8 *) Pin_ResetShiftCRC__PRT) 
/* Connect to Analog Globals */                                                  
#define Pin_ResetShiftCRC_AG                     (* (reg8 *) Pin_ResetShiftCRC__AG)                       
/* Analog MUX bux enable */
#define Pin_ResetShiftCRC_AMUX                   (* (reg8 *) Pin_ResetShiftCRC__AMUX) 
/* Bidirectional Enable */                                                        
#define Pin_ResetShiftCRC_BIE                    (* (reg8 *) Pin_ResetShiftCRC__BIE)
/* Bit-mask for Aliased Register Access */
#define Pin_ResetShiftCRC_BIT_MASK               (* (reg8 *) Pin_ResetShiftCRC__BIT_MASK)
/* Bypass Enable */
#define Pin_ResetShiftCRC_BYP                    (* (reg8 *) Pin_ResetShiftCRC__BYP)
/* Port wide control signals */                                                   
#define Pin_ResetShiftCRC_CTL                    (* (reg8 *) Pin_ResetShiftCRC__CTL)
/* Drive Modes */
#define Pin_ResetShiftCRC_DM0                    (* (reg8 *) Pin_ResetShiftCRC__DM0) 
#define Pin_ResetShiftCRC_DM1                    (* (reg8 *) Pin_ResetShiftCRC__DM1)
#define Pin_ResetShiftCRC_DM2                    (* (reg8 *) Pin_ResetShiftCRC__DM2) 
/* Input Buffer Disable Override */
#define Pin_ResetShiftCRC_INP_DIS                (* (reg8 *) Pin_ResetShiftCRC__INP_DIS)
/* LCD Common or Segment Drive */
#define Pin_ResetShiftCRC_LCD_COM_SEG            (* (reg8 *) Pin_ResetShiftCRC__LCD_COM_SEG)
/* Enable Segment LCD */
#define Pin_ResetShiftCRC_LCD_EN                 (* (reg8 *) Pin_ResetShiftCRC__LCD_EN)
/* Slew Rate Control */
#define Pin_ResetShiftCRC_SLW                    (* (reg8 *) Pin_ResetShiftCRC__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Pin_ResetShiftCRC_PRTDSI__CAPS_SEL       (* (reg8 *) Pin_ResetShiftCRC__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Pin_ResetShiftCRC_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Pin_ResetShiftCRC__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Pin_ResetShiftCRC_PRTDSI__OE_SEL0        (* (reg8 *) Pin_ResetShiftCRC__PRTDSI__OE_SEL0) 
#define Pin_ResetShiftCRC_PRTDSI__OE_SEL1        (* (reg8 *) Pin_ResetShiftCRC__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Pin_ResetShiftCRC_PRTDSI__OUT_SEL0       (* (reg8 *) Pin_ResetShiftCRC__PRTDSI__OUT_SEL0) 
#define Pin_ResetShiftCRC_PRTDSI__OUT_SEL1       (* (reg8 *) Pin_ResetShiftCRC__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Pin_ResetShiftCRC_PRTDSI__SYNC_OUT       (* (reg8 *) Pin_ResetShiftCRC__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Pin_ResetShiftCRC__SIO_CFG)
    #define Pin_ResetShiftCRC_SIO_HYST_EN        (* (reg8 *) Pin_ResetShiftCRC__SIO_HYST_EN)
    #define Pin_ResetShiftCRC_SIO_REG_HIFREQ     (* (reg8 *) Pin_ResetShiftCRC__SIO_REG_HIFREQ)
    #define Pin_ResetShiftCRC_SIO_CFG            (* (reg8 *) Pin_ResetShiftCRC__SIO_CFG)
    #define Pin_ResetShiftCRC_SIO_DIFF           (* (reg8 *) Pin_ResetShiftCRC__SIO_DIFF)
#endif /* (Pin_ResetShiftCRC__SIO_CFG) */

/* Interrupt Registers */
#if defined(Pin_ResetShiftCRC__INTSTAT)
    #define Pin_ResetShiftCRC_INTSTAT            (* (reg8 *) Pin_ResetShiftCRC__INTSTAT)
    #define Pin_ResetShiftCRC_SNAP               (* (reg8 *) Pin_ResetShiftCRC__SNAP)
    
	#define Pin_ResetShiftCRC_0_INTTYPE_REG 		(* (reg8 *) Pin_ResetShiftCRC__0__INTTYPE)
#endif /* (Pin_ResetShiftCRC__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Pin_ResetShiftCRC_H */


/* [] END OF FILE */
