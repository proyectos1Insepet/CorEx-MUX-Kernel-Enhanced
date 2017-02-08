/*******************************************************************************
* File Name: DataLinkTimeout.h
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
#if !defined(CY_ISR_DataLinkTimeout_H)
#define CY_ISR_DataLinkTimeout_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void DataLinkTimeout_Start(void);
void DataLinkTimeout_StartEx(cyisraddress address);
void DataLinkTimeout_Stop(void);

CY_ISR_PROTO(DataLinkTimeout_Interrupt);

void DataLinkTimeout_SetVector(cyisraddress address);
cyisraddress DataLinkTimeout_GetVector(void);

void DataLinkTimeout_SetPriority(uint8 priority);
uint8 DataLinkTimeout_GetPriority(void);

void DataLinkTimeout_Enable(void);
uint8 DataLinkTimeout_GetState(void);
void DataLinkTimeout_Disable(void);

void DataLinkTimeout_SetPending(void);
void DataLinkTimeout_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the DataLinkTimeout ISR. */
#define DataLinkTimeout_INTC_VECTOR            ((reg32 *) DataLinkTimeout__INTC_VECT)

/* Address of the DataLinkTimeout ISR priority. */
#define DataLinkTimeout_INTC_PRIOR             ((reg8 *) DataLinkTimeout__INTC_PRIOR_REG)

/* Priority of the DataLinkTimeout interrupt. */
#define DataLinkTimeout_INTC_PRIOR_NUMBER      DataLinkTimeout__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable DataLinkTimeout interrupt. */
#define DataLinkTimeout_INTC_SET_EN            ((reg32 *) DataLinkTimeout__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the DataLinkTimeout interrupt. */
#define DataLinkTimeout_INTC_CLR_EN            ((reg32 *) DataLinkTimeout__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the DataLinkTimeout interrupt state to pending. */
#define DataLinkTimeout_INTC_SET_PD            ((reg32 *) DataLinkTimeout__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the DataLinkTimeout interrupt. */
#define DataLinkTimeout_INTC_CLR_PD            ((reg32 *) DataLinkTimeout__INTC_CLR_PD_REG)


#endif /* CY_ISR_DataLinkTimeout_H */


/* [] END OF FILE */
