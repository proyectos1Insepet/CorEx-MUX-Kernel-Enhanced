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

#include "GlobalDefinitions.h"
#include "SinkManager.h"
#include "UARTManager.h"
#include "Dispenser.h"
#include "RFLink.h"

//@Created By: HIJH
//@Date: Septembre de 2016
//Receives all requests from the remote peer and dispatches the stream to the appropriate
//RF Request / Response Handler
void RFServerRequestHandler(void *pparam)
{
    PSINKMESSAGEPTR pmsg = (PSINKMESSAGEPTR)pparam;
    PNEAR_REQRESPTR ptreqresiterator =  GetRFRequestResolver(pmsg->_buffer[_RF_STREAM_COMMAND_INDEX_]);
    if(ptreqresiterator->_prequestresponse && !_g_dispenserlayoutinfo._inconfiguration)
    {
        //Here we attempt to resolve the request and dispatch the predefined response
        //In case of any error (asserted as false), the error function pointer will be invoked
        if(!ptreqresiterator->_prequestresponse(pmsg))
            if(ptreqresiterator->_perror)
                ptreqresiterator->_perror(pmsg);
    }
    //Retrigger the autonomous counter on every RF request
    _g_rfautorun._toautonomoustimetoken = 0x0000;
}

//@Created By: HIJH
//@Septembre de 2016
void RFErrorHandler(void *pparam)
{
}

PNEAR_REQRESPTR GetRFRequestResolver(uint8 commandid)
{
    PNEAR_REQRESPTR retval = PNEAR_NULLPTR;
    PNEAR_REQRESPTR ptreqresiterator =  _g_rfresolvers;
    while(ptreqresiterator->_requestcode != RF_MUX_NULL && !_g_dispenserlayoutinfo._inconfiguration)
    {
        if(commandid == ptreqresiterator->_requestcode)
        {
            retval = ptreqresiterator;
            break;
        }
        
        ptreqresiterator++;
    }
    return retval;
}

/* [] END OF FILE */
