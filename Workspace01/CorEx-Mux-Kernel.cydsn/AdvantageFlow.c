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
@Date: Novembre de 2016
*/

#include "GlobalDefinitions.h"
#include "Dispenser.h"

#ifdef ADVANTAGE_PROTOCOL
    
void InitiateTransaction(void *pparam);
void TerminateTransaction(void *pparam);
void TerminateTransactionAndPrint(void *pparam);

//@Created By: HIJH
//@Septembre de 2016
PumpTransaction *GetTransactionByName(uint8 codename)
{
    PumpTransaction *pretval = NULL;
    PumpTransaction *ptr = _g_standardtransactions;
    while(ptr->_transactionname != 0xFF)
    {
        if(ptr->_transactionname == codename)
        {
            pretval = ptr;
            break;
        }
        ptr++;
    }
    return pretval;
}

/*
@Created By: HIJH
@Date: Septembre de 2016
Pump transactional templates are dynamic structures comprised of "transactional atoms (TA)" that switch the context, once 
they are signaled for processing. These "TA" are signaled from a FIFO queue to the dispenser handler one by one .
They hold their own model of consistency for the maximum reliability of the transaction (retry, repush, pre and post conditions handling etc.)
*/
PumpTransaction _g_standardtransactions[] = 
{
    { _PUMP_FILL_BY_MONEY_, 
        {
            /////////////////////////////////////////////////////////////////////////
            //DATA TO PUMP TO START THE PRESET PROCEDURE
            /////////////////////////////////////////////////////////////////////////
            {
                PumpDataToPump,  //Request pointer
                AcquireDatatoPumpResponse, //Response pointer
                PumpJobTimeout, //Timeout pointer
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _MAX_RETRIES_,
                PUMP_DATA_TO_PUMP_RESPONSE_SIZE,//ONE BYTE OF RESPONSE (0xDp = READY TO RECEIVE DATA)
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _RETRY_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, false, false, false },
                { PUMP_SEND_DATA, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            /////////////////////////////////////////////////////////////////////////
            //SENDING PRESET INFO (EIGHT DIGITS FOR THE MONEY/VOLUME PRESET)
            /////////////////////////////////////////////////////////////////////////
            #ifdef _PRESET_NO_HOSE_
            {
                PumpPresetNoHose, //Request pointer
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_TIMEOUT_,
                PUMP_NO_RESPONSE_SIZE,//NO RESPONSE REQUIRED
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, false, false, false },
                { PUMP_SEND_DATA, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            #endif
            #ifdef _PRESET_HOSE_
            {
                PumpPresetHose, //Request pointer
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_TIMEOUT_,
                PUMP_NO_RESPONSE_SIZE,//NO RESPONSE REQUIRED
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, false, false, false },
                { PUMP_SEND_DATA, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            #endif
            /////////////////////////////////////////////////////////////////////////
            //ACQUIRE PUMPSTATE TO KICK OFF THE TRANSACTION
            /////////////////////////////////////////////////////////////////////////
            {
                PumpStateRequest, //Request pointer
                AcquirePumpStateResponse, //Response pointer
                PumpJobTimeout, //Timeout pointer
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_REPUSH_,
                PUMP_STATE_RESPONSE_SIZE,//ONE BYTE OF RESPONSE
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            /////////////////////////////////////////////////////////////////////////
            //SENDING THE AUTHORIZATION TO THE PUMP
            /////////////////////////////////////////////////////////////////////////
            {
                PumpAuthorization, //Request Pointer
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_TIMEOUT_,
                PUMP_AUTHORIZATION_RESPONSE_SIZE,//NO RESPONSE REQUIRED
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, true, false, false },
                { PUMP_IDLE, PUMP_CALLING, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            /////////////////////////////////////////////////////////////////////////
            //ACQUIRE PUMPSTATE TO KICK OFF THE TRANSACTION
            /////////////////////////////////////////////////////////////////////////
            {
                PumpStateRequest, //Request pointer
                AcquirePumpStateResponse, //Response pointer
                PumpJobTimeout, //Timeout pointer
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_REPUSH_,
                PUMP_STATE_RESPONSE_SIZE,//ONE BYTE OF RESPONSE
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, true, false, false },
                { PUMP_AUTHORIZED, PUMP_BUSY, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PumpPresetNotification
            },
            {
                PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, 0x00, 0x00, 0x00, false, {}, {}, {}, {},
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
            }
        }
    },
    { _PUMP_FILL_BY_VOLUME_, 
        {
            /////////////////////////////////////////////////////////////////////////
            //DATA TO PUMP TO START THE PRESET PROCEDURE
            /////////////////////////////////////////////////////////////////////////
            {
                PumpDataToPump,  //Request pointer
                AcquireDatatoPumpResponse, //Response pointer
                PumpJobTimeout, //Timeout pointer
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _MAX_RETRIES_,
                PUMP_DATA_TO_PUMP_RESPONSE_SIZE,//ONE BYTE OF RESPONSE (0xDp = READY TO RECEIVE DATA)
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _RETRY_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, false, false, false },
                { PUMP_SEND_DATA, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            /////////////////////////////////////////////////////////////////////////
            //SENDING PRESET INFO (EIGHT DIGITS FOR THE MONEY/VOLUME PRESET)
            /////////////////////////////////////////////////////////////////////////
            #ifdef _PRESET_NO_HOSE_
            {
                PumpPresetNoHose, //Request pointer
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_TIMEOUT_,
                PUMP_NO_RESPONSE_SIZE,//NO RESPONSE REQUIRED
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, false, false, false },
                { PUMP_SEND_DATA, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            #endif
            #ifdef _PRESET_HOSE_
            {
                PumpPresetHose, //Request pointer
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_TIMEOUT_,
                PUMP_NO_RESPONSE_SIZE,//NO RESPONSE REQUIRED
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, false, false, false },
                { PUMP_SEND_DATA, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            #endif
            /////////////////////////////////////////////////////////////////////////
            //ACQUIRE PUMPSTATE TO KICK OFF THE TRANSACTION
            /////////////////////////////////////////////////////////////////////////
            {
                PumpStateRequest, //Request pointer
                AcquirePumpStateResponse, //Response pointer
                PumpJobTimeout, //Timeout pointer
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_REPUSH_,
                PUMP_STATE_RESPONSE_SIZE,//ONE BYTE OF RESPONSE
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            /////////////////////////////////////////////////////////////////////////
            //SENDING THE AUTHORIZATION TO THE PUMP
            /////////////////////////////////////////////////////////////////////////
            {
                PumpAuthorization, //Request Pointer
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_TIMEOUT_,
                PUMP_AUTHORIZATION_RESPONSE_SIZE,//NO RESPONSE REQUIRED
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, true, false, false },
                { PUMP_IDLE, PUMP_CALLING, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            /////////////////////////////////////////////////////////////////////////
            //ACQUIRE PUMPSTATE TO KICK OFF THE TRANSACTION
            /////////////////////////////////////////////////////////////////////////
            {
                PumpStateRequest, //Request pointer
                AcquirePumpStateResponse, //Response pointer
                PumpJobTimeout, //Timeout pointer
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_REPUSH_,
                PUMP_STATE_RESPONSE_SIZE,//ONE BYTE OF RESPONSE
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, true, false, false },
                { PUMP_AUTHORIZED, PUMP_BUSY, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PumpPresetNotification
            },
            /////////////////////////////////////////////////////////////////////////
            //RETRIEVE TRANSACTION DATA (CHANGES STATE FROM EOT TO IDLE)
            /////////////////////////////////////////////////////////////////////////
            {
                PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, 0x00, 0x00, 0x00, false, {}, {}, {}, {},
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
            }
        }
    },
    { _PUMP_FILL_FULL_, 
        {
            /////////////////////////////////////////////////////////////////////////
            //DATA TO PUMP TO START THE PRESET PROCEDURE
            /////////////////////////////////////////////////////////////////////////
            {
                PumpDataToPump,  //Request pointer
                AcquireDatatoPumpResponse, //Response pointer
                PumpJobTimeout, //Timeout pointer
                PNEAR_NULLPTR,
                _NO_TIMEOUT_, //Timeout Limit (msecs)
                _MAX_RETRIES_,
                PUMP_DATA_TO_PUMP_RESPONSE_SIZE,//ONE BYTE OF RESPONSE (0xDp = READY TO RECEIVE DATA)
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _RETRY_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, false, false, false },
                { PUMP_SEND_DATA, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            /////////////////////////////////////////////////////////////////////////
            //SENDING PRESET INFO (EIGHT DIGITS FOR THE MONEY/VOLUME PRESET)
            /////////////////////////////////////////////////////////////////////////
            #ifdef _PRESET_NO_HOSE_
            {
                PumpFullPresetNoHose, //Request pointer
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_TIMEOUT_,
                PUMP_NO_RESPONSE_SIZE,//NO RESPONSE REQUIRED
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, false, false, false },
                { PUMP_SEND_DATA, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            #endif
            #ifdef _PRESET_HOSE_
            {
                PumpFullPresetHose, //Request pointer
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_TIMEOUT_,
                PUMP_NO_RESPONSE_SIZE,//NO RESPONSE REQUIRED
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, false, false, false },
                { PUMP_SEND_DATA, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            #endif
            /////////////////////////////////////////////////////////////////////////
            //ACQUIRE PUMPSTATE TO KICK OFF THE TRANSACTION
            /////////////////////////////////////////////////////////////////////////
            {
                PumpStateRequest, //Request pointer
                AcquirePumpStateResponse, //Response pointer
                PumpJobTimeout, //Timeout pointer
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_REPUSH_,
                PUMP_STATE_RESPONSE_SIZE,//ONE BYTE OF RESPONSE
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            /////////////////////////////////////////////////////////////////////////
            //SENDING THE AUTHORIZATION TO THE PUMP
            /////////////////////////////////////////////////////////////////////////
            {
                PumpAuthorization, //Request Pointer
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_TIMEOUT_,
                PUMP_AUTHORIZATION_RESPONSE_SIZE,//NO RESPONSE REQUIRED
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, true, false, false },
                { PUMP_IDLE, PUMP_CALLING, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            /////////////////////////////////////////////////////////////////////////
            //ACQUIRE PUMPSTATE TO KICK OFF THE TRANSACTION
            /////////////////////////////////////////////////////////////////////////
            {
                PumpStateRequest, //Request pointer
                AcquirePumpStateResponse, //Response pointer
                PumpJobTimeout, //Timeout pointer
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_REPUSH_,
                PUMP_STATE_RESPONSE_SIZE,//ONE BYTE OF RESPONSE
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, true, false, false },
                { PUMP_AUTHORIZED, PUMP_BUSY, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PumpPresetNotification
            },
            {
                PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, 0x00, 0x00, 0x00, false, {}, {}, {}, {},
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
            }
        }
    },
    { _PUMP_FILL_CREDIT_, 
        {
            /////////////////////////////////////////////////////////////////////////
            //ACQUIRE PUMPSTATE TO KICK OFF THE WHOLE TRANSACTION
            /////////////////////////////////////////////////////////////////////////
            {
                PumpStateRequest, //Request pointer
                AcquirePumpStateResponse, //Response pointer
                PumpJobTimeout, //Timeout pointer
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_REPUSH_,
                PUMP_STATE_RESPONSE_SIZE,//ONE BYTE OF RESPONSE
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            /////////////////////////////////////////////////////////////////////////
            //DATA TO PUMP TO START CHANGING THE PRICE FOR THIS TRANSACTION
            /////////////////////////////////////////////////////////////////////////
            {
                PumpDataToPump,  //Request pointer
                AcquireDatatoPumpResponse, //Response pointer
                PumpJobTimeout, //Timeout pointer
                PNEAR_NULLPTR,
                _NO_TIMEOUT_, //Timeout Limit (msecs)
                _MAX_RETRIES_,
                PUMP_DATA_TO_PUMP_RESPONSE_SIZE,//ONE BYTE OF RESPONSE (0xDp = READY TO RECEIVE DATA)
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _RETRY_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, false, false, false },
                { PUMP_SEND_DATA, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            /////////////////////////////////////////////////////////////////////////
            //SENDING THE TEMPORARY PRICE VALUE FOR THIS CREDIT TRANSACTION
            /////////////////////////////////////////////////////////////////////////
            {
                PumpDefault2CreditPpuChange, 
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, 
                _NO_TIMEOUT_,
                PUMP_NO_RESPONSE_SIZE,
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, false, false, false },
                { PUMP_SEND_DATA, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            /////////////////////////////////////////////////////////////////////////
            //INQUIRY PUMP STATE FOR A "FAIL" CONDITION
            /////////////////////////////////////////////////////////////////////////
            {
                PumpStateRequest,  //Request pointer
                AcquirePumpStateResponse, 
                PumpJobTimeout,
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_TIMEOUT_,
                PUMP_STATE_RESPONSE_SIZE,//ONE BYTE OF RESPONSE
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            /////////////////////////////////////////////////////////////////////////
            //DATA TO PUMP TO START THE PRESET PROCEDURE
            /////////////////////////////////////////////////////////////////////////
            {
                PumpDataToPump,  //Request pointer
                AcquireDatatoPumpResponse, //Response pointer
                PumpJobTimeout, //Timeout pointer
                PNEAR_NULLPTR,
                _NO_TIMEOUT_, //Timeout Limit (msecs)
                _MAX_RETRIES_,
                PUMP_DATA_TO_PUMP_RESPONSE_SIZE,//ONE BYTE OF RESPONSE (0xDp = READY TO RECEIVE DATA)
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _RETRY_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, false, false, false },
                { PUMP_SEND_DATA, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            /////////////////////////////////////////////////////////////////////////
            //SENDING PRESET INFO (EIGHT DIGITS FOR THE MONEY/VOLUME PRESET)
            /////////////////////////////////////////////////////////////////////////
            {
                PumpCreditPresetHose, //Request pointer
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_TIMEOUT_,
                PUMP_NO_RESPONSE_SIZE,//NO RESPONSE REQUIRED
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, false, false, false },
                { PUMP_SEND_DATA, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            /////////////////////////////////////////////////////////////////////////
            //INQUIRY PUMP STATE FOR A "FAIL" CONDITION
            /////////////////////////////////////////////////////////////////////////
            {
                PumpStateRequest,  //Request pointer
                AcquirePumpStateResponse, 
                PumpJobTimeout,
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_TIMEOUT_,
                PUMP_STATE_RESPONSE_SIZE,//ONE BYTE OF RESPONSE
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, false, false, false },
                { PUMP_SEND_DATA, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, true, false, false },
                { PUMP_IDLE, PUMP_CALLING, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PumpCreditPresetNotification
            },
            /////////////////////////////////////////////////////////////////////////
            //SENDING THE AUTHORIZATION TO THE PUMP
            /////////////////////////////////////////////////////////////////////////
            {
                PumpAuthorization, //Request Pointer
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_TIMEOUT_,
                PUMP_AUTHORIZATION_RESPONSE_SIZE,//NO RESPONSE REQUIRED
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, true, false, false },
                { PUMP_IDLE, PUMP_CALLING, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            /////////////////////////////////////////////////////////////////////////
            //PUMP STATE REFRESH TO AUTHORIZED/BUSY
            /////////////////////////////////////////////////////////////////////////
            {
                PumpStateRequest,  //Request pointer
                AcquirePumpStateResponse, 
                PumpJobTimeout,
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_TIMEOUT_,
                PUMP_STATE_RESPONSE_SIZE,//ONE BYTE OF RESPONSE
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },

            {
                PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, 0x00, 0x00, 0x00, false, {}, {}, {}, {},
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
            }
        }
    },
    { _PUMP_RESTORE_PRICE_, 
        {
            /////////////////////////////////////////////////////////////////////////
            //DATA TO PUMP TO START CHANGING THE PRICE FOR THIS TRANSACTION
            /////////////////////////////////////////////////////////////////////////
            {
                PumpDataToPump,  //Request pointer
                AcquireDatatoPumpResponse, //Response pointer
                PumpJobTimeout, //Timeout pointer
                PNEAR_NULLPTR,
                _NO_TIMEOUT_,//NO TIMEOUT VALUE MEANS FOREVER!! - Timeout Limit (msecs)
                _NO_TIMEOUT_, //Max repush timeout since this is a LRJ
                PUMP_DATA_TO_PUMP_RESPONSE_SIZE,//ONE BYTE OF RESPONSE (0xDp = READY TO RECEIVE DATA)
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { PUMP_SEND_DATA, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            /////////////////////////////////////////////////////////////////////////
            //RESTORING THE PREDEFINED PRICE VALUE FOR THIS HOSE
            /////////////////////////////////////////////////////////////////////////
            {
                PumpCredit2DefaultPpuChange, 
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _NO_TIMEOUT_,//NO TIMEOUT VALUE MEANS FOREVER!! - Timeout Limit (msecs)
                _NO_TIMEOUT_, //Max repush timeout since this is a LRJ
                PUMP_NO_RESPONSE_SIZE,
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { true, false, false, false },
                { PUMP_SEND_DATA, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            {
                PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, 0x00, 0x00, 0x00, false, {}, {}, {}, {},
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            }
        }
    },
    { _PUMP_TOTALS_, 
        {
            /////////////////////////////////////////////////////////////////////////
            //RETRIEVE TOTALS DATA
            /////////////////////////////////////////////////////////////////////////
            {
                PumpTotalsDataRequest, //Request Pointer
                ProcessPumpTotalsDataReport, 
                PumpJobTimeout,
                AcquirePumpTotalsResponseResolver,
                #ifdef ADVANTAGE_PROTOCOL
                _MAX_TOTALS_RESPONSE_TIMEOUT_EX_,//Timeout Limit (msecs)
                #endif
                #ifndef ADVANTAGE_PROTOCOL
                _MAX_TOTALS_RESPONSE_TIMEOUT_,//Timeout Limit (msecs)
                #endif
                _NO_TIMEOUT_, //Max repush timeout since this is a LRJ
                PUMP_NO_RESPONSE_SIZE,
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            {
                PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, 0x00, 0x00, 0x00, false, {}, {}, {}, {},
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
            }
        }
    },
    { _PUMP_PPU_CHANGE_, 
        {
            //DATA TO PUMP TO START THE PRESET
            {
                PumpDataToPump, 
                AcquireDatatoPumpResponse, 
                PumpJobTimeout,
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, 
                _NO_TIMEOUT_,
                PUMP_DATA_TO_PUMP_RESPONSE_SIZE,
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { PUMP_SEND_DATA, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            //SENDING PRESET INFO
            {
                PumpPpuChange, 
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, 
                _NO_TIMEOUT_,
                PUMP_NO_RESPONSE_SIZE,
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { PUMP_SEND_DATA, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            {
                PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, 0x00, 0x00, 0x00, false, {}, {}, {}, {},
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            }
        }
    },
    { _PUMP_STATE_, 
        {
            /////////////////////////////////////////////////////////////////////////
            //ACQUIRE PUMPSTATE TO KICK OFF THE TRANSACTION
            /////////////////////////////////////////////////////////////////////////
            {
                PumpStateRequest, //Request pointer
                AcquirePumpStateResponse, //Response pointer
                PumpStateJobTimeout, //Timeout pointer
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_REPUSH_,
                PUMP_STATE_RESPONSE_SIZE,//ONE BYTE OF RESPONSE
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            {
                PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, 0x00, 0x00, 0x00, false, {}, {}, {}, {},
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            }
        }
    },
    { _PUMP_EOT_, 
        {
            /////////////////////////////////////////////////////////////////////////
            //RETRIEVE TRANSACTION DATA (CHANGES STATE FROM EOT TO IDLE)
            /////////////////////////////////////////////////////////////////////////
            {
                PumpTransactionDataRequest, //Request Pointer
                ProcessPumpTransactionData, 
                PumpJobTimeout,
                AcquirePumpTransactionDataResponseResolver,
                #ifdef ADVANTAGE_PROTOCOL
                _MAX_TRANSACTION_RESPONSE_TIMEOUT_EX_,//Timeout Limit (msecs)
                #endif
                #ifndef ADVANTAGE_PROTOCOL
                _MAX_TRANSACTION_RESPONSE_TIMEOUT_,//Timeout Limit (msecs)
                #endif
                _NO_TIMEOUT_, //Max repush timeout since this is a LRJ
                PUMP_NO_RESPONSE_SIZE,
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                TerminateTransactionAndPrint
            },
            /////////////////////////////////////////////////////////////////////////
            //ACQUIRE PUMPSTATE TO KICK OFF THE TRANSACTION
            /////////////////////////////////////////////////////////////////////////
            {
                PumpStateRequest, //Request pointer
                AcquirePumpStateResponse, //Response pointer
                PumpStateJobTimeout, //Timeout pointer
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_REPUSH_,
                PUMP_STATE_RESPONSE_SIZE,//ONE BYTE OF RESPONSE
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            /////////////////////////////////////////////////////////////////////////
            //ACQUIRE PUMPSTATE TO KICK OFF THE TRANSACTION
            /////////////////////////////////////////////////////////////////////////
            {
                PumpStateRequest, //Request pointer
                AcquirePumpStateResponse, //Response pointer
                PumpStateJobTimeout, //Timeout pointer
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_REPUSH_,
                PUMP_STATE_RESPONSE_SIZE,//ONE BYTE OF RESPONSE
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            {
                PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, 0x00, 0x00, 0x00, false, {}, {}, {}, {},
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            }
        }
    },
    { _PUMP_NETWORK_ENUMERATOR_, 
        {
            //PUMPSTATE ACQUISITION TO SCAN FOR ASSIGNED PUMP IDENTIFIERS
            {
                PumpEnumeratorRequest, 
                AcquirePumpEnumeratorResponse, 
                PumpEnumeratorTimeout,
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, 
                _NO_TIMEOUT_,
                PUMP_STATE_RESPONSE_SIZE,
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            {
                PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, 0x00, 0x00, 0x00, false, {}, {}, {}, {},
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            }
        }
    },
    { _PUMP_PROGRAM_MODE_, 
        {
            {
                PumpStartupAuthorization, 
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _NO_TIMEOUT_, 
                _NO_TIMEOUT_,
                PUMP_NO_RESPONSE_SIZE,
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            //PUMP STOP COMMAND DIRECTED TO A SINGLE PUMP
            {
                PumpStopRequest, 
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _NO_TIMEOUT_, 
                _NO_TIMEOUT_,
                PUMP_NO_RESPONSE_SIZE,
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            {
                PumpStartupAuthorization, 
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _NO_TIMEOUT_, 
                _NO_TIMEOUT_,
                PUMP_NO_RESPONSE_SIZE,
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            //PUMP STOP COMMAND DIRECTED TO A SINGLE PUMP
            {
                PumpStopRequest, 
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _NO_TIMEOUT_, 
                _NO_TIMEOUT_,
                PUMP_NO_RESPONSE_SIZE,
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            {
                PumpStartupAuthorization, 
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _NO_TIMEOUT_, 
                _NO_TIMEOUT_,
                PUMP_NO_RESPONSE_SIZE,
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            //PUMP STOP COMMAND DIRECTED TO A SINGLE PUMP
            {
                PumpStopRequest, 
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _NO_TIMEOUT_, 
                _NO_TIMEOUT_,
                PUMP_NO_RESPONSE_SIZE,
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            {
                PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, 0x00, 0x00, 0x00, false, {}, {}, {}, {},
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            }
        }
    },
    { _PUMP_STOP_, 
        {
            //PUMP STOP COMMAND DIRECTED TO A SINGLE PUMP
            {
                PumpStopRequest, 
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _NO_TIMEOUT_, 
                _NO_TIMEOUT_,
                PUMP_NO_RESPONSE_SIZE,
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            {
                PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, 0x00, 0x00, 0x00, false, {}, {}, {}, {},
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            }
        }
    },
    { _PUMP_STOPALL_,
        {
            //PUMP STOP COMMAND DIRECTED TO ALL PUMPS (STOP BROADCAST)
            {
                PumpStopAllRequest, 
                PNEAR_NULLPTR, 
                PNEAR_NULLPTR,
                PNEAR_NULLPTR,
                _NO_TIMEOUT_, 
                _NO_TIMEOUT_,
                PUMP_NO_RESPONSE_SIZE,
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            {
                PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, 0x00, 0x00, 0x00, false, {}, {}, {}, {},
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            }
        }
    },
    { _PUMP_COMPLETE_CONFIGURATION_,
        {
            /////////////////////////////////////////////////////////////////////////
            //DATA TO PUMP TO START THE PRESET PROCEDURE
            /////////////////////////////////////////////////////////////////////////
            {
                PumpDataToPump,  //Request pointer
                AcquireDatatoPumpResponse, //Response pointer
                PNEAR_NULLPTR, //Timeout pointer
                PNEAR_NULLPTR,
                _NO_TIMEOUT_, //Timeout Limit (msecs)
                _MAX_RETRIES_,
                PUMP_DATA_TO_PUMP_RESPONSE_SIZE,//ONE BYTE OF RESPONSE (0xDp = READY TO RECEIVE DATA)
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _RETRY_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { PUMP_SEND_DATA, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            /////////////////////////////////////////////////////////////////////////
            //HERE WE'RE ACTUALLY SENDING THE SFC TO THE TARGETED PUMP
            /////////////////////////////////////////////////////////////////////////
            {
                PumpCompleteConfiguration, 
                AcquirePumpCompleteConfiguration, 
                PNEAR_NULLPTR,
                AcquirePumpSpecialFunctionResponseResolver,
                _NO_TIMEOUT_, 
                _NO_TIMEOUT_,
                PUMP_NO_RESPONSE_SIZE,
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { PUMP_SEND_DATA, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            /////////////////////////////////////////////////////////////////////////
            //ACQUIRE PUMPSTATE TO KICK OFF THE TRANSACTION
            /////////////////////////////////////////////////////////////////////////
            {
                PumpStateRequest, //Request pointer
                AcquirePumpStateResponse, //Response pointer
                PumpStateJobTimeout, //Timeout pointer
                PNEAR_NULLPTR,
                _MAX_RESPONSE_TIMEOUT_, //Timeout Limit (msecs)
                _NO_REPUSH_,
                PUMP_STATE_RESPONSE_SIZE,//ONE BYTE OF RESPONSE
                //If this flag is set, the "_repushtimeoutlimit" field will be interpreted as retries instead of a "Repush TImeout"
                _REPUSH_ACTIVATED_,
                //Precondition Codes: Validated before executing the main Request.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                //Poscondition Codes:  Validated before executing the Callback.
                //If there are codes within the array and none of them is matched, then "RE-PUSH"
                { false, false, false, false },
                { 0xFF, 0xFF, 0xFF, 0xFF },
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            },
            {
                PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, PNEAR_NULLPTR, 0x00, 0x00, 0x00, false, {}, {}, {}, {},
                PNEAR_NULLPTR,
                PNEAR_NULLPTR
            }
        }
    },
    { 0xFF,
        {}
    }
};

#endif

/* [] END OF FILE */
