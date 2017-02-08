/*******************************************************************************
* File Name: GlobalCounter.h  
* Version 3.0
*
*  Description:
*   Contains the function prototypes and constants available to the counter
*   user module.
*
*   Note:
*    None
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
    
#if !defined(CY_COUNTER_GlobalCounter_H)
#define CY_COUNTER_GlobalCounter_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */

extern uint8 GlobalCounter_initVar;

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component Counter_v3_0 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */

/* Error message for removed GlobalCounter_CounterUDB_sCTRLReg_ctrlreg through optimization */
#ifdef GlobalCounter_CounterUDB_sCTRLReg_ctrlreg__REMOVED
    #error Counter_v3_0 detected with a constant 0 for the enable, a \
                                constant 0 for the count or constant 1 for \
                                the reset. This will prevent the component from\
                                operating.
#endif /* GlobalCounter_CounterUDB_sCTRLReg_ctrlreg__REMOVED */


/**************************************
*           Parameter Defaults        
**************************************/

#define GlobalCounter_Resolution            16u
#define GlobalCounter_UsingFixedFunction    1u
#define GlobalCounter_ControlRegRemoved     0u
#define GlobalCounter_COMPARE_MODE_SOFTWARE 0u
#define GlobalCounter_CAPTURE_MODE_SOFTWARE 0u
#define GlobalCounter_RunModeUsed           0u


/***************************************
*       Type defines
***************************************/


/**************************************************************************
 * Sleep Mode API Support
 * Backup structure for Sleep Wake up operations
 *************************************************************************/
typedef struct
{
    uint8 CounterEnableState; 
    uint16 CounterUdb;         /* Current Counter Value */

    #if (!GlobalCounter_ControlRegRemoved)
        uint8 CounterControlRegister;               /* Counter Control Register */
    #endif /* (!GlobalCounter_ControlRegRemoved) */

}GlobalCounter_backupStruct;


/**************************************
 *  Function Prototypes
 *************************************/
void    GlobalCounter_Start(void) ;
void    GlobalCounter_Stop(void) ;
void    GlobalCounter_SetInterruptMode(uint8 interruptsMask) ;
uint8   GlobalCounter_ReadStatusRegister(void) ;
#define GlobalCounter_GetInterruptSource() GlobalCounter_ReadStatusRegister()
#if(!GlobalCounter_ControlRegRemoved)
    uint8   GlobalCounter_ReadControlRegister(void) ;
    void    GlobalCounter_WriteControlRegister(uint8 control) \
        ;
#endif /* (!GlobalCounter_ControlRegRemoved) */
#if (!(GlobalCounter_UsingFixedFunction && (CY_PSOC5A)))
    void    GlobalCounter_WriteCounter(uint16 counter) \
            ; 
#endif /* (!(GlobalCounter_UsingFixedFunction && (CY_PSOC5A))) */
uint16  GlobalCounter_ReadCounter(void) ;
uint16  GlobalCounter_ReadCapture(void) ;
void    GlobalCounter_WritePeriod(uint16 period) \
    ;
uint16  GlobalCounter_ReadPeriod( void ) ;
#if (!GlobalCounter_UsingFixedFunction)
    void    GlobalCounter_WriteCompare(uint16 compare) \
        ;
    uint16  GlobalCounter_ReadCompare( void ) \
        ;
#endif /* (!GlobalCounter_UsingFixedFunction) */

#if (GlobalCounter_COMPARE_MODE_SOFTWARE)
    void    GlobalCounter_SetCompareMode(uint8 compareMode) ;
#endif /* (GlobalCounter_COMPARE_MODE_SOFTWARE) */
#if (GlobalCounter_CAPTURE_MODE_SOFTWARE)
    void    GlobalCounter_SetCaptureMode(uint8 captureMode) ;
#endif /* (GlobalCounter_CAPTURE_MODE_SOFTWARE) */
void GlobalCounter_ClearFIFO(void)     ;
void GlobalCounter_Init(void)          ;
void GlobalCounter_Enable(void)        ;
void GlobalCounter_SaveConfig(void)    ;
void GlobalCounter_RestoreConfig(void) ;
void GlobalCounter_Sleep(void)         ;
void GlobalCounter_Wakeup(void)        ;


/***************************************
*   Enumerated Types and Parameters
***************************************/

/* Enumerated Type B_Counter__CompareModes, Used in Compare Mode retained for backward compatibility of tests*/
#define GlobalCounter__B_COUNTER__LESS_THAN 1
#define GlobalCounter__B_COUNTER__LESS_THAN_OR_EQUAL 2
#define GlobalCounter__B_COUNTER__EQUAL 0
#define GlobalCounter__B_COUNTER__GREATER_THAN 3
#define GlobalCounter__B_COUNTER__GREATER_THAN_OR_EQUAL 4
#define GlobalCounter__B_COUNTER__SOFTWARE 5

/* Enumerated Type Counter_CompareModes */
#define GlobalCounter_CMP_MODE_LT 1u
#define GlobalCounter_CMP_MODE_LTE 2u
#define GlobalCounter_CMP_MODE_EQ 0u
#define GlobalCounter_CMP_MODE_GT 3u
#define GlobalCounter_CMP_MODE_GTE 4u
#define GlobalCounter_CMP_MODE_SOFTWARE_CONTROLLED 5u

/* Enumerated Type B_Counter__CaptureModes, Used in Capture Mode retained for backward compatibility of tests*/
#define GlobalCounter__B_COUNTER__NONE 0
#define GlobalCounter__B_COUNTER__RISING_EDGE 1
#define GlobalCounter__B_COUNTER__FALLING_EDGE 2
#define GlobalCounter__B_COUNTER__EITHER_EDGE 3
#define GlobalCounter__B_COUNTER__SOFTWARE_CONTROL 4

/* Enumerated Type Counter_CompareModes */
#define GlobalCounter_CAP_MODE_NONE 0u
#define GlobalCounter_CAP_MODE_RISE 1u
#define GlobalCounter_CAP_MODE_FALL 2u
#define GlobalCounter_CAP_MODE_BOTH 3u
#define GlobalCounter_CAP_MODE_SOFTWARE_CONTROLLED 4u


/***************************************
 *  Initialization Values
 **************************************/
#define GlobalCounter_CAPTURE_MODE_CONF       0u
#define GlobalCounter_INIT_PERIOD_VALUE       1000u
#define GlobalCounter_INIT_COUNTER_VALUE      1000u
#if (GlobalCounter_UsingFixedFunction)
#define GlobalCounter_INIT_INTERRUPTS_MASK    ((uint8)((uint8)1u << GlobalCounter_STATUS_ZERO_INT_EN_MASK_SHIFT))
#else
#define GlobalCounter_INIT_COMPARE_VALUE      0u
#define GlobalCounter_INIT_INTERRUPTS_MASK ((uint8)((uint8)1u << GlobalCounter_STATUS_ZERO_INT_EN_MASK_SHIFT) | \
        ((uint8)((uint8)0u << GlobalCounter_STATUS_CAPTURE_INT_EN_MASK_SHIFT)) | \
        ((uint8)((uint8)0u << GlobalCounter_STATUS_CMP_INT_EN_MASK_SHIFT)) | \
        ((uint8)((uint8)0u << GlobalCounter_STATUS_OVERFLOW_INT_EN_MASK_SHIFT)) | \
        ((uint8)((uint8)0u << GlobalCounter_STATUS_UNDERFLOW_INT_EN_MASK_SHIFT)))
#define GlobalCounter_DEFAULT_COMPARE_MODE    1u

#if( 0 != GlobalCounter_CAPTURE_MODE_CONF)
    #define GlobalCounter_DEFAULT_CAPTURE_MODE    ((uint8)((uint8)0u << GlobalCounter_CTRL_CAPMODE0_SHIFT))
#else    
    #define GlobalCounter_DEFAULT_CAPTURE_MODE    (0u )
#endif /* ( 0 != GlobalCounter_CAPTURE_MODE_CONF) */

#endif /* (GlobalCounter_UsingFixedFunction) */


/**************************************
 *  Registers
 *************************************/
#if (GlobalCounter_UsingFixedFunction)
    #define GlobalCounter_STATICCOUNT_LSB     (*(reg16 *) GlobalCounter_CounterHW__CAP0 )
    #define GlobalCounter_STATICCOUNT_LSB_PTR ( (reg16 *) GlobalCounter_CounterHW__CAP0 )
    #define GlobalCounter_PERIOD_LSB          (*(reg16 *) GlobalCounter_CounterHW__PER0 )
    #define GlobalCounter_PERIOD_LSB_PTR      ( (reg16 *) GlobalCounter_CounterHW__PER0 )
    /* MODE must be set to 1 to set the compare value */
    #define GlobalCounter_COMPARE_LSB         (*(reg16 *) GlobalCounter_CounterHW__CNT_CMP0 )
    #define GlobalCounter_COMPARE_LSB_PTR     ( (reg16 *) GlobalCounter_CounterHW__CNT_CMP0 )
    /* MODE must be set to 0 to get the count */
    #define GlobalCounter_COUNTER_LSB         (*(reg16 *) GlobalCounter_CounterHW__CNT_CMP0 )
    #define GlobalCounter_COUNTER_LSB_PTR     ( (reg16 *) GlobalCounter_CounterHW__CNT_CMP0 )
    #define GlobalCounter_RT1                 (*(reg8 *) GlobalCounter_CounterHW__RT1)
    #define GlobalCounter_RT1_PTR             ( (reg8 *) GlobalCounter_CounterHW__RT1)
#else
    
    #if (GlobalCounter_Resolution <= 8u) /* 8-bit Counter */
    
        #define GlobalCounter_STATICCOUNT_LSB     (*(reg8 *) \
            GlobalCounter_CounterUDB_sC16_counterdp_u0__F0_REG )
        #define GlobalCounter_STATICCOUNT_LSB_PTR ( (reg8 *) \
            GlobalCounter_CounterUDB_sC16_counterdp_u0__F0_REG )
        #define GlobalCounter_PERIOD_LSB          (*(reg8 *) \
            GlobalCounter_CounterUDB_sC16_counterdp_u0__D0_REG )
        #define GlobalCounter_PERIOD_LSB_PTR      ( (reg8 *) \
            GlobalCounter_CounterUDB_sC16_counterdp_u0__D0_REG )
        #define GlobalCounter_COMPARE_LSB         (*(reg8 *) \
            GlobalCounter_CounterUDB_sC16_counterdp_u0__D1_REG )
        #define GlobalCounter_COMPARE_LSB_PTR     ( (reg8 *) \
            GlobalCounter_CounterUDB_sC16_counterdp_u0__D1_REG )
        #define GlobalCounter_COUNTER_LSB         (*(reg8 *) \
            GlobalCounter_CounterUDB_sC16_counterdp_u0__A0_REG )  
        #define GlobalCounter_COUNTER_LSB_PTR     ( (reg8 *)\
            GlobalCounter_CounterUDB_sC16_counterdp_u0__A0_REG )
    
    #elif(GlobalCounter_Resolution <= 16u) /* 16-bit Counter */
        #if(CY_PSOC3) /* 8-bit address space */ 
            #define GlobalCounter_STATICCOUNT_LSB     (*(reg16 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__F0_REG )
            #define GlobalCounter_STATICCOUNT_LSB_PTR ( (reg16 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__F0_REG )
            #define GlobalCounter_PERIOD_LSB          (*(reg16 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__D0_REG )
            #define GlobalCounter_PERIOD_LSB_PTR      ( (reg16 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__D0_REG )
            #define GlobalCounter_COMPARE_LSB         (*(reg16 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__D1_REG )
            #define GlobalCounter_COMPARE_LSB_PTR     ( (reg16 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__D1_REG )
            #define GlobalCounter_COUNTER_LSB         (*(reg16 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__A0_REG )  
            #define GlobalCounter_COUNTER_LSB_PTR     ( (reg16 *)\
                GlobalCounter_CounterUDB_sC16_counterdp_u0__A0_REG )
        #else /* 16-bit address space */
            #define GlobalCounter_STATICCOUNT_LSB     (*(reg16 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__16BIT_F0_REG )
            #define GlobalCounter_STATICCOUNT_LSB_PTR ( (reg16 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__16BIT_F0_REG )
            #define GlobalCounter_PERIOD_LSB          (*(reg16 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__16BIT_D0_REG )
            #define GlobalCounter_PERIOD_LSB_PTR      ( (reg16 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__16BIT_D0_REG )
            #define GlobalCounter_COMPARE_LSB         (*(reg16 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__16BIT_D1_REG )
            #define GlobalCounter_COMPARE_LSB_PTR     ( (reg16 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__16BIT_D1_REG )
            #define GlobalCounter_COUNTER_LSB         (*(reg16 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__16BIT_A0_REG )  
            #define GlobalCounter_COUNTER_LSB_PTR     ( (reg16 *)\
                GlobalCounter_CounterUDB_sC16_counterdp_u0__16BIT_A0_REG )
        #endif /* CY_PSOC3 */   
    #elif(GlobalCounter_Resolution <= 24u) /* 24-bit Counter */
        
        #define GlobalCounter_STATICCOUNT_LSB     (*(reg32 *) \
            GlobalCounter_CounterUDB_sC16_counterdp_u0__F0_REG )
        #define GlobalCounter_STATICCOUNT_LSB_PTR ( (reg32 *) \
            GlobalCounter_CounterUDB_sC16_counterdp_u0__F0_REG )
        #define GlobalCounter_PERIOD_LSB          (*(reg32 *) \
            GlobalCounter_CounterUDB_sC16_counterdp_u0__D0_REG )
        #define GlobalCounter_PERIOD_LSB_PTR      ( (reg32 *) \
            GlobalCounter_CounterUDB_sC16_counterdp_u0__D0_REG )
        #define GlobalCounter_COMPARE_LSB         (*(reg32 *) \
            GlobalCounter_CounterUDB_sC16_counterdp_u0__D1_REG )
        #define GlobalCounter_COMPARE_LSB_PTR     ( (reg32 *) \
            GlobalCounter_CounterUDB_sC16_counterdp_u0__D1_REG )
        #define GlobalCounter_COUNTER_LSB         (*(reg32 *) \
            GlobalCounter_CounterUDB_sC16_counterdp_u0__A0_REG )  
        #define GlobalCounter_COUNTER_LSB_PTR     ( (reg32 *)\
            GlobalCounter_CounterUDB_sC16_counterdp_u0__A0_REG )
    
    #else /* 32-bit Counter */
        #if(CY_PSOC3 || CY_PSOC5) /* 8-bit address space */
            #define GlobalCounter_STATICCOUNT_LSB     (*(reg32 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__F0_REG )
            #define GlobalCounter_STATICCOUNT_LSB_PTR ( (reg32 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__F0_REG )
            #define GlobalCounter_PERIOD_LSB          (*(reg32 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__D0_REG )
            #define GlobalCounter_PERIOD_LSB_PTR      ( (reg32 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__D0_REG )
            #define GlobalCounter_COMPARE_LSB         (*(reg32 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__D1_REG )
            #define GlobalCounter_COMPARE_LSB_PTR     ( (reg32 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__D1_REG )
            #define GlobalCounter_COUNTER_LSB         (*(reg32 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__A0_REG )  
            #define GlobalCounter_COUNTER_LSB_PTR     ( (reg32 *)\
                GlobalCounter_CounterUDB_sC16_counterdp_u0__A0_REG )
        #else /* 32-bit address space */
            #define GlobalCounter_STATICCOUNT_LSB     (*(reg32 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__32BIT_F0_REG )
            #define GlobalCounter_STATICCOUNT_LSB_PTR ( (reg32 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__32BIT_F0_REG )
            #define GlobalCounter_PERIOD_LSB          (*(reg32 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__32BIT_D0_REG )
            #define GlobalCounter_PERIOD_LSB_PTR      ( (reg32 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__32BIT_D0_REG )
            #define GlobalCounter_COMPARE_LSB         (*(reg32 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__32BIT_D1_REG )
            #define GlobalCounter_COMPARE_LSB_PTR     ( (reg32 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__32BIT_D1_REG )
            #define GlobalCounter_COUNTER_LSB         (*(reg32 *) \
                GlobalCounter_CounterUDB_sC16_counterdp_u0__32BIT_A0_REG )  
            #define GlobalCounter_COUNTER_LSB_PTR     ( (reg32 *)\
                GlobalCounter_CounterUDB_sC16_counterdp_u0__32BIT_A0_REG )
        #endif /* CY_PSOC3 || CY_PSOC5 */   
    #endif

	#define GlobalCounter_COUNTER_LSB_PTR_8BIT     ( (reg8 *)\
                GlobalCounter_CounterUDB_sC16_counterdp_u0__A0_REG )
				
    #define GlobalCounter_AUX_CONTROLDP0 \
        (*(reg8 *) GlobalCounter_CounterUDB_sC16_counterdp_u0__DP_AUX_CTL_REG)
    
    #define GlobalCounter_AUX_CONTROLDP0_PTR \
        ( (reg8 *) GlobalCounter_CounterUDB_sC16_counterdp_u0__DP_AUX_CTL_REG)
    
    #if (GlobalCounter_Resolution == 16 || GlobalCounter_Resolution == 24 || GlobalCounter_Resolution == 32)
       #define GlobalCounter_AUX_CONTROLDP1 \
           (*(reg8 *) GlobalCounter_CounterUDB_sC16_counterdp_u1__DP_AUX_CTL_REG)
       #define GlobalCounter_AUX_CONTROLDP1_PTR \
           ( (reg8 *) GlobalCounter_CounterUDB_sC16_counterdp_u1__DP_AUX_CTL_REG)
    #endif /* (GlobalCounter_Resolution == 16 || GlobalCounter_Resolution == 24 || GlobalCounter_Resolution == 32) */
    
    #if (GlobalCounter_Resolution == 24 || GlobalCounter_Resolution == 32)
       #define GlobalCounter_AUX_CONTROLDP2 \
           (*(reg8 *) GlobalCounter_CounterUDB_sC16_counterdp_u2__DP_AUX_CTL_REG)
       #define GlobalCounter_AUX_CONTROLDP2_PTR \
           ( (reg8 *) GlobalCounter_CounterUDB_sC16_counterdp_u2__DP_AUX_CTL_REG)
    #endif /* (GlobalCounter_Resolution == 24 || GlobalCounter_Resolution == 32) */
    
    #if (GlobalCounter_Resolution == 32)
       #define GlobalCounter_AUX_CONTROLDP3 \
           (*(reg8 *) GlobalCounter_CounterUDB_sC16_counterdp_u3__DP_AUX_CTL_REG)
       #define GlobalCounter_AUX_CONTROLDP3_PTR \
           ( (reg8 *) GlobalCounter_CounterUDB_sC16_counterdp_u3__DP_AUX_CTL_REG)
    #endif /* (GlobalCounter_Resolution == 32) */

#endif  /* (GlobalCounter_UsingFixedFunction) */

#if (GlobalCounter_UsingFixedFunction)
    #define GlobalCounter_STATUS         (*(reg8 *) GlobalCounter_CounterHW__SR0 )
    /* In Fixed Function Block Status and Mask are the same register */
    #define GlobalCounter_STATUS_MASK             (*(reg8 *) GlobalCounter_CounterHW__SR0 )
    #define GlobalCounter_STATUS_MASK_PTR         ( (reg8 *) GlobalCounter_CounterHW__SR0 )
    #define GlobalCounter_CONTROL                 (*(reg8 *) GlobalCounter_CounterHW__CFG0)
    #define GlobalCounter_CONTROL_PTR             ( (reg8 *) GlobalCounter_CounterHW__CFG0)
    #define GlobalCounter_CONTROL2                (*(reg8 *) GlobalCounter_CounterHW__CFG1)
    #define GlobalCounter_CONTROL2_PTR            ( (reg8 *) GlobalCounter_CounterHW__CFG1)
    #if (CY_PSOC3 || CY_PSOC5LP)
        #define GlobalCounter_CONTROL3       (*(reg8 *) GlobalCounter_CounterHW__CFG2)
        #define GlobalCounter_CONTROL3_PTR   ( (reg8 *) GlobalCounter_CounterHW__CFG2)
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */
    #define GlobalCounter_GLOBAL_ENABLE           (*(reg8 *) GlobalCounter_CounterHW__PM_ACT_CFG)
    #define GlobalCounter_GLOBAL_ENABLE_PTR       ( (reg8 *) GlobalCounter_CounterHW__PM_ACT_CFG)
    #define GlobalCounter_GLOBAL_STBY_ENABLE      (*(reg8 *) GlobalCounter_CounterHW__PM_STBY_CFG)
    #define GlobalCounter_GLOBAL_STBY_ENABLE_PTR  ( (reg8 *) GlobalCounter_CounterHW__PM_STBY_CFG)
    

    /********************************
    *    Constants
    ********************************/

    /* Fixed Function Block Chosen */
    #define GlobalCounter_BLOCK_EN_MASK          GlobalCounter_CounterHW__PM_ACT_MSK
    #define GlobalCounter_BLOCK_STBY_EN_MASK     GlobalCounter_CounterHW__PM_STBY_MSK 
    
    /* Control Register Bit Locations */    
    /* As defined in Register Map, part of TMRX_CFG0 register */
    #define GlobalCounter_CTRL_ENABLE_SHIFT      0x00u
    #define GlobalCounter_ONESHOT_SHIFT          0x02u
    /* Control Register Bit Masks */
    #define GlobalCounter_CTRL_ENABLE            ((uint8)((uint8)0x01u << GlobalCounter_CTRL_ENABLE_SHIFT))         
    #define GlobalCounter_ONESHOT                ((uint8)((uint8)0x01u << GlobalCounter_ONESHOT_SHIFT))

    /* Control2 Register Bit Masks */
    /* Set the mask for run mode */
    #if (CY_PSOC5A)
        /* Use CFG1 Mode bits to set run mode */
        #define GlobalCounter_CTRL_MODE_SHIFT        0x01u    
        #define GlobalCounter_CTRL_MODE_MASK         ((uint8)((uint8)0x07u << GlobalCounter_CTRL_MODE_SHIFT))
    #endif /* (CY_PSOC5A) */
    #if (CY_PSOC3 || CY_PSOC5LP)
        /* Use CFG2 Mode bits to set run mode */
        #define GlobalCounter_CTRL_MODE_SHIFT        0x00u    
        #define GlobalCounter_CTRL_MODE_MASK         ((uint8)((uint8)0x03u << GlobalCounter_CTRL_MODE_SHIFT))
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */
    /* Set the mask for interrupt (raw/status register) */
    #define GlobalCounter_CTRL2_IRQ_SEL_SHIFT     0x00u
    #define GlobalCounter_CTRL2_IRQ_SEL          ((uint8)((uint8)0x01u << GlobalCounter_CTRL2_IRQ_SEL_SHIFT))     
    
    /* Status Register Bit Locations */
    #define GlobalCounter_STATUS_ZERO_SHIFT      0x07u  /* As defined in Register Map, part of TMRX_SR0 register */ 

    /* Status Register Interrupt Enable Bit Locations */
    #define GlobalCounter_STATUS_ZERO_INT_EN_MASK_SHIFT      (GlobalCounter_STATUS_ZERO_SHIFT - 0x04u)

    /* Status Register Bit Masks */                           
    #define GlobalCounter_STATUS_ZERO            ((uint8)((uint8)0x01u << GlobalCounter_STATUS_ZERO_SHIFT))

    /* Status Register Interrupt Bit Masks*/
    #define GlobalCounter_STATUS_ZERO_INT_EN_MASK       ((uint8)((uint8)GlobalCounter_STATUS_ZERO >> 0x04u))
    
    /*RT1 Synch Constants: Applicable for PSoC3 and PSoC5LP */
    #define GlobalCounter_RT1_SHIFT            0x04u
    #define GlobalCounter_RT1_MASK             ((uint8)((uint8)0x03u << GlobalCounter_RT1_SHIFT))  /* Sync TC and CMP bit masks */
    #define GlobalCounter_SYNC                 ((uint8)((uint8)0x03u << GlobalCounter_RT1_SHIFT))
    #define GlobalCounter_SYNCDSI_SHIFT        0x00u
    #define GlobalCounter_SYNCDSI_MASK         ((uint8)((uint8)0x0Fu << GlobalCounter_SYNCDSI_SHIFT)) /* Sync all DSI inputs */
    #define GlobalCounter_SYNCDSI_EN           ((uint8)((uint8)0x0Fu << GlobalCounter_SYNCDSI_SHIFT)) /* Sync all DSI inputs */
    
#else /* !GlobalCounter_UsingFixedFunction */
    #define GlobalCounter_STATUS               (* (reg8 *) GlobalCounter_CounterUDB_sSTSReg_stsreg__STATUS_REG )
    #define GlobalCounter_STATUS_PTR           (  (reg8 *) GlobalCounter_CounterUDB_sSTSReg_stsreg__STATUS_REG )
    #define GlobalCounter_STATUS_MASK          (* (reg8 *) GlobalCounter_CounterUDB_sSTSReg_stsreg__MASK_REG )
    #define GlobalCounter_STATUS_MASK_PTR      (  (reg8 *) GlobalCounter_CounterUDB_sSTSReg_stsreg__MASK_REG )
    #define GlobalCounter_STATUS_AUX_CTRL      (*(reg8 *) GlobalCounter_CounterUDB_sSTSReg_stsreg__STATUS_AUX_CTL_REG)
    #define GlobalCounter_STATUS_AUX_CTRL_PTR  ( (reg8 *) GlobalCounter_CounterUDB_sSTSReg_stsreg__STATUS_AUX_CTL_REG)
    #define GlobalCounter_CONTROL              (* (reg8 *) GlobalCounter_CounterUDB_sCTRLReg_ctrlreg__CONTROL_REG )
    #define GlobalCounter_CONTROL_PTR          (  (reg8 *) GlobalCounter_CounterUDB_sCTRLReg_ctrlreg__CONTROL_REG )


    /********************************
    *    Constants
    ********************************/
    /* Control Register Bit Locations */
    #define GlobalCounter_CTRL_CAPMODE0_SHIFT    0x03u       /* As defined by Verilog Implementation */
    #define GlobalCounter_CTRL_RESET_SHIFT       0x06u       /* As defined by Verilog Implementation */
    #define GlobalCounter_CTRL_ENABLE_SHIFT      0x07u       /* As defined by Verilog Implementation */
    /* Control Register Bit Masks */
    #define GlobalCounter_CTRL_CMPMODE_MASK      0x07u 
    #define GlobalCounter_CTRL_CAPMODE_MASK      0x03u  
    #define GlobalCounter_CTRL_RESET             ((uint8)((uint8)0x01u << GlobalCounter_CTRL_RESET_SHIFT))  
    #define GlobalCounter_CTRL_ENABLE            ((uint8)((uint8)0x01u << GlobalCounter_CTRL_ENABLE_SHIFT)) 

    /* Status Register Bit Locations */
    #define GlobalCounter_STATUS_CMP_SHIFT       0x00u       /* As defined by Verilog Implementation */
    #define GlobalCounter_STATUS_ZERO_SHIFT      0x01u       /* As defined by Verilog Implementation */
    #define GlobalCounter_STATUS_OVERFLOW_SHIFT  0x02u       /* As defined by Verilog Implementation */
    #define GlobalCounter_STATUS_UNDERFLOW_SHIFT 0x03u       /* As defined by Verilog Implementation */
    #define GlobalCounter_STATUS_CAPTURE_SHIFT   0x04u       /* As defined by Verilog Implementation */
    #define GlobalCounter_STATUS_FIFOFULL_SHIFT  0x05u       /* As defined by Verilog Implementation */
    #define GlobalCounter_STATUS_FIFONEMP_SHIFT  0x06u       /* As defined by Verilog Implementation */
    /* Status Register Interrupt Enable Bit Locations - UDB Status Interrupt Mask match Status Bit Locations*/
    #define GlobalCounter_STATUS_CMP_INT_EN_MASK_SHIFT       GlobalCounter_STATUS_CMP_SHIFT       
    #define GlobalCounter_STATUS_ZERO_INT_EN_MASK_SHIFT      GlobalCounter_STATUS_ZERO_SHIFT      
    #define GlobalCounter_STATUS_OVERFLOW_INT_EN_MASK_SHIFT  GlobalCounter_STATUS_OVERFLOW_SHIFT  
    #define GlobalCounter_STATUS_UNDERFLOW_INT_EN_MASK_SHIFT GlobalCounter_STATUS_UNDERFLOW_SHIFT 
    #define GlobalCounter_STATUS_CAPTURE_INT_EN_MASK_SHIFT   GlobalCounter_STATUS_CAPTURE_SHIFT   
    #define GlobalCounter_STATUS_FIFOFULL_INT_EN_MASK_SHIFT  GlobalCounter_STATUS_FIFOFULL_SHIFT  
    #define GlobalCounter_STATUS_FIFONEMP_INT_EN_MASK_SHIFT  GlobalCounter_STATUS_FIFONEMP_SHIFT  
    /* Status Register Bit Masks */                
    #define GlobalCounter_STATUS_CMP             ((uint8)((uint8)0x01u << GlobalCounter_STATUS_CMP_SHIFT))  
    #define GlobalCounter_STATUS_ZERO            ((uint8)((uint8)0x01u << GlobalCounter_STATUS_ZERO_SHIFT)) 
    #define GlobalCounter_STATUS_OVERFLOW        ((uint8)((uint8)0x01u << GlobalCounter_STATUS_OVERFLOW_SHIFT)) 
    #define GlobalCounter_STATUS_UNDERFLOW       ((uint8)((uint8)0x01u << GlobalCounter_STATUS_UNDERFLOW_SHIFT)) 
    #define GlobalCounter_STATUS_CAPTURE         ((uint8)((uint8)0x01u << GlobalCounter_STATUS_CAPTURE_SHIFT)) 
    #define GlobalCounter_STATUS_FIFOFULL        ((uint8)((uint8)0x01u << GlobalCounter_STATUS_FIFOFULL_SHIFT))
    #define GlobalCounter_STATUS_FIFONEMP        ((uint8)((uint8)0x01u << GlobalCounter_STATUS_FIFONEMP_SHIFT))
    /* Status Register Interrupt Bit Masks  - UDB Status Interrupt Mask match Status Bit Locations */
    #define GlobalCounter_STATUS_CMP_INT_EN_MASK            GlobalCounter_STATUS_CMP                    
    #define GlobalCounter_STATUS_ZERO_INT_EN_MASK           GlobalCounter_STATUS_ZERO            
    #define GlobalCounter_STATUS_OVERFLOW_INT_EN_MASK       GlobalCounter_STATUS_OVERFLOW        
    #define GlobalCounter_STATUS_UNDERFLOW_INT_EN_MASK      GlobalCounter_STATUS_UNDERFLOW       
    #define GlobalCounter_STATUS_CAPTURE_INT_EN_MASK        GlobalCounter_STATUS_CAPTURE         
    #define GlobalCounter_STATUS_FIFOFULL_INT_EN_MASK       GlobalCounter_STATUS_FIFOFULL        
    #define GlobalCounter_STATUS_FIFONEMP_INT_EN_MASK       GlobalCounter_STATUS_FIFONEMP         
    

    /* StatusI Interrupt Enable bit Location in the Auxilliary Control Register */
    #define GlobalCounter_STATUS_ACTL_INT_EN     0x10u /* As defined for the ACTL Register */
    
    /* Datapath Auxillary Control Register definitions */
    #define GlobalCounter_AUX_CTRL_FIFO0_CLR         0x01u   /* As defined by Register map */
    #define GlobalCounter_AUX_CTRL_FIFO1_CLR         0x02u   /* As defined by Register map */
    #define GlobalCounter_AUX_CTRL_FIFO0_LVL         0x04u   /* As defined by Register map */
    #define GlobalCounter_AUX_CTRL_FIFO1_LVL         0x08u   /* As defined by Register map */
    #define GlobalCounter_STATUS_ACTL_INT_EN_MASK    0x10u   /* As defined for the ACTL Register */
    
#endif /* GlobalCounter_UsingFixedFunction */

#endif  /* CY_COUNTER_GlobalCounter_H */


/* [] END OF FILE */

