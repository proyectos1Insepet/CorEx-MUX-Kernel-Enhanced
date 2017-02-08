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
 *
 * ========================================
*/

#include <project.h>
#include "SinkManager.h"
#include "SinkObservables.h"
#include "GlobalDefinitions.h"

void iButton1Scan(void *pparam);
void iButton2Scan(void *pparam);

void iButtonSinkRegistration()
{
    PSINKSUBSCRIBERPTR psubscriber = AllocateSubscriberSlot();
    if(psubscriber)
    {
        uint8 index = 0;
        //The maximum number of observable identifiers is _OBSERVED_IDENTIFIERS_
        //located in the header file "SinkManager.h"
        //Every identifier is paired with its respective Callback method pointer
        psubscriber->_arrmsgid[index] = IBUTTON1_SCAN;
        psubscriber->Callback[index++] = iButton1Scan;
        
        psubscriber->_arrmsgid[index] = IBUTTON2_SCAN;
        psubscriber->Callback[index++] = iButton2Scan;
    }
}

/* [] END OF FILE */
