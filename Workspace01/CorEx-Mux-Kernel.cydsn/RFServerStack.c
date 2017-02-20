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

#include "GlobalDefinitions.h"
#include "UARTManager.h"
#include "RFLink.h"

bool RFStateReqResp(void *pparam);
bool RFCreditSerialAuthReqResp(void *pparam);
bool RFCreditAuthorizationReqResp(void *pparam);
bool RFCreditSalePresetReportReqResp(void *pparam);
bool RFCashSalePresetReportReqResp(void *pparam);
bool RFTotalsDataReqResp(void *pparam);
bool RFTotalsDataReportReqResp(void *pparam);
bool RFPriceChangeReqResp(void *pparam);
bool RFGeneralPrintReqResp(void *pparam);
bool RFCopyPrintReqResp(void *pparam);
bool RFCreditSalePrintReqResp(void *pparam);
bool RFCashSalePrintReqResp(void *pparam);
bool RFPendingCreditSaleReqResp(void *pparam);
bool RFCashCreditSaleFinalReportReqResp(void *pparam);
bool RFPendingCashSaleReqResp(void *pparam);
bool RFBroadcastConfigurationReqResp(void *pparam);
bool RFPositionConfigurationReqResp(void *pparam);
bool RFIslandOperDataReqResp(void *pparam);
bool RFIslandAdminReqResp(void *pparam);
uint16 GeneralPrintRequestPayloadSizeResolver(LPVOID pbuffer);

void RFErrorHandler(void *pparam);

//@Created By: HIJH
//@Date: Septembre de 2016
//Server RPC mapping of invocation codes and Request / Response Handlers
RFRequestResponse _g_rfresolvers[] = 
{
    { 
        RF_MUX_STATE_REQUEST, RF_MUX_STATE_RESPONSE,
        RFStateReqResp,
        RFErrorHandler,
        _RF_MAX_TIMEOUT_LIMIT_
    },
    { 
        RF_MUX_CREDIT_SERIAL_AUTH_REQUEST, RF_MUX_CREDIT_SERIAL_AUTH_RESPONSE,
        RFCreditSerialAuthReqResp,
        RFErrorHandler,
        _RF_MAX_TIMEOUT_LIMIT_
    },
    { 
        RF_MUX_CREDIT_PRESET_AUTHORIZATION_REQUEST, RF_MUX_CREDIT_PRESET_AUTHORIZATION_RESPONSE,
        RFCreditAuthorizationReqResp,
        RFErrorHandler,
        _RF_MAX_TIMEOUT_LIMIT_
    },
    { 
        RF_MUX_CREDIT_PRESET_NOTIFICATION_REQUEST, RF_MUX_CREDIT_PRESET_NOTIFICATION_RESPONSE,
        RFCreditSalePresetReportReqResp,
        RFErrorHandler,
        _RF_MAX_TIMEOUT_LIMIT_
    },
    { 
        RF_MUX_CASH_SALE_START_REQUEST, RF_MUX_CASH_SALE_START_RESPONSE,
        RFCashSalePresetReportReqResp,
        RFErrorHandler,
        _RF_MAX_TIMEOUT_LIMIT_
    },
    { 
        RF_MUX_CASH_SALE_END_REPORT_REQUEST, RF_MUX_CASH_SALE_END_REPORT_RESPONSE,
        RFCashCreditSaleFinalReportReqResp,
        RFErrorHandler,
        _RF_MAX_TIMEOUT_LIMIT_
    },
    {
        RF_MUX_LAST_CASH_SALE_REQUEST, RF_MUX_LAST_CASH_SALE_RESPONSE,
        RFPendingCashSaleReqResp,
        RFErrorHandler,
        _RF_MAX_TIMEOUT_LIMIT_
    },
    { 
        RF_MUX_TOTALS_REQUEST, RF_MUX_TOTALS_RESPONSE,
        RFTotalsDataReqResp,
        RFErrorHandler,
        _RF_MAX_TIMEOUT_LIMIT_
    },
    { 
        RF_MUX_TOTALS_REPORT_REQUEST, RF_MUX_TOTALS_REPORT_RESPONSE,
        RFTotalsDataReportReqResp,
        RFErrorHandler,
        _RF_MAX_TIMEOUT_LIMIT_
    },
    { 
        RF_MUX_PRICE_CHANGE_REQUEST, RF_MUX_PRICE_CHANGE_RESPONSE,
        RFPriceChangeReqResp,
        RFErrorHandler,
        _RF_MAX_TIMEOUT_LIMIT_
    },
    { 
        RF_MUX_GENERAL_PRINT_REQUEST, RF_MUX_GENERAL_PRINT_RESPONSE,
        RFGeneralPrintReqResp,
        RFErrorHandler,
        _RF_MAX_TIMEOUT_LIMIT_
    },
    { 
        RF_MUX_COPY_REQUEST, RF_MUX_COPY_REQUEST_RESPONSE,
        RFCopyPrintReqResp,
        RFErrorHandler,
        _RF_MAX_TIMEOUT_LIMIT_
    },
    { 
        RF_MUX_CREDIT_SALE_PRINT_REQUEST, RF_MUX_CREDIT_SALE_PRINT_RESPONSE,
        RFCreditSalePrintReqResp,
        RFErrorHandler,
        _RF_MAX_TIMEOUT_LIMIT_
    },
    { 
        RF_MUX_CASH_SALE_PRINT_REQUEST, RF_MUX_CASH_SALE_PRINT_RESPONSE,
        RFCashSalePrintReqResp,
        RFErrorHandler,
        _RF_MAX_TIMEOUT_LIMIT_
    },
    { 
        RF_MUX_PENDING_CREDIT_SALE_REQUEST, RF_MUX_PENDING_CREDIT_SALE_RESPONSE,
        RFPendingCreditSaleReqResp,
        RFErrorHandler,
        _RF_MAX_TIMEOUT_LIMIT_
    },
    { 
        RF_MUX_BROADCAST_CFG_REQUEST, RF_MUX_BROADCAST_CFG_RESPONSE,
        RFBroadcastConfigurationReqResp,
        RFErrorHandler,
        _RF_MAX_TIMEOUT_LIMIT_
    },
    { 
        RF_MUX_POSITION_CFG_REQUEST, RF_MUX_POSITION_CFG_RESPONSE,
        RFPositionConfigurationReqResp,
        RFErrorHandler,
        _RF_MAX_TIMEOUT_LIMIT_
    },
    { 
        RF_MUX_ISLAND_OPER_DATA_REQUEST, RF_MUX_ISLAND_OPER_DATA_RESPONSE,
        RFIslandOperDataReqResp,
        RFErrorHandler,
        _RF_MAX_TIMEOUT_LIMIT_
    },
    { 
        RF_MUX_ISLAND_ADMIN_REQUEST, RF_MUX_ISLAND_ADMIN_RESPONSE,
        RFIslandAdminReqResp,
        RFErrorHandler,
        _RF_MAX_TIMEOUT_LIMIT_
    },
    {
        RF_MUX_NULL, RF_MUX_NULL,
        NULL, NULL, _RF_NO_TIMEOUT_
    }
};

//@Created By: HIJH
//@Date: Septembre de 2016
//Static payload size / RPC code map. If the specified size is
//zero, then the payload size is dynamic
RFRequestPayloadLengthMap _g_rfreqpayloadmap[] = 
{
    //FRAME SIZE MAPPING/ DEFINITION FOR THE COMMUNICATION COMING FROM THE REMOTE PEER (NSX)
    { RF_MUX_STATE_REQUEST,                         8u,   NULL },
    { RF_MUX_CREDIT_SERIAL_AUTH_REQUEST,            8u,   NULL },
    { RF_MUX_CREDIT_PRESET_AUTHORIZATION_REQUEST,   25u,  NULL },
    { RF_MUX_CASH_SALE_START_REQUEST,               8u,   NULL},
    { RF_MUX_CASH_SALE_END_REPORT_REQUEST,          8u,   NULL },
    
    { RF_MUX_LAST_CASH_SALE_REQUEST,                8u,   NULL},
    { RF_MUX_PENDING_CREDIT_SALE_REQUEST,           8u,   NULL },
    
    { RF_MUX_TOTALS_REQUEST,                        8u,   NULL },
    { RF_MUX_PRICE_CHANGE_REQUEST,                  15u,  NULL },
    { RF_MUX_GENERAL_PRINT_REQUEST,                 0u,   GeneralPrintRequestPayloadSizeResolver },
    { RF_MUX_COPY_REQUEST,                          0u,   GeneralPrintRequestPayloadSizeResolver },
    { RF_MUX_CREDIT_SALE_PRINT_REQUEST,             118u, NULL },
    { RF_MUX_CASH_SALE_PRINT_REQUEST,               19u,  NULL },
    { RF_MUX_BROADCAST_CFG_REQUEST,                 338u, NULL },
    { RF_MUX_POSITION_CFG_REQUEST,                  14u,  NULL },
    { RF_MUX_ISLAND_OPER_DATA_REQUEST,              9u,   NULL },
    { RF_MUX_ISLAND_ADMIN_REQUEST,                  9u,   NULL },
    
    //FRAME SIZE MAPPING/ DEFINITIONS FOR THE COMMUNICATION COMING FROM THE SIBBLINGS

    ////////////////////////////////////////////////
    //THESE IDENTIFIERS ARE RESERVED FOR FUTURE USE
    ////////////////////////////////////////////////    
    
    { RF_MUX_NULL, 0x0000, NULL }    
};

RFCommandStateMap _g_rfcommandstatemap[] =
{
    { RF_IDLE,              0x00 },
    { RF_DELIVERING,        RF_MUX_CASH_SALE_START_REQUEST },
    { RF_DELIVERING,        RF_MUX_CREDIT_PRESET_NOTIFICATION_REQUEST },
    { RF_CASHSALEREPORT,    RF_MUX_CASH_SALE_END_REPORT_REQUEST },
    { RF_CREDITSALEAUTH,    RF_MUX_CREDIT_PRESET_AUTHORIZATION_RESPONSE },
    { RF_WORKSHIFTREQ,      RF_MUX_ISLAND_OPER_DATA_REQUEST },
    { RF_CREDITSALEREPORT,  RF_MUX_PENDING_CREDIT_SALE_REQUEST },
    { RF_COPY_RECEIPT,      RF_MUX_COPY_REQUEST  },
    { RF_MUX_NULL,          RF_MUX_NULL }
};

//@Created By: HIJH
//@Date: Septembre de 2016
//This function acquires the payload size based on the unmapped RPC code
uint16 RFGetPayloadLengthFromRequestCode(uint8 reqcode)
{
    uint16 retval = 0x0000;
    PNEAR_RFREQUESTPAYLOADLENGTHMAP preqpaylliterator = _g_rfreqpayloadmap;
    while(preqpaylliterator->_requestcode != RF_MUX_NULL)
    {
        if(preqpaylliterator->_requestcode == reqcode)
        {
            retval = preqpaylliterator->_requestpayloadsize;
            break;
        }
        preqpaylliterator++;
    }
    return retval;
}

//@Created By: HIJH
//@Date: Octobre de 2016
//This function acquires the payload size resolver based on the unmapped RPC code
PayloadSizeResolver RFGetDynamicPayloadLengthFromRequestCode(uint8 reqcode)
{
    PayloadSizeResolver pretval = PNEAR_NULLPTR;
    PNEAR_RFREQUESTPAYLOADLENGTHMAP preqpaylliterator = _g_rfreqpayloadmap;
    while(preqpaylliterator->_requestcode != RF_MUX_NULL)
    {
        if(preqpaylliterator->_requestcode == reqcode)
        {
            pretval = preqpaylliterator->_fpPayloadSizeResolver;
            break;
        }
        preqpaylliterator++;
    }
    return pretval;
}

//@Created By: HIJH
//@Date: Septembre de 2016
//This function unmaps the RPC request code coming from the RF Link and returns 
//the mapped handler
PNEAR_REQRESPTR RFGetRequestResponseServerHandler(uint8 reqcode)
{
    PNEAR_REQRESPTR pretval = PNEAR_NULLPTR;
    PNEAR_REQRESPTR preqrespiterator = &_g_rfresolvers[0];
    while((preqrespiterator->_requestcode != RF_MUX_NULL) &&
    (preqrespiterator->_responsecode != RF_MUX_NULL))
    {
        if(preqrespiterator->_requestcode == reqcode)
        {
            pretval = preqrespiterator;
            break;
        }
        preqrespiterator++;
    }
    return pretval;
}


uint8 RFGetCRC(uint8 commandid, PNEAR_BYTE_PTR pbuffer)
{
    uint8 retval = 0x00;
    PNEAR_RFREQUESTPAYLOADLENGTHMAP preqplliterator = (PNEAR_RFREQUESTPAYLOADLENGTHMAP)_g_rfreqpayloadmap;
    while(preqplliterator->_requestcode != RF_MUX_NULL)
    {
        if(preqplliterator->_requestcode == commandid)
        {
            if(preqplliterator->_requestpayloadsize > 0)
                retval = pbuffer[preqplliterator->_requestpayloadsize - 1];
            else
                retval = pbuffer[preqplliterator->_fpPayloadSizeResolver(pbuffer) - 1];

            break;
        }
        preqplliterator++;
    }
    
    return retval;
}

uint8 RFUnmapCommand2State(uint8 commandid)
{
    uint8 retval = RF_MUX_NULL;
    PNEAR_RFCOMMANDSTATEMAPPTR ptr = &_g_rfcommandstatemap[0x00];
    while(ptr->_commandid != RF_MUX_NULL)
    {
        if(ptr->_commandid == commandid)
        {
            retval = ptr->_stateid;
            break;
        }
        
        ptr++;
    }
    return retval;
}

/* [] END OF FILE */
