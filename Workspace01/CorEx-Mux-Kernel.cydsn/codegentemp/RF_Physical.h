/*******************************************************************************
* File Name: RF_Physical.h
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


#if !defined(CY_UART_RF_Physical_H)
#define CY_UART_RF_Physical_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"


/***************************************
* Conditional Compilation Parameters
***************************************/

#define RF_Physical_RX_ENABLED                     (1u)
#define RF_Physical_TX_ENABLED                     (1u)
#define RF_Physical_HD_ENABLED                     (0u)
#define RF_Physical_RX_INTERRUPT_ENABLED           (1u)
#define RF_Physical_TX_INTERRUPT_ENABLED           (0u)
#define RF_Physical_INTERNAL_CLOCK_USED            (1u)
#define RF_Physical_RXHW_ADDRESS_ENABLED           (0u)
#define RF_Physical_OVER_SAMPLE_COUNT              (8u)
#define RF_Physical_PARITY_TYPE                    (0u)
#define RF_Physical_PARITY_TYPE_SW                 (0u)
#define RF_Physical_BREAK_DETECT                   (0u)
#define RF_Physical_BREAK_BITS_TX                  (13u)
#define RF_Physical_BREAK_BITS_RX                  (13u)
#define RF_Physical_TXCLKGEN_DP                    (1u)
#define RF_Physical_USE23POLLING                   (1u)
#define RF_Physical_FLOW_CONTROL                   (0u)
#define RF_Physical_CLK_FREQ                       (0u)
#define RF_Physical_TX_BUFFER_SIZE                 (4u)
#define RF_Physical_RX_BUFFER_SIZE                 (512u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(RF_Physical_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define RF_Physical_CONTROL_REG_REMOVED            (0u)
#else
    #define RF_Physical_CONTROL_REG_REMOVED            (1u)
#endif /* End RF_Physical_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct RF_Physical_backupStruct_
{
    uint8 enableState;

    #if(RF_Physical_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End RF_Physical_CONTROL_REG_REMOVED */

} RF_Physical_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void RF_Physical_Start(void) ;
void RF_Physical_Stop(void) ;
uint8 RF_Physical_ReadControlRegister(void) ;
void RF_Physical_WriteControlRegister(uint8 control) ;

void RF_Physical_Init(void) ;
void RF_Physical_Enable(void) ;
void RF_Physical_SaveConfig(void) ;
void RF_Physical_RestoreConfig(void) ;
void RF_Physical_Sleep(void) ;
void RF_Physical_Wakeup(void) ;

/* Only if RX is enabled */
#if( (RF_Physical_RX_ENABLED) || (RF_Physical_HD_ENABLED) )

    #if (RF_Physical_RX_INTERRUPT_ENABLED)
        #define RF_Physical_EnableRxInt()  CyIntEnable (RF_Physical_RX_VECT_NUM)
        #define RF_Physical_DisableRxInt() CyIntDisable(RF_Physical_RX_VECT_NUM)
        CY_ISR_PROTO(RF_Physical_RXISR);
    #endif /* RF_Physical_RX_INTERRUPT_ENABLED */

    void RF_Physical_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void RF_Physical_SetRxAddress1(uint8 address) ;
    void RF_Physical_SetRxAddress2(uint8 address) ;

    void  RF_Physical_SetRxInterruptMode(uint8 intSrc) ;
    uint8 RF_Physical_ReadRxData(void) ;
    uint8 RF_Physical_ReadRxStatus(void) ;
    uint8 RF_Physical_GetChar(void) ;
    uint16 RF_Physical_GetByte(void) ;
    uint16 RF_Physical_GetRxBufferSize(void)
                                                            ;
    void RF_Physical_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define RF_Physical_GetRxInterruptSource   RF_Physical_ReadRxStatus

#endif /* End (RF_Physical_RX_ENABLED) || (RF_Physical_HD_ENABLED) */

/* Only if TX is enabled */
#if(RF_Physical_TX_ENABLED || RF_Physical_HD_ENABLED)

    #if(RF_Physical_TX_INTERRUPT_ENABLED)
        #define RF_Physical_EnableTxInt()  CyIntEnable (RF_Physical_TX_VECT_NUM)
        #define RF_Physical_DisableTxInt() CyIntDisable(RF_Physical_TX_VECT_NUM)
        #define RF_Physical_SetPendingTxInt() CyIntSetPending(RF_Physical_TX_VECT_NUM)
        #define RF_Physical_ClearPendingTxInt() CyIntClearPending(RF_Physical_TX_VECT_NUM)
        CY_ISR_PROTO(RF_Physical_TXISR);
    #endif /* RF_Physical_TX_INTERRUPT_ENABLED */

    void RF_Physical_SetTxInterruptMode(uint8 intSrc) ;
    void RF_Physical_WriteTxData(uint8 txDataByte) ;
    uint8 RF_Physical_ReadTxStatus(void) ;
    void RF_Physical_PutChar(uint8 txDataByte) ;
    void RF_Physical_PutString(const char8 string[]) ;
    void RF_Physical_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void RF_Physical_PutCRLF(uint8 txDataByte) ;
    void RF_Physical_ClearTxBuffer(void) ;
    void RF_Physical_SetTxAddressMode(uint8 addressMode) ;
    void RF_Physical_SendBreak(uint8 retMode) ;
    uint8 RF_Physical_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define RF_Physical_PutStringConst         RF_Physical_PutString
    #define RF_Physical_PutArrayConst          RF_Physical_PutArray
    #define RF_Physical_GetTxInterruptSource   RF_Physical_ReadTxStatus

#endif /* End RF_Physical_TX_ENABLED || RF_Physical_HD_ENABLED */

#if(RF_Physical_HD_ENABLED)
    void RF_Physical_LoadRxConfig(void) ;
    void RF_Physical_LoadTxConfig(void) ;
#endif /* End RF_Physical_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_RF_Physical) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    RF_Physical_CyBtldrCommStart(void) CYSMALL ;
    void    RF_Physical_CyBtldrCommStop(void) CYSMALL ;
    void    RF_Physical_CyBtldrCommReset(void) CYSMALL ;
    cystatus RF_Physical_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus RF_Physical_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_RF_Physical)
        #define CyBtldrCommStart    RF_Physical_CyBtldrCommStart
        #define CyBtldrCommStop     RF_Physical_CyBtldrCommStop
        #define CyBtldrCommReset    RF_Physical_CyBtldrCommReset
        #define CyBtldrCommWrite    RF_Physical_CyBtldrCommWrite
        #define CyBtldrCommRead     RF_Physical_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_RF_Physical) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define RF_Physical_BYTE2BYTE_TIME_OUT (25u)
    #define RF_Physical_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define RF_Physical_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define RF_Physical_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define RF_Physical_SET_SPACE      (0x00u)
#define RF_Physical_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (RF_Physical_TX_ENABLED) || (RF_Physical_HD_ENABLED) )
    #if(RF_Physical_TX_INTERRUPT_ENABLED)
        #define RF_Physical_TX_VECT_NUM            (uint8)RF_Physical_TXInternalInterrupt__INTC_NUMBER
        #define RF_Physical_TX_PRIOR_NUM           (uint8)RF_Physical_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* RF_Physical_TX_INTERRUPT_ENABLED */

    #define RF_Physical_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define RF_Physical_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define RF_Physical_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(RF_Physical_TX_ENABLED)
        #define RF_Physical_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (RF_Physical_HD_ENABLED) */
        #define RF_Physical_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (RF_Physical_TX_ENABLED) */

    #define RF_Physical_TX_STS_COMPLETE            (uint8)(0x01u << RF_Physical_TX_STS_COMPLETE_SHIFT)
    #define RF_Physical_TX_STS_FIFO_EMPTY          (uint8)(0x01u << RF_Physical_TX_STS_FIFO_EMPTY_SHIFT)
    #define RF_Physical_TX_STS_FIFO_FULL           (uint8)(0x01u << RF_Physical_TX_STS_FIFO_FULL_SHIFT)
    #define RF_Physical_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << RF_Physical_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (RF_Physical_TX_ENABLED) || (RF_Physical_HD_ENABLED)*/

#if( (RF_Physical_RX_ENABLED) || (RF_Physical_HD_ENABLED) )
    #if(RF_Physical_RX_INTERRUPT_ENABLED)
        #define RF_Physical_RX_VECT_NUM            (uint8)RF_Physical_RXInternalInterrupt__INTC_NUMBER
        #define RF_Physical_RX_PRIOR_NUM           (uint8)RF_Physical_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* RF_Physical_RX_INTERRUPT_ENABLED */
    #define RF_Physical_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define RF_Physical_RX_STS_BREAK_SHIFT             (0x01u)
    #define RF_Physical_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define RF_Physical_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define RF_Physical_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define RF_Physical_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define RF_Physical_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define RF_Physical_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define RF_Physical_RX_STS_MRKSPC           (uint8)(0x01u << RF_Physical_RX_STS_MRKSPC_SHIFT)
    #define RF_Physical_RX_STS_BREAK            (uint8)(0x01u << RF_Physical_RX_STS_BREAK_SHIFT)
    #define RF_Physical_RX_STS_PAR_ERROR        (uint8)(0x01u << RF_Physical_RX_STS_PAR_ERROR_SHIFT)
    #define RF_Physical_RX_STS_STOP_ERROR       (uint8)(0x01u << RF_Physical_RX_STS_STOP_ERROR_SHIFT)
    #define RF_Physical_RX_STS_OVERRUN          (uint8)(0x01u << RF_Physical_RX_STS_OVERRUN_SHIFT)
    #define RF_Physical_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << RF_Physical_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define RF_Physical_RX_STS_ADDR_MATCH       (uint8)(0x01u << RF_Physical_RX_STS_ADDR_MATCH_SHIFT)
    #define RF_Physical_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << RF_Physical_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define RF_Physical_RX_HW_MASK                     (0x7Fu)
#endif /* End (RF_Physical_RX_ENABLED) || (RF_Physical_HD_ENABLED) */

/* Control Register definitions */
#define RF_Physical_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define RF_Physical_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define RF_Physical_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define RF_Physical_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define RF_Physical_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define RF_Physical_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define RF_Physical_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define RF_Physical_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define RF_Physical_CTRL_HD_SEND               (uint8)(0x01u << RF_Physical_CTRL_HD_SEND_SHIFT)
#define RF_Physical_CTRL_HD_SEND_BREAK         (uint8)(0x01u << RF_Physical_CTRL_HD_SEND_BREAK_SHIFT)
#define RF_Physical_CTRL_MARK                  (uint8)(0x01u << RF_Physical_CTRL_MARK_SHIFT)
#define RF_Physical_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << RF_Physical_CTRL_PARITY_TYPE0_SHIFT)
#define RF_Physical_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << RF_Physical_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define RF_Physical_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define RF_Physical_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define RF_Physical_SEND_BREAK                         (0x00u)
#define RF_Physical_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define RF_Physical_REINIT                             (0x02u)
#define RF_Physical_SEND_WAIT_REINIT                   (0x03u)

#define RF_Physical_OVER_SAMPLE_8                      (8u)
#define RF_Physical_OVER_SAMPLE_16                     (16u)

#define RF_Physical_BIT_CENTER                         (RF_Physical_OVER_SAMPLE_COUNT - 2u)

#define RF_Physical_FIFO_LENGTH                        (4u)
#define RF_Physical_NUMBER_OF_START_BIT                (1u)
#define RF_Physical_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define RF_Physical_TXBITCTR_BREAKBITS8X   ((RF_Physical_BREAK_BITS_TX * RF_Physical_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define RF_Physical_TXBITCTR_BREAKBITS ((RF_Physical_BREAK_BITS_TX * RF_Physical_OVER_SAMPLE_COUNT) - 1u)

#define RF_Physical_HALF_BIT_COUNT   \
                            (((RF_Physical_OVER_SAMPLE_COUNT / 2u) + (RF_Physical_USE23POLLING * 1u)) - 2u)
#if (RF_Physical_OVER_SAMPLE_COUNT == RF_Physical_OVER_SAMPLE_8)
    #define RF_Physical_HD_TXBITCTR_INIT   (((RF_Physical_BREAK_BITS_TX + \
                            RF_Physical_NUMBER_OF_START_BIT) * RF_Physical_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define RF_Physical_RXBITCTR_INIT  ((((RF_Physical_BREAK_BITS_RX + RF_Physical_NUMBER_OF_START_BIT) \
                            * RF_Physical_OVER_SAMPLE_COUNT) + RF_Physical_HALF_BIT_COUNT) - 1u)

#else /* RF_Physical_OVER_SAMPLE_COUNT == RF_Physical_OVER_SAMPLE_16 */
    #define RF_Physical_HD_TXBITCTR_INIT   ((8u * RF_Physical_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define RF_Physical_RXBITCTR_INIT      (((7u * RF_Physical_OVER_SAMPLE_COUNT) - 1u) + \
                                                      RF_Physical_HALF_BIT_COUNT)
#endif /* End RF_Physical_OVER_SAMPLE_COUNT */

#define RF_Physical_HD_RXBITCTR_INIT                   RF_Physical_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 RF_Physical_initVar;
#if (RF_Physical_TX_INTERRUPT_ENABLED && RF_Physical_TX_ENABLED)
    extern volatile uint8 RF_Physical_txBuffer[RF_Physical_TX_BUFFER_SIZE];
    extern volatile uint8 RF_Physical_txBufferRead;
    extern uint8 RF_Physical_txBufferWrite;
#endif /* (RF_Physical_TX_INTERRUPT_ENABLED && RF_Physical_TX_ENABLED) */
#if (RF_Physical_RX_INTERRUPT_ENABLED && (RF_Physical_RX_ENABLED || RF_Physical_HD_ENABLED))
    extern uint8 RF_Physical_errorStatus;
    extern volatile uint8 RF_Physical_rxBuffer[RF_Physical_RX_BUFFER_SIZE];
    extern volatile uint16 RF_Physical_rxBufferRead;
    extern volatile uint16 RF_Physical_rxBufferWrite;
    extern volatile uint8 RF_Physical_rxBufferLoopDetect;
    extern volatile uint8 RF_Physical_rxBufferOverflow;
    #if (RF_Physical_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 RF_Physical_rxAddressMode;
        extern volatile uint8 RF_Physical_rxAddressDetected;
    #endif /* (RF_Physical_RXHW_ADDRESS_ENABLED) */
#endif /* (RF_Physical_RX_INTERRUPT_ENABLED && (RF_Physical_RX_ENABLED || RF_Physical_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define RF_Physical__B_UART__AM_SW_BYTE_BYTE 1
#define RF_Physical__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define RF_Physical__B_UART__AM_HW_BYTE_BY_BYTE 3
#define RF_Physical__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define RF_Physical__B_UART__AM_NONE 0

#define RF_Physical__B_UART__NONE_REVB 0
#define RF_Physical__B_UART__EVEN_REVB 1
#define RF_Physical__B_UART__ODD_REVB 2
#define RF_Physical__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define RF_Physical_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define RF_Physical_NUMBER_OF_STOP_BITS    (1u)

#if (RF_Physical_RXHW_ADDRESS_ENABLED)
    #define RF_Physical_RX_ADDRESS_MODE    (0u)
    #define RF_Physical_RX_HW_ADDRESS1     (0u)
    #define RF_Physical_RX_HW_ADDRESS2     (0u)
#endif /* (RF_Physical_RXHW_ADDRESS_ENABLED) */

#define RF_Physical_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << RF_Physical_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << RF_Physical_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << RF_Physical_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << RF_Physical_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << RF_Physical_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << RF_Physical_RX_STS_BREAK_SHIFT) \
                                        | (0 << RF_Physical_RX_STS_OVERRUN_SHIFT))

#define RF_Physical_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << RF_Physical_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << RF_Physical_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << RF_Physical_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << RF_Physical_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef RF_Physical_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define RF_Physical_CONTROL_REG \
                            (* (reg8 *) RF_Physical_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define RF_Physical_CONTROL_PTR \
                            (  (reg8 *) RF_Physical_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End RF_Physical_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(RF_Physical_TX_ENABLED)
    #define RF_Physical_TXDATA_REG          (* (reg8 *) RF_Physical_BUART_sTX_TxShifter_u0__F0_REG)
    #define RF_Physical_TXDATA_PTR          (  (reg8 *) RF_Physical_BUART_sTX_TxShifter_u0__F0_REG)
    #define RF_Physical_TXDATA_AUX_CTL_REG  (* (reg8 *) RF_Physical_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define RF_Physical_TXDATA_AUX_CTL_PTR  (  (reg8 *) RF_Physical_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define RF_Physical_TXSTATUS_REG        (* (reg8 *) RF_Physical_BUART_sTX_TxSts__STATUS_REG)
    #define RF_Physical_TXSTATUS_PTR        (  (reg8 *) RF_Physical_BUART_sTX_TxSts__STATUS_REG)
    #define RF_Physical_TXSTATUS_MASK_REG   (* (reg8 *) RF_Physical_BUART_sTX_TxSts__MASK_REG)
    #define RF_Physical_TXSTATUS_MASK_PTR   (  (reg8 *) RF_Physical_BUART_sTX_TxSts__MASK_REG)
    #define RF_Physical_TXSTATUS_ACTL_REG   (* (reg8 *) RF_Physical_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define RF_Physical_TXSTATUS_ACTL_PTR   (  (reg8 *) RF_Physical_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(RF_Physical_TXCLKGEN_DP)
        #define RF_Physical_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) RF_Physical_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define RF_Physical_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) RF_Physical_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define RF_Physical_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) RF_Physical_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define RF_Physical_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) RF_Physical_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define RF_Physical_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) RF_Physical_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define RF_Physical_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) RF_Physical_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define RF_Physical_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) RF_Physical_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define RF_Physical_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) RF_Physical_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define RF_Physical_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) RF_Physical_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define RF_Physical_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) RF_Physical_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* RF_Physical_TXCLKGEN_DP */

#endif /* End RF_Physical_TX_ENABLED */

#if(RF_Physical_HD_ENABLED)

    #define RF_Physical_TXDATA_REG             (* (reg8 *) RF_Physical_BUART_sRX_RxShifter_u0__F1_REG )
    #define RF_Physical_TXDATA_PTR             (  (reg8 *) RF_Physical_BUART_sRX_RxShifter_u0__F1_REG )
    #define RF_Physical_TXDATA_AUX_CTL_REG     (* (reg8 *) RF_Physical_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define RF_Physical_TXDATA_AUX_CTL_PTR     (  (reg8 *) RF_Physical_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define RF_Physical_TXSTATUS_REG           (* (reg8 *) RF_Physical_BUART_sRX_RxSts__STATUS_REG )
    #define RF_Physical_TXSTATUS_PTR           (  (reg8 *) RF_Physical_BUART_sRX_RxSts__STATUS_REG )
    #define RF_Physical_TXSTATUS_MASK_REG      (* (reg8 *) RF_Physical_BUART_sRX_RxSts__MASK_REG )
    #define RF_Physical_TXSTATUS_MASK_PTR      (  (reg8 *) RF_Physical_BUART_sRX_RxSts__MASK_REG )
    #define RF_Physical_TXSTATUS_ACTL_REG      (* (reg8 *) RF_Physical_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define RF_Physical_TXSTATUS_ACTL_PTR      (  (reg8 *) RF_Physical_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End RF_Physical_HD_ENABLED */

#if( (RF_Physical_RX_ENABLED) || (RF_Physical_HD_ENABLED) )
    #define RF_Physical_RXDATA_REG             (* (reg8 *) RF_Physical_BUART_sRX_RxShifter_u0__F0_REG )
    #define RF_Physical_RXDATA_PTR             (  (reg8 *) RF_Physical_BUART_sRX_RxShifter_u0__F0_REG )
    #define RF_Physical_RXADDRESS1_REG         (* (reg8 *) RF_Physical_BUART_sRX_RxShifter_u0__D0_REG )
    #define RF_Physical_RXADDRESS1_PTR         (  (reg8 *) RF_Physical_BUART_sRX_RxShifter_u0__D0_REG )
    #define RF_Physical_RXADDRESS2_REG         (* (reg8 *) RF_Physical_BUART_sRX_RxShifter_u0__D1_REG )
    #define RF_Physical_RXADDRESS2_PTR         (  (reg8 *) RF_Physical_BUART_sRX_RxShifter_u0__D1_REG )
    #define RF_Physical_RXDATA_AUX_CTL_REG     (* (reg8 *) RF_Physical_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define RF_Physical_RXBITCTR_PERIOD_REG    (* (reg8 *) RF_Physical_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define RF_Physical_RXBITCTR_PERIOD_PTR    (  (reg8 *) RF_Physical_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define RF_Physical_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) RF_Physical_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define RF_Physical_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) RF_Physical_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define RF_Physical_RXBITCTR_COUNTER_REG   (* (reg8 *) RF_Physical_BUART_sRX_RxBitCounter__COUNT_REG )
    #define RF_Physical_RXBITCTR_COUNTER_PTR   (  (reg8 *) RF_Physical_BUART_sRX_RxBitCounter__COUNT_REG )

    #define RF_Physical_RXSTATUS_REG           (* (reg8 *) RF_Physical_BUART_sRX_RxSts__STATUS_REG )
    #define RF_Physical_RXSTATUS_PTR           (  (reg8 *) RF_Physical_BUART_sRX_RxSts__STATUS_REG )
    #define RF_Physical_RXSTATUS_MASK_REG      (* (reg8 *) RF_Physical_BUART_sRX_RxSts__MASK_REG )
    #define RF_Physical_RXSTATUS_MASK_PTR      (  (reg8 *) RF_Physical_BUART_sRX_RxSts__MASK_REG )
    #define RF_Physical_RXSTATUS_ACTL_REG      (* (reg8 *) RF_Physical_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define RF_Physical_RXSTATUS_ACTL_PTR      (  (reg8 *) RF_Physical_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (RF_Physical_RX_ENABLED) || (RF_Physical_HD_ENABLED) */

#if(RF_Physical_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define RF_Physical_INTCLOCK_CLKEN_REG     (* (reg8 *) RF_Physical_IntClock__PM_ACT_CFG)
    #define RF_Physical_INTCLOCK_CLKEN_PTR     (  (reg8 *) RF_Physical_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define RF_Physical_INTCLOCK_CLKEN_MASK    RF_Physical_IntClock__PM_ACT_MSK
#endif /* End RF_Physical_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(RF_Physical_TX_ENABLED)
    #define RF_Physical_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End RF_Physical_TX_ENABLED */

#if(RF_Physical_HD_ENABLED)
    #define RF_Physical_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End RF_Physical_HD_ENABLED */

#if( (RF_Physical_RX_ENABLED) || (RF_Physical_HD_ENABLED) )
    #define RF_Physical_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (RF_Physical_RX_ENABLED) || (RF_Physical_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define RF_Physical_WAIT_1_MS      RF_Physical_BL_CHK_DELAY_MS   

#define RF_Physical_TXBUFFERSIZE   RF_Physical_TX_BUFFER_SIZE
#define RF_Physical_RXBUFFERSIZE   RF_Physical_RX_BUFFER_SIZE

#if (RF_Physical_RXHW_ADDRESS_ENABLED)
    #define RF_Physical_RXADDRESSMODE  RF_Physical_RX_ADDRESS_MODE
    #define RF_Physical_RXHWADDRESS1   RF_Physical_RX_HW_ADDRESS1
    #define RF_Physical_RXHWADDRESS2   RF_Physical_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define RF_Physical_RXAddressMode  RF_Physical_RXADDRESSMODE
#endif /* (RF_Physical_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define RF_Physical_initvar                    RF_Physical_initVar

#define RF_Physical_RX_Enabled                 RF_Physical_RX_ENABLED
#define RF_Physical_TX_Enabled                 RF_Physical_TX_ENABLED
#define RF_Physical_HD_Enabled                 RF_Physical_HD_ENABLED
#define RF_Physical_RX_IntInterruptEnabled     RF_Physical_RX_INTERRUPT_ENABLED
#define RF_Physical_TX_IntInterruptEnabled     RF_Physical_TX_INTERRUPT_ENABLED
#define RF_Physical_InternalClockUsed          RF_Physical_INTERNAL_CLOCK_USED
#define RF_Physical_RXHW_Address_Enabled       RF_Physical_RXHW_ADDRESS_ENABLED
#define RF_Physical_OverSampleCount            RF_Physical_OVER_SAMPLE_COUNT
#define RF_Physical_ParityType                 RF_Physical_PARITY_TYPE

#if( RF_Physical_TX_ENABLED && (RF_Physical_TXBUFFERSIZE > RF_Physical_FIFO_LENGTH))
    #define RF_Physical_TXBUFFER               RF_Physical_txBuffer
    #define RF_Physical_TXBUFFERREAD           RF_Physical_txBufferRead
    #define RF_Physical_TXBUFFERWRITE          RF_Physical_txBufferWrite
#endif /* End RF_Physical_TX_ENABLED */
#if( ( RF_Physical_RX_ENABLED || RF_Physical_HD_ENABLED ) && \
     (RF_Physical_RXBUFFERSIZE > RF_Physical_FIFO_LENGTH) )
    #define RF_Physical_RXBUFFER               RF_Physical_rxBuffer
    #define RF_Physical_RXBUFFERREAD           RF_Physical_rxBufferRead
    #define RF_Physical_RXBUFFERWRITE          RF_Physical_rxBufferWrite
    #define RF_Physical_RXBUFFERLOOPDETECT     RF_Physical_rxBufferLoopDetect
    #define RF_Physical_RXBUFFER_OVERFLOW      RF_Physical_rxBufferOverflow
#endif /* End RF_Physical_RX_ENABLED */

#ifdef RF_Physical_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define RF_Physical_CONTROL                RF_Physical_CONTROL_REG
#endif /* End RF_Physical_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(RF_Physical_TX_ENABLED)
    #define RF_Physical_TXDATA                 RF_Physical_TXDATA_REG
    #define RF_Physical_TXSTATUS               RF_Physical_TXSTATUS_REG
    #define RF_Physical_TXSTATUS_MASK          RF_Physical_TXSTATUS_MASK_REG
    #define RF_Physical_TXSTATUS_ACTL          RF_Physical_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(RF_Physical_TXCLKGEN_DP)
        #define RF_Physical_TXBITCLKGEN_CTR        RF_Physical_TXBITCLKGEN_CTR_REG
        #define RF_Physical_TXBITCLKTX_COMPLETE    RF_Physical_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define RF_Physical_TXBITCTR_PERIOD        RF_Physical_TXBITCTR_PERIOD_REG
        #define RF_Physical_TXBITCTR_CONTROL       RF_Physical_TXBITCTR_CONTROL_REG
        #define RF_Physical_TXBITCTR_COUNTER       RF_Physical_TXBITCTR_COUNTER_REG
    #endif /* RF_Physical_TXCLKGEN_DP */
#endif /* End RF_Physical_TX_ENABLED */

#if(RF_Physical_HD_ENABLED)
    #define RF_Physical_TXDATA                 RF_Physical_TXDATA_REG
    #define RF_Physical_TXSTATUS               RF_Physical_TXSTATUS_REG
    #define RF_Physical_TXSTATUS_MASK          RF_Physical_TXSTATUS_MASK_REG
    #define RF_Physical_TXSTATUS_ACTL          RF_Physical_TXSTATUS_ACTL_REG
#endif /* End RF_Physical_HD_ENABLED */

#if( (RF_Physical_RX_ENABLED) || (RF_Physical_HD_ENABLED) )
    #define RF_Physical_RXDATA                 RF_Physical_RXDATA_REG
    #define RF_Physical_RXADDRESS1             RF_Physical_RXADDRESS1_REG
    #define RF_Physical_RXADDRESS2             RF_Physical_RXADDRESS2_REG
    #define RF_Physical_RXBITCTR_PERIOD        RF_Physical_RXBITCTR_PERIOD_REG
    #define RF_Physical_RXBITCTR_CONTROL       RF_Physical_RXBITCTR_CONTROL_REG
    #define RF_Physical_RXBITCTR_COUNTER       RF_Physical_RXBITCTR_COUNTER_REG
    #define RF_Physical_RXSTATUS               RF_Physical_RXSTATUS_REG
    #define RF_Physical_RXSTATUS_MASK          RF_Physical_RXSTATUS_MASK_REG
    #define RF_Physical_RXSTATUS_ACTL          RF_Physical_RXSTATUS_ACTL_REG
#endif /* End  (RF_Physical_RX_ENABLED) || (RF_Physical_HD_ENABLED) */

#if(RF_Physical_INTERNAL_CLOCK_USED)
    #define RF_Physical_INTCLOCK_CLKEN         RF_Physical_INTCLOCK_CLKEN_REG
#endif /* End RF_Physical_INTERNAL_CLOCK_USED */

#define RF_Physical_WAIT_FOR_COMLETE_REINIT    RF_Physical_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_RF_Physical_H */


/* [] END OF FILE */
