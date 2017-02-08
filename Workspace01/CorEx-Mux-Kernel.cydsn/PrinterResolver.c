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

#include "Dispenser.h"
#include "UARTManager.h"

void PrinterUARTSend(void *ptr)
{
    PUARTMESSAGEPTR pmessage = (PUARTMESSAGEPTR)ptr;
        
//pmessage->_puartmanager->WriteTxData(_MESSAGE_START0_);
/*
    uint8 index = 0;
    for(; index < pmessage->_messagelength; index++)
    {
        pmessage->_puartmanager->WriteTxData(pmessage->_messagetx[index]);
    }
*/    
}

/* [] END OF FILE */
