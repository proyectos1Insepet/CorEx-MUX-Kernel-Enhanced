/*******************************************************************************
* File Name: Printer.h
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


#if !defined(CY_UART_Printer_H)
#define CY_UART_Printer_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"


/***************************************
* Conditional Compilation Parameters
***************************************/

#define Printer_RX_ENABLED                     (1u)
#define Printer_TX_ENABLED                     (1u)
#define Printer_HD_ENABLED                     (0u)
#define Printer_RX_INTERRUPT_ENABLED           (0u)
#define Printer_TX_INTERRUPT_ENABLED           (0u)
#define Printer_INTERNAL_CLOCK_USED            (0u)
#define Printer_RXHW_ADDRESS_ENABLED           (0u)
#define Printer_OVER_SAMPLE_COUNT              (8u)
#define Printer_PARITY_TYPE                    (0u)
#define Printer_PARITY_TYPE_SW                 (0u)
#define Printer_BREAK_DETECT                   (0u)
#define Printer_BREAK_BITS_TX                  (13u)
#define Printer_BREAK_BITS_RX                  (13u)
#define Printer_TXCLKGEN_DP                    (1u)
#define Printer_USE23POLLING                   (1u)
#define Printer_FLOW_CONTROL                   (0u)
#define Printer_CLK_FREQ                       (0u)
#define Printer_TX_BUFFER_SIZE                 (4u)
#define Printer_RX_BUFFER_SIZE                 (4u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(Printer_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define Printer_CONTROL_REG_REMOVED            (0u)
#else
    #define Printer_CONTROL_REG_REMOVED            (1u)
#endif /* End Printer_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct Printer_backupStruct_
{
    uint8 enableState;

    #if(Printer_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End Printer_CONTROL_REG_REMOVED */

} Printer_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void Printer_Start(void) ;
void Printer_Stop(void) ;
uint8 Printer_ReadControlRegister(void) ;
void Printer_WriteControlRegister(uint8 control) ;

void Printer_Init(void) ;
void Printer_Enable(void) ;
void Printer_SaveConfig(void) ;
void Printer_RestoreConfig(void) ;
void Printer_Sleep(void) ;
void Printer_Wakeup(void) ;

/* Only if RX is enabled */
#if( (Printer_RX_ENABLED) || (Printer_HD_ENABLED) )

    #if (Printer_RX_INTERRUPT_ENABLED)
        #define Printer_EnableRxInt()  CyIntEnable (Printer_RX_VECT_NUM)
        #define Printer_DisableRxInt() CyIntDisable(Printer_RX_VECT_NUM)
        CY_ISR_PROTO(Printer_RXISR);
    #endif /* Printer_RX_INTERRUPT_ENABLED */

    void Printer_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void Printer_SetRxAddress1(uint8 address) ;
    void Printer_SetRxAddress2(uint8 address) ;

    void  Printer_SetRxInterruptMode(uint8 intSrc) ;
    uint8 Printer_ReadRxData(void) ;
    uint8 Printer_ReadRxStatus(void) ;
    uint8 Printer_GetChar(void) ;
    uint16 Printer_GetByte(void) ;
    uint8 Printer_GetRxBufferSize(void)
                                                            ;
    void Printer_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define Printer_GetRxInterruptSource   Printer_ReadRxStatus

#endif /* End (Printer_RX_ENABLED) || (Printer_HD_ENABLED) */

/* Only if TX is enabled */
#if(Printer_TX_ENABLED || Printer_HD_ENABLED)

    #if(Printer_TX_INTERRUPT_ENABLED)
        #define Printer_EnableTxInt()  CyIntEnable (Printer_TX_VECT_NUM)
        #define Printer_DisableTxInt() CyIntDisable(Printer_TX_VECT_NUM)
        #define Printer_SetPendingTxInt() CyIntSetPending(Printer_TX_VECT_NUM)
        #define Printer_ClearPendingTxInt() CyIntClearPending(Printer_TX_VECT_NUM)
        CY_ISR_PROTO(Printer_TXISR);
    #endif /* Printer_TX_INTERRUPT_ENABLED */

    void Printer_SetTxInterruptMode(uint8 intSrc) ;
    void Printer_WriteTxData(uint8 txDataByte) ;
    uint8 Printer_ReadTxStatus(void) ;
    void Printer_PutChar(uint8 txDataByte) ;
    void Printer_PutString(const char8 string[]) ;
    void Printer_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void Printer_PutCRLF(uint8 txDataByte) ;
    void Printer_ClearTxBuffer(void) ;
    void Printer_SetTxAddressMode(uint8 addressMode) ;
    void Printer_SendBreak(uint8 retMode) ;
    uint8 Printer_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define Printer_PutStringConst         Printer_PutString
    #define Printer_PutArrayConst          Printer_PutArray
    #define Printer_GetTxInterruptSource   Printer_ReadTxStatus

#endif /* End Printer_TX_ENABLED || Printer_HD_ENABLED */

#if(Printer_HD_ENABLED)
    void Printer_LoadRxConfig(void) ;
    void Printer_LoadTxConfig(void) ;
#endif /* End Printer_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Printer) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    Printer_CyBtldrCommStart(void) CYSMALL ;
    void    Printer_CyBtldrCommStop(void) CYSMALL ;
    void    Printer_CyBtldrCommReset(void) CYSMALL ;
    cystatus Printer_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus Printer_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Printer)
        #define CyBtldrCommStart    Printer_CyBtldrCommStart
        #define CyBtldrCommStop     Printer_CyBtldrCommStop
        #define CyBtldrCommReset    Printer_CyBtldrCommReset
        #define CyBtldrCommWrite    Printer_CyBtldrCommWrite
        #define CyBtldrCommRead     Printer_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Printer) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define Printer_BYTE2BYTE_TIME_OUT (25u)
    #define Printer_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define Printer_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define Printer_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define Printer_SET_SPACE      (0x00u)
#define Printer_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (Printer_TX_ENABLED) || (Printer_HD_ENABLED) )
    #if(Printer_TX_INTERRUPT_ENABLED)
        #define Printer_TX_VECT_NUM            (uint8)Printer_TXInternalInterrupt__INTC_NUMBER
        #define Printer_TX_PRIOR_NUM           (uint8)Printer_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Printer_TX_INTERRUPT_ENABLED */

    #define Printer_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define Printer_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define Printer_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(Printer_TX_ENABLED)
        #define Printer_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (Printer_HD_ENABLED) */
        #define Printer_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (Printer_TX_ENABLED) */

    #define Printer_TX_STS_COMPLETE            (uint8)(0x01u << Printer_TX_STS_COMPLETE_SHIFT)
    #define Printer_TX_STS_FIFO_EMPTY          (uint8)(0x01u << Printer_TX_STS_FIFO_EMPTY_SHIFT)
    #define Printer_TX_STS_FIFO_FULL           (uint8)(0x01u << Printer_TX_STS_FIFO_FULL_SHIFT)
    #define Printer_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << Printer_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (Printer_TX_ENABLED) || (Printer_HD_ENABLED)*/

#if( (Printer_RX_ENABLED) || (Printer_HD_ENABLED) )
    #if(Printer_RX_INTERRUPT_ENABLED)
        #define Printer_RX_VECT_NUM            (uint8)Printer_RXInternalInterrupt__INTC_NUMBER
        #define Printer_RX_PRIOR_NUM           (uint8)Printer_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Printer_RX_INTERRUPT_ENABLED */
    #define Printer_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define Printer_RX_STS_BREAK_SHIFT             (0x01u)
    #define Printer_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define Printer_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define Printer_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define Printer_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define Printer_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define Printer_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define Printer_RX_STS_MRKSPC           (uint8)(0x01u << Printer_RX_STS_MRKSPC_SHIFT)
    #define Printer_RX_STS_BREAK            (uint8)(0x01u << Printer_RX_STS_BREAK_SHIFT)
    #define Printer_RX_STS_PAR_ERROR        (uint8)(0x01u << Printer_RX_STS_PAR_ERROR_SHIFT)
    #define Printer_RX_STS_STOP_ERROR       (uint8)(0x01u << Printer_RX_STS_STOP_ERROR_SHIFT)
    #define Printer_RX_STS_OVERRUN          (uint8)(0x01u << Printer_RX_STS_OVERRUN_SHIFT)
    #define Printer_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << Printer_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define Printer_RX_STS_ADDR_MATCH       (uint8)(0x01u << Printer_RX_STS_ADDR_MATCH_SHIFT)
    #define Printer_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << Printer_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define Printer_RX_HW_MASK                     (0x7Fu)
#endif /* End (Printer_RX_ENABLED) || (Printer_HD_ENABLED) */

/* Control Register definitions */
#define Printer_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define Printer_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define Printer_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define Printer_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define Printer_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define Printer_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define Printer_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define Printer_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define Printer_CTRL_HD_SEND               (uint8)(0x01u << Printer_CTRL_HD_SEND_SHIFT)
#define Printer_CTRL_HD_SEND_BREAK         (uint8)(0x01u << Printer_CTRL_HD_SEND_BREAK_SHIFT)
#define Printer_CTRL_MARK                  (uint8)(0x01u << Printer_CTRL_MARK_SHIFT)
#define Printer_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << Printer_CTRL_PARITY_TYPE0_SHIFT)
#define Printer_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << Printer_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define Printer_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define Printer_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define Printer_SEND_BREAK                         (0x00u)
#define Printer_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define Printer_REINIT                             (0x02u)
#define Printer_SEND_WAIT_REINIT                   (0x03u)

#define Printer_OVER_SAMPLE_8                      (8u)
#define Printer_OVER_SAMPLE_16                     (16u)

#define Printer_BIT_CENTER                         (Printer_OVER_SAMPLE_COUNT - 2u)

#define Printer_FIFO_LENGTH                        (4u)
#define Printer_NUMBER_OF_START_BIT                (1u)
#define Printer_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define Printer_TXBITCTR_BREAKBITS8X   ((Printer_BREAK_BITS_TX * Printer_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define Printer_TXBITCTR_BREAKBITS ((Printer_BREAK_BITS_TX * Printer_OVER_SAMPLE_COUNT) - 1u)

#define Printer_HALF_BIT_COUNT   \
                            (((Printer_OVER_SAMPLE_COUNT / 2u) + (Printer_USE23POLLING * 1u)) - 2u)
#if (Printer_OVER_SAMPLE_COUNT == Printer_OVER_SAMPLE_8)
    #define Printer_HD_TXBITCTR_INIT   (((Printer_BREAK_BITS_TX + \
                            Printer_NUMBER_OF_START_BIT) * Printer_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define Printer_RXBITCTR_INIT  ((((Printer_BREAK_BITS_RX + Printer_NUMBER_OF_START_BIT) \
                            * Printer_OVER_SAMPLE_COUNT) + Printer_HALF_BIT_COUNT) - 1u)

#else /* Printer_OVER_SAMPLE_COUNT == Printer_OVER_SAMPLE_16 */
    #define Printer_HD_TXBITCTR_INIT   ((8u * Printer_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define Printer_RXBITCTR_INIT      (((7u * Printer_OVER_SAMPLE_COUNT) - 1u) + \
                                                      Printer_HALF_BIT_COUNT)
#endif /* End Printer_OVER_SAMPLE_COUNT */

#define Printer_HD_RXBITCTR_INIT                   Printer_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 Printer_initVar;
#if (Printer_TX_INTERRUPT_ENABLED && Printer_TX_ENABLED)
    extern volatile uint8 Printer_txBuffer[Printer_TX_BUFFER_SIZE];
    extern volatile uint8 Printer_txBufferRead;
    extern uint8 Printer_txBufferWrite;
#endif /* (Printer_TX_INTERRUPT_ENABLED && Printer_TX_ENABLED) */
#if (Printer_RX_INTERRUPT_ENABLED && (Printer_RX_ENABLED || Printer_HD_ENABLED))
    extern uint8 Printer_errorStatus;
    extern volatile uint8 Printer_rxBuffer[Printer_RX_BUFFER_SIZE];
    extern volatile uint8 Printer_rxBufferRead;
    extern volatile uint8 Printer_rxBufferWrite;
    extern volatile uint8 Printer_rxBufferLoopDetect;
    extern volatile uint8 Printer_rxBufferOverflow;
    #if (Printer_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 Printer_rxAddressMode;
        extern volatile uint8 Printer_rxAddressDetected;
    #endif /* (Printer_RXHW_ADDRESS_ENABLED) */
#endif /* (Printer_RX_INTERRUPT_ENABLED && (Printer_RX_ENABLED || Printer_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define Printer__B_UART__AM_SW_BYTE_BYTE 1
#define Printer__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define Printer__B_UART__AM_HW_BYTE_BY_BYTE 3
#define Printer__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define Printer__B_UART__AM_NONE 0

#define Printer__B_UART__NONE_REVB 0
#define Printer__B_UART__EVEN_REVB 1
#define Printer__B_UART__ODD_REVB 2
#define Printer__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define Printer_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define Printer_NUMBER_OF_STOP_BITS    (1u)

#if (Printer_RXHW_ADDRESS_ENABLED)
    #define Printer_RX_ADDRESS_MODE    (0u)
    #define Printer_RX_HW_ADDRESS1     (0u)
    #define Printer_RX_HW_ADDRESS2     (0u)
#endif /* (Printer_RXHW_ADDRESS_ENABLED) */

#define Printer_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << Printer_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << Printer_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << Printer_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << Printer_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << Printer_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << Printer_RX_STS_BREAK_SHIFT) \
                                        | (0 << Printer_RX_STS_OVERRUN_SHIFT))

#define Printer_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << Printer_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << Printer_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << Printer_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << Printer_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef Printer_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Printer_CONTROL_REG \
                            (* (reg8 *) Printer_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define Printer_CONTROL_PTR \
                            (  (reg8 *) Printer_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End Printer_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Printer_TX_ENABLED)
    #define Printer_TXDATA_REG          (* (reg8 *) Printer_BUART_sTX_TxShifter_u0__F0_REG)
    #define Printer_TXDATA_PTR          (  (reg8 *) Printer_BUART_sTX_TxShifter_u0__F0_REG)
    #define Printer_TXDATA_AUX_CTL_REG  (* (reg8 *) Printer_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Printer_TXDATA_AUX_CTL_PTR  (  (reg8 *) Printer_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Printer_TXSTATUS_REG        (* (reg8 *) Printer_BUART_sTX_TxSts__STATUS_REG)
    #define Printer_TXSTATUS_PTR        (  (reg8 *) Printer_BUART_sTX_TxSts__STATUS_REG)
    #define Printer_TXSTATUS_MASK_REG   (* (reg8 *) Printer_BUART_sTX_TxSts__MASK_REG)
    #define Printer_TXSTATUS_MASK_PTR   (  (reg8 *) Printer_BUART_sTX_TxSts__MASK_REG)
    #define Printer_TXSTATUS_ACTL_REG   (* (reg8 *) Printer_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define Printer_TXSTATUS_ACTL_PTR   (  (reg8 *) Printer_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(Printer_TXCLKGEN_DP)
        #define Printer_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) Printer_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Printer_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) Printer_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Printer_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) Printer_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define Printer_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) Printer_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define Printer_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) Printer_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Printer_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) Printer_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Printer_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Printer_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Printer_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Printer_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Printer_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) Printer_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define Printer_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) Printer_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* Printer_TXCLKGEN_DP */

#endif /* End Printer_TX_ENABLED */

#if(Printer_HD_ENABLED)

    #define Printer_TXDATA_REG             (* (reg8 *) Printer_BUART_sRX_RxShifter_u0__F1_REG )
    #define Printer_TXDATA_PTR             (  (reg8 *) Printer_BUART_sRX_RxShifter_u0__F1_REG )
    #define Printer_TXDATA_AUX_CTL_REG     (* (reg8 *) Printer_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define Printer_TXDATA_AUX_CTL_PTR     (  (reg8 *) Printer_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Printer_TXSTATUS_REG           (* (reg8 *) Printer_BUART_sRX_RxSts__STATUS_REG )
    #define Printer_TXSTATUS_PTR           (  (reg8 *) Printer_BUART_sRX_RxSts__STATUS_REG )
    #define Printer_TXSTATUS_MASK_REG      (* (reg8 *) Printer_BUART_sRX_RxSts__MASK_REG )
    #define Printer_TXSTATUS_MASK_PTR      (  (reg8 *) Printer_BUART_sRX_RxSts__MASK_REG )
    #define Printer_TXSTATUS_ACTL_REG      (* (reg8 *) Printer_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Printer_TXSTATUS_ACTL_PTR      (  (reg8 *) Printer_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End Printer_HD_ENABLED */

#if( (Printer_RX_ENABLED) || (Printer_HD_ENABLED) )
    #define Printer_RXDATA_REG             (* (reg8 *) Printer_BUART_sRX_RxShifter_u0__F0_REG )
    #define Printer_RXDATA_PTR             (  (reg8 *) Printer_BUART_sRX_RxShifter_u0__F0_REG )
    #define Printer_RXADDRESS1_REG         (* (reg8 *) Printer_BUART_sRX_RxShifter_u0__D0_REG )
    #define Printer_RXADDRESS1_PTR         (  (reg8 *) Printer_BUART_sRX_RxShifter_u0__D0_REG )
    #define Printer_RXADDRESS2_REG         (* (reg8 *) Printer_BUART_sRX_RxShifter_u0__D1_REG )
    #define Printer_RXADDRESS2_PTR         (  (reg8 *) Printer_BUART_sRX_RxShifter_u0__D1_REG )
    #define Printer_RXDATA_AUX_CTL_REG     (* (reg8 *) Printer_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Printer_RXBITCTR_PERIOD_REG    (* (reg8 *) Printer_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Printer_RXBITCTR_PERIOD_PTR    (  (reg8 *) Printer_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Printer_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Printer_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Printer_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Printer_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Printer_RXBITCTR_COUNTER_REG   (* (reg8 *) Printer_BUART_sRX_RxBitCounter__COUNT_REG )
    #define Printer_RXBITCTR_COUNTER_PTR   (  (reg8 *) Printer_BUART_sRX_RxBitCounter__COUNT_REG )

    #define Printer_RXSTATUS_REG           (* (reg8 *) Printer_BUART_sRX_RxSts__STATUS_REG )
    #define Printer_RXSTATUS_PTR           (  (reg8 *) Printer_BUART_sRX_RxSts__STATUS_REG )
    #define Printer_RXSTATUS_MASK_REG      (* (reg8 *) Printer_BUART_sRX_RxSts__MASK_REG )
    #define Printer_RXSTATUS_MASK_PTR      (  (reg8 *) Printer_BUART_sRX_RxSts__MASK_REG )
    #define Printer_RXSTATUS_ACTL_REG      (* (reg8 *) Printer_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Printer_RXSTATUS_ACTL_PTR      (  (reg8 *) Printer_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (Printer_RX_ENABLED) || (Printer_HD_ENABLED) */

#if(Printer_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define Printer_INTCLOCK_CLKEN_REG     (* (reg8 *) Printer_IntClock__PM_ACT_CFG)
    #define Printer_INTCLOCK_CLKEN_PTR     (  (reg8 *) Printer_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define Printer_INTCLOCK_CLKEN_MASK    Printer_IntClock__PM_ACT_MSK
#endif /* End Printer_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(Printer_TX_ENABLED)
    #define Printer_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End Printer_TX_ENABLED */

#if(Printer_HD_ENABLED)
    #define Printer_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End Printer_HD_ENABLED */

#if( (Printer_RX_ENABLED) || (Printer_HD_ENABLED) )
    #define Printer_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (Printer_RX_ENABLED) || (Printer_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define Printer_WAIT_1_MS      Printer_BL_CHK_DELAY_MS   

#define Printer_TXBUFFERSIZE   Printer_TX_BUFFER_SIZE
#define Printer_RXBUFFERSIZE   Printer_RX_BUFFER_SIZE

#if (Printer_RXHW_ADDRESS_ENABLED)
    #define Printer_RXADDRESSMODE  Printer_RX_ADDRESS_MODE
    #define Printer_RXHWADDRESS1   Printer_RX_HW_ADDRESS1
    #define Printer_RXHWADDRESS2   Printer_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define Printer_RXAddressMode  Printer_RXADDRESSMODE
#endif /* (Printer_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define Printer_initvar                    Printer_initVar

#define Printer_RX_Enabled                 Printer_RX_ENABLED
#define Printer_TX_Enabled                 Printer_TX_ENABLED
#define Printer_HD_Enabled                 Printer_HD_ENABLED
#define Printer_RX_IntInterruptEnabled     Printer_RX_INTERRUPT_ENABLED
#define Printer_TX_IntInterruptEnabled     Printer_TX_INTERRUPT_ENABLED
#define Printer_InternalClockUsed          Printer_INTERNAL_CLOCK_USED
#define Printer_RXHW_Address_Enabled       Printer_RXHW_ADDRESS_ENABLED
#define Printer_OverSampleCount            Printer_OVER_SAMPLE_COUNT
#define Printer_ParityType                 Printer_PARITY_TYPE

#if( Printer_TX_ENABLED && (Printer_TXBUFFERSIZE > Printer_FIFO_LENGTH))
    #define Printer_TXBUFFER               Printer_txBuffer
    #define Printer_TXBUFFERREAD           Printer_txBufferRead
    #define Printer_TXBUFFERWRITE          Printer_txBufferWrite
#endif /* End Printer_TX_ENABLED */
#if( ( Printer_RX_ENABLED || Printer_HD_ENABLED ) && \
     (Printer_RXBUFFERSIZE > Printer_FIFO_LENGTH) )
    #define Printer_RXBUFFER               Printer_rxBuffer
    #define Printer_RXBUFFERREAD           Printer_rxBufferRead
    #define Printer_RXBUFFERWRITE          Printer_rxBufferWrite
    #define Printer_RXBUFFERLOOPDETECT     Printer_rxBufferLoopDetect
    #define Printer_RXBUFFER_OVERFLOW      Printer_rxBufferOverflow
#endif /* End Printer_RX_ENABLED */

#ifdef Printer_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Printer_CONTROL                Printer_CONTROL_REG
#endif /* End Printer_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Printer_TX_ENABLED)
    #define Printer_TXDATA                 Printer_TXDATA_REG
    #define Printer_TXSTATUS               Printer_TXSTATUS_REG
    #define Printer_TXSTATUS_MASK          Printer_TXSTATUS_MASK_REG
    #define Printer_TXSTATUS_ACTL          Printer_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(Printer_TXCLKGEN_DP)
        #define Printer_TXBITCLKGEN_CTR        Printer_TXBITCLKGEN_CTR_REG
        #define Printer_TXBITCLKTX_COMPLETE    Printer_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define Printer_TXBITCTR_PERIOD        Printer_TXBITCTR_PERIOD_REG
        #define Printer_TXBITCTR_CONTROL       Printer_TXBITCTR_CONTROL_REG
        #define Printer_TXBITCTR_COUNTER       Printer_TXBITCTR_COUNTER_REG
    #endif /* Printer_TXCLKGEN_DP */
#endif /* End Printer_TX_ENABLED */

#if(Printer_HD_ENABLED)
    #define Printer_TXDATA                 Printer_TXDATA_REG
    #define Printer_TXSTATUS               Printer_TXSTATUS_REG
    #define Printer_TXSTATUS_MASK          Printer_TXSTATUS_MASK_REG
    #define Printer_TXSTATUS_ACTL          Printer_TXSTATUS_ACTL_REG
#endif /* End Printer_HD_ENABLED */

#if( (Printer_RX_ENABLED) || (Printer_HD_ENABLED) )
    #define Printer_RXDATA                 Printer_RXDATA_REG
    #define Printer_RXADDRESS1             Printer_RXADDRESS1_REG
    #define Printer_RXADDRESS2             Printer_RXADDRESS2_REG
    #define Printer_RXBITCTR_PERIOD        Printer_RXBITCTR_PERIOD_REG
    #define Printer_RXBITCTR_CONTROL       Printer_RXBITCTR_CONTROL_REG
    #define Printer_RXBITCTR_COUNTER       Printer_RXBITCTR_COUNTER_REG
    #define Printer_RXSTATUS               Printer_RXSTATUS_REG
    #define Printer_RXSTATUS_MASK          Printer_RXSTATUS_MASK_REG
    #define Printer_RXSTATUS_ACTL          Printer_RXSTATUS_ACTL_REG
#endif /* End  (Printer_RX_ENABLED) || (Printer_HD_ENABLED) */

#if(Printer_INTERNAL_CLOCK_USED)
    #define Printer_INTCLOCK_CLKEN         Printer_INTCLOCK_CLKEN_REG
#endif /* End Printer_INTERNAL_CLOCK_USED */

#define Printer_WAIT_FOR_COMLETE_REINIT    Printer_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_Printer_H */


/* [] END OF FILE */
