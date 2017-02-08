/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

// system includes
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

/*
It is advised that the Controller checks whether the response comes from
the pooled Pump.

The appropriate actions should be taken if the Pump State is not
compatible with the Controller status.
*/

//High Nibble is the current command while the Low Nibble is the PUMP ID
#define PUMP_COMMAND(c,p)   (c | p)
//High Nibble is the current state while the Low Nibble is the PUMP ID
#define ID_FROM_PUMPSTATE(ps)   (uint8)(ps & 0x0F)
//High Nibble is the current state while the Low Nibble is the PUMP ID
#define STATE_FROM_PUMPSTATE(ps)   (uint8)((ps >> 4) & 0x0F)

#define _MAX_STATE_TRANSITIONS_     6
#define _MAX_STATE_COMMANDS_        10

//It is recommended that it should be at least 5 retries.
#define _MAX_RETRIES_               10 
//No-response timer should be at least 68 msec.
#define _MAX_RESPONSE_TIMEOUT_      70 //Max milliseconds allowed to receive the response from the PUMP
#define _MAX_INTERWORD_WAIT_        _MAX_RESPONSE_TIMEOUT_
//Milliseconds to wait after a command has been posted to the PUMP
#define _MAX_INTERCOMMAND_WAIT_     8

//The MSN is always E for Data Words
#define _DATA_WORD_MASK_        0xE0
//The MSN is always F for Control Words
#define _CONTROL_WORD_MASK_     0xF0

//For a maximum of 16 PUMPS per Controller
enum _PUMP_ID_
{
    PUMP1 =  0x01,
    PUMP2 =  0x02,
    PUMP3,
    PUMP4,
    PUMP5,
    PUMP6,
    PUMP7,
    PUMP8,
    PUMP9,
    PUMP10,
    PUMP11,
    PUMP12,
    PUMP13,
    PUMP14,
    PUMP15,
    PUMP16 = 0x00
};

//For a maximum of four HOSES per PUMP. This is in case of a dispenser with two
//simultaneous deliveries (Gilbarco Veeder-Root Normal Pumps) and "Octupla" configuration.
enum _HOSE_ID_
{
    HOSE0 = _DATA_WORD_MASK_ | 0x00,
    HOSE1 = _DATA_WORD_MASK_ | 0x01,
    HOSE2 = _DATA_WORD_MASK_ | 0x02,
    HOSE3 = _DATA_WORD_MASK_ | 0x03
};

//Must be a bitwise OR operation between these commands and the respective PUMP ID
//since the MSN carries the command while the LSN carries the PUMP ID
enum _COMMAND_ID_
{
    PUMP_STATE_REQUEST                  = 0x00,
    PUMP_AUTHORIZATION                  = 0x10,
    PUMP_DATA_TO_PUMP                   = 0x20,
    PUMP_PUMP_STOP                      = 0x30,
    PUMP_TRANSACTION_DATA_REQUEST       = 0x40,
    PUMP_TOTALS_DATA_REQUEST            = 0x50,
    PUMP_REAL_TIME_MONEY_REQUEST        = 0x60,
    PUMP_REAL_TIME_HOSE_VOLUME_REQUEST  = 0x70,
    PUMP_ALL_PUMPS_STOP                 = 0xFC
};

//These are the control words required to cast the frame of data that
//will be posted/received to/from the PUMP.
enum _CONTROL_WORDS_
{    
    SOM     = 0xFF,//Start Of Message
    VPT     = 0xF1,//Volume Preset Type
    MPT     = 0xF2,//Money Preset Type
    HDn     = 0xF6,//Hose Data Next
    PUDn    = 0xF7,//PPU Data Next
    PRDn    = 0xF8,//Preset Data Next
    VDn     = 0xF9,//Volume Data Next
    MDn     = 0xFA,//Money Data Next
    LRCn    = 0xFB,//LRC Next
    SFn     = 0xFE,//Special Functions Next
    EOM     = 0xF0 //End Of Message
};

enum _PUMP_STATES_
{
    PUMP_FAIL       = 0x00,
    PUMP_IDLE       = 0x06,
    PUMP_CALLING    = 0x07,
    PUMP_AUTHORIZED = 0x08,
    PUMP_BUSY       = 0x09,
    PUMP_EOT        = 0x0B,
    PUMP_STOPPED    = 0x0C,
    //SPECIAL PUMP STATE
    PUMP_SEND_DATA  = 0x0D
};

typedef struct _PUMP_STATE_TRANSITION_MAP_
{
    uint8_t _currentstate;
    uint8_t _allowedstates[_MAX_STATE_TRANSITIONS_]; //The state that has the maximum number of transitions is BUSY (4)
} PumpStateTransitionMap;

typedef struct _PUMP_STATE_COMMAND_MAP_
{
    uint8_t _currentstate;
    uint8_t _allowedcommands[_MAX_STATE_COMMANDS_];
} PumpStateCommandMap;

typedef struct _PUMP_COMMAND_TO_ID_MAP_
{
    uint8_t _commandid;
    void (*CommandPtr)(void *pparam);
} PumpCommandIDMap;

void PumpStateRequest(void *pparam);
void PumpAuthorization(void *pparam);
void PumpDataToPump(void *pparam);
void PumpPumpStop(void *pparam);
void PumpTransactionDataRequest(void *pparam);
void PumpTotalsDataRequest(void *pparam);
void PumpRealTimeMoneyRequest(void *pparam);
void PumpRealTimeHoseVolumeRequest(void *pparam);
void PumpAllPumpsStop(void *pparam);

/* [] END OF FILE */
