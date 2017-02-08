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
#ifndef _DISPATCHER_DEFINITIONS_
#define _DISPATCHER_DEFINITIONS_
    
#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"

typedef void (*HandlerPtr)();

typedef struct _DISPATCH_TABLE_
{
    uint8 _commandid;
    HandlerPtr _hptr;
} DispatchTable;

//***********************************************************************************
//REMOTE DISPATCHER COMMAND ID DEFINITIONS
//ADD HERE CUSTOM IDENTIFIERS TO BE HANDLED
#define _DISP_HANDLER0_             0xa0
#define _DISP_HANDLER1_             0xa1
#define _DISP_HANDLER2_             0xa2
#define _DISP_HANDLER3_             0xa3

//***********************************************************************************
//LOCAL DISPATCHER COMMAND ID DEFINITIONS
//ADD HERE CUSTOM IDENTIFIERS TO BE HANDLED
#define _DISP_HANDLER0_             0xa0
#define _DISP_HANDLER1_             0xa1
#define _DISP_HANDLER2_             0xa2
#define _DISP_HANDLER3_             0xa3
//***********************************************************************************
//DISPATCHER HANDLER PROTOTYPES
//ADD HERE THE HANDLER PROTOTYPES
//***********************************************************************************
//void MyHandler();

//***********************************************************************************
#define _RF_LOOP_TEST_
//SELECT ONE OF THE PEERS AS INITIALIZER OF THE COMMUNICATION LOOP
#define _RF_LOOP_TEST_TRANSMITTER_
//#define  _RF_LOOP_TEST_RECEIVER_
//THIS BLOCK IS FOR TESTING PURPOSES ONLY!!
void InitTest();
void LoopHandler();
void DispHandler();
//***********************************************************************************


#endif

/* [] END OF FILE */
