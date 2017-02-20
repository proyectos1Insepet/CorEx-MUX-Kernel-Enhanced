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
#include <stdint.h>     // definicion estandar de tipos de datos
#include <stdbool.h>    // definicion estandar de tipo booleano para c
#include <math.h>

////////////////////////////////////////////////////////////////////////////////
//GLOBAL PROTOCOL AND TESTING CONFIGURATION SETTINGS
////////////////////////////////////////////////////////////////////////////////
//*****************************************************************************
//++++++++++++  PAY ATTENTION TO THE "#define" and "#undef" DIRECTIVES TO AVOID CRITICAL MISTAKES!!!
//++++++++++++  
//++++++++++++  CHANGE TO THE APPROPRIATE CURRENT LOOP SPEED (EITHER 5787 OR ANY OTHER)
//++++++++++++  VALIDATE THE REQUIRED STARTUP TIME (FAST STARTUP / SLOW STARTUP)
//++++++++++++  THE LOGO MUST BE SELECTED IN THE "CONFIGURATION.H" FILE
//++++++++++++  ALSO PICK THE RIGHT PRINTER TO OUTPUT THE TRANSACTION INFORMATION (PANEL / KIOSK)
////////////////////////////////////////////////////////////////////////////////
//#define _ALLOW_CREDIT_SALE_TESTING_
#undef _ALLOW_CREDIT_SALE_TESTING_
////////////////////////////////////////////////////////////////////////////////
//PROTOCOL SELECTION
//#define  PRIME_PROTOCOL
//#define  ENCORE_PROTOCOL
#define  ADVANTAGE_PROTOCOL
//#define  ADVANTAGE_PROTOCOL_SLOWSTARTUP
////////////////////////////////////////////////////////////////////////////////
//THIS DEFINITION IS FOR TESTING PURPOSES ONLY SINCE IT ALLOWS FOR FAST STARTUP!!!
//#define  ENCORE_PROTOCOL_TESTING
////////////////////////////////////////////////////////////////////////////////

//This is to enable/disable the hose request on cash sales
#define  _PRESET_HOSE_
//#define  _PRESET_NO_HOSE_

//This to enable/disable the hose request on credit sales (ONLY FOR PRIME DISPENSERS)
#define PRIME_CREDIT_SALE_NOHOSE
//#undef PRIME_CREDIT_SALE_NOHOSE

//This is to enable/disable the pump/position lock after a credit sale (since credit sales change the fuel price)
//#define CREDIT_SALE_PUMP_LOCK
#undef  CREDIT_SALE_PUMP_LOCK

#undef FORCE_PRICES //STORES A CUSTOM PRICE IN THE EEPROM AND CHANGES THE PRICE ON THE DISPLAYS
#undef FORCE_DATETIME//STORES A CUSTOM TIME AND DATE IN THE EEPROM THAT WILL BE REFLECTED ON THE DISPLAYS
#undef PUMPS_UNLOCKED //UNLOCKS THE POSITION TO ALLOW FOR TESTING
//#define PUMPS_UNLOCKED //UNLOCKS THE POSITION TO ALLOW FOR TESTING

////////////////////////////////////////////////////////////////////////////////
//MULTIPLE PRESET (ONLY FOR PRIME DISPENSERS)
#undef PRIME_MULTIPLE_PRESET

////////////////////////////////////////////////////////////////////////////////
//TIME TO WAIT AT STARTUP (PRIME AND ADVANTAGE ARE FAST STARTUP / ENCORE IS SLOW STARTUP)
#ifdef ENCORE_PROTOCOL
#define SLOW_STARTUP
#endif

#ifdef ENCORE_PROTOCOL_TESTING
#define FAST_STARTUP
#define ENCORE_PROTOCOL
#endif

#ifdef PRIME_PROTOCOL
#define FAST_STARTUP
#endif
#ifdef ADVANTAGE_PROTOCOL
#undef  SLOW_STARTUP
#define FAST_STARTUP
#endif

#ifdef ADVANTAGE_PROTOCOL_SLOWSTARTUP
#define ADVANTAGE_PROTOCOL
#undef  FAST_STARTUP
#define SLOW_STARTUP
#endif

#ifdef PRIME_PROTOCOL
    #include "Prime.h"
#endif
#ifdef ENCORE_PROTOCOL
    #include "Encore.h"
#endif
#ifdef ADVANTAGE_PROTOCOL
    #include "Advantage.h"
#endif

#define SIDE1     1
#define SIDE2     2

typedef struct
{
    uint8 _side;
    uint8 _refcode;
    PNEAR_PUMPPTR  _ppump;
} PumpCodeMapping;

//These transactional states will be queried by the remote peer (NSX in this case but could be another agent)
enum _TRANSACTIONAL_STATES_
{
    _PUMP_POSITION_IDLE_        = 0x08,
    _PUMP_POSITION_UNKNOWN_     = 0x00,
    _PUMP_POSITION_DISPENSING_  = 0x01,
    _PUMP_POSITION_TRANSREPORT_ = 0x02,
    _PUMP_POSITION_CREDIT_      = 0x03,
    _PUMP_POSITION_ERROR_       = 0x04,
};

enum _DISPENSER_POSITION_LOCK_STATE_
{
    PUMP_CLOSED  = 0x00,
    PUMP_OPEN    = 0x01,
    PUMP_OPINVAL = 0x02
};

void ResetPumpRoutes();
void DispenserUARTSend(void *ptr);
PumpTransaction *GetTransactionByName(uint8 codename);
void DispenserUpdatePumpState(void *pparam);
uint8 DispenserLocalHoseID2RemoteHoseID(uint8 pumpindex, uint8 hoseid);
uint8 DispenserRemoteHoseID2LocalHoseID(uint8 pumpindex, uint8 hoseid);
uint16 DispenserGetEepromPPUPageFromPumpID(uint8 pumpid);
PNEAR_PUMPPTR GetPumpFromRemotePositionID(uint8 positionid);

typedef void (*PNEARDISPENSERACTIVATEPROGRAMMING)(void* pparam);

/* [] END OF FILE */
