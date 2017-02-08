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
#include <stdint.h>     // definicion estandar de tipos de datos
#include <stdbool.h>    // definicion estandar de tipo booleano para c
#include <math.h>

#include "GlobalDefinitions.h"

#define _AUXPSOC_I2C_SLAVE_ADDRESS_      0x07

uint8 WriteAuxPSoC(PNEAR_BYTE_PTR lpbuffer, uint16 buffersize, uint8 printerport);

/* [] END OF FILE */
