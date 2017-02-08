/*******************************************************************************
* File Name: CounterClock.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_CounterClock_H)
#define CY_CLOCK_CounterClock_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component cy_clock_v2_20 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void CounterClock_Start(void) ;
void CounterClock_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void CounterClock_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void CounterClock_StandbyPower(uint8 state) ;
void CounterClock_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 CounterClock_GetDividerRegister(void) ;
void CounterClock_SetModeRegister(uint8 modeBitMask) ;
void CounterClock_ClearModeRegister(uint8 modeBitMask) ;
uint8 CounterClock_GetModeRegister(void) ;
void CounterClock_SetSourceRegister(uint8 clkSource) ;
uint8 CounterClock_GetSourceRegister(void) ;
#if defined(CounterClock__CFG3)
void CounterClock_SetPhaseRegister(uint8 clkPhase) ;
uint8 CounterClock_GetPhaseRegister(void) ;
#endif /* defined(CounterClock__CFG3) */

#define CounterClock_Enable()                       CounterClock_Start()
#define CounterClock_Disable()                      CounterClock_Stop()
#define CounterClock_SetDivider(clkDivider)         CounterClock_SetDividerRegister(clkDivider, 1u)
#define CounterClock_SetDividerValue(clkDivider)    CounterClock_SetDividerRegister((clkDivider) - 1u, 1u)
#define CounterClock_SetMode(clkMode)               CounterClock_SetModeRegister(clkMode)
#define CounterClock_SetSource(clkSource)           CounterClock_SetSourceRegister(clkSource)
#if defined(CounterClock__CFG3)
#define CounterClock_SetPhase(clkPhase)             CounterClock_SetPhaseRegister(clkPhase)
#define CounterClock_SetPhaseValue(clkPhase)        CounterClock_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(CounterClock__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define CounterClock_CLKEN              (* (reg8 *) CounterClock__PM_ACT_CFG)
#define CounterClock_CLKEN_PTR          ((reg8 *) CounterClock__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define CounterClock_CLKSTBY            (* (reg8 *) CounterClock__PM_STBY_CFG)
#define CounterClock_CLKSTBY_PTR        ((reg8 *) CounterClock__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define CounterClock_DIV_LSB            (* (reg8 *) CounterClock__CFG0)
#define CounterClock_DIV_LSB_PTR        ((reg8 *) CounterClock__CFG0)
#define CounterClock_DIV_PTR            ((reg16 *) CounterClock__CFG0)

/* Clock MSB divider configuration register. */
#define CounterClock_DIV_MSB            (* (reg8 *) CounterClock__CFG1)
#define CounterClock_DIV_MSB_PTR        ((reg8 *) CounterClock__CFG1)

/* Mode and source configuration register */
#define CounterClock_MOD_SRC            (* (reg8 *) CounterClock__CFG2)
#define CounterClock_MOD_SRC_PTR        ((reg8 *) CounterClock__CFG2)

#if defined(CounterClock__CFG3)
/* Analog clock phase configuration register */
#define CounterClock_PHASE              (* (reg8 *) CounterClock__CFG3)
#define CounterClock_PHASE_PTR          ((reg8 *) CounterClock__CFG3)
#endif /* defined(CounterClock__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define CounterClock_CLKEN_MASK         CounterClock__PM_ACT_MSK
#define CounterClock_CLKSTBY_MASK       CounterClock__PM_STBY_MSK

/* CFG2 field masks */
#define CounterClock_SRC_SEL_MSK        CounterClock__CFG2_SRC_SEL_MASK
#define CounterClock_MODE_MASK          (~(CounterClock_SRC_SEL_MSK))

#if defined(CounterClock__CFG3)
/* CFG3 phase mask */
#define CounterClock_PHASE_MASK         CounterClock__CFG3_PHASE_DLY_MASK
#endif /* defined(CounterClock__CFG3) */

#endif /* CY_CLOCK_CounterClock_H */


/* [] END OF FILE */
