/* ========================================
 *
 * Copyright INSEPET SISTEMA, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
@Created by: HIJH
@Date: Septembre 3 de 2016

*/

#include "GlobalDefinitions.h"

//PUMP allowed transitions from current state
PumpStateTransitionMap _g_pumpstm[] = 
{
    //Current State ----  Allowed (Optimal) State Transitions
    { PUMP_IDLE,        { PUMP_CALLING, PUMP_AUTHORIZED, 0xFF, 0xFF, 0xFF, 0xFF } },
    { PUMP_AUTHORIZED,  { PUMP_IDLE, PUMP_CALLING, PUMP_BUSY, 0xFF, 0xFF, 0xFF } },
    { PUMP_CALLING,     { PUMP_IDLE, PUMP_BUSY, PUMP_AUTHORIZED, 0xFF, 0xFF, 0xFF } },
    { PUMP_BUSY,        { PUMP_CALLING, PUMP_STOPPED, PUMP_IDLE, PUMP_EOT, 0xFF, 0xFF } },
    { PUMP_EOT,         { PUMP_IDLE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } },
    { PUMP_STOPPED,     { PUMP_BUSY, PUMP_EOT, 0xFF, 0xFF, 0xFF, 0xFF } },
};

//PUMP allowed commands from current state
PumpStateCommandMap _g_pumpscm[] = 
{
    //Current State ----  Allowed Commands
    { PUMP_IDLE,        { PUMP_STATE_REQUEST, PUMP_AUTHORIZATION, PUMP_PUMP_STOP, PUMP_ALL_PUMPS_STOP, PUMP_TRANSACTION_DATA_REQUEST, PUMP_TOTALS_DATA_REQUEST, PUMP_DATA_TO_PUMP, 0xFF, 0xFF, 0xFF } },
    { PUMP_AUTHORIZED,  { PUMP_STATE_REQUEST, PUMP_PUMP_STOP, PUMP_ALL_PUMPS_STOP, PUMP_TRANSACTION_DATA_REQUEST, PUMP_TOTALS_DATA_REQUEST, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } },
    { PUMP_CALLING,     { PUMP_STATE_REQUEST, PUMP_AUTHORIZATION, PUMP_PUMP_STOP, PUMP_ALL_PUMPS_STOP, PUMP_TRANSACTION_DATA_REQUEST, PUMP_TOTALS_DATA_REQUEST, PUMP_DATA_TO_PUMP, PUMP_REAL_TIME_HOSE_VOLUME_REQUEST, 0xFF, 0xFF } },
    { PUMP_BUSY,        { PUMP_STATE_REQUEST, PUMP_PUMP_STOP, PUMP_ALL_PUMPS_STOP, PUMP_REAL_TIME_MONEY_REQUEST, PUMP_REAL_TIME_HOSE_VOLUME_REQUEST, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } },
    { PUMP_EOT,         { PUMP_STATE_REQUEST, PUMP_TRANSACTION_DATA_REQUEST, PUMP_TOTALS_DATA_REQUEST, PUMP_REAL_TIME_MONEY_REQUEST, PUMP_REAL_TIME_HOSE_VOLUME_REQUEST, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } },
    { PUMP_STOPPED,     { PUMP_STATE_REQUEST, PUMP_AUTHORIZATION, PUMP_TRANSACTION_DATA_REQUEST, PUMP_TOTALS_DATA_REQUEST, PUMP_REAL_TIME_MONEY_REQUEST, PUMP_REAL_TIME_HOSE_VOLUME_REQUEST, 0xFF, 0xFF, 0xFF, 0xFF } }
};

//Command ID mapping to function pointer
PumpCommandIDMap _g_commandidmap[] =
{
    { PUMP_STATE_REQUEST,                   PumpStateRequest },
    { PUMP_AUTHORIZATION,                   PumpAuthorization },
    { PUMP_DATA_TO_PUMP,                    PumpDataToPump },
    { PUMP_PUMP_STOP,                       PumpPumpStop },
    { PUMP_TRANSACTION_DATA_REQUEST,        PumpTransactionDataRequest },
    { PUMP_TOTALS_DATA_REQUEST,             PumpTotalsDataRequest },
    { PUMP_REAL_TIME_MONEY_REQUEST,         PumpRealTimeMoneyRequest },
    { PUMP_REAL_TIME_HOSE_VOLUME_REQUEST,   PumpRealTimeHoseVolumeRequest },
    { PUMP_ALL_PUMPS_STOP,                  PumpAllPumpsStop }
};

/* [] END OF FILE */
