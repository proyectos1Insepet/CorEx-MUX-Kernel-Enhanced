/*******************************************************************************
* File Name: CRC_Security.h
* Version 2.40
*
* Description:
*  This file provides constants and parameter values for the CRC component.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CRC_CRC_Security_H)
#define CY_CRC_CRC_Security_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"


/***************************************
*   Conditional Compilation Parameters
****************************************/

/* Check to see if required defines such as CY_PSOC5A are available */
/* They are defined starting with cy_boot v3.0 */

#if !defined (CY_PSOC5A)
    #error Component CRC_v2_40 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5A) */

#define CRC_Security_CRC_SIZE                   (8u)
#define CRC_Security_TIME_MULTIPLEXING_ENABLE   (0u)


/***************************************
*       Type defines
***************************************/

/* Structure to save registers before go to sleep */
typedef struct
{
    uint8 enableState;

    /* Save D0 and D1 registers are none-retention for Panther ES1 */
    #if (CY_UDB_V0)

        #if (CRC_Security_CRC_SIZE <= 32u)
            uint8 polynomial;

        #else
            uint32 polynomialUpper;
            uint32 polynomialLower;

        #endif  /* End (CRC_Security_CRC_SIZE <= 32u) */

    #endif  /* End (CY_UDB_V0) */

} CRC_Security_BACKUP_STRUCT;

extern uint8 CRC_Security_initVar;

extern CRC_Security_BACKUP_STRUCT CRC_Security_backup;


/***************************************
*        Function Prototypes
****************************************/

void CRC_Security_Init(void) ;
void CRC_Security_Enable(void) ;
void CRC_Security_Start(void) ;
void CRC_Security_Stop(void) ;
void CRC_Security_SaveConfig(void) ;
void CRC_Security_Sleep(void) ;
void CRC_Security_RestoreConfig(void) ;
void CRC_Security_Wakeup(void) ;

#if (CRC_Security_CRC_SIZE <= 32u)    /* 8-32 bits CRC */
    uint8 CRC_Security_ReadCRC(void) ;
    void CRC_Security_WriteSeed(uint8 seed)  ;
    uint8 CRC_Security_ReadPolynomial(void)
                                               ;
    void CRC_Security_WritePolynomial(uint8 polynomial)
                                          ;

#else                                    /* 33-64 bits CRC */
    uint32 CRC_Security_ReadCRCUpper(void) ;
    uint32 CRC_Security_ReadCRCLower(void) ;
    void CRC_Security_WriteSeedUpper(uint32 seed) ;
    void CRC_Security_WriteSeedLower(uint32 seed) ;
    uint32 CRC_Security_ReadPolynomialUpper(void) ;
    uint32 CRC_Security_ReadPolynomialLower(void) ;
    void CRC_Security_WritePolynomialUpper(uint32 polynomial)
                                                ;
    void CRC_Security_WritePolynomialLower(uint32 polynomial)
                                                ;

#endif  /* End (CRC_Security_CRC_SIZE <= 32u) */

#if (CRC_Security_CRC_SIZE <= 32u) /* 8-32 bits CRC */
    void CRC_Security_ResetSeedInit(uint8 seed)
                                        ;
#else
    void CRC_Security_ResetSeedInitUpper(uint32 seed)
                                        ;
    void CRC_Security_ResetSeedInitLower(uint32 seed)
                                        ;
#endif  /* End (CRC_Security_CRC_SIZE <= 32u) */


/***************************************
*    Initial Parameter Constants
***************************************/

#if (CRC_Security_CRC_SIZE <= 32u)
    #define CRC_Security_DEFAULT_POLYNOM            (0xC6u)
    #define CRC_Security_DEFAULT_SEED               (0xABu)

#else
    #define CRC_Security_DEFAULT_POLYNOM_UPPER      (0x0u)
    #define CRC_Security_DEFAULT_POLYNOM_LOWER      (0xC6u)
    #define CRC_Security_DEFAULT_SEED_UPPER         (0x0u)
    #define CRC_Security_DEFAULT_SEED_LOWER         (0xABu)

#endif  /* End (CRC_Security_CRC_SIZE <= 32u) */


/***************************************
*           API Constants
***************************************/

#define CRC_Security_MASK                           (0xFFu)


/***************************************
*             Registers
***************************************/

#define CRC_Security_EXECUTE_DFF_RESET    { CRC_Security_CONTROL_REG |= CRC_Security_CTRL_RESET; \
    CRC_Security_CONTROL_REG &= (uint8)~CRC_Security_CTRL_RESET; }


#if (CRC_Security_TIME_MULTIPLEXING_ENABLE)
    #if (CRC_Security_CRC_SIZE <= 16u)      /* 16 bits CRC */
        /* Polynomial */
        #define CRC_Security_POLYNOM_A__D1_REG          (*(reg8 *) CRC_Security_b0_CRCdp_a__D1_REG )
        #define CRC_Security_POLYNOM_A__D1_PTR          ( (reg8 *) CRC_Security_b0_CRCdp_a__D1_REG )
        #define CRC_Security_POLYNOM_A__D0_REG          (*(reg8 *) CRC_Security_b0_CRCdp_a__D0_REG )
        #define CRC_Security_POLYNOM_A__D0_PTR          ( (reg8 *) CRC_Security_b0_CRCdp_a__D0_REG )
        /* Seed */
        #define CRC_Security_SEED_A__A1_REG             (*(reg8 *) CRC_Security_b0_CRCdp_a__A1_REG )
        #define CRC_Security_SEED_A__A1_PTR             ( (reg8 *) CRC_Security_b0_CRCdp_a__A1_REG )
        #define CRC_Security_SEED_A__A0_REG             (*(reg8 *) CRC_Security_b0_CRCdp_a__A0_REG )
        #define CRC_Security_SEED_A__A0_PTR             ( (reg8 *) CRC_Security_b0_CRCdp_a__A0_REG )
        /* Seed COPY */
        #define CRC_Security_SEED_COPY_A__A1_REG        (*(reg8 *) CRC_Security_b0_CRCdp_a__F1_REG )
        #define CRC_Security_SEED_COPY_A__A1_PTR        ( (reg8 *) CRC_Security_b0_CRCdp_a__F1_REG )
        #define CRC_Security_SEED_COPY_A__A0_REG        (*(reg8 *) CRC_Security_b0_CRCdp_a__F0_REG )
        #define CRC_Security_SEED_COPY_A__A0_PTR        ( (reg8 *) CRC_Security_b0_CRCdp_a__F0_REG )

    #elif (CRC_Security_CRC_SIZE <= 24u)      /* 24 bits CRC */
        /* Polynomial */
        #define CRC_Security_POLYNOM_B__D1_REG          (*(reg8 *) CRC_Security_b1_CRCdp_b__D1_REG )
        #define CRC_Security_POLYNOM_B__D1_PTR          ( (reg8 *) CRC_Security_b1_CRCdp_b__D1_REG )
        #define CRC_Security_POLYNOM_B__D0_REG          (*(reg8 *) CRC_Security_b1_CRCdp_b__D0_REG )
        #define CRC_Security_POLYNOM_B__D0_PTR          ( (reg8 *) CRC_Security_b1_CRCdp_b__D0_REG )
        #define CRC_Security_POLYNOM_A__D0_REG          (*(reg8 *) CRC_Security_b0_CRCdp_a__D0_REG )
        #define CRC_Security_POLYNOM_A__D0_PTR          ( (reg8 *) CRC_Security_b0_CRCdp_a__D0_REG )
        /* Seed */
        #define CRC_Security_SEED_B__A1_REG             (*(reg8 *) CRC_Security_b1_CRCdp_b__A1_REG )
        #define CRC_Security_SEED_B__A1_PTR             ( (reg8 *) CRC_Security_b1_CRCdp_b__A1_REG )
        #define CRC_Security_SEED_B__A0_REG             (*(reg8 *) CRC_Security_b1_CRCdp_b__A0_REG )
        #define CRC_Security_SEED_B__A0_PTR             ( (reg8 *) CRC_Security_b1_CRCdp_b__A0_REG )
        #define CRC_Security_SEED_A__A0_REG             (*(reg8 *) CRC_Security_b0_CRCdp_a__A0_REG )
        #define CRC_Security_SEED_A__A0_PTR             ( (reg8 *) CRC_Security_b0_CRCdp_a__A0_REG )
        /* Seed COPY */
        #define CRC_Security_SEED_COPY_B__A1_REG        (*(reg8 *) CRC_Security_b1_CRCdp_b__F1_REG )
        #define CRC_Security_SEED_COPY_B__A1_PTR        ( (reg8 *) CRC_Security_b1_CRCdp_b__F1_REG )
        #define CRC_Security_SEED_COPY_B__A0_REG        (*(reg8 *) CRC_Security_b1_CRCdp_b__F0_REG )
        #define CRC_Security_SEED_COPY_B__A0_PTR        ( (reg8 *) CRC_Security_b1_CRCdp_b__F0_REG )
        #define CRC_Security_SEED_COPY_A__A0_REG        (*(reg8 *) CRC_Security_b0_CRCdp_a__F0_REG )
        #define CRC_Security_SEED_COPY_A__A0_PTR        ( (reg8 *) CRC_Security_b0_CRCdp_a__F0_REG )

    #elif (CRC_Security_CRC_SIZE <= 32u)      /* 32 bits CRC */
        /* Polynomial */
        #define CRC_Security_POLYNOM_B__D1_REG          (*(reg8 *) CRC_Security_b1_CRCdp_b__D1_REG )
        #define CRC_Security_POLYNOM_B__D1_PTR          ( (reg8 *) CRC_Security_b1_CRCdp_b__D1_REG )
        #define CRC_Security_POLYNOM_A__D1_REG          (*(reg8 *) CRC_Security_b0_CRCdp_a__D1_REG )
        #define CRC_Security_POLYNOM_A__D1_PTR          ( (reg8 *) CRC_Security_b0_CRCdp_a__D1_REG )
        #define CRC_Security_POLYNOM_B__D0_REG          (*(reg8 *) CRC_Security_b1_CRCdp_b__D0_REG )
        #define CRC_Security_POLYNOM_B__D0_PTR          ( (reg8 *) CRC_Security_b1_CRCdp_b__D0_REG )
        #define CRC_Security_POLYNOM_A__D0_REG          (*(reg8 *) CRC_Security_b0_CRCdp_a__D0_REG )
        #define CRC_Security_POLYNOM_A__D0_PTR          ( (reg8 *) CRC_Security_b0_CRCdp_a__D0_REG )
        /* Seed */
        #define CRC_Security_SEED_B__A1_REG             (*(reg8 *) CRC_Security_b1_CRCdp_b__A1_REG )
        #define CRC_Security_SEED_B__A1_PTR             ( (reg8 *) CRC_Security_b1_CRCdp_b__A1_REG )
        #define CRC_Security_SEED_A__A1_REG             (*(reg8 *) CRC_Security_b0_CRCdp_a__A1_REG )
        #define CRC_Security_SEED_A__A1_PTR             ( (reg8 *) CRC_Security_b0_CRCdp_a__A1_REG )
        #define CRC_Security_SEED_B__A0_REG             (*(reg8 *) CRC_Security_b1_CRCdp_b__A0_REG )
        #define CRC_Security_SEED_B__A0_PTR             ( (reg8 *) CRC_Security_b1_CRCdp_b__A0_REG )
        #define CRC_Security_SEED_A__A0_REG             (*(reg8 *) CRC_Security_b0_CRCdp_a__A0_REG )
        #define CRC_Security_SEED_A__A0_PTR             ( (reg8 *) CRC_Security_b0_CRCdp_a__A0_REG )
        /* Seed COPY */
        #define CRC_Security_SEED_COPY_B__A1_REG        (*(reg8 *) CRC_Security_b1_CRCdp_b__F1_REG )
        #define CRC_Security_SEED_COPY_B__A1_PTR        ( (reg8 *) CRC_Security_b1_CRCdp_b__F1_REG )
        #define CRC_Security_SEED_COPY_A__A1_REG        (*(reg8 *) CRC_Security_b0_CRCdp_a__F1_REG )
        #define CRC_Security_SEED_COPY_A__A1_PTR        ( (reg8 *) CRC_Security_b0_CRCdp_a__F1_REG )
        #define CRC_Security_SEED_COPY_B__A0_REG        (*(reg8 *) CRC_Security_b1_CRCdp_b__F0_REG )
        #define CRC_Security_SEED_COPY_B__A0_PTR        ( (reg8 *) CRC_Security_b1_CRCdp_b__F0_REG )
        #define CRC_Security_SEED_COPY_A__A0_REG        (*(reg8 *) CRC_Security_b0_CRCdp_a__F0_REG )
        #define CRC_Security_SEED_COPY_A__A0_PTR        ( (reg8 *) CRC_Security_b0_CRCdp_a__F0_REG )

    #elif (CRC_Security_CRC_SIZE <= 40u)      /* 40 bits CRC */
        /* Polynomial Upper */
        #define CRC_Security_POLYNOM_UPPER_C__D1_REG            (*(reg8 *) CRC_Security_b2_CRCdp_c__D1_REG )
        #define CRC_Security_POLYNOM_UPPER_C__D1_PTR            ( (reg8 *) CRC_Security_b2_CRCdp_c__D1_REG )
        /* Polynomial Lower */
        #define CRC_Security_POLYNOM_LOWER_B__D1_REG            (*(reg8 *) CRC_Security_b1_CRCdp_b__D1_REG )
        #define CRC_Security_POLYNOM_LOWER_B__D1_PTR            ( (reg8 *) CRC_Security_b1_CRCdp_b__D1_REG )
        #define CRC_Security_POLYNOM_LOWER_C__D0_REG            (*(reg8 *) CRC_Security_b2_CRCdp_c__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_C__D0_PTR            ( (reg8 *) CRC_Security_b2_CRCdp_c__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_B__D0_REG            (*(reg8 *) CRC_Security_b1_CRCdp_b__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_B__D0_PTR            ( (reg8 *) CRC_Security_b1_CRCdp_b__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_A__D0_REG            (*(reg8 *) CRC_Security_b0_CRCdp_a__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_A__D0_PTR            ( (reg8 *) CRC_Security_b0_CRCdp_a__D0_REG )
        /* Seed Upper */
        #define CRC_Security_SEED_UPPER_C__A1_REG               (*(reg8 *) CRC_Security_b2_CRCdp_c__A1_REG )
        #define CRC_Security_SEED_UPPER_C__A1_PTR               ( (reg8 *) CRC_Security_b2_CRCdp_c__A1_REG )
        /* Seed Lower */
        #define CRC_Security_SEED_LOWER_B__A1_REG               (*(reg8 *) CRC_Security_b1_CRCdp_b__A1_REG )
        #define CRC_Security_SEED_LOWER_B__A1_PTR               ( (reg8 *) CRC_Security_b1_CRCdp_b__A1_REG )
        #define CRC_Security_SEED_LOWER_C__A0_REG               (*(reg8 *) CRC_Security_b2_CRCdp_c__A0_REG )
        #define CRC_Security_SEED_LOWER_C__A0_PTR               ( (reg8 *) CRC_Security_b2_CRCdp_c__A0_REG )
        #define CRC_Security_SEED_LOWER_B__A0_REG               (*(reg8 *) CRC_Security_b1_CRCdp_b__A0_REG )
        #define CRC_Security_SEED_LOWER_B__A0_PTR               ( (reg8 *) CRC_Security_b1_CRCdp_b__A0_REG )
        #define CRC_Security_SEED_LOWER_A__A0_REG               (*(reg8 *) CRC_Security_b0_CRCdp_a__A0_REG )
        #define CRC_Security_SEED_LOWER_A__A0_PTR               ( (reg8 *) CRC_Security_b0_CRCdp_a__A0_REG )
        /* Seed COPY Upper */
        #define CRC_Security_SEED_UPPER_COPY_C__A1_REG          (*(reg8 *) CRC_Security_b2_CRCdp_c__F1_REG )
        #define CRC_Security_SEED_UPPER_COPY_C__A1_PTR          ( (reg8 *) CRC_Security_b2_CRCdp_c__F1_REG )
        /* Seed COPY Lower */
        #define CRC_Security_SEED_LOWER_COPY_B__A1_REG          (*(reg8 *) CRC_Security_b1_CRCdp_b__F1_REG )
        #define CRC_Security_SEED_LOWER_COPY_B__A1_PTR          ( (reg8 *) CRC_Security_b1_CRCdp_b__F1_REG )
        #define CRC_Security_SEED_LOWER_COPY_C__A0_REG          (*(reg8 *) CRC_Security_b2_CRCdp_c__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_C__A0_PTR          ( (reg8 *) CRC_Security_b2_CRCdp_c__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_B__A0_REG          (*(reg8 *) CRC_Security_b1_CRCdp_b__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_B__A0_PTR          ( (reg8 *) CRC_Security_b1_CRCdp_b__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_A__A0_REG          (*(reg8 *) CRC_Security_b0_CRCdp_a__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_A__A0_PTR          ( (reg8 *) CRC_Security_b0_CRCdp_a__F0_REG )

    #elif (CRC_Security_CRC_SIZE <= 48u)      /* 48 bits CRC */
        /* Polynomial Upper */
        #define CRC_Security_POLYNOM_UPPER_C__D1_REG            (*(reg8 *) CRC_Security_b2_CRCdp_c__D1_REG )
        #define CRC_Security_POLYNOM_UPPER_C__D1_PTR            ( (reg8 *) CRC_Security_b2_CRCdp_c__D1_REG )
        #define CRC_Security_POLYNOM_UPPER_B__D1_REG            (*(reg8 *) CRC_Security_b1_CRCdp_b__D1_REG )
        #define CRC_Security_POLYNOM_UPPER_B__D1_PTR            ( (reg8 *) CRC_Security_b1_CRCdp_b__D1_REG )
        /* Polynomial Lower */
        #define CRC_Security_POLYNOM_LOWER_A__D1_REG            (*(reg8 *) CRC_Security_b0_CRCdp_a__D1_REG )
        #define CRC_Security_POLYNOM_LOWER_A__D1_PTR            ( (reg8 *) CRC_Security_b0_CRCdp_a__D1_REG )
        #define CRC_Security_POLYNOM_LOWER_C__D0_REG            (*(reg8 *) CRC_Security_b2_CRCdp_c__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_C__D0_PTR            ( (reg8 *) CRC_Security_b2_CRCdp_c__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_B__D0_REG            (*(reg8 *) CRC_Security_b1_CRCdp_b__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_B__D0_PTR            ( (reg8 *) CRC_Security_b1_CRCdp_b__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_A__D0_REG            (*(reg8 *) CRC_Security_b0_CRCdp_a__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_A__D0_PTR            ( (reg8 *) CRC_Security_b0_CRCdp_a__D0_REG )
        /* Seed Upper */
        #define CRC_Security_SEED_UPPER_C__A1_REG               (*(reg8 *) CRC_Security_b2_CRCdp_c__A1_REG )
        #define CRC_Security_SEED_UPPER_C__A1_PTR               ( (reg8 *) CRC_Security_b2_CRCdp_c__A1_REG )
        #define CRC_Security_SEED_UPPER_B__A1_REG               (*(reg8 *) CRC_Security_b1_CRCdp_b__A1_REG )
        #define CRC_Security_SEED_UPPER_B__A1_PTR               ( (reg8 *) CRC_Security_b1_CRCdp_b__A1_REG )
        /* Seed Lower */
        #define CRC_Security_SEED_LOWER_A__A1_REG               (*(reg8 *) CRC_Security_b0_CRCdp_a__A1_REG )
        #define CRC_Security_SEED_LOWER_A__A1_PTR               ( (reg8 *) CRC_Security_b0_CRCdp_a__A1_REG )
        #define CRC_Security_SEED_LOWER_C__A0_REG               (*(reg8 *) CRC_Security_b2_CRCdp_c__A0_REG )
        #define CRC_Security_SEED_LOWER_C__A0_PTR               ( (reg8 *) CRC_Security_b2_CRCdp_c__A0_REG )
        #define CRC_Security_SEED_LOWER_B__A0_REG               (*(reg8 *) CRC_Security_b1_CRCdp_b__A0_REG )
        #define CRC_Security_SEED_LOWER_B__A0_PTR               ( (reg8 *) CRC_Security_b1_CRCdp_b__A0_REG )
        #define CRC_Security_SEED_LOWER_A__A0_REG               (*(reg8 *) CRC_Security_b0_CRCdp_a__A0_REG )
        #define CRC_Security_SEED_LOWER_A__A0_PTR               ( (reg8 *) CRC_Security_b0_CRCdp_a__A0_REG )
        /* Seed COPY Upper */
        #define CRC_Security_SEED_UPPER_COPY_C__A1_REG          (*(reg8 *) CRC_Security_b2_CRCdp_c__F1_REG )
        #define CRC_Security_SEED_UPPER_COPY_C__A1_PTR          ( (reg8 *) CRC_Security_b2_CRCdp_c__F1_REG )
        #define CRC_Security_SEED_UPPER_COPY_B__A1_REG          (*(reg8 *) CRC_Security_b1_CRCdp_b__F1_REG )
        #define CRC_Security_SEED_UPPER_COPY_B__A1_PTR          ( (reg8 *) CRC_Security_b1_CRCdp_b__F1_REG )
        /* Seed COPY Lower */
        #define CRC_Security_SEED_LOWER_COPY_A__A1_REG          (*(reg8 *) CRC_Security_b0_CRCdp_a__F1_REG )
        #define CRC_Security_SEED_LOWER_COPY_A__A1_PTR          ( (reg8 *) CRC_Security_b0_CRCdp_a__F1_REG )
        #define CRC_Security_SEED_LOWER_COPY_C__A0_REG          (*(reg8 *) CRC_Security_b2_CRCdp_c__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_C__A0_PTR          ( (reg8 *) CRC_Security_b2_CRCdp_c__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_B__A0_REG          (*(reg8 *) CRC_Security_b1_CRCdp_b__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_B__A0_PTR          ( (reg8 *) CRC_Security_b1_CRCdp_b__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_A__A0_REG          (*(reg8 *) CRC_Security_b0_CRCdp_a__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_A__A0_PTR          ( (reg8 *) CRC_Security_b0_CRCdp_a__F0_REG )

    #elif (CRC_Security_CRC_SIZE <= 56u)      /* 56 bits CRC */
        /* Polynom Upper */
        #define CRC_Security_POLYNOM_UPPER_D__D1_REG            (*(reg8 *) CRC_Security_b3_CRCdp_d__D1_REG )
        #define CRC_Security_POLYNOM_UPPER_D__D1_PTR            ( (reg8 *) CRC_Security_b3_CRCdp_d__D1_REG )
        #define CRC_Security_POLYNOM_UPPER_C__D1_REG            (*(reg8 *) CRC_Security_b2_CRCdp_c__D1_REG )
        #define CRC_Security_POLYNOM_UPPER_C__D1_PTR            ( (reg8 *) CRC_Security_b2_CRCdp_c__D1_REG )
        #define CRC_Security_POLYNOM_UPPER_B__D1_REG            (*(reg8 *) CRC_Security_b1_CRCdp_b__D1_REG )
        #define CRC_Security_POLYNOM_UPPER_B__D1_PTR            ( (reg8 *) CRC_Security_b1_CRCdp_b__D1_REG )
        /* Polynom Lower */
        #define CRC_Security_POLYNOM_LOWER_D__D0_REG            (*(reg8 *) CRC_Security_b3_CRCdp_d__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_D__D0_PTR            ( (reg8 *) CRC_Security_b3_CRCdp_d__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_C__D0_REG            (*(reg8 *) CRC_Security_b2_CRCdp_c__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_C__D0_PTR            ( (reg8 *) CRC_Security_b2_CRCdp_c__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_B__D0_REG            (*(reg8 *) CRC_Security_b1_CRCdp_b__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_B__D0_PTR            ( (reg8 *) CRC_Security_b1_CRCdp_b__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_A__D0_REG            (*(reg8 *) CRC_Security_b0_CRCdp_a__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_A__D0_PTR            ( (reg8 *) CRC_Security_b0_CRCdp_a__D0_REG )
        /* Seed Upper */
        #define CRC_Security_SEED_UPPER_D__A1_REG               (*(reg8 *) CRC_Security_b3_CRCdp_d__A1_REG )
        #define CRC_Security_SEED_UPPER_D__A1_PTR               ( (reg8 *) CRC_Security_b3_CRCdp_d__A1_REG )
        #define CRC_Security_SEED_UPPER_C__A1_REG               (*(reg8 *) CRC_Security_b2_CRCdp_c__A1_REG )
        #define CRC_Security_SEED_UPPER_C__A1_PTR               ( (reg8 *) CRC_Security_b2_CRCdp_c__A1_REG )
        #define CRC_Security_SEED_UPPER_B__A1_REG               (*(reg8 *) CRC_Security_b1_CRCdp_b__A1_REG )
        #define CRC_Security_SEED_UPPER_B__A1_PTR               ( (reg8 *) CRC_Security_b1_CRCdp_b__A1_REG )
        /* Seed Lower */
        #define CRC_Security_SEED_LOWER_D__A0_REG               (*(reg8 *) CRC_Security_b3_CRCdp_d__A0_REG )
        #define CRC_Security_SEED_LOWER_D__A0_PTR                  ( (reg8 *) CRC_Security_b3_CRCdp_d__A0_REG )
        #define CRC_Security_SEED_LOWER_C__A0_REG               (*(reg8 *) CRC_Security_b2_CRCdp_c__A0_REG )
        #define CRC_Security_SEED_LOWER_C__A0_PTR               ( (reg8 *) CRC_Security_b2_CRCdp_c__A0_REG )
        #define CRC_Security_SEED_LOWER_B__A0_REG               (*(reg8 *) CRC_Security_b1_CRCdp_b__A0_REG )
        #define CRC_Security_SEED_LOWER_B__A0_PTR               ( (reg8 *) CRC_Security_b1_CRCdp_b__A0_REG )
        #define CRC_Security_SEED_LOWER_A__A0_REG               (*(reg8 *) CRC_Security_b0_CRCdp_a__A0_REG )
        #define CRC_Security_SEED_LOWER_A__A0_PTR               ( (reg8 *) CRC_Security_b0_CRCdp_a__A0_REG )
        /* COPY Seed Upper */
        #define CRC_Security_SEED_UPPER_COPY_D__A1_REG          (*(reg8 *) CRC_Security_b3_CRCdp_d__F1_REG )
        #define CRC_Security_SEED_UPPER_COPY_D__A1_PTR          ( (reg8 *) CRC_Security_b3_CRCdp_d__F1_REG )
        #define CRC_Security_SEED_UPPER_COPY_C__A1_REG          (*(reg8 *) CRC_Security_b2_CRCdp_c__F1_REG )
        #define CRC_Security_SEED_UPPER_COPY_C__A1_PTR          ( (reg8 *) CRC_Security_b2_CRCdp_c__F1_REG )
        #define CRC_Security_SEED_UPPER_COPY_B__A1_REG          (*(reg8 *) CRC_Security_b1_CRCdp_b__F1_REG )
        #define CRC_Security_SEED_UPPER_COPY_B__A1_PTR          ( (reg8 *) CRC_Security_b1_CRCdp_b__F1_REG )
        /* COPY Seed Lower */
        #define CRC_Security_SEED_LOWER_COPY_D__A0_REG          (*(reg8 *) CRC_Security_b3_CRCdp_d__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_D__A0_PTR          ( (reg8 *) CRC_Security_b3_CRCdp_d__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_C__A0_REG          (*(reg8 *) CRC_Security_b2_CRCdp_c__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_C__A0_PTR          ( (reg8 *) CRC_Security_b2_CRCdp_c__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_B__A0_REG          (*(reg8 *) CRC_Security_b1_CRCdp_b__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_B__A0_PTR          ( (reg8 *) CRC_Security_b1_CRCdp_b__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_A__A0_REG          (*(reg8 *) CRC_Security_b0_CRCdp_a__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_A__A0_PTR          ( (reg8 *) CRC_Security_b0_CRCdp_a__F0_REG )

    #else                                        /* 64 bits CRC */
        /* Polynom Upper */
        #define CRC_Security_POLYNOM_UPPER_D__D1_REG            (*(reg8 *) CRC_Security_b3_CRCdp_d__D1_REG )
        #define CRC_Security_POLYNOM_UPPER_D__D1_PTR            ( (reg8 *) CRC_Security_b3_CRCdp_d__D1_REG )
        #define CRC_Security_POLYNOM_UPPER_C__D1_REG            (*(reg8 *) CRC_Security_b2_CRCdp_c__D1_REG )
        #define CRC_Security_POLYNOM_UPPER_C__D1_PTR            ( (reg8 *) CRC_Security_b2_CRCdp_c__D1_REG )
        #define CRC_Security_POLYNOM_UPPER_B__D1_REG            (*(reg8 *) CRC_Security_b1_CRCdp_b__D1_REG )
        #define CRC_Security_POLYNOM_UPPER_B__D1_PTR            ( (reg8 *) CRC_Security_b1_CRCdp_b__D1_REG )
        #define CRC_Security_POLYNOM_UPPER_A__D1_REG            (*(reg8 *) CRC_Security_b0_CRCdp_a__D1_REG )
        #define CRC_Security_POLYNOM_UPPER_A__D1_PTR            ( (reg8 *) CRC_Security_b0_CRCdp_a__D1_REG )
        /* Polynom Lower */
        #define CRC_Security_POLYNOM_LOWER_D__D0_REG            (*(reg8 *) CRC_Security_b3_CRCdp_d__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_D__D0_PTR            ( (reg8 *) CRC_Security_b3_CRCdp_d__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_C__D0_REG            (*(reg8 *) CRC_Security_b2_CRCdp_c__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_C__D0_PTR            ( (reg8 *) CRC_Security_b2_CRCdp_c__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_B__D0_REG            (*(reg8 *) CRC_Security_b1_CRCdp_b__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_B__D0_PTR            ( (reg8 *) CRC_Security_b1_CRCdp_b__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_A__D0_REG            (*(reg8 *) CRC_Security_b0_CRCdp_a__D0_REG )
        #define CRC_Security_POLYNOM_LOWER_A__D0_PTR            ( (reg8 *) CRC_Security_b0_CRCdp_a__D0_REG )
        /* Seed Upper */
        #define CRC_Security_SEED_UPPER_D__A1_REG               (*(reg8 *) CRC_Security_b3_CRCdp_d__A1_REG )
        #define CRC_Security_SEED_UPPER_D__A1_PTR               ( (reg8 *) CRC_Security_b3_CRCdp_d__A1_REG )
        #define CRC_Security_SEED_UPPER_C__A1_REG               (*(reg8 *) CRC_Security_b2_CRCdp_c__A1_REG )
        #define CRC_Security_SEED_UPPER_C__A1_PTR               ( (reg8 *) CRC_Security_b2_CRCdp_c__A1_REG )
        #define CRC_Security_SEED_UPPER_B__A1_REG               (*(reg8 *) CRC_Security_b1_CRCdp_b__A1_REG )
        #define CRC_Security_SEED_UPPER_B__A1_PTR               ( (reg8 *) CRC_Security_b1_CRCdp_b__A1_REG )
        #define CRC_Security_SEED_UPPER_A__A1_REG               (*(reg8 *) CRC_Security_b0_CRCdp_a__A1_REG )
        #define CRC_Security_SEED_UPPER_A__A1_PTR               ( (reg8 *) CRC_Security_b0_CRCdp_a__A1_REG )
        /* Seed Lower */
        #define CRC_Security_SEED_LOWER_D__A0_REG               (*(reg8 *) CRC_Security_b3_CRCdp_d__A0_REG )
        #define CRC_Security_SEED_LOWER_D__A0_PTR               ( (reg8 *) CRC_Security_b3_CRCdp_d__A0_REG )
        #define CRC_Security_SEED_LOWER_C__A0_REG               (*(reg8 *) CRC_Security_b2_CRCdp_c__A0_REG )
        #define CRC_Security_SEED_LOWER_C__A0_PTR               ( (reg8 *) CRC_Security_b2_CRCdp_c__A0_REG )
        #define CRC_Security_SEED_LOWER_B__A0_REG               (*(reg8 *) CRC_Security_b1_CRCdp_b__A0_REG )
        #define CRC_Security_SEED_LOWER_B__A0_PTR               ( (reg8 *) CRC_Security_b1_CRCdp_b__A0_REG )
        #define CRC_Security_SEED_LOWER_A__A0_REG               (*(reg8 *) CRC_Security_b0_CRCdp_a__A0_REG )
        #define CRC_Security_SEED_LOWER_A__A0_PTR               ( (reg8 *) CRC_Security_b0_CRCdp_a__A0_REG )
        /* COPY Seed Upper */
        #define CRC_Security_SEED_UPPER_COPY_D__A1_REG          (*(reg8 *) CRC_Security_b3_CRCdp_d__F1_REG )
        #define CRC_Security_SEED_UPPER_COPY_D__A1_PTR          ( (reg8 *) CRC_Security_b3_CRCdp_d__F1_REG )
        #define CRC_Security_SEED_UPPER_COPY_C__A1_REG          (*(reg8 *) CRC_Security_b2_CRCdp_c__F1_REG )
        #define CRC_Security_SEED_UPPER_COPY_C__A1_PTR          ( (reg8 *) CRC_Security_b2_CRCdp_c__F1_REG )
        #define CRC_Security_SEED_UPPER_COPY_B__A1_REG          (*(reg8 *) CRC_Security_b1_CRCdp_b__F1_REG )
        #define CRC_Security_SEED_UPPER_COPY_B__A1_PTR          ( (reg8 *) CRC_Security_b1_CRCdp_b__F1_REG )
        #define CRC_Security_SEED_UPPER_COPY_A__A1_REG          (*(reg8 *) CRC_Security_b0_CRCdp_a__F1_REG )
        #define CRC_Security_SEED_UPPER_COPY_A__A1_PTR          ( (reg8 *) CRC_Security_b0_CRCdp_a__F1_REG )
        /* COPY Seed Lower */
        #define CRC_Security_SEED_LOWER_COPY_D__A0_REG          (*(reg8 *) CRC_Security_b3_CRCdp_d__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_D__A0_PTR          ( (reg8 *) CRC_Security_b3_CRCdp_d__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_C__A0_REG          (*(reg8 *) CRC_Security_b2_CRCdp_c__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_C__A0_PTR          ( (reg8 *) CRC_Security_b2_CRCdp_c__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_B__A0_REG          (*(reg8 *) CRC_Security_b1_CRCdp_b__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_B__A0_PTR          ( (reg8 *) CRC_Security_b1_CRCdp_b__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_A__A0_REG          (*(reg8 *) CRC_Security_b0_CRCdp_a__F0_REG )
        #define CRC_Security_SEED_LOWER_COPY_A__A0_PTR          ( (reg8 *) CRC_Security_b0_CRCdp_a__F0_REG )
    #endif  /* End (CRC_Security_CRC_SIZE <= 8u) */

#else
    #if (CY_PSOC3 || CY_PSOC5)
        #if (CRC_Security_CRC_SIZE <= 32u)         /* 8-32 bits CRC */
            /* Polynomial */
            #define CRC_Security_POLYNOM_PTR      ( (reg8 *)\
                                                CRC_Security_sC8_CRCdp_u0__D0_REG )
            /* Seed */
            #define CRC_Security_SEED_PTR         ( (reg8 *)\
                                                CRC_Security_sC8_CRCdp_u0__A0_REG )
            /* Seed COPY */
            #define CRC_Security_SEED_COPY_PTR    ( (reg8 *)\
                                                CRC_Security_sC8_CRCdp_u0__F0_REG )

        #endif  /* End (CRC_Security_CRC_SIZE <= 32u) */
    #else /* PSoC4 */
        #if (CRC_Security_CRC_SIZE <= 8u)         /* 8 bits CRC */
            /* Polynomial */
            #define CRC_Security_POLYNOM_PTR      ( (reg8 *)\
                                                CRC_Security_sC8_CRCdp_u0__D0_REG )
            /* Seed */
            #define CRC_Security_SEED_PTR         ( (reg8 *)\
                                                CRC_Security_sC8_CRCdp_u0__A0_REG )
            /* Seed COPY */
            #define CRC_Security_SEED_COPY_PTR    ( (reg8 *)\
                                                CRC_Security_sC8_CRCdp_u0__F0_REG )

        #elif (CRC_Security_CRC_SIZE <= 16u)         /* 16 bits CRC */
            /* Polynomial */
            #define CRC_Security_POLYNOM_PTR      ( (reg8 *)\
                                            CRC_Security_sC8_CRCdp_u0__16BIT_D0_REG )
            /* Seed */
            #define CRC_Security_SEED_PTR         ( (reg8 *)\
                                            CRC_Security_sC8_CRCdp_u0__16BIT_A0_REG )
            /* Seed COPY */
            #define CRC_Security_SEED_COPY_PTR    ( (reg8 *)\
                                            CRC_Security_sC8_CRCdp_u0__16BIT_F0_REG )

        #elif (CRC_Security_CRC_SIZE <= 24u)         /* 24 bits CRC */
            /* Polynomial */
            #define CRC_Security_POLYNOM_PTR      ( (reg8 *)\
                                                CRC_Security_sC8_CRCdp_u0__D0_REG )
            /* Seed */
            #define CRC_Security_SEED_PTR         ( (reg8 *)\
                                                CRC_Security_sC8_CRCdp_u0__A0_REG )
            /* Seed COPY */
            #define CRC_Security_SEED_COPY_PTR    ( (reg8 *)\
                                                CRC_Security_sC8_CRCdp_u0__F0_REG )

         #else                                          /* 32 bits CRC */
            /* Polynomial */
            #define CRC_Security_POLYNOM_PTR      ( (reg8 *)\
                                            CRC_Security_sC8_CRCdp_u0__32BIT_D0_REG )
            /* Seed */
            #define CRC_Security_SEED_PTR         ( (reg8 *)\
                                            CRC_Security_sC8_CRCdp_u0__32BIT_A0_REG )
            /* Seed COPY */
            #define CRC_Security_SEED_COPY_PTR    ( (reg8 *)\
                                            CRC_Security_sC8_CRCdp_u0__32BIT_F0_REG )

        #endif  /* End (CRC_Security_CRC_SIZE <= 32u) */

    #endif  /* End (CY_PSOC3 || CY_PSOC5) */

#endif  /* End (CRC_Security_TIME_MULTIPLEXING_ENABLE) */

/* Aux control */
#if (CRC_Security_TIME_MULTIPLEXING_ENABLE)
    #if (CRC_Security_CRC_SIZE <= 16u)      /* 8-16 bits CRC */
        #define CRC_Security_AUX_CONTROL_A_REG              (*(reg8 *) CRC_Security_b0_CRCdp_a__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_A_PTR              ( (reg8 *) CRC_Security_b0_CRCdp_a__DP_AUX_CTL_REG )

    #elif (CRC_Security_CRC_SIZE <= 32u)      /* 17-32 bits CRC */
        #define CRC_Security_AUX_CONTROL_A_REG              (*(reg8 *) CRC_Security_b0_CRCdp_a__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_A_PTR              ( (reg8 *) CRC_Security_b0_CRCdp_a__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_B_REG              (*(reg8 *) CRC_Security_b1_CRCdp_b__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_B_PTR              ( (reg8 *) CRC_Security_b1_CRCdp_b__DP_AUX_CTL_REG )

    #elif (CRC_Security_CRC_SIZE <= 48u)      /* 33-48 bits CRC */
        #define CRC_Security_AUX_CONTROL_A_REG              (*(reg8 *) CRC_Security_b0_CRCdp_a__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_A_PTR              ( (reg8 *) CRC_Security_b0_CRCdp_a__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_B_REG              (*(reg8 *) CRC_Security_b1_CRCdp_b__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_B_PTR              ( (reg8 *) CRC_Security_b1_CRCdp_b__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_C_REG              (*(reg8 *) CRC_Security_b2_CRCdp_c__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_C_PTR              ( (reg8 *) CRC_Security_b2_CRCdp_c__DP_AUX_CTL_REG )

    #else                                         /* 48-64 bits CRC */
        #define CRC_Security_AUX_CONTROL_A_REG              (*(reg8 *) CRC_Security_b0_CRCdp_a__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_A_PTR              ( (reg8 *) CRC_Security_b0_CRCdp_a__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_B_REG              (*(reg8 *) CRC_Security_b1_CRCdp_b__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_B_PTR              ( (reg8 *) CRC_Security_b1_CRCdp_b__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_C_REG              (*(reg8 *) CRC_Security_b2_CRCdp_c__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_C_PTR              ( (reg8 *) CRC_Security_b2_CRCdp_c__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_D_REG              (*(reg8 *) CRC_Security_b3_CRCdp_d__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_D_PTR              ( (reg8 *) CRC_Security_b3_CRCdp_d__DP_AUX_CTL_REG )

    #endif  /* End (CRC_Security_CRC_SIZE <= 8u) */

#else
    #if (CRC_Security_CRC_SIZE <= 8u)      /* 8 bits CRC */
        #define CRC_Security_AUX_CONTROL_A_REG          (*(reg8 *) CRC_Security_sC8_CRCdp_u0__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_A_PTR          ( (reg8 *) CRC_Security_sC8_CRCdp_u0__DP_AUX_CTL_REG )

    #elif (CRC_Security_CRC_SIZE <= 16u)      /* 16 bits CRC */
        #define CRC_Security_AUX_CONTROL_A_REG          (*(reg8 *) CRC_Security_sC16_CRCdp_u0__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_A_PTR          ( (reg8 *) CRC_Security_sC16_CRCdp_u0__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_B_REG          (*(reg8 *) CRC_Security_sC16_CRCdp_u1__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_B_PTR          ( (reg8 *) CRC_Security_sC16_CRCdp_u1__DP_AUX_CTL_REG )

    #elif (CRC_Security_CRC_SIZE <= 24u)      /* 24-39 bits CRC */
        #define CRC_Security_AUX_CONTROL_A_REG          (*(reg8 *) CRC_Security_sC24_CRCdp_u0__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_A_PTR          ( (reg8 *) CRC_Security_sC24_CRCdp_u0__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_B_REG          (*(reg8 *) CRC_Security_sC24_CRCdp_u1__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_B_PTR          ( (reg8 *) CRC_Security_sC24_CRCdp_u1__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_C_REG          (*(reg8 *) CRC_Security_sC24_CRCdp_u2__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_C_PTR          ( (reg8 *) CRC_Security_sC24_CRCdp_u2__DP_AUX_CTL_REG )

    #elif (CRC_Security_CRC_SIZE <= 32u)      /* 40-55 bits CRC */
        #define CRC_Security_AUX_CONTROL_A_REG          (*(reg8 *) CRC_Security_sC32_CRCdp_u0__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_A_PTR          ( (reg8 *) CRC_Security_sC32_CRCdp_u0__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_B_REG          (*(reg8 *) CRC_Security_sC32_CRCdp_u1__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_B_PTR          ( (reg8 *) CRC_Security_sC32_CRCdp_u1__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_C_REG          (*(reg8 *) CRC_Security_sC32_CRCdp_u2__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_C_PTR          ( (reg8 *) CRC_Security_sC32_CRCdp_u2__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_D_REG          (*(reg8 *) CRC_Security_sC32_CRCdp_u3__DP_AUX_CTL_REG )
        #define CRC_Security_AUX_CONTROL_D_PTR          ( (reg8 *) CRC_Security_sC32_CRCdp_u3__DP_AUX_CTL_REG )

    #else                                         /* 56-64 bits CRC */
        /* Not supported */
    #endif  /* End (CRC_Security_CRC_SIZE <= 8u) */

#endif  /* End (CRC_Security_TIME_MULTIPLEXING_ENABLE) */

#if (CY_UDB_V0)
    #define CRC_Security_CONTROL_REG                    (*(reg8 *) CRC_Security_AsyncCtrl_CtrlReg__CONTROL_REG )
    #define CRC_Security_CONTROL_PTR                    ( (reg8 *) CRC_Security_AsyncCtrl_CtrlReg__CONTROL_REG )

#else
    #define CRC_Security_CONTROL_REG                    (*(reg8 *) CRC_Security_SyncCtrl_CtrlReg__CONTROL_REG )
    #define CRC_Security_CONTROL_PTR                    ( (reg8 *) CRC_Security_SyncCtrl_CtrlReg__CONTROL_REG )

#endif  /* End (CY_UDB_V0) */


/***************************************
*       Register Constants
***************************************/

/* Control register definitions */
#define CRC_Security_CTRL_ENABLE                    (0x01u)
#define CRC_Security_CTRL_RESET                     (0x02u)

/* Aux Control register definitions */
#define CRC_Security_AUXCTRL_FIFO_SINGLE_REG        (0x03u)

#define CRC_Security_IS_CRC_ENABLE(reg)             (((reg) & CRC_Security_CTRL_ENABLE) != 0u)

#endif  /* End CY_CRC_CRC_Security_H */

/* [] END OF FILE */
