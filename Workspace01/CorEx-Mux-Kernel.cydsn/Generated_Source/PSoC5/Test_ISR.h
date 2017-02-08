/*******************************************************************************
* File Name: Test_ISR.h
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
#if !defined(CY_ISR_Test_ISR_H)
#define CY_ISR_Test_ISR_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void Test_ISR_Start(void);
void Test_ISR_StartEx(cyisraddress address);
void Test_ISR_Stop(void);

CY_ISR_PROTO(Test_ISR_Interrupt);

void Test_ISR_SetVector(cyisraddress address);
cyisraddress Test_ISR_GetVector(void);

void Test_ISR_SetPriority(uint8 priority);
uint8 Test_ISR_GetPriority(void);

void Test_ISR_Enable(void);
uint8 Test_ISR_GetState(void);
void Test_ISR_Disable(void);

void Test_ISR_SetPending(void);
void Test_ISR_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the Test_ISR ISR. */
#define Test_ISR_INTC_VECTOR            ((reg32 *) Test_ISR__INTC_VECT)

/* Address of the Test_ISR ISR priority. */
#define Test_ISR_INTC_PRIOR             ((reg8 *) Test_ISR__INTC_PRIOR_REG)

/* Priority of the Test_ISR interrupt. */
#define Test_ISR_INTC_PRIOR_NUMBER      Test_ISR__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable Test_ISR interrupt. */
#define Test_ISR_INTC_SET_EN            ((reg32 *) Test_ISR__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the Test_ISR interrupt. */
#define Test_ISR_INTC_CLR_EN            ((reg32 *) Test_ISR__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the Test_ISR interrupt state to pending. */
#define Test_ISR_INTC_SET_PD            ((reg32 *) Test_ISR__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the Test_ISR interrupt. */
#define Test_ISR_INTC_CLR_PD            ((reg32 *) Test_ISR__INTC_CLR_PD_REG)


#endif /* CY_ISR_Test_ISR_H */


/* [] END OF FILE */
