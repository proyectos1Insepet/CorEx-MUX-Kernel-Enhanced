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

#define _MAX_TIMEOUT_REGISTERS_     16

typedef enum
{
    TOIDLE = 0x01,
    TOBUSY
} TIMEOUTSLOTSTATE;

typedef struct
{
    uint8 _slotstate;
    uint8 _timeoutidentifier;
    uint16 _responsetimeoutlimit;//in Milliseconds
    uint8  _timeoutmultiplier;//Overrun counter
    uint16 _timeoutreference;//Time reference
    
    void (*Callback)(uint8 identifier);
} TimeoutRegister;

/* [] END OF FILE */
