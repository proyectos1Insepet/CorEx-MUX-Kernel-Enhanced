/*******************************************************************************
* File Name: iButton2.h  
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

#if !defined(CY_PINS_iButton2_H) /* Pins iButton2_H */
#define CY_PINS_iButton2_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "iButton2_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 iButton2__PORT == 15 && ((iButton2__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    iButton2_Write(uint8 value);
void    iButton2_SetDriveMode(uint8 mode);
uint8   iButton2_ReadDataReg(void);
uint8   iButton2_Read(void);
void    iButton2_SetInterruptMode(uint16 position, uint16 mode);
uint8   iButton2_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the iButton2_SetDriveMode() function.
     *  @{
     */
        #define iButton2_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define iButton2_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define iButton2_DM_RES_UP          PIN_DM_RES_UP
        #define iButton2_DM_RES_DWN         PIN_DM_RES_DWN
        #define iButton2_DM_OD_LO           PIN_DM_OD_LO
        #define iButton2_DM_OD_HI           PIN_DM_OD_HI
        #define iButton2_DM_STRONG          PIN_DM_STRONG
        #define iButton2_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define iButton2_MASK               iButton2__MASK
#define iButton2_SHIFT              iButton2__SHIFT
#define iButton2_WIDTH              1u

/* Interrupt constants */
#if defined(iButton2__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in iButton2_SetInterruptMode() function.
     *  @{
     */
        #define iButton2_INTR_NONE      (uint16)(0x0000u)
        #define iButton2_INTR_RISING    (uint16)(0x0001u)
        #define iButton2_INTR_FALLING   (uint16)(0x0002u)
        #define iButton2_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define iButton2_INTR_MASK      (0x01u) 
#endif /* (iButton2__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define iButton2_PS                     (* (reg8 *) iButton2__PS)
/* Data Register */
#define iButton2_DR                     (* (reg8 *) iButton2__DR)
/* Port Number */
#define iButton2_PRT_NUM                (* (reg8 *) iButton2__PRT) 
/* Connect to Analog Globals */                                                  
#define iButton2_AG                     (* (reg8 *) iButton2__AG)                       
/* Analog MUX bux enable */
#define iButton2_AMUX                   (* (reg8 *) iButton2__AMUX) 
/* Bidirectional Enable */                                                        
#define iButton2_BIE                    (* (reg8 *) iButton2__BIE)
/* Bit-mask for Aliased Register Access */
#define iButton2_BIT_MASK               (* (reg8 *) iButton2__BIT_MASK)
/* Bypass Enable */
#define iButton2_BYP                    (* (reg8 *) iButton2__BYP)
/* Port wide control signals */                                                   
#define iButton2_CTL                    (* (reg8 *) iButton2__CTL)
/* Drive Modes */
#define iButton2_DM0                    (* (reg8 *) iButton2__DM0) 
#define iButton2_DM1                    (* (reg8 *) iButton2__DM1)
#define iButton2_DM2                    (* (reg8 *) iButton2__DM2) 
/* Input Buffer Disable Override */
#define iButton2_INP_DIS                (* (reg8 *) iButton2__INP_DIS)
/* LCD Common or Segment Drive */
#define iButton2_LCD_COM_SEG            (* (reg8 *) iButton2__LCD_COM_SEG)
/* Enable Segment LCD */
#define iButton2_LCD_EN                 (* (reg8 *) iButton2__LCD_EN)
/* Slew Rate Control */
#define iButton2_SLW                    (* (reg8 *) iButton2__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define iButton2_PRTDSI__CAPS_SEL       (* (reg8 *) iButton2__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define iButton2_PRTDSI__DBL_SYNC_IN    (* (reg8 *) iButton2__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define iButton2_PRTDSI__OE_SEL0        (* (reg8 *) iButton2__PRTDSI__OE_SEL0) 
#define iButton2_PRTDSI__OE_SEL1        (* (reg8 *) iButton2__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define iButton2_PRTDSI__OUT_SEL0       (* (reg8 *) iButton2__PRTDSI__OUT_SEL0) 
#define iButton2_PRTDSI__OUT_SEL1       (* (reg8 *) iButton2__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define iButton2_PRTDSI__SYNC_OUT       (* (reg8 *) iButton2__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(iButton2__SIO_CFG)
    #define iButton2_SIO_HYST_EN        (* (reg8 *) iButton2__SIO_HYST_EN)
    #define iButton2_SIO_REG_HIFREQ     (* (reg8 *) iButton2__SIO_REG_HIFREQ)
    #define iButton2_SIO_CFG            (* (reg8 *) iButton2__SIO_CFG)
    #define iButton2_SIO_DIFF           (* (reg8 *) iButton2__SIO_DIFF)
#endif /* (iButton2__SIO_CFG) */

/* Interrupt Registers */
#if defined(iButton2__INTSTAT)
    #define iButton2_INTSTAT            (* (reg8 *) iButton2__INTSTAT)
    #define iButton2_SNAP               (* (reg8 *) iButton2__SNAP)
    
	#define iButton2_0_INTTYPE_REG 		(* (reg8 *) iButton2__0__INTTYPE)
#endif /* (iButton2__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_iButton2_H */


/* [] END OF FILE */
