/* ========================================
 *
 * Copyright INSEPET SISTEMAS, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//******************************* PARAMETRIC CONFIGURATION STARTS HERE!!!!
/*
It is advised that the Controller checks whether the response comes from
the pooled Pump.

The appropriate actions should be taken if the Pump State is not
compatible with the Controller status.
*/

typedef char  *PSTR;
typedef uint8 *LPRAWPTR;
#define PSTRCAST(a)     ((char*)a)
#define PBYTECAST(a)     ((uint8*)a)

//This is the buffer span for each block (hose) in the response to the "PUMP TOTALS DATA" request
#define _PUMP_TOTALS_BUFFER_SHIFT_          42
#define _PUMP_TOTALS_BUFFER_SHIFT_4DIGITS_  30

//High Nibble is the current command while the Low Nibble is the PUMP ID
#define PUMP_COMMAND(c,p)   ((c & 0xF0) | (p & 0x0F))
//High Nibble is the current state while the Low Nibble is the PUMP ID
#define ID_FROM_PUMPSTATE(ps)   (uint8)(ps & 0x0F)
//High Nibble is the current state while the Low Nibble is the PUMP ID
#define STATE_FROM_PUMPSTATE(ps)   (uint8)((ps >> 4) & 0x0F)

#define _PUMP_RX_BUFFER_SIZE_               384
#define _PUMP_TRANSACTION_BUFFER_SIZE_      384

//It is recommended that it should be at least 5 retries.
#define _MAX_RETRIES_               20 
//Timeout defined for Long Running Jobs (LRJ)
#define _MAX_REPUSH_TIMEOUT_        600000 //Max milliseconds allowed for a transactional job to repush over (long running jobs)
#define _MIN_REPUSH_TIMEOUT_        30000 //Max milliseconds allowed for a transactional job to repush over (long running jobs)
//No-response timer should be at least 68 msec.
#define _MAX_RESPONSE_TIMEOUT_        80 //Max milliseconds allowed to receive the response from the PUMP
#define _EXT_RESPONSE_TIMEOUT_        100 //Max milliseconds allowed to receive the response from the PUMP

#define _MAX_TRANSACTION_RESPONSE_TIMEOUT_  500
#define _MAX_TOTALS_RESPONSE_TIMEOUT_       1000

#define _MAX_TRANSACTION_RESPONSE_TIMEOUT_EX_  500
#define _MAX_TOTALS_RESPONSE_TIMEOUT_EX_       1000

#define _NO_TIMEOUT_                0x0000
#define _NO_REPUSH_                 _NO_TIMEOUT_
#define _INVOKE_DELAY_              100 //Time to wait before invoke of the Request
#define _NO_DELAY_                  0x0000 //Time to wait before invoke of the Request

#define _REPUSH_ACTIVATED_          false
#define _RETRY_ACTIVATED_           true

//Milliseconds to wait after a command has been posted to the PUMP
#define _INTERWORD_WAIT_         CyDelay(1)
#define _INTERFRAME_WAIT_        CyDelay(50)

//MAXIMUM AMOUNTS FOR MONEY PRESETS
#define _PUMP_MAXIMUM_PRESET_7_DIGITS_      9990000
#define _PUMP_MAXIMUM_PRESET_6_DIGITS_      990000
#define _PUMP_MAXIMUM_PRESET_5_DIGITS_      99000
#define _PUMP_MAXIMUM_PRESET_4_DIGITS_      9900

//The MSN is always E for Data Words
#define _DATA_WORD_MASK_        0xE0
//The MSN is always F for Control Words
#define _CONTROL_WORD_MASK_     0xF0

#define _MAX_NUMBER_OF_PUMPS_        4
#define _PUMP_JOB_QUEUE_SIZE_        48

#define _PUMP_MAX_QUEUE_TRANSTATES_  8
#define _PUMP_TRANSTATE_BUFFER_SIZE_ 256

#define _VOLUME_PRESET_MULTIPLIER       1000

#define _PUMP_MAXIMUM_PRESET_GALLONS_       500

enum _DECIMAL_SEPARATOR_
{
    _POINT_AS_DECIMAL_SEPARATOR_ = 0x00,
    _COMMA_AS_DECIMAL_SEPARATOR_ = 0x01
};


//For a maximum of four HOSES per PUMP. This is in case of a dispenser with two
//simultaneous deliveries (Gilbarco Veeder-Root Normal Pumps) and "Octupla" configuration.
enum _HOSE_ID_
{
    HOSE0 = _DATA_WORD_MASK_ | 0x00,
    HOSE1 = _DATA_WORD_MASK_ | 0x01,
    HOSE2 = _DATA_WORD_MASK_ | 0x02,
    HOSE3 = _DATA_WORD_MASK_ | 0x03,
    ///////////////////////////////////////////////////////////////////////////
    //CAREFUL SINCE "DATA TO PUMP" COMMAND WILL FAIL IF
    //DATA WORDS AREN'T APPROPRIATELY FORMATED (SEE _DATA_WORD_MASK_ DEFINITION)
    ///////////////////////////////////////////////////////////////////////////
    HOSEUNKNOWN =               0xFF
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
    PUMP_ALL_PUMPS_STOP                 = 0xFC,    
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
    PUMP_PEOT       = 0x0A,
    PUMP_FEOT       = 0x0B,
    PUMP_STOPPED    = 0x0C,
    //SPECIAL PUMP STATE
    PUMP_SEND_DATA  = 0x0D,

    ///Set upon initialization
    PUMP_UNKNOWN    = 0xFF
};

#define PUMP_NO_RESPONSE_SIZE                                   0x00
#define PUMP_PUMP_STOP_RESPONSE_SIZE                            0x00
#define PUMP_ALLPUMPS_STOP_RESPONSE_SIZE                        0x00
#define PUMP_AUTHORIZATION_RESPONSE_SIZE                        0x00
#define PUMP_STATE_RESPONSE_SIZE                                0x01
#define PUMP_DATA_TO_PUMP_RESPONSE_SIZE                         0x01
#define PUMP_TRANSACTION_DATA_REQUEST_RESPONSE_SIZE             0x27 //39 Bytes
#define PUMP_TRANSACTION_DATA_REQUEST_RESPONSE_SIZE_4DIGITS     0x21 //33 Bytes
#define PUMP_REALTIME_MONEY_REQUEST_RESPONSE_SIZE               0x08
#define PUMP_REALTIME_HOSE_VOLUME_REQUEST_RESPONSE_SIZE         0x09

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//WHEN SENDING THE COMMAND "TOTALS DATA REQUEST" TO A PUMP, IT WILL RESPOND WITH A VARIABLE DATA LENGTH
//THAT DEPENDS ON THE NUMBER OF HOSES PER PUMP. THE DATA LENGTH FOR ONE HOSE IS 42 BYTES. TO RETRIEVE
//THE INFORMATION FOR THE REMAINING HOSES, ALL OF THE INDEXES MUST BE SHIFTED BY "42". THE FOLLOWING MACRO
//MAKES THE CALCULATION

//TDR: TOTALS DATA REQUEST
//h: Hose index {0, 1, 2, 3}
//p: Position within the data stream
#define PUMP_TDR_HOSE_DATA(h,p)     (p + (42 * h))
#define PUMP_TDR_HOSE_DATA_4D(h,p)  (p + (30 * h))
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//This structure holds the entire PUMP layout. Since we are dealing with two basic 
//layouts, these are "Two simultaneous deliveries / Four simultaneous deliveries".
//All of them support the following hose configurations:
/******************************************************************
Normal Pumps (2 simultaneous deliveries) => Two PUMPS
*******************************************************************
Dual, Dupla
Side 1 = 1A Side 2 = 2A
Quádrupla
Side 1 Hose A = 1A Side 2 Hose A = 2A
Side 1 Hose B = 1B Side 2 Hose B = 2B
Sêxtupla
Side 1 Hose A = 1A Side 2 Hose A = 2A
Side 1 Hose B = 1B Side 2 Hose B = 2B
Side 1 Hose C = 1C Side 2 Hose C = 2C
Óctupla
Side 1 Hose A = 1A Side 2 Hose A = 2A
Side 1 Hose B = 1B Side 2 Hose B = 2B
Side 1 Hose C = 1C Side 2 Hose C = 2C
Side 1 Hose D = 1D Side 2 Hose D = 2D

*******************************************************************
QUATTRO Pumps (4 simultaneous deliveries) => Four PUMPS
*******************************************************************
Quádrupla QUATTRO
Side 1 Hose A = 1A Side 2 Hose A = 2A
Side 1 Hose B = 3A Side 2 Hose B = 4A
(Pump ID 1 & 2 will behave in communications like a Dual Pump)
(Pump ID 3 & 4 will behave in communications like a Dual Pump)
Sêxtupla QUATTRO
Side 1 Hose A = 1A Side 2 Hose A = 2A
Side 1 Hose B = 1B Side 2 Hose B = 2B
Side 1 Hose C = 3A Side 2 Hose C = 4A
(Pump ID 1 & 2 will behave in communications like a Quádrupla Pump)
(Pump ID 3 & 4 will behave in communications like a Dual Pump)
Óctupla QUATTRO
Side 1 Hose A = 1A Side 2 Hose A = 2A
Side 1 Hose B = 1B Side 2 Hose B = 2B
Side 1 Hose C = 3A Side 2 Hose C = 4A
Side 1 Hose D = 3B Side 2 Hose D = 4B
(Pump ID 1 & 2 will behave in communications like a Quádrupla Pump)
(Pump ID 3 & 4 will behave in communications like a Quádrupla Pump)
*/
//Pump Transactional Identifiers
enum _TRANSACTION_NAMES_
{
    _PUMP_FILL_BY_MONEY_ = 0x0F,//This code is raised from the screen for the type of programming
    _PUMP_FILL_BY_VOLUME_ = 0x10,//This code is raised from the screen for the type of programming
    _PUMP_FILL_FULL_ = 0x43,//This code is raised from the screen for the type of programming
    _PUMP_FILL_CREDIT_,//This code is raised from the screen for the type of sale
    _PUMP_PPU_CHANGE_,
    _PUMP_RESTORE_PRICE_,
    _PUMP_TOTALS_,
    _PUMP_STATE_,
    _PUMP_EOT_,
    _PUMP_EOT_STARTUP_,
    _PUMP_NETWORK_ENUMERATOR_,
    _PUMP_STOP_,
    _PUMP_STOPALL_,
    _PUMP_COMPLETE_CONFIGURATION_,
    _PUMP_PROGRAM_MODE_,
};

enum _PUMP_TRANS_STATE_
{
    _PUMP_BUSY_ = 0x01,
    _PUMP_IDLE_,
    _PUMP_UNKNOWN_,
};

enum _PUMP_ROUNDROBIN_STATE_
{
    _PUMP_RR_ACTIVE_ = 0x01,
    _PUMP_RR_INACTIVE_
};

enum _PUMP_TRANS_HEALTH_
{
    _PUMP_OK_ = 0x01,
    _PUMP_FAIL_
};

enum _PUMP_TRANSTATE_NODE_STATE_
{
    _PUMP_TRANSTATE_FREE_,
    _PUMP_TRANSTATE_ALLOCATED_,
    _PUMP_TRANSTATE_BUSY_
};

typedef struct
{
    uint8  _state;
    uint8  _transtate;
    char   _buffer[_PUMP_TRANSTATE_BUFFER_SIZE_];
    uint16 _buffersize;
    
} PumpTransactionalState, *PNEAR_PUMPTRANSACTIONALSTATEPTR;

typedef struct
{
    volatile uint8 _var0;
    volatile uint8 _var1;
    
    volatile uint16 _var2;
    volatile uint16 _var3;

    volatile uint32 _var4;
    volatile uint32 _var5;

    //This 64 bit variables are mainly used to store the pump totals per hose in a QUATTRO dispenser
    //since there are maximum 2 hoses per position/pump
    //In an Encore Dispenser the number of variables should be duplicated since there could be 4 hoses per position/pump
    volatile uint64 _var6;
    volatile uint64 _var7;
    volatile uint64 _var8;
    volatile uint64 _var9;
    
    volatile double _varA;
    volatile double _varB;
    
} PumpDataContainer;

enum _PUMP_PRESET_TYPE_
{
    PRESET_VOLUME = 0x01,
    PRESET_MONEY  = 0x02
};

typedef struct
{
    //THIS AUTHORIZATION REFERS TO THE CREDIT SALE BEING AUTHORIZED BY THE REMOTE SYSTEM
    volatile bool    _authorized;//AUTHORIZED: true, NON AUTHORIZED: false
    volatile uint8   _presettype;
    volatile uint8   _hoseid;
    volatile uint16  _price;
    volatile uint32 _presetamount;
    volatile uint32 _volumeamount;
    char8 _serial[8];
    
} PumpAuthorizationInfo;


typedef struct _PUMP_LAYOUT_
{
    uint8 _pumpid;
    uint8 _pumpindex;
    //PUMP CONTEXT
    volatile uint8 _pumpstate; //These are the pump states coming from the protocol
    volatile uint8 _currenthose;
    volatile uint8 _hoseactivestate;
    volatile bool _acquiringstate;
    
    //THIS AUTHORIZATION REFERS TO THE PUMP'S TRANSACTIONAL AUTHORIZATION BEFORE DELIVERING FUEL (AUTHORIZED => BUSY => EOT)
    volatile bool _authorized;//AUTHORIZED: true, NON AUTHORIZED: false
    volatile bool _restoreprice;
    volatile bool _presetdone;
    volatile bool _pumplocked;
    volatile uint8 _roundrobinstate;
    volatile bool _positionlocked;
    volatile uint8 _pumprftransstate; //These are the pump states coming from the protocol
    
    volatile bool _print;
    
    volatile uint32 _rtmoneyamount;
    volatile uint32 _rtvolumeamount;
        
    ///TRANSACTION CONTEXT
    volatile uint8 _transhealth;//OK, FAIL
    volatile uint8 _transtate;//IDLE, BUSY
    
    void *_pcurrtransaction;
    char8 _trasactionbuffer[_PUMP_TRANSACTION_BUFFER_SIZE_];

    char8  _rxbuffer[_PUMP_RX_BUFFER_SIZE_];
    uint16 _rxbuffersize;
    //This is the transactional queue that will hold the different selected states through which the pumps passes
    //Since this queue has a very limited size, in case it is FULL, it will block the new arriving states until
    //they are evacuated
    PumpTransactionalState _transqueue[_PUMP_MAX_QUEUE_TRANSTATES_];
    
    volatile bool _transqueuelocked;

    void (*PumpTransQueueLock)(void *pvthis);
    void (*PumpTransQueueUnlock)(void *pvthis);
    bool (*PumpTransQueueFind)(void *pvthis, uint8 transtate);
    void (*PumpTransQueueEnqueue)(void *pvthis, PNEAR_PUMPTRANSACTIONALSTATEPTR pts);
    void (*PumpTransQueueDeallocate)(void *pvthis, PNEAR_PUMPTRANSACTIONALSTATEPTR pts);
    PNEAR_PUMPTRANSACTIONALSTATEPTR (*PumpTransQueueAllocate)(void *pvthis);
    PNEAR_PUMPTRANSACTIONALSTATEPTR (*PumpTransQueueDequeue)(void *pvthis);

    bool (*PumpValidState)(void *pvthis);
    
    PumpDataContainer _datacontainer;
    PumpAuthorizationInfo _authorizationinfo;
    
    void (*CleanUp)(void *pthis);
    
} Pump, *PNEAR_PUMPPTR;

#define _MAX_CONDITION_CODES_       4
#define _PUMP_TRANSACTION_JOBS_     18

typedef struct _PUMP_TRANSACTION_JOB_
{
    void (*Request)(void *pparam);
    void (*Response)(void *pparam);
    void (*TimeoutCallback)(void *pparam);
    bool (*ResponseSizeResolver)(void *pparam, void *pbuff, uint16 buffsize);
    uint16 _responsetimeoutlimit;
    uint32 _repushtimeoutlimit;
    uint8  _responsesize;
    //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
    bool  _retryinsteadofrepush;
    
    //These flags are mapped one-to-one with the precondition, so 
    //they allow for those to be evaluated just once
    bool   _oneshotpreconditions[_MAX_CONDITION_CODES_];
    //Expected current codes. if (0xFF) then it is ignored
    uint8  _preconditions[_MAX_CONDITION_CODES_];

    //These flags are mapped one-to-one with the postcondition, so 
    //they allow for those to be evaluated just once
    bool   _oneshotpostconditions[_MAX_CONDITION_CODES_];
    //Expected response codes. if (0xFF) then it is ignored
    uint8  _postconditions[_MAX_CONDITION_CODES_];

    void (*Initialization)(void *pparam);
    void (*Finalization)(void *pparam);
} PumpTransactionJob, *PNEAR_PUMPTRANSACTIONJOBPTR;

typedef struct _PUMP_TRANSACTION_
{
    uint8_t _transactionname;//The name of the transaction
    PumpTransactionJob _jobs[_PUMP_TRANSACTION_JOBS_];
} PumpTransaction, *PNEAR_PUMPTRANSACTIONPTR;

enum _QUEUE_JOB_STATE_
{
    QUEUE_JOB_FREE,
    QUEUE_JOB_ALLOCATED,
    QUEUE_JOB_BUSY
};

typedef struct
{
    volatile bool _reenqueue;
    volatile uint8 _reenqueuecounter;
    //This belongs to the context of the JobQueue
    //FREE / ALLOCATED / BUSY
    volatile uint8  _state;
    
    volatile uint16 _delay;
    volatile uint16 _delaymultiplier;
    
    volatile uint16 _repushretriescounter;
    volatile uint16 _repushtimeoutseed;
    volatile uint16 _repushtimeoutmultiplier;
    
    volatile uint16 _retriescounter;
    volatile uint16 _timeoutseed;
    volatile uint16 _timeoutmultiplier;
        
    PumpTransactionJob *_prequest;
    PumpTransactionJob *_presponse;
    Pump *_ppump;
    
    void (*Callback)(void *pparam);
    void *_pdata;
    
} PumpTransactionJobContext, *PPUMPTRANSACTIONJOBCONTEXTPTR;

typedef struct _PUMP_JOB_QUEUE_
{
    volatile uint16 _queueindex;
    
    PumpTransactionJobContext _jobqueueheap[_PUMP_JOB_QUEUE_SIZE_];
    PumpTransactionJobContext *_pjobqueueallocated[_PUMP_JOB_QUEUE_SIZE_];
    
    void (*Rotate)(void *this);
    PumpTransactionJobContext *(*Allocate)(void *this);
    void (*Enqueue)(void *this, PumpTransactionJobContext *pjob);
    PumpTransactionJobContext* (*Peek)(void *this);
    void (*Dequeue)(void *this);
    void (*Clear)(void *this);
    
} PumpJobQueue;

typedef struct
{
    void *_pparam;
    volatile bool _bufferready;
    volatile uint16 _responsesize;
    bool (*ResponseSizeResolver)(void *pparam, void *pbuffer, uint16 buffersize);
    char8 _rxbuffer[512];
    
} PumpResponseContext ;

typedef struct 
{
    volatile bool _inconfiguration;
    volatile bool _currentloopopen;
    ////////////////////////////////
    //Normal Pumps have 2 positions
    //QUATTRO Pumps have 4 positions
    uint8 _numpositions;
    ////////////////////////////////
    //The number of hoses depends on the pump layout
    //For NORMAL PUMPS it could be:
    //Dual, Dupla => 1 hose per position. Two in total    (1 per side)
    //Quádrupla   => 2 hoses per position. Four in total  (2 per side)
    //Sêxtupla    => 3 hoses per position. Six in total   (3 per side)
    //Óctupla     => 4 hoses per position. Eight in total (4 per side)
    //For QUATTRO PUMPS it could be:
    //Quádrupla   => 1 hose per position.  Four in total  (2 per side)
    //Sêxtupla    => 2 hoses on position 1. 2 hoses on position 2.
    //            => 1 hose on position 3. 1 hose on position 4.
    //            => Six hoses in total (3 per side)
    //Óctupla     => 2 hoses per position. Eight in total (4 per side)
    uint8 _hosesposition1;
    uint8 _hosesposition2;
    uint8 _hosesposition3;
    uint8 _hosesposition4;
    
    uint8  _moneydecimals;
    uint8  _ppudecimals;
    uint8  _volumedecimals;
    
    uint8  _volumeunit;
    
    uint16 _minmoneyvalue;
    uint16 _minvolumevalue;    
    
    uint8  _displaydigitsmode;
    //The number of digits used for the FULL preset depends on the "DisplayDigitsMode"
    uint8  _presetdigits;
    //Dispenser Pumps Offset to allow more than 15 positions to be handled by one remote router (CDG)
    uint8  _positionoffset;
    //Whether point or comma will be used as decimal separator
    uint8  _pointcommadecimalmode:1;
} DispenserLayoutInfo;

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

void PumpStateRequest(void *pparam);
void PumpAuthorization(void *pparam);
void PumpStartupAuthorization(void *pparam);
void PumpDataToPump(void *pparam);
void PumpPumpStop(void *pparam);
void PumpRTMoneyRequest(void *pparam);
void PumpRTHoseVolumeRequest(void *pparam);
void PumpTransactionDataRequest(void *pparam);
void PumpTotalsDataRequest(void *pparam);
void PumpRealTimeMoneyRequest(void *pparam);
void PumpRealTimeHoseVolumeRequest(void *pparam);
void PumpAllPumpsStop(void *pparam);

void CastMoneyValue(char8 *pbuffer, uint8 buffersize);
void CastVolumeValue(char8 *pbuffer, uint8 buffersize);
char8 TranslateSpecialFuncionDigit(char8 refval);
void AcquireDispenserLayoutInformation(void *pparam);
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
void AcquirePumpStateResponse(void *pparam);
void AcquireDatatoPumpResponse(void *pparam);
void ProcessPumpTransactionData(void *pparam);
void ProcessPumpRTMoneyRequest(void *pparam);
void ProcessPumpTransactionDataRestore(void *pparam);
void ProcessPumpTotalsDataReport(void *pparam);
void ProcessPumpTotalsDataPpuCheck(void *pparam);
void PumpJobTimeout(void *pparam);
void PumpStateJobTimeout(void *pparam);
void PumpStopRequest(void *pparam);
void PumpStopAllRequest(void *pparam);
void PumpCompleteConfiguration(void *pparam);
void AcquirePumpCompleteConfiguration(void *pparam);
bool AcquirePumpSpecialFunctionResponseResolver(void *pparam, void *pbuffer, uint16 buffersize);
bool AcquirePumpTotalsResponseResolver(void *pparam, void *pbuffer, uint16 buffersize);
bool AcquirePumpTransactionDataResponseResolver(void *pparam, void *pbuffer, uint16 buffersize);
void PumpEnumeratorRequest(void *pparam);
void AcquirePumpEnumeratorResponse(void *pparam);

void PumpEnumeratorTimeout(void *pparam);
void PumpPresetHose(void *pparam);
void PumpPresetNoHose(void *pparam);
void PumpCreditPresetHose(void *pparam);
void PumpPpuChange(void *pparam);
void AcquirePpuChangeResponse(void *pparam);
void PumpDefault2CreditPpuChange(void *pparam);
void PumpCredit2DefaultPpuChange(void *pparam);
void PumpFullPresetHose(void *pparam);
void PumpFullPresetNoHose(void *pparam);
void PumpPresetNotification(void *pparam);
void PumpCreditPresetNotification(void *pparam);
/////////////////////////////////////////////////////////

extern PumpJobQueue _g_pumpjobqueue;
extern Pump _g_pumps[_MAX_NUMBER_OF_PUMPS_];
extern PumpTransaction _g_standardtransactions[];
extern DispenserLayoutInfo _g_dispenserlayoutinfo;

/* [] END OF FILE */
