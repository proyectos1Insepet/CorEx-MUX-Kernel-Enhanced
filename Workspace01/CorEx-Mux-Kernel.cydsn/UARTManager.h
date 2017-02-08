/* ========================================
 *
 * Copyright SISTEMAS INSEPET, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF SISTEMAS INSEPET.
 *
 * Created by: HIJH
 * Date: Septembre 2 de 2016
 * The UART manager implements various software patterns that allow for further flexibility,
 * scalability and evolution. It manages all of the communication processes based on the UART
 * ports and orchestrates all of the participants by administering independent state machines,
 * Callbacks and Handlers. It is based on the principle of "Separation of Concerns" and  software
 * patterns like Observer, Command and some others.
 * ========================================
*/

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"

// system includes
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#define _MAX_MESSAGES_          20   //Max number of available message slots to be used by the entire application
#define _MAX_MESSAGE_SIZE_      256 //Max message size is 1KB

#define     _UART_RETRIES_       10

//********** DEPRECATED!!!!
#define     _MESSAGE_START0_   0xaa
#define     _MESSAGE_START1_   0x55
//********** DEPRECATED!!!!
#define     INTERBYTE_DELAY         CyDelay(1);
#define     NO_INTERBYTE_DELAY

//@Structure to hold all of the necesary pointers to
//@manage a UART channel
//Only the most common function will be pointed
typedef struct _UART_MANAGER_
{
    uint8  _identifier;
    
    ///Entry point to assign custom UART handlers
    void (*CustomUARTSend)(void* pmessage);
    
    ///Standard UART methods available for the UART processing
    void (*Start)(void) ;
    void (*Stop)(void) ;
    uint8 (*ReadControlRegister)(void) ;
    void (*WriteControlRegister)(uint8 control) ;
    
    uint8 (*ReadRxData)(void);
    uint8 (*ReadRxStatus)(void);
    uint8 (*GetChar)(void);
    uint16 (*GetByte)(void);
    uint16 (*GetRxBufferSize)(void);
    void (*ClearRxBuffer)(void);
    
    void (*WriteTxData)(uint8 txDataByte);
    uint8 (*ReadTxStatus)(void);
    void (*PutChar)(uint8 txDataByte);
    void (*PutString)(const char8 string[]);
    void (*PutArray)(const uint8 string[], uint8 byteCount);
    void (*PutCRLF)(uint8 txDataByte);
    void (*ClearTxBuffer)(void);
    
} UARTManager, *PUARTMANAGERPTR;

typedef struct _UART_RESOLVER_
{
    uint8  _identifier;
    ///Entry point to assign custom UART resolver
    bool (*PtrResolver)(uint8* pbuffer, uint16 buffersize);
} UARTResolver, *PNEAR_UARTRESOLVERPTR;

typedef enum _UART_MESSAGE_STATE_
{
    //Flag to mark a busy slot in the message array
    FREE = 0xc0,
    ALLOCATED,
    PENDING
} UARTMESSAGESTATE;

typedef struct _UART_MESSAGE_
{
    volatile uint8  _messagestate;
    volatile uint16 _messagelength; //Applies to both TX and RX
    char8  _messagetx[_MAX_MESSAGE_SIZE_];
    void (*ErrorHandler)(uint8 source, char8 *pmessage) ;
    
    UARTManager *_puartmanager;    
} UARTMessage, *PUARTMESSAGEPTR;

void InitUARTManager();

UARTMessage* GetUARTMessageSlot(uint8 manager);
UARTManager* GetUARTManager(uint8 manager);
bool InvokeResolver(uint8 identifier, uint8* pbuffer, uint16 buffersize);
void UARTSinkRegistration();

///*****************************************************************************************
///MANAGER IDENTIFIERS MUST MATCH THE ORDER IN THE MANAGERS ARRAY (SEE THE UARTPorts.c FILE)
///USE THE SAME CONVENTION TO NAME THE IDENTIFIERS
typedef enum _UART_MANAGERS_AVAILABLE_
{
    UART_RF = 0xa0,
    UART_PRINTER,
    UART_DISPENSER,
    UART_DISPLAY1,
    UART_DISPLAY2,
    UART_NULL
    
} UARTMANAGERSAVAILABLE;


/* [] END OF FILE */
