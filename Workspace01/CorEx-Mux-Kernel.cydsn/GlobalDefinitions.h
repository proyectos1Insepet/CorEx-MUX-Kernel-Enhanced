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

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"

// system includes
#include <stdint.h>     // definicion estandar de tipos de datos
#include <stdbool.h>    // definicion estandar de tipo booleano para c
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define HIGH    true
#define LOW     false
    
#define LOOP_FOREVER    \
for(;;) loop_invoke();

#define GLOBAL_LOOP_PUSH(m) loop_push(m)
#define FOR(a, b, c)    \
a;for(; b; c)

#define INITITALIZATION_DELAY0     CyDelay(1000)
#define INITITALIZATION_DELAY1     CyDelay(500)
//**********************************************************************
typedef void (*PTR_LOOP)();

typedef void *LPVOID;

#define _MAX_LOOP_POINTERS   16
typedef struct __LOOP_Container
{
    PTR_LOOP _ptrarr[_MAX_LOOP_POINTERS];
    uint8_t _index;
    
} LOOPContainer;

void loop_push(PTR_LOOP _func);
void loop_invoke();

uint16 RGB216BITS(uint8 r, uint8 g, uint8 b);
void GlobalInitializer();
char8 CalcCRC(char8 *pbuffer, uint8 buffersize);
uint8 CRCCheck(char8 crcseed, uint8 data);
uint8 RawCRCCheck(char8 *pdata, uint16 buffersize);
void B78AD90CF552D9B30A(uint64 number, uint8 *pbuffer);
uint64 BCDPositiveValueToUInt(uint8 *pbuffer, uint8 buffersize, uint8 mask);
uint32 MSDBCDBUFF2HEX(uint8 *pbuffer, uint16 buffersize);
uint32 LSDBCDBUFF2HEX(uint8 *pbuffer, uint16 buffersize);
double DLSDBCDBUFF2HEX(uint8 *pbuffer, uint16 buffersize);
void PSTRBUFFTOGG(uint8 *pbuffer, uint8 buffersize);
bool ISASCIIPRNBLE(uint8 *pbuffer, uint8 buffersize);
bool ISVALIDBCD(uint8 *pbuffer, uint16 buffersize);

#define PNEAR_NULLPTR NULL
#define RANDOMDELAYTOKEN     (uint16)(1024.0*((double)rand()/(double)RAND_MAX) + 10)
#define RANDOMDELAY                 \
{                                   \
    uint16 delay = RANDOMDELAYTOKEN;\
    uint16 delaytoken = 0x0000;     \
    while(delaytoken++ < delay);    \
}



typedef char8  *PNEAR_STR_PTR;
typedef uint8  *PNEAR_BYTE_PTR;
typedef uint16 *PNEAR_WORD_PTR;
uint8 HoseDispenser;

/* [] END OF FILE */
