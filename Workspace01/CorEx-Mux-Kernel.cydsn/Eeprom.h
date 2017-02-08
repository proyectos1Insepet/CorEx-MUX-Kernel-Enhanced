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

*************************** EEPROM GLOBAL MEMORY MAP ***************************
*/
#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"

// system includes
#include <stdint.h>     // definicion estandar de tipos de datos
#include <stdbool.h>    // definicion estandar de tipo booleano para c
#include <math.h>

#define _EEPROM_SLAVE_ADDRESS_BY_MEMARRAY_      0x54
#define _EEPROM_SLAVE_ADDRESS_BY_PAGEADDR_      0x5C
#define _EEPROM_PAGE_RAWSIZE_                   66   //64 bytes per eeprom page + 2 bytes for page addressing

#define _EEPROM_PAGE_FIRST32START_               0x00
#define _EEPROM_PAGE_SECOND32START_              0x20

#define _EEPROM_PAGE_FIRST16START_               0x00
#define _EEPROM_PAGE_SECOND16START_              0x10
#define _EEPROM_PAGE_THIRD16START_               0x20
#define _EEPROM_PAGE_FOURTH16START_              0x30

#define _EEPROM_QUARTER_PAGE_SIZE_               0x10
#define _EEPROM_HALF_PAGE_SIZE_                  0x20
#define _EEPROM_FULL_PAGE_SIZE_                  0x40


enum _EEPROM_MEM_FORMAT_
{
    //Station Unique Identifier
    //Station corporate name, corporate id, address, phone, motto1, motto2
    //Station Corporate Name (0:64 bytes)
    EEPROM_CONFIGURATION_PAGE0 = 0x0000,
    //General configuration page 1
    EEPROM_CONFIGURATION_PAGE1 = 0x0040,
    //General configuration page 2
    EEPROM_CONFIGURATION_PAGE2 = 0x0080,
    //Station Motto1 (0:64 bytes)
    EEPROM_CONFIGURATION_PAGE3 = 0x00C0,
    //Station Motto2 (0:64 bytes)
    EEPROM_CONFIGURATION_PAGE4 = 0x0100,
    //Station Unique Identifier (0:2 bytes) / Min PRESET Money (2:16 bytes)
    EEPROM_CONFIGURATION_PAGE5 = 0x0140,
    //Min PRESET Volumen (0:16 bytes) / Money Format (16:8 bytes) / Volume Format (24:8 bytes)
    EEPROM_CONFIGURATION_PAGE6 = 0x0180,
    //Money Symbol (0:4 bytes) / Volume Symbol (4:4 bytes) / Number of printed copies (8:1 byte) / Pump Position Offset (9:1) / Timeout to Autonomous (10:1)  / Printer port #1 (11:1)   / Printer port #2 (12:1)  / Configuration Password 13:4
    EEPROM_CONFIGURATION_PAGE7 = 0x01C0,
        
    // Pump/Position 1 Id (0:1 byte)
    // Pump/Position 1 => Hose/Product 1 Id (1:1 byte) 
    // Pump/Position 1 => Hose/Product 2 Id (2:1 byte) 
    // Pump/Position 1 => Hose/Product 3 Id (3:1 byte) 
    // Pump/Position 1 => Hose/Product 4 Id (4:1 byte) 
    //Island Operator's Shift Work State [0x00:Closed, 0x01:Open] (5:1)
    EEPROM_DISPENSER_PUMP1_LAYOUT_PAGE = 0x0200,
    // Pump/Position 2 Id (0:1 byte) 
    // Pump/Position 2 => Hose/Product 1 Id (1:1 byte) 
    // Pump/Position 2 => Hose/Product 2 Id (2:1 byte) 
    // Pump/Position 2 => Hose/Product 3 Id (3:1 byte) 
    // Pump/Position 2 => Hose/Product 4 Id (4:1 byte) 
    //Island Operator's Shift Work State [0x00:Closed, 0x01:Open] (5:1)
    EEPROM_DISPENSER_PUMP2_LAYOUT_PAGE = 0x0240,
    // Pump/Position 3 Id (0:1 byte)    
    // Pump/Position 3 => Hose/Product 1 Id (1:1 byte) 
    // Pump/Position 3 => Hose/Product 2 Id (2:1 byte) 
    // Pump/Position 3 => Hose/Product 3 Id (3:1 byte) 
    // Pump/Position 3 => Hose/Product 4 Id (4:1 byte) 
    //Island Operator's Shift Work State [0x00:Closed, 0x01:Open] (5:1)
    EEPROM_DISPENSER_PUMP3_LAYOUT_PAGE = 0x0280,
    // Pump/Position 4 Id (0:1 byte)    
    // Pump/Position 4 => Hose/Product 1 Id (1:1 byte) 
    // Pump/Position 4 => Hose/Product 2 Id (2:1 byte) 
    // Pump/Position 4 => Hose/Product 3 Id (3:1 byte) 
    // Pump/Position 4 => Hose/Product 4 Id (4:1 byte) 
    //Island Operator's Shift Work State [0x00:Closed, 0x01:Open] (5:1)
    EEPROM_DISPENSER_PUMP4_LAYOUT_PAGE = 0x02C0,
    
    //Currently all pumps/positions are symetrical since all provide the same products
    //This memory layout supports extra room for future market requirements
    //Four products per pump each 16 byte in length (64 bytes per page)
    EEPROM_DISPENSER_PUMP1_PRODUCT_NAMES_PAGE = 0x0300,
    EEPROM_DISPENSER_PUMP2_PRODUCT_NAMES_PAGE = 0x0340,
    EEPROM_DISPENSER_PUMP3_PRODUCT_NAMES_PAGE = 0x0380,
    EEPROM_DISPENSER_PUMP4_PRODUCT_NAMES_PAGE = 0x03C0,
    
    //Currently all pumps/positions are symetrical since all provide the same products
    //This memory layout supports extra room for future market requirements
    //Four PPUs per pump each 16 byte in length (64 bytes per page)
    EEPROM_DISPENSER_PUMP1_PPU_INFO_PAGE = 0x0400,
    EEPROM_DISPENSER_PUMP2_PPU_INFO_PAGE = 0x0440,
    EEPROM_DISPENSER_PUMP3_PPU_INFO_PAGE = 0x0480,
    EEPROM_DISPENSER_PUMP4_PPU_INFO_PAGE = 0x04C0,
    
    //THIS PAGES WILL HOLD RESERVED INFORMATION THAT INSEPET COULD
    //USE TO AUDIT THE NECESSARY INFORMATION FROM THE MUX DEVICE
    EEPROM_INSEPET_RESERVED_PAGE1 = 0x0500,
    EEPROM_INSEPET_RESERVED_PAGE2 = 0x0540,
    EEPROM_INSEPET_RESERVED_PAGE3 = 0x0580,
    EEPROM_INSEPET_RESERVED_PAGE4 = 0x05C0,
    
    ////////////////////////////////////////////////////////////////////////
    //Four memory areas available per position/pump (64 bytes per page)
    //Memory areas with indexes numbered as _PUMP?1, will hold the first part of the transaction for CASH transactions (data captured throught the displays)
    //Memory areas with indexes numbered as _PUMP?2, will hold the second part of the transaction for CASH transactions (the end of the transaction with their corresponding values)
    //Memory areas with indexes numbered as _PUMP?3, will hold the first part of the transaction for CREDIT transactions (data captured throught the displays)
    //Memory areas with indexes numbered as _PUMP?4, will hold the second part of the transaction  for CREDIT transactions (the end of the transaction with their corresponding values)
    EEPROM_DISPENSER_PUMP11_TRANSACTION_PAGE = 0x0600,
    EEPROM_DISPENSER_PUMP12_TRANSACTION_PAGE = 0x0640,
    EEPROM_DISPENSER_PUMP13_TRANSACTION_PAGE = 0x0680,
    EEPROM_DISPENSER_PUMP14_TRANSACTION_PAGE = 0x06C0,
    
    EEPROM_DISPENSER_PUMP21_TRANSACTION_PAGE = 0x0700,
    EEPROM_DISPENSER_PUMP22_TRANSACTION_PAGE = 0x0740,
    EEPROM_DISPENSER_PUMP23_TRANSACTION_PAGE = 0x0780,
    EEPROM_DISPENSER_PUMP24_TRANSACTION_PAGE = 0x07C0,
    
    EEPROM_DISPENSER_PUMP31_TRANSACTION_PAGE = 0x0800,
    EEPROM_DISPENSER_PUMP32_TRANSACTION_PAGE = 0x0840,
    EEPROM_DISPENSER_PUMP33_TRANSACTION_PAGE = 0x0880,
    EEPROM_DISPENSER_PUMP34_TRANSACTION_PAGE = 0x08C0,
    
    EEPROM_DISPENSER_PUMP41_TRANSACTION_PAGE = 0x0900,
    EEPROM_DISPENSER_PUMP42_TRANSACTION_PAGE = 0x0940,
    EEPROM_DISPENSER_PUMP43_TRANSACTION_PAGE = 0x0980,
    EEPROM_DISPENSER_PUMP44_TRANSACTION_PAGE = 0x09C0,
    
};

uint8* GetEepromBuffer();
void ClearEepromBuffer();
void LoadEepromPage(uint16 page);
void StoreEepromPage(uint16 page);
void I2CBusLock();
void I2CBusUnlock();

/* [] END OF FILE */
