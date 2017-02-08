/*******************************************************************************
* File Name: Dispenser.h
* Version 2.50
*
* Description:
*  Contains the function prototypes and constants available to the UART
*  user module.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_UART_Dispenser_H)
#define CY_UART_Dispenser_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"


/***************************************
* Conditional Compilation Parameters
***************************************/

#define Dispenser_RX_ENABLED                     (1u)
#define Dispenser_TX_ENABLED                     (1u)
#define Dispenser_HD_ENABLED                     (0u)
#define Dispenser_RX_INTERRUPT_ENABLED           (1u)
#define Dispenser_TX_INTERRUPT_ENABLED           (0u)
#define Dispenser_INTERNAL_CLOCK_USED            (0u)
#define Dispenser_RXHW_ADDRESS_ENABLED           (0u)
#define Dispenser_OVER_SAMPLE_COUNT              (8u)
#define Dispenser_PARITY_TYPE                    (1u)
#define Dispenser_PARITY_TYPE_SW                 (0u)
#define Dispenser_BREAK_DETECT                   (0u)
#define Dispenser_BREAK_BITS_TX                  (13u)
#define Dispenser_BREAK_BITS_RX                  (13u)
#define Dispenser_TXCLKGEN_DP                    (1u)
#define Dispenser_USE23POLLING                   (1u)
#define Dispenser_FLOW_CONTROL                   (0u)
#define Dispenser_CLK_FREQ                       (0u)
#define Dispenser_TX_BUFFER_SIZE                 (4u)
#define Dispenser_RX_BUFFER_SIZE                 (256u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(Dispenser_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define Dispenser_CONTROL_REG_REMOVED            (0u)
#else
    #define Dispenser_CONTROL_REG_REMOVED            (1u)
#endif /* End Dispenser_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct Dispenser_backupStruct_
{
    uint8 enableState;

    #if(Dispenser_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End Dispenser_CONTROL_REG_REMOVED */

} Dispenser_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void Dispenser_Start(void) ;
void Dispenser_Stop(void) ;
uint8 Dispenser_ReadControlRegister(void) ;
void Dispenser_WriteControlRegister(uint8 control) ;

void Dispenser_Init(void) ;
void Dispenser_Enable(void) ;
void Dispenser_SaveConfig(void) ;
void Dispenser_RestoreConfig(void) ;
void Dispenser_Sleep(void) ;
void Dispenser_Wakeup(void) ;

/* Only if RX is enabled */
#if( (Dispenser_RX_ENABLED) || (Dispenser_HD_ENABLED) )

    #if (Dispenser_RX_INTERRUPT_ENABLED)
        #define Dispenser_EnableRxInt()  CyIntEnable (Dispenser_RX_VECT_NUM)
        #define Dispenser_DisableRxInt() CyIntDisable(Dispenser_RX_VECT_NUM)
        CY_ISR_PROTO(Dispenser_RXISR);
    #endif /* Dispenser_RX_INTERRUPT_ENABLED */

    void Dispenser_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void Dispenser_SetRxAddress1(uint8 address) ;
    void Dispenser_SetRxAddress2(uint8 address) ;

    void  Dispenser_SetRxInterruptMode(uint8 intSrc) ;
    uint8 Dispenser_ReadRxData(void) ;
    uint8 Dispenser_ReadRxStatus(void) ;
    uint8 Dispenser_GetChar(void) ;
    uint16 Dispenser_GetByte(void) ;
    uint16 Dispenser_GetRxBufferSize(void)
                                                            ;
    void Dispenser_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define Dispenser_GetRxInterruptSource   Dispenser_ReadRxStatus

#endif /* End (Dispenser_RX_ENABLED) || (Dispenser_HD_ENABLED) */

/* Only if TX is enabled */
#if(Dispenser_TX_ENABLED || Dispenser_HD_ENABLED)

    #if(Dispenser_TX_INTERRUPT_ENABLED)
        #define Dispenser_EnableTxInt()  CyIntEnable (Dispenser_TX_VECT_NUM)
        #define Dispenser_DisableTxInt() CyIntDisable(Dispenser_TX_VECT_NUM)
        #define Dispenser_SetPendingTxInt() CyIntSetPending(Dispenser_TX_VECT_NUM)
        #define Dispenser_ClearPendingTxInt() CyIntClearPending(Dispenser_TX_VECT_NUM)
        CY_ISR_PROTO(Dispenser_TXISR);
    #endif /* Dispenser_TX_INTERRUPT_ENABLED */

    void Dispenser_SetTxInterruptMode(uint8 intSrc) ;
    void Dispenser_WriteTxData(uint8 txDataByte) ;
    uint8 Dispenser_ReadTxStatus(void) ;
    void Dispenser_PutChar(uint8 txDataByte) ;
    void Dispenser_PutString(const char8 string[]) ;
    void Dispenser_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void Dispenser_PutCRLF(uint8 txDataByte) ;
    void Dispenser_ClearTxBuffer(void) ;
    void Dispenser_SetTxAddressMode(uint8 addressMode) ;
    void Dispenser_SendBreak(uint8 retMode) ;
    uint8 Dispenser_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define Dispenser_PutStringConst         Dispenser_PutString
    #define Dispenser_PutArrayConst          Dispenser_PutArray
    #define Dispenser_GetTxInterruptSource   Dispenser_ReadTxStatus

#endif /* End Dispenser_TX_ENABLED || Dispenser_HD_ENABLED */

#if(Dispenser_HD_ENABLED)
    void Dispenser_LoadRxConfig(void) ;
    void Dispenser_LoadTxConfig(void) ;
#endif /* End Dispenser_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Dispenser) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    Dispenser_CyBtldrCommStart(void) CYSMALL ;
    void    Dispenser_CyBtldrCommStop(void) CYSMALL ;
    void    Dispenser_CyBtldrCommReset(void) CYSMALL ;
    cystatus Dispenser_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus Dispenser_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Dispenser)
        #define CyBtldrCommStart    Dispenser_CyBtldrCommStart
        #define CyBtldrCommStop     Dispenser_CyBtldrCommStop
        #define CyBtldrCommReset    Dispenser_CyBtldrCommReset
        #define CyBtldrCommWrite    Dispenser_CyBtldrCommWrite
        #define CyBtldrCommRead     Dispenser_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Dispenser) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define Dispenser_BYTE2BYTE_TIME_OUT (25u)
    #define Dispenser_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define Dispenser_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define Dispenser_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define Dispenser_SET_SPACE      (0x00u)
#define Dispenser_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (Dispenser_TX_ENABLED) || (Dispenser_HD_ENABLED) )
    #if(Dispenser_TX_INTERRUPT_ENABLED)
        #define Dispenser_TX_VECT_NUM            (uint8)Dispenser_TXInternalInterrupt__INTC_NUMBER
        #define Dispenser_TX_PRIOR_NUM           (uint8)Dispenser_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Dispenser_TX_INTERRUPT_ENABLED */

    #define Dispenser_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define Dispenser_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define Dispenser_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(Dispenser_TX_ENABLED)
        #define Dispenser_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (Dispenser_HD_ENABLED) */
        #define Dispenser_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (Dispenser_TX_ENABLED) */

    #define Dispenser_TX_STS_COMPLETE            (uint8)(0x01u << Dispenser_TX_STS_COMPLETE_SHIFT)
    #define Dispenser_TX_STS_FIFO_EMPTY          (uint8)(0x01u << Dispenser_TX_STS_FIFO_EMPTY_SHIFT)
    #define Dispenser_TX_STS_FIFO_FULL           (uint8)(0x01u << Dispenser_TX_STS_FIFO_FULL_SHIFT)
    #define Dispenser_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << Dispenser_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (Dispenser_TX_ENABLED) || (Dispenser_HD_ENABLED)*/

#if( (Dispenser_RX_ENABLED) || (Dispenser_HD_ENABLED) )
    #if(Dispenser_RX_INTERRUPT_ENABLED)
        #define Dispenser_RX_VECT_NUM            (uint8)Dispenser_RXInternalInterrupt__INTC_NUMBER
        #define Dispenser_RX_PRIOR_NUM           (uint8)Dispenser_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Dispenser_RX_INTERRUPT_ENABLED */
    #define Dispenser_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define Dispenser_RX_STS_BREAK_SHIFT             (0x01u)
    #define Dispenser_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define Dispenser_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define Dispenser_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define Dispenser_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define Dispenser_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define Dispenser_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define Dispenser_RX_STS_MRKSPC           (uint8)(0x01u << Dispenser_RX_STS_MRKSPC_SHIFT)
    #define Dispenser_RX_STS_BREAK            (uint8)(0x01u << Dispenser_RX_STS_BREAK_SHIFT)
    #define Dispenser_RX_STS_PAR_ERROR        (uint8)(0x01u << Dispenser_RX_STS_PAR_ERROR_SHIFT)
    #define Dispenser_RX_STS_STOP_ERROR       (uint8)(0x01u << Dispenser_RX_STS_STOP_ERROR_SHIFT)
    #define Dispenser_RX_STS_OVERRUN          (uint8)(0x01u << Dispenser_RX_STS_OVERRUN_SHIFT)
    #define Dispenser_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << Dispenser_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define Dispenser_RX_STS_ADDR_MATCH       (uint8)(0x01u << Dispenser_RX_STS_ADDR_MATCH_SHIFT)
    #define Dispenser_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << Dispenser_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define Dispenser_RX_HW_MASK                     (0x7Fu)
#endif /* End (Dispenser_RX_ENABLED) || (Dispenser_HD_ENABLED) */

/* Control Register definitions */
#define Dispenser_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define Dispenser_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define Dispenser_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define Dispenser_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define Dispenser_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define Dispenser_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define Dispenser_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define Dispenser_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define Dispenser_CTRL_HD_SEND               (uint8)(0x01u << Dispenser_CTRL_HD_SEND_SHIFT)
#define Dispenser_CTRL_HD_SEND_BREAK         (uint8)(0x01u << Dispenser_CTRL_HD_SEND_BREAK_SHIFT)
#define Dispenser_CTRL_MARK                  (uint8)(0x01u << Dispenser_CTRL_MARK_SHIFT)
#define Dispenser_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << Dispenser_CTRL_PARITY_TYPE0_SHIFT)
#define Dispenser_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << Dispenser_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define Dispenser_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define Dispenser_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define Dispenser_SEND_BREAK                         (0x00u)
#define Dispenser_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define Dispenser_REINIT                             (0x02u)
#define Dispenser_SEND_WAIT_REINIT                   (0x03u)

#define Dispenser_OVER_SAMPLE_8                      (8u)
#define Dispenser_OVER_SAMPLE_16                     (16u)

#define Dispenser_BIT_CENTER                         (Dispenser_OVER_SAMPLE_COUNT - 2u)

#define Dispenser_FIFO_LENGTH                        (4u)
#define Dispenser_NUMBER_OF_START_BIT                (1u)
#define Dispenser_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define Dispenser_TXBITCTR_BREAKBITS8X   ((Dispenser_BREAK_BITS_TX * Dispenser_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define Dispenser_TXBITCTR_BREAKBITS ((Dispenser_BREAK_BITS_TX * Dispenser_OVER_SAMPLE_COUNT) - 1u)

#define Dispenser_HALF_BIT_COUNT   \
                            (((Dispenser_OVER_SAMPLE_COUNT / 2u) + (Dispenser_USE23POLLING * 1u)) - 2u)
#if (Dispenser_OVER_SAMPLE_COUNT == Dispenser_OVER_SAMPLE_8)
    #define Dispenser_HD_TXBITCTR_INIT   (((Dispenser_BREAK_BITS_TX + \
                            Dispenser_NUMBER_OF_START_BIT) * Dispenser_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define Dispenser_RXBITCTR_INIT  ((((Dispenser_BREAK_BITS_RX + Dispenser_NUMBER_OF_START_BIT) \
                            * Dispenser_OVER_SAMPLE_COUNT) + Dispenser_HALF_BIT_COUNT) - 1u)

#else /* Dispenser_OVER_SAMPLE_COUNT == Dispenser_OVER_SAMPLE_16 */
    #define Dispenser_HD_TXBITCTR_INIT   ((8u * Dispenser_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define Dispenser_RXBITCTR_INIT      (((7u * Dispenser_OVER_SAMPLE_COUNT) - 1u) + \
                                                      Dispenser_HALF_BIT_COUNT)
#endif /* End Dispenser_OVER_SAMPLE_COUNT */

#define Dispenser_HD_RXBITCTR_INIT                   Dispenser_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 Dispenser_initVar;
#if (Dispenser_TX_INTERRUPT_ENABLED && Dispenser_TX_ENABLED)
    extern volatile uint8 Dispenser_txBuffer[Dispenser_TX_BUFFER_SIZE];
    extern volatile uint8 Dispenser_txBufferRead;
    extern uint8 Dispenser_txBufferWrite;
#endif /* (Dispenser_TX_INTERRUPT_ENABLED && Dispenser_TX_ENABLED) */
#if (Dispenser_RX_INTERRUPT_ENABLED && (Dispenser_RX_ENABLED || Dispenser_HD_ENABLED))
    extern uint8 Dispenser_errorStatus;
    extern volatile uint8 Dispenser_rxBuffer[Dispenser_RX_BUFFER_SIZE];
    extern volatile uint16 Dispenser_rxBufferRead;
    extern volatile uint16 Dispenser_rxBufferWrite;
    extern volatile uint8 Dispenser_rxBufferLoopDetect;
    extern volatile uint8 Dispenser_rxBufferOverflow;
    #if (Dispenser_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 Dispenser_rxAddressMode;
        extern volatile uint8 Dispenser_rxAddressDetected;
    #endif /* (Dispenser_RXHW_ADDRESS_ENABLED) */
#endif /* (Dispenser_RX_INTERRUPT_ENABLED && (Dispenser_RX_ENABLED || Dispenser_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define Dispenser__B_UART__AM_SW_BYTE_BYTE 1
#define Dispenser__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define Dispenser__B_UART__AM_HW_BYTE_BY_BYTE 3
#define Dispenser__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define Dispenser__B_UART__AM_NONE 0

#define Dispenser__B_UART__NONE_REVB 0
#define Dispenser__B_UART__EVEN_REVB 1
#define Dispenser__B_UART__ODD_REVB 2
#define Dispenser__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define Dispenser_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define Dispenser_NUMBER_OF_STOP_BITS    (1u)

#if (Dispenser_RXHW_ADDRESS_ENABLED)
    #define Dispenser_RX_ADDRESS_MODE    (0u)
    #define Dispenser_RX_HW_ADDRESS1     (0u)
    #define Dispenser_RX_HW_ADDRESS2     (0u)
#endif /* (Dispenser_RXHW_ADDRESS_ENABLED) */

#define Dispenser_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << Dispenser_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << Dispenser_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << Dispenser_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << Dispenser_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << Dispenser_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << Dispenser_RX_STS_BREAK_SHIFT) \
                                        | (0 << Dispenser_RX_STS_OVERRUN_SHIFT))

#define Dispenser_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << Dispenser_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << Dispenser_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << Dispenser_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << Dispenser_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef Dispenser_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Dispenser_CONTROL_REG \
                            (* (reg8 *) Dispenser_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define Dispenser_CONTROL_PTR \
                            (  (reg8 *) Dispenser_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End Dispenser_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Dispenser_TX_ENABLED)
    #define Dispenser_TXDATA_REG          (* (reg8 *) Dispenser_BUART_sTX_TxShifter_u0__F0_REG)
    #define Dispenser_TXDATA_PTR          (  (reg8 *) Dispenser_BUART_sTX_TxShifter_u0__F0_REG)
    #define Dispenser_TXDATA_AUX_CTL_REG  (* (reg8 *) Dispenser_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Dispenser_TXDATA_AUX_CTL_PTR  (  (reg8 *) Dispenser_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Dispenser_TXSTATUS_REG        (* (reg8 *) Dispenser_BUART_sTX_TxSts__STATUS_REG)
    #define Dispenser_TXSTATUS_PTR        (  (reg8 *) Dispenser_BUART_sTX_TxSts__STATUS_REG)
    #define Dispenser_TXSTATUS_MASK_REG   (* (reg8 *) Dispenser_BUART_sTX_TxSts__MASK_REG)
    #define Dispenser_TXSTATUS_MASK_PTR   (  (reg8 *) Dispenser_BUART_sTX_TxSts__MASK_REG)
    #define Dispenser_TXSTATUS_ACTL_REG   (* (reg8 *) Dispenser_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define Dispenser_TXSTATUS_ACTL_PTR   (  (reg8 *) Dispenser_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(Dispenser_TXCLKGEN_DP)
        #define Dispenser_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) Dispenser_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Dispenser_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) Dispenser_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Dispenser_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) Dispenser_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define Dispenser_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) Dispenser_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define Dispenser_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) Dispenser_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Dispenser_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) Dispenser_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Dispenser_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Dispenser_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Dispenser_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Dispenser_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Dispenser_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) Dispenser_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define Dispenser_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) Dispenser_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* Dispenser_TXCLKGEN_DP */

#endif /* End Dispenser_TX_ENABLED */

#if(Dispenser_HD_ENABLED)

    #define Dispenser_TXDATA_REG             (* (reg8 *) Dispenser_BUART_sRX_RxShifter_u0__F1_REG )
    #define Dispenser_TXDATA_PTR             (  (reg8 *) Dispenser_BUART_sRX_RxShifter_u0__F1_REG )
    #define Dispenser_TXDATA_AUX_CTL_REG     (* (reg8 *) Dispenser_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define Dispenser_TXDATA_AUX_CTL_PTR     (  (reg8 *) Dispenser_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Dispenser_TXSTATUS_REG           (* (reg8 *) Dispenser_BUART_sRX_RxSts__STATUS_REG )
    #define Dispenser_TXSTATUS_PTR           (  (reg8 *) Dispenser_BUART_sRX_RxSts__STATUS_REG )
    #define Dispenser_TXSTATUS_MASK_REG      (* (reg8 *) Dispenser_BUART_sRX_RxSts__MASK_REG )
    #define Dispenser_TXSTATUS_MASK_PTR      (  (reg8 *) Dispenser_BUART_sRX_RxSts__MASK_REG )
    #define Dispenser_TXSTATUS_ACTL_REG      (* (reg8 *) Dispenser_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Dispenser_TXSTATUS_ACTL_PTR      (  (reg8 *) Dispenser_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End Dispenser_HD_ENABLED */

#if( (Dispenser_RX_ENABLED) || (Dispenser_HD_ENABLED) )
    #define Dispenser_RXDATA_REG             (* (reg8 *) Dispenser_BUART_sRX_RxShifter_u0__F0_REG )
    #define Dispenser_RXDATA_PTR             (  (reg8 *) Dispenser_BUART_sRX_RxShifter_u0__F0_REG )
    #define Dispenser_RXADDRESS1_REG         (* (reg8 *) Dispenser_BUART_sRX_RxShifter_u0__D0_REG )
    #define Dispenser_RXADDRESS1_PTR         (  (reg8 *) Dispenser_BUART_sRX_RxShifter_u0__D0_REG )
    #define Dispenser_RXADDRESS2_REG         (* (reg8 *) Dispenser_BUART_sRX_RxShifter_u0__D1_REG )
    #define Dispenser_RXADDRESS2_PTR         (  (reg8 *) Dispenser_BUART_sRX_RxShifter_u0__D1_REG )
    #define Dispenser_RXDATA_AUX_CTL_REG     (* (reg8 *) Dispenser_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Dispenser_RXBITCTR_PERIOD_REG    (* (reg8 *) Dispenser_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Dispenser_RXBITCTR_PERIOD_PTR    (  (reg8 *) Dispenser_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Dispenser_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Dispenser_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Dispenser_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Dispenser_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Dispenser_RXBITCTR_COUNTER_REG   (* (reg8 *) Dispenser_BUART_sRX_RxBitCounter__COUNT_REG )
    #define Dispenser_RXBITCTR_COUNTER_PTR   (  (reg8 *) Dispenser_BUART_sRX_RxBitCounter__COUNT_REG )

    #define Dispenser_RXSTATUS_REG           (* (reg8 *) Dispenser_BUART_sRX_RxSts__STATUS_REG )
    #define Dispenser_RXSTATUS_PTR           (  (reg8 *) Dispenser_BUART_sRX_RxSts__STATUS_REG )
    #define Dispenser_RXSTATUS_MASK_REG      (* (reg8 *) Dispenser_BUART_sRX_RxSts__MASK_REG )
    #define Dispenser_RXSTATUS_MASK_PTR      (  (reg8 *) Dispenser_BUART_sRX_RxSts__MASK_REG )
    #define Dispenser_RXSTATUS_ACTL_REG      (* (reg8 *) Dispenser_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Dispenser_RXSTATUS_ACTL_PTR      (  (reg8 *) Dispenser_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (Dispenser_RX_ENABLED) || (Dispenser_HD_ENABLED) */

#if(Dispenser_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define Dispenser_INTCLOCK_CLKEN_REG     (* (reg8 *) Dispenser_IntClock__PM_ACT_CFG)
    #define Dispenser_INTCLOCK_CLKEN_PTR     (  (reg8 *) Dispenser_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define Dispenser_INTCLOCK_CLKEN_MASK    Dispenser_IntClock__PM_ACT_MSK
#endif /* End Dispenser_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(Dispenser_TX_ENABLED)
    #define Dispenser_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End Dispenser_TX_ENABLED */

#if(Dispenser_HD_ENABLED)
    #define Dispenser_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End Dispenser_HD_ENABLED */

#if( (Dispenser_RX_ENABLED) || (Dispenser_HD_ENABLED) )
    #define Dispenser_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (Dispenser_RX_ENABLED) || (Dispenser_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define Dispenser_WAIT_1_MS      Dispenser_BL_CHK_DELAY_MS   

#define Dispenser_TXBUFFERSIZE   Dispenser_TX_BUFFER_SIZE
#define Dispenser_RXBUFFERSIZE   Dispenser_RX_BUFFER_SIZE

#if (Dispenser_RXHW_ADDRESS_ENABLED)
    #define Dispenser_RXADDRESSMODE  Dispenser_RX_ADDRESS_MODE
    #define Dispenser_RXHWADDRESS1   Dispenser_RX_HW_ADDRESS1
    #define Dispenser_RXHWADDRESS2   Dispenser_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define Dispenser_RXAddressMode  Dispenser_RXADDRESSMODE
#endif /* (Dispenser_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define Dispenser_initvar                    Dispenser_initVar

#define Dispenser_RX_Enabled                 Dispenser_RX_ENABLED
#define Dispenser_TX_Enabled                 Dispenser_TX_ENABLED
#define Dispenser_HD_Enabled                 Dispenser_HD_ENABLED
#define Dispenser_RX_IntInterruptEnabled     Dispenser_RX_INTERRUPT_ENABLED
#define Dispenser_TX_IntInterruptEnabled     Dispenser_TX_INTERRUPT_ENABLED
#define Dispenser_InternalClockUsed          Dispenser_INTERNAL_CLOCK_USED
#define Dispenser_RXHW_Address_Enabled       Dispenser_RXHW_ADDRESS_ENABLED
#define Dispenser_OverSampleCount            Dispenser_OVER_SAMPLE_COUNT
#define Dispenser_ParityType                 Dispenser_PARITY_TYPE

#if( Dispenser_TX_ENABLED && (Dispenser_TXBUFFERSIZE > Dispenser_FIFO_LENGTH))
    #define Dispenser_TXBUFFER               Dispenser_txBuffer
    #define Dispenser_TXBUFFERREAD           Dispenser_txBufferRead
    #define Dispenser_TXBUFFERWRITE          Dispenser_txBufferWrite
#endif /* End Dispenser_TX_ENABLED */
#if( ( Dispenser_RX_ENABLED || Dispenser_HD_ENABLED ) && \
     (Dispenser_RXBUFFERSIZE > Dispenser_FIFO_LENGTH) )
    #define Dispenser_RXBUFFER               Dispenser_rxBuffer
    #define Dispenser_RXBUFFERREAD           Dispenser_rxBufferRead
    #define Dispenser_RXBUFFERWRITE          Dispenser_rxBufferWrite
    #define Dispenser_RXBUFFERLOOPDETECT     Dispenser_rxBufferLoopDetect
    #define Dispenser_RXBUFFER_OVERFLOW      Dispenser_rxBufferOverflow
#endif /* End Dispenser_RX_ENABLED */

#ifdef Dispenser_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Dispenser_CONTROL                Dispenser_CONTROL_REG
#endif /* End Dispenser_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Dispenser_TX_ENABLED)
    #define Dispenser_TXDATA                 Dispenser_TXDATA_REG
    #define Dispenser_TXSTATUS               Dispenser_TXSTATUS_REG
    #define Dispenser_TXSTATUS_MASK          Dispenser_TXSTATUS_MASK_REG
    #define Dispenser_TXSTATUS_ACTL          Dispenser_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(Dispenser_TXCLKGEN_DP)
        #define Dispenser_TXBITCLKGEN_CTR        Dispenser_TXBITCLKGEN_CTR_REG
        #define Dispenser_TXBITCLKTX_COMPLETE    Dispenser_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define Dispenser_TXBITCTR_PERIOD        Dispenser_TXBITCTR_PERIOD_REG
        #define Dispenser_TXBITCTR_CONTROL       Dispenser_TXBITCTR_CONTROL_REG
        #define Dispenser_TXBITCTR_COUNTER       Dispenser_TXBITCTR_COUNTER_REG
    #endif /* Dispenser_TXCLKGEN_DP */
#endif /* End Dispenser_TX_ENABLED */

#if(Dispenser_HD_ENABLED)
    #define Dispenser_TXDATA                 Dispenser_TXDATA_REG
    #define Dispenser_TXSTATUS               Dispenser_TXSTATUS_REG
    #define Dispenser_TXSTATUS_MASK          Dispenser_TXSTATUS_MASK_REG
    #define Dispenser_TXSTATUS_ACTL          Dispenser_TXSTATUS_ACTL_REG
#endif /* End Dispenser_HD_ENABLED */

#if( (Dispenser_RX_ENABLED) || (Dispenser_HD_ENABLED) )
    #define Dispenser_RXDATA                 Dispenser_RXDATA_REG
    #define Dispenser_RXADDRESS1             Dispenser_RXADDRESS1_REG
    #define Dispenser_RXADDRESS2             Dispenser_RXADDRESS2_REG
    #define Dispenser_RXBITCTR_PERIOD        Dispenser_RXBITCTR_PERIOD_REG
    #define Dispenser_RXBITCTR_CONTROL       Dispenser_RXBITCTR_CONTROL_REG
    #define Dispenser_RXBITCTR_COUNTER       Dispenser_RXBITCTR_COUNTER_REG
    #define Dispenser_RXSTATUS               Dispenser_RXSTATUS_REG
    #define Dispenser_RXSTATUS_MASK          Dispenser_RXSTATUS_MASK_REG
    #define Dispenser_RXSTATUS_ACTL          Dispenser_RXSTATUS_ACTL_REG
#endif /* End  (Dispenser_RX_ENABLED) || (Dispenser_HD_ENABLED) */

#if(Dispenser_INTERNAL_CLOCK_USED)
    #define Dispenser_INTCLOCK_CLKEN         Dispenser_INTCLOCK_CLKEN_REG
#endif /* End Dispenser_INTERNAL_CLOCK_USED */

#define Dispenser_WAIT_FOR_COMLETE_REINIT    Dispenser_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_Dispenser_H */


/* [] END OF FILE */
