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
#include <project.h>
#include "GlobalDefinitions.h"

#include "SinkManager.h"
#include "SinkObservables.h"
#include "RFLink.h"

CY_ISR_PROTO(CounterIsr);
CY_ISR_PROTO(DispenserStateIsr);

void InitTimers()
{
    Timer_Tick_Start();
    Counter_ISR_StartEx(CounterIsr);
    
    Timer_DispenserState_Start();
    DispenserStateTimeout_ISR_StartEx(DispenserStateIsr);
    
}

CY_ISR(CounterIsr)
{
    Timer_Tick_ReadStatusRegister();
    PSINKMESSAGEPTR psinkmsg = AllocateMessageSlotConditional(SINK_UPDATE_COUNTERS);
    if(psinkmsg)
    {
        psinkmsg->_messageid = SINK_UPDATE_COUNTERS;
        psinkmsg->_messagetype = FIRSTFOUND;
        psinkmsg->_messagestate = SINK_BUSY;
    } 
    _g_rfrx._rftimeoutmultiplier++;
}

CY_ISR(DispenserStateIsr)
{
    Timer_DispenserState_ReadStatusRegister();
    PSINKMESSAGEPTR psinkmsg = AllocateMessageSlotConditional(DISPENSER_UPDATE_PUMP_TRANSACTION_TIMEOUTS);
    if(psinkmsg)
    {
        psinkmsg->_messageid = DISPENSER_UPDATE_PUMP_TRANSACTION_TIMEOUTS;
        psinkmsg->_messagetype = FIRSTFOUND;
        psinkmsg->_messagestate = SINK_BUSY;
    }
}

/* [] END OF FILE */
