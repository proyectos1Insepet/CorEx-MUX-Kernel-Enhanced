/*******************************************************************************
* File Name: Display1.h
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


#if !defined(CY_UART_Display1_H)
#define CY_UART_Display1_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"


/***************************************
* Conditional Compilation Parameters
***************************************/

#define Display1_RX_ENABLED                     (1u)
#define Display1_TX_ENABLED                     (1u)
#define Display1_HD_ENABLED                     (0u)
#define Display1_RX_INTERRUPT_ENABLED           (1u)
#define Display1_TX_INTERRUPT_ENABLED           (0u)
#define Display1_INTERNAL_CLOCK_USED            (1u)
#define Display1_RXHW_ADDRESS_ENABLED           (0u)
#define Display1_OVER_SAMPLE_COUNT              (8u)
#define Display1_PARITY_TYPE                    (0u)
#define Display1_PARITY_TYPE_SW                 (0u)
#define Display1_BREAK_DETECT                   (0u)
#define Display1_BREAK_BITS_TX                  (13u)
#define Display1_BREAK_BITS_RX                  (13u)
#define Display1_TXCLKGEN_DP                    (1u)
#define Display1_USE23POLLING                   (1u)
#define Display1_FLOW_CONTROL                   (0u)
#define Display1_CLK_FREQ                       (0u)
#define Display1_TX_BUFFER_SIZE                 (4u)
#define Display1_RX_BUFFER_SIZE                 (32u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(Display1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define Display1_CONTROL_REG_REMOVED            (0u)
#else
    #define Display1_CONTROL_REG_REMOVED            (1u)
#endif /* End Display1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct Display1_backupStruct_
{
    uint8 enableState;

    #if(Display1_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End Display1_CONTROL_REG_REMOVED */

} Display1_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void Display1_Start(void) ;
void Display1_Stop(void) ;
uint8 Display1_ReadControlRegister(void) ;
void Display1_WriteControlRegister(uint8 control) ;

void Display1_Init(void) ;
void Display1_Enable(void) ;
void Display1_SaveConfig(void) ;
void Display1_RestoreConfig(void) ;
void Display1_Sleep(void) ;
void Display1_Wakeup(void) ;

/* Only if RX is enabled */
#if( (Display1_RX_ENABLED) || (Display1_HD_ENABLED) )

    #if (Display1_RX_INTERRUPT_ENABLED)
        #define Display1_EnableRxInt()  CyIntEnable (Display1_RX_VECT_NUM)
        #define Display1_DisableRxInt() CyIntDisable(Display1_RX_VECT_NUM)
        CY_ISR_PROTO(Display1_RXISR);
    #endif /* Display1_RX_INTERRUPT_ENABLED */

    void Display1_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void Display1_SetRxAddress1(uint8 address) ;
    void Display1_SetRxAddress2(uint8 address) ;

    void  Display1_SetRxInterruptMode(uint8 intSrc) ;
    uint8 Display1_ReadRxData(void) ;
    uint8 Display1_ReadRxStatus(void) ;
    uint8 Display1_GetChar(void) ;
    uint16 Display1_GetByte(void) ;
    uint8 Display1_GetRxBufferSize(void)
                                                            ;
    void Display1_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define Display1_GetRxInterruptSource   Display1_ReadRxStatus

#endif /* End (Display1_RX_ENABLED) || (Display1_HD_ENABLED) */

/* Only if TX is enabled */
#if(Display1_TX_ENABLED || Display1_HD_ENABLED)

    #if(Display1_TX_INTERRUPT_ENABLED)
        #define Display1_EnableTxInt()  CyIntEnable (Display1_TX_VECT_NUM)
        #define Display1_DisableTxInt() CyIntDisable(Display1_TX_VECT_NUM)
        #define Display1_SetPendingTxInt() CyIntSetPending(Display1_TX_VECT_NUM)
        #define Display1_ClearPendingTxInt() CyIntClearPending(Display1_TX_VECT_NUM)
        CY_ISR_PROTO(Display1_TXISR);
    #endif /* Display1_TX_INTERRUPT_ENABLED */

    void Display1_SetTxInterruptMode(uint8 intSrc) ;
    void Display1_WriteTxData(uint8 txDataByte) ;
    uint8 Display1_ReadTxStatus(void) ;
    void Display1_PutChar(uint8 txDataByte) ;
    void Display1_PutString(const char8 string[]) ;
    void Display1_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void Display1_PutCRLF(uint8 txDataByte) ;
    void Display1_ClearTxBuffer(void) ;
    void Display1_SetTxAddressMode(uint8 addressMode) ;
    void Display1_SendBreak(uint8 retMode) ;
    uint8 Display1_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define Display1_PutStringConst         Display1_PutString
    #define Display1_PutArrayConst          Display1_PutArray
    #define Display1_GetTxInterruptSource   Display1_ReadTxStatus

#endif /* End Display1_TX_ENABLED || Display1_HD_ENABLED */

#if(Display1_HD_ENABLED)
    void Display1_LoadRxConfig(void) ;
    void Display1_LoadTxConfig(void) ;
#endif /* End Display1_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Display1) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    Display1_CyBtldrCommStart(void) CYSMALL ;
    void    Display1_CyBtldrCommStop(void) CYSMALL ;
    void    Display1_CyBtldrCommReset(void) CYSMALL ;
    cystatus Display1_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus Display1_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Display1)
        #define CyBtldrCommStart    Display1_CyBtldrCommStart
        #define CyBtldrCommStop     Display1_CyBtldrCommStop
        #define CyBtldrCommReset    Display1_CyBtldrCommReset
        #define CyBtldrCommWrite    Display1_CyBtldrCommWrite
        #define CyBtldrCommRead     Display1_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Display1) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define Display1_BYTE2BYTE_TIME_OUT (25u)
    #define Display1_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define Display1_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define Display1_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define Display1_SET_SPACE      (0x00u)
#define Display1_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (Display1_TX_ENABLED) || (Display1_HD_ENABLED) )
    #if(Display1_TX_INTERRUPT_ENABLED)
        #define Display1_TX_VECT_NUM            (uint8)Display1_TXInternalInterrupt__INTC_NUMBER
        #define Display1_TX_PRIOR_NUM           (uint8)Display1_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Display1_TX_INTERRUPT_ENABLED */

    #define Display1_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define Display1_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define Display1_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(Display1_TX_ENABLED)
        #define Display1_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (Display1_HD_ENABLED) */
        #define Display1_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (Display1_TX_ENABLED) */

    #define Display1_TX_STS_COMPLETE            (uint8)(0x01u << Display1_TX_STS_COMPLETE_SHIFT)
    #define Display1_TX_STS_FIFO_EMPTY          (uint8)(0x01u << Display1_TX_STS_FIFO_EMPTY_SHIFT)
    #define Display1_TX_STS_FIFO_FULL           (uint8)(0x01u << Display1_TX_STS_FIFO_FULL_SHIFT)
    #define Display1_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << Display1_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (Display1_TX_ENABLED) || (Display1_HD_ENABLED)*/

#if( (Display1_RX_ENABLED) || (Display1_HD_ENABLED) )
    #if(Display1_RX_INTERRUPT_ENABLED)
        #define Display1_RX_VECT_NUM            (uint8)Display1_RXInternalInterrupt__INTC_NUMBER
        #define Display1_RX_PRIOR_NUM           (uint8)Display1_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Display1_RX_INTERRUPT_ENABLED */
    #define Display1_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define Display1_RX_STS_BREAK_SHIFT             (0x01u)
    #define Display1_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define Display1_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define Display1_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define Display1_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define Display1_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define Display1_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define Display1_RX_STS_MRKSPC           (uint8)(0x01u << Display1_RX_STS_MRKSPC_SHIFT)
    #define Display1_RX_STS_BREAK            (uint8)(0x01u << Display1_RX_STS_BREAK_SHIFT)
    #define Display1_RX_STS_PAR_ERROR        (uint8)(0x01u << Display1_RX_STS_PAR_ERROR_SHIFT)
    #define Display1_RX_STS_STOP_ERROR       (uint8)(0x01u << Display1_RX_STS_STOP_ERROR_SHIFT)
    #define Display1_RX_STS_OVERRUN          (uint8)(0x01u << Display1_RX_STS_OVERRUN_SHIFT)
    #define Display1_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << Display1_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define Display1_RX_STS_ADDR_MATCH       (uint8)(0x01u << Display1_RX_STS_ADDR_MATCH_SHIFT)
    #define Display1_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << Display1_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define Display1_RX_HW_MASK                     (0x7Fu)
#endif /* End (Display1_RX_ENABLED) || (Display1_HD_ENABLED) */

/* Control Register definitions */
#define Display1_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define Display1_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define Display1_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define Display1_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define Display1_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define Display1_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define Display1_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define Display1_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define Display1_CTRL_HD_SEND               (uint8)(0x01u << Display1_CTRL_HD_SEND_SHIFT)
#define Display1_CTRL_HD_SEND_BREAK         (uint8)(0x01u << Display1_CTRL_HD_SEND_BREAK_SHIFT)
#define Display1_CTRL_MARK                  (uint8)(0x01u << Display1_CTRL_MARK_SHIFT)
#define Display1_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << Display1_CTRL_PARITY_TYPE0_SHIFT)
#define Display1_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << Display1_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define Display1_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define Display1_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define Display1_SEND_BREAK                         (0x00u)
#define Display1_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define Display1_REINIT                             (0x02u)
#define Display1_SEND_WAIT_REINIT                   (0x03u)

#define Display1_OVER_SAMPLE_8                      (8u)
#define Display1_OVER_SAMPLE_16                     (16u)

#define Display1_BIT_CENTER                         (Display1_OVER_SAMPLE_COUNT - 2u)

#define Display1_FIFO_LENGTH                        (4u)
#define Display1_NUMBER_OF_START_BIT                (1u)
#define Display1_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define Display1_TXBITCTR_BREAKBITS8X   ((Display1_BREAK_BITS_TX * Display1_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define Display1_TXBITCTR_BREAKBITS ((Display1_BREAK_BITS_TX * Display1_OVER_SAMPLE_COUNT) - 1u)

#define Display1_HALF_BIT_COUNT   \
                            (((Display1_OVER_SAMPLE_COUNT / 2u) + (Display1_USE23POLLING * 1u)) - 2u)
#if (Display1_OVER_SAMPLE_COUNT == Display1_OVER_SAMPLE_8)
    #define Display1_HD_TXBITCTR_INIT   (((Display1_BREAK_BITS_TX + \
                            Display1_NUMBER_OF_START_BIT) * Display1_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define Display1_RXBITCTR_INIT  ((((Display1_BREAK_BITS_RX + Display1_NUMBER_OF_START_BIT) \
                            * Display1_OVER_SAMPLE_COUNT) + Display1_HALF_BIT_COUNT) - 1u)

#else /* Display1_OVER_SAMPLE_COUNT == Display1_OVER_SAMPLE_16 */
    #define Display1_HD_TXBITCTR_INIT   ((8u * Display1_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define Display1_RXBITCTR_INIT      (((7u * Display1_OVER_SAMPLE_COUNT) - 1u) + \
                                                      Display1_HALF_BIT_COUNT)
#endif /* End Display1_OVER_SAMPLE_COUNT */

#define Display1_HD_RXBITCTR_INIT                   Display1_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 Display1_initVar;
#if (Display1_TX_INTERRUPT_ENABLED && Display1_TX_ENABLED)
    extern volatile uint8 Display1_txBuffer[Display1_TX_BUFFER_SIZE];
    extern volatile uint8 Display1_txBufferRead;
    extern uint8 Display1_txBufferWrite;
#endif /* (Display1_TX_INTERRUPT_ENABLED && Display1_TX_ENABLED) */
#if (Display1_RX_INTERRUPT_ENABLED && (Display1_RX_ENABLED || Display1_HD_ENABLED))
    extern uint8 Display1_errorStatus;
    extern volatile uint8 Display1_rxBuffer[Display1_RX_BUFFER_SIZE];
    extern volatile uint8 Display1_rxBufferRead;
    extern volatile uint8 Display1_rxBufferWrite;
    extern volatile uint8 Display1_rxBufferLoopDetect;
    extern volatile uint8 Display1_rxBufferOverflow;
    #if (Display1_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 Display1_rxAddressMode;
        extern volatile uint8 Display1_rxAddressDetected;
    #endif /* (Display1_RXHW_ADDRESS_ENABLED) */
#endif /* (Display1_RX_INTERRUPT_ENABLED && (Display1_RX_ENABLED || Display1_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define Display1__B_UART__AM_SW_BYTE_BYTE 1
#define Display1__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define Display1__B_UART__AM_HW_BYTE_BY_BYTE 3
#define Display1__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define Display1__B_UART__AM_NONE 0

#define Display1__B_UART__NONE_REVB 0
#define Display1__B_UART__EVEN_REVB 1
#define Display1__B_UART__ODD_REVB 2
#define Display1__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define Display1_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define Display1_NUMBER_OF_STOP_BITS    (1u)

#if (Display1_RXHW_ADDRESS_ENABLED)
    #define Display1_RX_ADDRESS_MODE    (0u)
    #define Display1_RX_HW_ADDRESS1     (0u)
    #define Display1_RX_HW_ADDRESS2     (0u)
#endif /* (Display1_RXHW_ADDRESS_ENABLED) */

#define Display1_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << Display1_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << Display1_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << Display1_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << Display1_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << Display1_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << Display1_RX_STS_BREAK_SHIFT) \
                                        | (0 << Display1_RX_STS_OVERRUN_SHIFT))

#define Display1_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << Display1_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << Display1_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << Display1_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << Display1_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef Display1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Display1_CONTROL_REG \
                            (* (reg8 *) Display1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define Display1_CONTROL_PTR \
                            (  (reg8 *) Display1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End Display1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Display1_TX_ENABLED)
    #define Display1_TXDATA_REG          (* (reg8 *) Display1_BUART_sTX_TxShifter_u0__F0_REG)
    #define Display1_TXDATA_PTR          (  (reg8 *) Display1_BUART_sTX_TxShifter_u0__F0_REG)
    #define Display1_TXDATA_AUX_CTL_REG  (* (reg8 *) Display1_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Display1_TXDATA_AUX_CTL_PTR  (  (reg8 *) Display1_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Display1_TXSTATUS_REG        (* (reg8 *) Display1_BUART_sTX_TxSts__STATUS_REG)
    #define Display1_TXSTATUS_PTR        (  (reg8 *) Display1_BUART_sTX_TxSts__STATUS_REG)
    #define Display1_TXSTATUS_MASK_REG   (* (reg8 *) Display1_BUART_sTX_TxSts__MASK_REG)
    #define Display1_TXSTATUS_MASK_PTR   (  (reg8 *) Display1_BUART_sTX_TxSts__MASK_REG)
    #define Display1_TXSTATUS_ACTL_REG   (* (reg8 *) Display1_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define Display1_TXSTATUS_ACTL_PTR   (  (reg8 *) Display1_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(Display1_TXCLKGEN_DP)
        #define Display1_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) Display1_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Display1_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) Display1_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Display1_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) Display1_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define Display1_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) Display1_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define Display1_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) Display1_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Display1_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) Display1_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Display1_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Display1_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Display1_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Display1_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Display1_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) Display1_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define Display1_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) Display1_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* Display1_TXCLKGEN_DP */

#endif /* End Display1_TX_ENABLED */

#if(Display1_HD_ENABLED)

    #define Display1_TXDATA_REG             (* (reg8 *) Display1_BUART_sRX_RxShifter_u0__F1_REG )
    #define Display1_TXDATA_PTR             (  (reg8 *) Display1_BUART_sRX_RxShifter_u0__F1_REG )
    #define Display1_TXDATA_AUX_CTL_REG     (* (reg8 *) Display1_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define Display1_TXDATA_AUX_CTL_PTR     (  (reg8 *) Display1_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Display1_TXSTATUS_REG           (* (reg8 *) Display1_BUART_sRX_RxSts__STATUS_REG )
    #define Display1_TXSTATUS_PTR           (  (reg8 *) Display1_BUART_sRX_RxSts__STATUS_REG )
    #define Display1_TXSTATUS_MASK_REG      (* (reg8 *) Display1_BUART_sRX_RxSts__MASK_REG )
    #define Display1_TXSTATUS_MASK_PTR      (  (reg8 *) Display1_BUART_sRX_RxSts__MASK_REG )
    #define Display1_TXSTATUS_ACTL_REG      (* (reg8 *) Display1_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Display1_TXSTATUS_ACTL_PTR      (  (reg8 *) Display1_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End Display1_HD_ENABLED */

#if( (Display1_RX_ENABLED) || (Display1_HD_ENABLED) )
    #define Display1_RXDATA_REG             (* (reg8 *) Display1_BUART_sRX_RxShifter_u0__F0_REG )
    #define Display1_RXDATA_PTR             (  (reg8 *) Display1_BUART_sRX_RxShifter_u0__F0_REG )
    #define Display1_RXADDRESS1_REG         (* (reg8 *) Display1_BUART_sRX_RxShifter_u0__D0_REG )
    #define Display1_RXADDRESS1_PTR         (  (reg8 *) Display1_BUART_sRX_RxShifter_u0__D0_REG )
    #define Display1_RXADDRESS2_REG         (* (reg8 *) Display1_BUART_sRX_RxShifter_u0__D1_REG )
    #define Display1_RXADDRESS2_PTR         (  (reg8 *) Display1_BUART_sRX_RxShifter_u0__D1_REG )
    #define Display1_RXDATA_AUX_CTL_REG     (* (reg8 *) Display1_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Display1_RXBITCTR_PERIOD_REG    (* (reg8 *) Display1_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Display1_RXBITCTR_PERIOD_PTR    (  (reg8 *) Display1_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Display1_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Display1_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Display1_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Display1_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Display1_RXBITCTR_COUNTER_REG   (* (reg8 *) Display1_BUART_sRX_RxBitCounter__COUNT_REG )
    #define Display1_RXBITCTR_COUNTER_PTR   (  (reg8 *) Display1_BUART_sRX_RxBitCounter__COUNT_REG )

    #define Display1_RXSTATUS_REG           (* (reg8 *) Display1_BUART_sRX_RxSts__STATUS_REG )
    #define Display1_RXSTATUS_PTR           (  (reg8 *) Display1_BUART_sRX_RxSts__STATUS_REG )
    #define Display1_RXSTATUS_MASK_REG      (* (reg8 *) Display1_BUART_sRX_RxSts__MASK_REG )
    #define Display1_RXSTATUS_MASK_PTR      (  (reg8 *) Display1_BUART_sRX_RxSts__MASK_REG )
    #define Display1_RXSTATUS_ACTL_REG      (* (reg8 *) Display1_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Display1_RXSTATUS_ACTL_PTR      (  (reg8 *) Display1_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (Display1_RX_ENABLED) || (Display1_HD_ENABLED) */

#if(Display1_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define Display1_INTCLOCK_CLKEN_REG     (* (reg8 *) Display1_IntClock__PM_ACT_CFG)
    #define Display1_INTCLOCK_CLKEN_PTR     (  (reg8 *) Display1_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define Display1_INTCLOCK_CLKEN_MASK    Display1_IntClock__PM_ACT_MSK
#endif /* End Display1_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(Display1_TX_ENABLED)
    #define Display1_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End Display1_TX_ENABLED */

#if(Display1_HD_ENABLED)
    #define Display1_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End Display1_HD_ENABLED */

#if( (Display1_RX_ENABLED) || (Display1_HD_ENABLED) )
    #define Display1_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (Display1_RX_ENABLED) || (Display1_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define Display1_WAIT_1_MS      Display1_BL_CHK_DELAY_MS   

#define Display1_TXBUFFERSIZE   Display1_TX_BUFFER_SIZE
#define Display1_RXBUFFERSIZE   Display1_RX_BUFFER_SIZE

#if (Display1_RXHW_ADDRESS_ENABLED)
    #define Display1_RXADDRESSMODE  Display1_RX_ADDRESS_MODE
    #define Display1_RXHWADDRESS1   Display1_RX_HW_ADDRESS1
    #define Display1_RXHWADDRESS2   Display1_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define Display1_RXAddressMode  Display1_RXADDRESSMODE
#endif /* (Display1_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define Display1_initvar                    Display1_initVar

#define Display1_RX_Enabled                 Display1_RX_ENABLED
#define Display1_TX_Enabled                 Display1_TX_ENABLED
#define Display1_HD_Enabled                 Display1_HD_ENABLED
#define Display1_RX_IntInterruptEnabled     Display1_RX_INTERRUPT_ENABLED
#define Display1_TX_IntInterruptEnabled     Display1_TX_INTERRUPT_ENABLED
#define Display1_InternalClockUsed          Display1_INTERNAL_CLOCK_USED
#define Display1_RXHW_Address_Enabled       Display1_RXHW_ADDRESS_ENABLED
#define Display1_OverSampleCount            Display1_OVER_SAMPLE_COUNT
#define Display1_ParityType                 Display1_PARITY_TYPE

#if( Display1_TX_ENABLED && (Display1_TXBUFFERSIZE > Display1_FIFO_LENGTH))
    #define Display1_TXBUFFER               Display1_txBuffer
    #define Display1_TXBUFFERREAD           Display1_txBufferRead
    #define Display1_TXBUFFERWRITE          Display1_txBufferWrite
#endif /* End Display1_TX_ENABLED */
#if( ( Display1_RX_ENABLED || Display1_HD_ENABLED ) && \
     (Display1_RXBUFFERSIZE > Display1_FIFO_LENGTH) )
    #define Display1_RXBUFFER               Display1_rxBuffer
    #define Display1_RXBUFFERREAD           Display1_rxBufferRead
    #define Display1_RXBUFFERWRITE          Display1_rxBufferWrite
    #define Display1_RXBUFFERLOOPDETECT     Display1_rxBufferLoopDetect
    #define Display1_RXBUFFER_OVERFLOW      Display1_rxBufferOverflow
#endif /* End Display1_RX_ENABLED */

#ifdef Display1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Display1_CONTROL                Display1_CONTROL_REG
#endif /* End Display1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Display1_TX_ENABLED)
    #define Display1_TXDATA                 Display1_TXDATA_REG
    #define Display1_TXSTATUS               Display1_TXSTATUS_REG
    #define Display1_TXSTATUS_MASK          Display1_TXSTATUS_MASK_REG
    #define Display1_TXSTATUS_ACTL          Display1_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(Display1_TXCLKGEN_DP)
        #define Display1_TXBITCLKGEN_CTR        Display1_TXBITCLKGEN_CTR_REG
        #define Display1_TXBITCLKTX_COMPLETE    Display1_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define Display1_TXBITCTR_PERIOD        Display1_TXBITCTR_PERIOD_REG
        #define Display1_TXBITCTR_CONTROL       Display1_TXBITCTR_CONTROL_REG
        #define Display1_TXBITCTR_COUNTER       Display1_TXBITCTR_COUNTER_REG
    #endif /* Display1_TXCLKGEN_DP */
#endif /* End Display1_TX_ENABLED */

#if(Display1_HD_ENABLED)
    #define Display1_TXDATA                 Display1_TXDATA_REG
    #define Display1_TXSTATUS               Display1_TXSTATUS_REG
    #define Display1_TXSTATUS_MASK          Display1_TXSTATUS_MASK_REG
    #define Display1_TXSTATUS_ACTL          Display1_TXSTATUS_ACTL_REG
#endif /* End Display1_HD_ENABLED */

#if( (Display1_RX_ENABLED) || (Display1_HD_ENABLED) )
    #define Display1_RXDATA                 Display1_RXDATA_REG
    #define Display1_RXADDRESS1             Display1_RXADDRESS1_REG
    #define Display1_RXADDRESS2             Display1_RXADDRESS2_REG
    #define Display1_RXBITCTR_PERIOD        Display1_RXBITCTR_PERIOD_REG
    #define Display1_RXBITCTR_CONTROL       Display1_RXBITCTR_CONTROL_REG
    #define Display1_RXBITCTR_COUNTER       Display1_RXBITCTR_COUNTER_REG
    #define Display1_RXSTATUS               Display1_RXSTATUS_REG
    #define Display1_RXSTATUS_MASK          Display1_RXSTATUS_MASK_REG
    #define Display1_RXSTATUS_ACTL          Display1_RXSTATUS_ACTL_REG
#endif /* End  (Display1_RX_ENABLED) || (Display1_HD_ENABLED) */

#if(Display1_INTERNAL_CLOCK_USED)
    #define Display1_INTCLOCK_CLKEN         Display1_INTCLOCK_CLKEN_REG
#endif /* End Display1_INTERNAL_CLOCK_USED */

#define Display1_WAIT_FOR_COMLETE_REINIT    Display1_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_Display1_H */


/* [] END OF FILE */
