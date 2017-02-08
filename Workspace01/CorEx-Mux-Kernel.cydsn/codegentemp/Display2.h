/*******************************************************************************
* File Name: Display2.h
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


#if !defined(CY_UART_Display2_H)
#define CY_UART_Display2_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"


/***************************************
* Conditional Compilation Parameters
***************************************/

#define Display2_RX_ENABLED                     (1u)
#define Display2_TX_ENABLED                     (1u)
#define Display2_HD_ENABLED                     (0u)
#define Display2_RX_INTERRUPT_ENABLED           (1u)
#define Display2_TX_INTERRUPT_ENABLED           (0u)
#define Display2_INTERNAL_CLOCK_USED            (1u)
#define Display2_RXHW_ADDRESS_ENABLED           (0u)
#define Display2_OVER_SAMPLE_COUNT              (8u)
#define Display2_PARITY_TYPE                    (0u)
#define Display2_PARITY_TYPE_SW                 (0u)
#define Display2_BREAK_DETECT                   (0u)
#define Display2_BREAK_BITS_TX                  (13u)
#define Display2_BREAK_BITS_RX                  (13u)
#define Display2_TXCLKGEN_DP                    (1u)
#define Display2_USE23POLLING                   (1u)
#define Display2_FLOW_CONTROL                   (0u)
#define Display2_CLK_FREQ                       (0u)
#define Display2_TX_BUFFER_SIZE                 (4u)
#define Display2_RX_BUFFER_SIZE                 (32u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(Display2_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define Display2_CONTROL_REG_REMOVED            (0u)
#else
    #define Display2_CONTROL_REG_REMOVED            (1u)
#endif /* End Display2_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct Display2_backupStruct_
{
    uint8 enableState;

    #if(Display2_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End Display2_CONTROL_REG_REMOVED */

} Display2_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void Display2_Start(void) ;
void Display2_Stop(void) ;
uint8 Display2_ReadControlRegister(void) ;
void Display2_WriteControlRegister(uint8 control) ;

void Display2_Init(void) ;
void Display2_Enable(void) ;
void Display2_SaveConfig(void) ;
void Display2_RestoreConfig(void) ;
void Display2_Sleep(void) ;
void Display2_Wakeup(void) ;

/* Only if RX is enabled */
#if( (Display2_RX_ENABLED) || (Display2_HD_ENABLED) )

    #if (Display2_RX_INTERRUPT_ENABLED)
        #define Display2_EnableRxInt()  CyIntEnable (Display2_RX_VECT_NUM)
        #define Display2_DisableRxInt() CyIntDisable(Display2_RX_VECT_NUM)
        CY_ISR_PROTO(Display2_RXISR);
    #endif /* Display2_RX_INTERRUPT_ENABLED */

    void Display2_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void Display2_SetRxAddress1(uint8 address) ;
    void Display2_SetRxAddress2(uint8 address) ;

    void  Display2_SetRxInterruptMode(uint8 intSrc) ;
    uint8 Display2_ReadRxData(void) ;
    uint8 Display2_ReadRxStatus(void) ;
    uint8 Display2_GetChar(void) ;
    uint16 Display2_GetByte(void) ;
    uint8 Display2_GetRxBufferSize(void)
                                                            ;
    void Display2_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define Display2_GetRxInterruptSource   Display2_ReadRxStatus

#endif /* End (Display2_RX_ENABLED) || (Display2_HD_ENABLED) */

/* Only if TX is enabled */
#if(Display2_TX_ENABLED || Display2_HD_ENABLED)

    #if(Display2_TX_INTERRUPT_ENABLED)
        #define Display2_EnableTxInt()  CyIntEnable (Display2_TX_VECT_NUM)
        #define Display2_DisableTxInt() CyIntDisable(Display2_TX_VECT_NUM)
        #define Display2_SetPendingTxInt() CyIntSetPending(Display2_TX_VECT_NUM)
        #define Display2_ClearPendingTxInt() CyIntClearPending(Display2_TX_VECT_NUM)
        CY_ISR_PROTO(Display2_TXISR);
    #endif /* Display2_TX_INTERRUPT_ENABLED */

    void Display2_SetTxInterruptMode(uint8 intSrc) ;
    void Display2_WriteTxData(uint8 txDataByte) ;
    uint8 Display2_ReadTxStatus(void) ;
    void Display2_PutChar(uint8 txDataByte) ;
    void Display2_PutString(const char8 string[]) ;
    void Display2_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void Display2_PutCRLF(uint8 txDataByte) ;
    void Display2_ClearTxBuffer(void) ;
    void Display2_SetTxAddressMode(uint8 addressMode) ;
    void Display2_SendBreak(uint8 retMode) ;
    uint8 Display2_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define Display2_PutStringConst         Display2_PutString
    #define Display2_PutArrayConst          Display2_PutArray
    #define Display2_GetTxInterruptSource   Display2_ReadTxStatus

#endif /* End Display2_TX_ENABLED || Display2_HD_ENABLED */

#if(Display2_HD_ENABLED)
    void Display2_LoadRxConfig(void) ;
    void Display2_LoadTxConfig(void) ;
#endif /* End Display2_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Display2) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    Display2_CyBtldrCommStart(void) CYSMALL ;
    void    Display2_CyBtldrCommStop(void) CYSMALL ;
    void    Display2_CyBtldrCommReset(void) CYSMALL ;
    cystatus Display2_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus Display2_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Display2)
        #define CyBtldrCommStart    Display2_CyBtldrCommStart
        #define CyBtldrCommStop     Display2_CyBtldrCommStop
        #define CyBtldrCommReset    Display2_CyBtldrCommReset
        #define CyBtldrCommWrite    Display2_CyBtldrCommWrite
        #define CyBtldrCommRead     Display2_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Display2) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define Display2_BYTE2BYTE_TIME_OUT (25u)
    #define Display2_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define Display2_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define Display2_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define Display2_SET_SPACE      (0x00u)
#define Display2_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (Display2_TX_ENABLED) || (Display2_HD_ENABLED) )
    #if(Display2_TX_INTERRUPT_ENABLED)
        #define Display2_TX_VECT_NUM            (uint8)Display2_TXInternalInterrupt__INTC_NUMBER
        #define Display2_TX_PRIOR_NUM           (uint8)Display2_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Display2_TX_INTERRUPT_ENABLED */

    #define Display2_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define Display2_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define Display2_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(Display2_TX_ENABLED)
        #define Display2_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (Display2_HD_ENABLED) */
        #define Display2_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (Display2_TX_ENABLED) */

    #define Display2_TX_STS_COMPLETE            (uint8)(0x01u << Display2_TX_STS_COMPLETE_SHIFT)
    #define Display2_TX_STS_FIFO_EMPTY          (uint8)(0x01u << Display2_TX_STS_FIFO_EMPTY_SHIFT)
    #define Display2_TX_STS_FIFO_FULL           (uint8)(0x01u << Display2_TX_STS_FIFO_FULL_SHIFT)
    #define Display2_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << Display2_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (Display2_TX_ENABLED) || (Display2_HD_ENABLED)*/

#if( (Display2_RX_ENABLED) || (Display2_HD_ENABLED) )
    #if(Display2_RX_INTERRUPT_ENABLED)
        #define Display2_RX_VECT_NUM            (uint8)Display2_RXInternalInterrupt__INTC_NUMBER
        #define Display2_RX_PRIOR_NUM           (uint8)Display2_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Display2_RX_INTERRUPT_ENABLED */
    #define Display2_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define Display2_RX_STS_BREAK_SHIFT             (0x01u)
    #define Display2_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define Display2_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define Display2_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define Display2_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define Display2_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define Display2_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define Display2_RX_STS_MRKSPC           (uint8)(0x01u << Display2_RX_STS_MRKSPC_SHIFT)
    #define Display2_RX_STS_BREAK            (uint8)(0x01u << Display2_RX_STS_BREAK_SHIFT)
    #define Display2_RX_STS_PAR_ERROR        (uint8)(0x01u << Display2_RX_STS_PAR_ERROR_SHIFT)
    #define Display2_RX_STS_STOP_ERROR       (uint8)(0x01u << Display2_RX_STS_STOP_ERROR_SHIFT)
    #define Display2_RX_STS_OVERRUN          (uint8)(0x01u << Display2_RX_STS_OVERRUN_SHIFT)
    #define Display2_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << Display2_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define Display2_RX_STS_ADDR_MATCH       (uint8)(0x01u << Display2_RX_STS_ADDR_MATCH_SHIFT)
    #define Display2_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << Display2_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define Display2_RX_HW_MASK                     (0x7Fu)
#endif /* End (Display2_RX_ENABLED) || (Display2_HD_ENABLED) */

/* Control Register definitions */
#define Display2_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define Display2_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define Display2_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define Display2_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define Display2_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define Display2_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define Display2_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define Display2_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define Display2_CTRL_HD_SEND               (uint8)(0x01u << Display2_CTRL_HD_SEND_SHIFT)
#define Display2_CTRL_HD_SEND_BREAK         (uint8)(0x01u << Display2_CTRL_HD_SEND_BREAK_SHIFT)
#define Display2_CTRL_MARK                  (uint8)(0x01u << Display2_CTRL_MARK_SHIFT)
#define Display2_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << Display2_CTRL_PARITY_TYPE0_SHIFT)
#define Display2_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << Display2_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define Display2_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define Display2_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define Display2_SEND_BREAK                         (0x00u)
#define Display2_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define Display2_REINIT                             (0x02u)
#define Display2_SEND_WAIT_REINIT                   (0x03u)

#define Display2_OVER_SAMPLE_8                      (8u)
#define Display2_OVER_SAMPLE_16                     (16u)

#define Display2_BIT_CENTER                         (Display2_OVER_SAMPLE_COUNT - 2u)

#define Display2_FIFO_LENGTH                        (4u)
#define Display2_NUMBER_OF_START_BIT                (1u)
#define Display2_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define Display2_TXBITCTR_BREAKBITS8X   ((Display2_BREAK_BITS_TX * Display2_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define Display2_TXBITCTR_BREAKBITS ((Display2_BREAK_BITS_TX * Display2_OVER_SAMPLE_COUNT) - 1u)

#define Display2_HALF_BIT_COUNT   \
                            (((Display2_OVER_SAMPLE_COUNT / 2u) + (Display2_USE23POLLING * 1u)) - 2u)
#if (Display2_OVER_SAMPLE_COUNT == Display2_OVER_SAMPLE_8)
    #define Display2_HD_TXBITCTR_INIT   (((Display2_BREAK_BITS_TX + \
                            Display2_NUMBER_OF_START_BIT) * Display2_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define Display2_RXBITCTR_INIT  ((((Display2_BREAK_BITS_RX + Display2_NUMBER_OF_START_BIT) \
                            * Display2_OVER_SAMPLE_COUNT) + Display2_HALF_BIT_COUNT) - 1u)

#else /* Display2_OVER_SAMPLE_COUNT == Display2_OVER_SAMPLE_16 */
    #define Display2_HD_TXBITCTR_INIT   ((8u * Display2_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define Display2_RXBITCTR_INIT      (((7u * Display2_OVER_SAMPLE_COUNT) - 1u) + \
                                                      Display2_HALF_BIT_COUNT)
#endif /* End Display2_OVER_SAMPLE_COUNT */

#define Display2_HD_RXBITCTR_INIT                   Display2_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 Display2_initVar;
#if (Display2_TX_INTERRUPT_ENABLED && Display2_TX_ENABLED)
    extern volatile uint8 Display2_txBuffer[Display2_TX_BUFFER_SIZE];
    extern volatile uint8 Display2_txBufferRead;
    extern uint8 Display2_txBufferWrite;
#endif /* (Display2_TX_INTERRUPT_ENABLED && Display2_TX_ENABLED) */
#if (Display2_RX_INTERRUPT_ENABLED && (Display2_RX_ENABLED || Display2_HD_ENABLED))
    extern uint8 Display2_errorStatus;
    extern volatile uint8 Display2_rxBuffer[Display2_RX_BUFFER_SIZE];
    extern volatile uint8 Display2_rxBufferRead;
    extern volatile uint8 Display2_rxBufferWrite;
    extern volatile uint8 Display2_rxBufferLoopDetect;
    extern volatile uint8 Display2_rxBufferOverflow;
    #if (Display2_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 Display2_rxAddressMode;
        extern volatile uint8 Display2_rxAddressDetected;
    #endif /* (Display2_RXHW_ADDRESS_ENABLED) */
#endif /* (Display2_RX_INTERRUPT_ENABLED && (Display2_RX_ENABLED || Display2_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define Display2__B_UART__AM_SW_BYTE_BYTE 1
#define Display2__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define Display2__B_UART__AM_HW_BYTE_BY_BYTE 3
#define Display2__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define Display2__B_UART__AM_NONE 0

#define Display2__B_UART__NONE_REVB 0
#define Display2__B_UART__EVEN_REVB 1
#define Display2__B_UART__ODD_REVB 2
#define Display2__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define Display2_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define Display2_NUMBER_OF_STOP_BITS    (1u)

#if (Display2_RXHW_ADDRESS_ENABLED)
    #define Display2_RX_ADDRESS_MODE    (0u)
    #define Display2_RX_HW_ADDRESS1     (0u)
    #define Display2_RX_HW_ADDRESS2     (0u)
#endif /* (Display2_RXHW_ADDRESS_ENABLED) */

#define Display2_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << Display2_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << Display2_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << Display2_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << Display2_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << Display2_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << Display2_RX_STS_BREAK_SHIFT) \
                                        | (0 << Display2_RX_STS_OVERRUN_SHIFT))

#define Display2_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << Display2_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << Display2_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << Display2_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << Display2_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef Display2_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Display2_CONTROL_REG \
                            (* (reg8 *) Display2_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define Display2_CONTROL_PTR \
                            (  (reg8 *) Display2_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End Display2_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Display2_TX_ENABLED)
    #define Display2_TXDATA_REG          (* (reg8 *) Display2_BUART_sTX_TxShifter_u0__F0_REG)
    #define Display2_TXDATA_PTR          (  (reg8 *) Display2_BUART_sTX_TxShifter_u0__F0_REG)
    #define Display2_TXDATA_AUX_CTL_REG  (* (reg8 *) Display2_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Display2_TXDATA_AUX_CTL_PTR  (  (reg8 *) Display2_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Display2_TXSTATUS_REG        (* (reg8 *) Display2_BUART_sTX_TxSts__STATUS_REG)
    #define Display2_TXSTATUS_PTR        (  (reg8 *) Display2_BUART_sTX_TxSts__STATUS_REG)
    #define Display2_TXSTATUS_MASK_REG   (* (reg8 *) Display2_BUART_sTX_TxSts__MASK_REG)
    #define Display2_TXSTATUS_MASK_PTR   (  (reg8 *) Display2_BUART_sTX_TxSts__MASK_REG)
    #define Display2_TXSTATUS_ACTL_REG   (* (reg8 *) Display2_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define Display2_TXSTATUS_ACTL_PTR   (  (reg8 *) Display2_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(Display2_TXCLKGEN_DP)
        #define Display2_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) Display2_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Display2_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) Display2_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Display2_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) Display2_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define Display2_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) Display2_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define Display2_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) Display2_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Display2_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) Display2_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Display2_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Display2_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Display2_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Display2_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Display2_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) Display2_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define Display2_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) Display2_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* Display2_TXCLKGEN_DP */

#endif /* End Display2_TX_ENABLED */

#if(Display2_HD_ENABLED)

    #define Display2_TXDATA_REG             (* (reg8 *) Display2_BUART_sRX_RxShifter_u0__F1_REG )
    #define Display2_TXDATA_PTR             (  (reg8 *) Display2_BUART_sRX_RxShifter_u0__F1_REG )
    #define Display2_TXDATA_AUX_CTL_REG     (* (reg8 *) Display2_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define Display2_TXDATA_AUX_CTL_PTR     (  (reg8 *) Display2_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Display2_TXSTATUS_REG           (* (reg8 *) Display2_BUART_sRX_RxSts__STATUS_REG )
    #define Display2_TXSTATUS_PTR           (  (reg8 *) Display2_BUART_sRX_RxSts__STATUS_REG )
    #define Display2_TXSTATUS_MASK_REG      (* (reg8 *) Display2_BUART_sRX_RxSts__MASK_REG )
    #define Display2_TXSTATUS_MASK_PTR      (  (reg8 *) Display2_BUART_sRX_RxSts__MASK_REG )
    #define Display2_TXSTATUS_ACTL_REG      (* (reg8 *) Display2_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Display2_TXSTATUS_ACTL_PTR      (  (reg8 *) Display2_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End Display2_HD_ENABLED */

#if( (Display2_RX_ENABLED) || (Display2_HD_ENABLED) )
    #define Display2_RXDATA_REG             (* (reg8 *) Display2_BUART_sRX_RxShifter_u0__F0_REG )
    #define Display2_RXDATA_PTR             (  (reg8 *) Display2_BUART_sRX_RxShifter_u0__F0_REG )
    #define Display2_RXADDRESS1_REG         (* (reg8 *) Display2_BUART_sRX_RxShifter_u0__D0_REG )
    #define Display2_RXADDRESS1_PTR         (  (reg8 *) Display2_BUART_sRX_RxShifter_u0__D0_REG )
    #define Display2_RXADDRESS2_REG         (* (reg8 *) Display2_BUART_sRX_RxShifter_u0__D1_REG )
    #define Display2_RXADDRESS2_PTR         (  (reg8 *) Display2_BUART_sRX_RxShifter_u0__D1_REG )
    #define Display2_RXDATA_AUX_CTL_REG     (* (reg8 *) Display2_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Display2_RXBITCTR_PERIOD_REG    (* (reg8 *) Display2_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Display2_RXBITCTR_PERIOD_PTR    (  (reg8 *) Display2_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Display2_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Display2_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Display2_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Display2_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Display2_RXBITCTR_COUNTER_REG   (* (reg8 *) Display2_BUART_sRX_RxBitCounter__COUNT_REG )
    #define Display2_RXBITCTR_COUNTER_PTR   (  (reg8 *) Display2_BUART_sRX_RxBitCounter__COUNT_REG )

    #define Display2_RXSTATUS_REG           (* (reg8 *) Display2_BUART_sRX_RxSts__STATUS_REG )
    #define Display2_RXSTATUS_PTR           (  (reg8 *) Display2_BUART_sRX_RxSts__STATUS_REG )
    #define Display2_RXSTATUS_MASK_REG      (* (reg8 *) Display2_BUART_sRX_RxSts__MASK_REG )
    #define Display2_RXSTATUS_MASK_PTR      (  (reg8 *) Display2_BUART_sRX_RxSts__MASK_REG )
    #define Display2_RXSTATUS_ACTL_REG      (* (reg8 *) Display2_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Display2_RXSTATUS_ACTL_PTR      (  (reg8 *) Display2_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (Display2_RX_ENABLED) || (Display2_HD_ENABLED) */

#if(Display2_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define Display2_INTCLOCK_CLKEN_REG     (* (reg8 *) Display2_IntClock__PM_ACT_CFG)
    #define Display2_INTCLOCK_CLKEN_PTR     (  (reg8 *) Display2_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define Display2_INTCLOCK_CLKEN_MASK    Display2_IntClock__PM_ACT_MSK
#endif /* End Display2_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(Display2_TX_ENABLED)
    #define Display2_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End Display2_TX_ENABLED */

#if(Display2_HD_ENABLED)
    #define Display2_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End Display2_HD_ENABLED */

#if( (Display2_RX_ENABLED) || (Display2_HD_ENABLED) )
    #define Display2_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (Display2_RX_ENABLED) || (Display2_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define Display2_WAIT_1_MS      Display2_BL_CHK_DELAY_MS   

#define Display2_TXBUFFERSIZE   Display2_TX_BUFFER_SIZE
#define Display2_RXBUFFERSIZE   Display2_RX_BUFFER_SIZE

#if (Display2_RXHW_ADDRESS_ENABLED)
    #define Display2_RXADDRESSMODE  Display2_RX_ADDRESS_MODE
    #define Display2_RXHWADDRESS1   Display2_RX_HW_ADDRESS1
    #define Display2_RXHWADDRESS2   Display2_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define Display2_RXAddressMode  Display2_RXADDRESSMODE
#endif /* (Display2_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define Display2_initvar                    Display2_initVar

#define Display2_RX_Enabled                 Display2_RX_ENABLED
#define Display2_TX_Enabled                 Display2_TX_ENABLED
#define Display2_HD_Enabled                 Display2_HD_ENABLED
#define Display2_RX_IntInterruptEnabled     Display2_RX_INTERRUPT_ENABLED
#define Display2_TX_IntInterruptEnabled     Display2_TX_INTERRUPT_ENABLED
#define Display2_InternalClockUsed          Display2_INTERNAL_CLOCK_USED
#define Display2_RXHW_Address_Enabled       Display2_RXHW_ADDRESS_ENABLED
#define Display2_OverSampleCount            Display2_OVER_SAMPLE_COUNT
#define Display2_ParityType                 Display2_PARITY_TYPE

#if( Display2_TX_ENABLED && (Display2_TXBUFFERSIZE > Display2_FIFO_LENGTH))
    #define Display2_TXBUFFER               Display2_txBuffer
    #define Display2_TXBUFFERREAD           Display2_txBufferRead
    #define Display2_TXBUFFERWRITE          Display2_txBufferWrite
#endif /* End Display2_TX_ENABLED */
#if( ( Display2_RX_ENABLED || Display2_HD_ENABLED ) && \
     (Display2_RXBUFFERSIZE > Display2_FIFO_LENGTH) )
    #define Display2_RXBUFFER               Display2_rxBuffer
    #define Display2_RXBUFFERREAD           Display2_rxBufferRead
    #define Display2_RXBUFFERWRITE          Display2_rxBufferWrite
    #define Display2_RXBUFFERLOOPDETECT     Display2_rxBufferLoopDetect
    #define Display2_RXBUFFER_OVERFLOW      Display2_rxBufferOverflow
#endif /* End Display2_RX_ENABLED */

#ifdef Display2_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Display2_CONTROL                Display2_CONTROL_REG
#endif /* End Display2_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Display2_TX_ENABLED)
    #define Display2_TXDATA                 Display2_TXDATA_REG
    #define Display2_TXSTATUS               Display2_TXSTATUS_REG
    #define Display2_TXSTATUS_MASK          Display2_TXSTATUS_MASK_REG
    #define Display2_TXSTATUS_ACTL          Display2_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(Display2_TXCLKGEN_DP)
        #define Display2_TXBITCLKGEN_CTR        Display2_TXBITCLKGEN_CTR_REG
        #define Display2_TXBITCLKTX_COMPLETE    Display2_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define Display2_TXBITCTR_PERIOD        Display2_TXBITCTR_PERIOD_REG
        #define Display2_TXBITCTR_CONTROL       Display2_TXBITCTR_CONTROL_REG
        #define Display2_TXBITCTR_COUNTER       Display2_TXBITCTR_COUNTER_REG
    #endif /* Display2_TXCLKGEN_DP */
#endif /* End Display2_TX_ENABLED */

#if(Display2_HD_ENABLED)
    #define Display2_TXDATA                 Display2_TXDATA_REG
    #define Display2_TXSTATUS               Display2_TXSTATUS_REG
    #define Display2_TXSTATUS_MASK          Display2_TXSTATUS_MASK_REG
    #define Display2_TXSTATUS_ACTL          Display2_TXSTATUS_ACTL_REG
#endif /* End Display2_HD_ENABLED */

#if( (Display2_RX_ENABLED) || (Display2_HD_ENABLED) )
    #define Display2_RXDATA                 Display2_RXDATA_REG
    #define Display2_RXADDRESS1             Display2_RXADDRESS1_REG
    #define Display2_RXADDRESS2             Display2_RXADDRESS2_REG
    #define Display2_RXBITCTR_PERIOD        Display2_RXBITCTR_PERIOD_REG
    #define Display2_RXBITCTR_CONTROL       Display2_RXBITCTR_CONTROL_REG
    #define Display2_RXBITCTR_COUNTER       Display2_RXBITCTR_COUNTER_REG
    #define Display2_RXSTATUS               Display2_RXSTATUS_REG
    #define Display2_RXSTATUS_MASK          Display2_RXSTATUS_MASK_REG
    #define Display2_RXSTATUS_ACTL          Display2_RXSTATUS_ACTL_REG
#endif /* End  (Display2_RX_ENABLED) || (Display2_HD_ENABLED) */

#if(Display2_INTERNAL_CLOCK_USED)
    #define Display2_INTCLOCK_CLKEN         Display2_INTCLOCK_CLKEN_REG
#endif /* End Display2_INTERNAL_CLOCK_USED */

#define Display2_WAIT_FOR_COMLETE_REINIT    Display2_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_Display2_H */


/* [] END OF FILE */
