/* ========================================
 *
 * Copyright INSEPET SISTEMAS, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF SISTEMAS INSEPET.
 *
 * ========================================
@Created By: HIJH
@Date: Septembre de 2016
*/

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"

// system includes
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#define _RF_STREAM_TARGET_INDEX0_                   0x03
#define _RF_STREAM_TARGET_INDEX1_                   0x04
#define _RF_STREAM_POSITION_INDEX_                  0x05
#define _RF_STREAM_COMMAND_INDEX_                   0x06

#define _RF_STREAM_FIXED_LENGTH_HEADER_BLOCK_       0x07

#define _RF_ACK_                        0x03
#define _RF_NACK_                       0x04

#define _RF_MAX_TIMEOUT_LIMIT_          50 //Maximum time to respond to the remote peer
#define _RF_NO_TIMEOUT_                 0

typedef enum
{
    RF_MUX_STATE_REQUEST   = 0xA1,
    RF_MUX_STATE_RESPONSE  = 0xA1,

    RF_MUX_CREDIT_SERIAL_AUTH_REQUEST   = 0xA2,
    RF_MUX_CREDIT_SERIAL_AUTH_RESPONSE  = 0xA2,
    
    RF_MUX_CREDIT_PRESET_AUTHORIZATION_REQUEST   = 0xA3,
    RF_MUX_CREDIT_PRESET_AUTHORIZATION_RESPONSE  = 0xA3,

    RF_MUX_CREDIT_PRESET_NOTIFICATION_REQUEST   = 0xAE,
    RF_MUX_CREDIT_PRESET_NOTIFICATION_RESPONSE  = 0xAE,
    
    RF_MUX_CASH_SALE_START_REQUEST   = 0xAA,
    RF_MUX_CASH_SALE_START_RESPONSE  = 0xAA,
    
    RF_MUX_CASH_SALE_END_REPORT_REQUEST   = 0xA4,
    RF_MUX_CASH_SALE_END_REPORT_RESPONSE  = 0xA4,
    
    RF_MUX_TOTALS_REQUEST   = 0xA5,
    RF_MUX_TOTALS_RESPONSE  = 0xA5,

    RF_MUX_TOTALS_REPORT_REQUEST   = 0xE8,
    RF_MUX_TOTALS_REPORT_RESPONSE  = 0xE8,
    
    RF_MUX_PRICE_CHANGE_REQUEST   = 0xA6,
    RF_MUX_PRICE_CHANGE_RESPONSE  = 0xA6,

    RF_MUX_GENERAL_PRINT_REQUEST   = 0xA7,
    RF_MUX_GENERAL_PRINT_RESPONSE  = 0xA7,
    
    RF_MUX_CREDIT_SALE_PRINT_REQUEST   = 0xA8,
    RF_MUX_CREDIT_SALE_PRINT_RESPONSE  = 0xA8,
    
    RF_MUX_CASH_SALE_PRINT_REQUEST   = 0xA9,
    RF_MUX_CASH_SALE_PRINT_RESPONSE  = 0xA9,

    RF_MUX_PENDING_CREDIT_SALE_REQUEST   = 0xAC,
    RF_MUX_PENDING_CREDIT_SALE_RESPONSE  = 0xAC,

    RF_MUX_LAST_CASH_SALE_REQUEST   = 0xAD,
    RF_MUX_LAST_CASH_SALE_RESPONSE  = 0xAD,
    
    RF_MUX_BROADCAST_CFG_REQUEST   = 0xE1,
    RF_MUX_BROADCAST_CFG_RESPONSE  = 0xE1,

    RF_MUX_POSITION_CFG_REQUEST   = 0xE2,
    RF_MUX_POSITION_CFG_RESPONSE  = 0xE2,

    RF_MUX_ISLAND_OPER_DATA_REQUEST   = 0xE3,
    RF_MUX_ISLAND_OPER_DATA_RESPONSE  = 0xE3,
    
    RF_MUX_ISLAND_ADMIN_REQUEST   = 0xE4,
    RF_MUX_ISLAND_ADMIN_RESPONSE  = 0xE4,
    
    RF_MUX_COPY_REQUEST   = 0xE6,
    RF_MUX_COPY_REQUEST_RESPONSE  = 0xE6,
    
    ////////////////////////////////////////////////
    //RESERVED FOR FUTURE USE
    ////////////////////////////////////////////////
    
    RF_MUX_NULL     = 0xFF
    
} RFCommands;

typedef bool (*RFReqRespFnPtr)(void *pparam);
typedef void (*ErrorHandlerFnPtr)(void *pparam);
//This structure is in charge of receiving every single communication stream coming
//from the remote peer (CDG), parsing, processing and responding over the same channel
//This is a traditional Client / Server model that opens and closes a communication port
typedef struct
{
    uint8 _requestcode;
    uint8 _responsecode;
    
    RFReqRespFnPtr _prequestresponse;
    ErrorHandlerFnPtr _perror;
    
    uint8 _responsetimeoutlimit;//Maximum time to respond to the peer
    
} RFRequestResponse, *PNEAR_REQRESPTR;

typedef uint16 (*PayloadSizeResolver)(void *pbuffer);

typedef struct
{
    uint8   _requestcode;
    uint16  _requestpayloadsize;
    PayloadSizeResolver _fpPayloadSizeResolver;
    
} RFRequestPayloadLengthMap, *PNEAR_RFREQUESTPAYLOADLENGTHMAP;

typedef struct
{
    uint16 _queueindex;
    void*  _prefmessage;
    
} RFServerNotificationQueue, *PNEAR_RFSERVERNOTIFICATIONQUEUEPTR;

enum _RF_STATES_
{
    RF_IDLE = 0x08,
    RF_DELIVERING = 0x01,
    RF_CASHSALEREPORT = 0x02,
    RF_CREDITSALEAUTH = 0x03,
    RF_WORKSHIFTREQ = 0x05,
    RF_CREDITSALEREPORT = 0x06,
    RF_ERROR = 0x04,
    RF_COPY_RECEIPT = 0x0B,
    RF_ZERO_SALE = 0x0C
};

typedef struct
{
    uint8 _stateid;
    uint8 _commandid;
    
} RFCommandStateMap, *PNEAR_RFCOMMANDSTATEMAPPTR;

enum _PARSE_RESULT_
{
    PARSE_FAILED = 0x00,
    PARSE_SUCCEDED = 0x01
};

typedef struct
{
    volatile uint16 _rfcounter;
    volatile uint32 _rftimeoutmultiplier;
    volatile uint32 _rfperpetualcounter;
    
} RFStreamInfo, *PNEAR_RFSTREAMINFOPTR;

enum _RF_AUTONOMOUS_MODE_
{
    RF_AUTONOMOUS = 0x01,
    RF_NON_AUTONOMOUS
};

typedef struct
{
    volatile uint8  _state;
    volatile uint16 _toautonomoustimetoken;
    volatile uint8  _timeouttoautonomous;
    
    volatile uint16 _timeouttoreceive;
    volatile uint16 _timeouttoreceivemultiplier;
} RFAutoRun, *PNEAR_AUTORUNPTR;


extern uint8 _g_rfslaveheader[];
extern uint8 _g_rfmasterheader[];
extern uint8 _g_rfmasterfooter[];

extern RFRequestResponse _g_rfresolvers[];

extern RFAutoRun _g_rfautorun;

uint8 RFGetCRC(uint8 commandid, uint8 *pbuffer);
uint8 RFUnmapCommand2State(uint8 commandid);
PNEAR_REQRESPTR GetRFRequestResolver(uint8 commandid);

extern RFStreamInfo _g_rfrx;

/* [] END OF FILE */
