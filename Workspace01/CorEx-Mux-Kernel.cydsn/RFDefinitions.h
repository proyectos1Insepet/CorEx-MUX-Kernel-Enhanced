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


#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"

// system includes
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include "RF_Physical.h"

//Global constant definitions
#define TRUE    0b00000001
#define FALSE   0b00000000

#define     _RF_DATALINK_RETRIES_       10
#define     _RF_DATALINK_BLOCK_START0_   0xaa
#define     _RF_DATALINK_BLOCK_START1_   0x55

#define     _RF_DATALINK_ACK_           0xac
#define     _RF_DATALINK_NACK_          0xfa
//This is the extra space required for the transmitting buffer (data + datalength{1 byte} + start{1 byte} + crc{1 byte} + end{1 byte})
#define     _RF_DATALINK_OFFSET_        4

#define     _SYMETRICAL_BUFFER_SIZE_    32
//This is the maximum size for a transaction in the system
#define     _TRANSACTIONAL_BUFFER_SIZE_    8192

//number of blocks(1 byte) + block index(1 byte) + block length(1 byte)
#define     _TRANSPORT_LAYER_SIZE_      3
//block start0(1 byte) + block start1(1 byte) + payload length(1 byte) + crc(1 byte)
#define     _DATALINK_LAYER_SIZE_      4

#define     RF_INTERBYTE_DELAY      CyDelay(1);
/*
@RF Networking Stack state machine
@Created by: HIJH
@Date: Août 31 de 2016
This structure carry all of the necessary fields to hold the state of the networking stack at every moment. 
The entire structure must be initialized and updated depending on which phase of the communication the 
networking stack is.

There are mainly two possible states, these are:

1) Standard Messaging State
2) Acknowledgement / No Acknowledgement State

For the first state 
*/
typedef struct _NETWORKING_STACK_STATE_
{
    //bit fields
    uint8 _healtylink:1;//Global flag that will be updated by the ping agent (the one that pings periodically the other peer)    
    uint8 _datalinkrxready:1; //This is always set once a block of data has arrived
    uint8 _datalinktimeout:1; //Timeout of the current transmission
    uint8 _datalinkackphase:1; 
    //variables
    uint8 _datalinkack;
    uint8 _datalinktimeoutcounter;
    uint32 _datalinktotalfailures;
    //This is the pointer for the transactional buffer that hold the current start position to be written
    uint16 _datalinktranspointer;
    
    //These are global buffers provided for the TX/RX operations
    char8 _rxbuffer[_SYMETRICAL_BUFFER_SIZE_];
    char8 _txbuffer[_SYMETRICAL_BUFFER_SIZE_];
    char8 _transbuffer[_TRANSACTIONAL_BUFFER_SIZE_];
} NetworkingStackState;

//The unique and global networking stack state machine
NetworkingStackState _g_networkingstack;


bool TransportRF(char8* pbuffer, uint16 buffersize);
void UnscrambleRxBuffer(uint8 position, uint8 buffersize);
void ScrambleTxBuffer(uint8 buffersize);
bool DataLinkRF(uint8 buffersize);

//Data Link timeout prototypes
void StartSecurityLayer();
void StartDataLinkTransmission();
void StartDataLinkReception();
void StartDataLinkTimeout();
void StopDataLinkTimeout();

char8 CalcCRC(char8 *pbuffer, uint8 buffersize);

double ceil(double val);


/* [] END OF FILE */
