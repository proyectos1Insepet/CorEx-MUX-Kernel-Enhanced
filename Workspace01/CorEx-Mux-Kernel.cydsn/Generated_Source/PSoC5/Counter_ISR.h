/*******************************************************************************
* File Name: Counter_ISR.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_Counter_ISR_H)
#define CY_ISR_Counter_ISR_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void Counter_ISR_Start(void);
void Counter_ISR_StartEx(cyisraddress address);
void Counter_ISR_Stop(void);

CY_ISR_PROTO(Counter_ISR_Interrupt);

void Counter_ISR_SetVector(cyisraddress address);
cyisraddress Counter_ISR_GetVector(void);

void Counter_ISR_SetPriority(uint8 priority);
uint8 Counter_ISR_GetPriority(void);

void Counter_ISR_Enable(void);
uint8 Counter_ISR_GetState(void);
void Counter_ISR_Disable(void);

void Counter_ISR_SetPending(void);
void Counter_ISR_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the Counter_ISR ISR. */
#define Counter_ISR_INTC_VECTOR            ((reg32 *) Counter_ISR__INTC_VECT)

/* Address of the Counter_ISR ISR priority. */
#define Counter_ISR_INTC_PRIOR             ((reg8 *) Counter_ISR__INTC_PRIOR_REG)

/* Priority of the Counter_ISR interrupt. */
#define Counter_ISR_INTC_PRIOR_NUMBER      Counter_ISR__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable Counter_ISR interrupt. */
#define Counter_ISR_INTC_SET_EN            ((reg32 *) Counter_ISR__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the Counter_ISR interrupt. */
#define Counter_ISR_INTC_CLR_EN            ((reg32 *) Counter_ISR__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the Counter_ISR interrupt state to pending. */
#define Counter_ISR_INTC_SET_PD            ((reg32 *) Counter_ISR__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the Counter_ISR interrupt. */
#define Counter_ISR_INTC_CLR_PD            ((reg32 *) Counter_ISR__INTC_CLR_PD_REG)


#endif /* CY_ISR_Counter_ISR_H */


/* [] END OF FILE */
