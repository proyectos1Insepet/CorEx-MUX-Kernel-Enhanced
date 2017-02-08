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
#include <stdint.h>
#include <stdbool.h>

#define interface struct

interface IProtocol
{
    //Function pointer used to initialize the protocol at startup
    void (*pInitialize)(void *param);
    //Function pointer used to parse the incoming buffer and validate its completeness
    bool (*pBufferResolver)(void *param, uint16_t buffersize);
    //Function pointer used to query the pump state
    void (*pCastPumpStateMessage)(void *param, uint16_t buffersize);
    //Function pointer used to forward the authorization message to the pump
    void (*pCastAuthorizationMessage)(void *param, uint16_t buffersize);
    //Function pointer used to forward raw data to the pump
    void (*pCastDataMessage)(void *param, uint16_t buffersize);
    //Function pointer used to cast the pump stop command
    void (*pCastPumpStopMessage)(void *param, uint16_t buffersize);
    //Function pointer used to get the transaction report
    void (*pCastTransactionMessage)(void *param, uint16_t buffersize);
    //Function pointer used to get the totals report
    void (*pCastTotalsMessage)(void *param, uint16_t buffersize);
    //Function pointer used to get the money in realtime while dispensing
    void (*pCastRealtimeMoneyMessage)(void *param, uint16_t buffersize);
    //Function pointer used to get the hose/volume in realtime while dispensing
    void (*pCastRealtimeHoseVolumeMessage)(void *param, uint16_t buffersize);
    //Function pointer used to stop all pumps
    void (*pCastAllPumpsStopMessage)(void *param, uint16_t buffersize);
};

extern interface IProtocol _g_protocol;

/* [] END OF FILE */
