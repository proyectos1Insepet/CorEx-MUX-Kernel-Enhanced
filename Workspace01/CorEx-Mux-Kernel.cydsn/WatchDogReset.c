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
@Created by: HIJH
@Date: Janvier 1 de 2017
*/

#include <project.h>
#include "GlobalDefinitions.h"
#include "SinkObservables.h"
#include "SinkManager.h"

/* [] END OF FILE */

volatile bool _g_wdtstarted = false;

void WatchDogStartup();
void WatchDogHandler();

//@Created By: HIJH
//@Janvier 1 de 2017
void WatchDogInitialize()
{
    PSINKSUBSCRIBERPTR psubscriber = AllocateSubscriberSlot();
    if(psubscriber)
    {
        uint8 index = 0;
        //The maximum number of observable identifiers is _OBSERVED_IDENTIFIERS_
        //located in the header file "SinkManager.h"
        //Every identifier is paired with its respective Callback method pointer
        psubscriber->_arrmsgid[index] = WATCHDOG_STARTUP;
        psubscriber->Callback[index++] = WatchDogStartup;
    }
    
    GLOBAL_LOOP_PUSH(WatchDogHandler);
}

//@Created By: HIJH
//@Janvier 1 de 2017
void WatchDogHandler()
{
    if(_g_wdtstarted)
        CyWdtClear();
}

//@Created By: HIJH
//@Janvier 1 de 2017
void WatchDogStartup()
{
    _g_wdtstarted = true;
    CyWdtStart(CYWDT_1024_TICKS,CYWDT_LPMODE_NOCHANGE);
}
