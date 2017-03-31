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
#include <math.h>
#include <stdint.h>
#include <stdbool.h>


#define _EOS_                       0x00
#define _ASCII_SPACE_               0x20
#define _ASCII_LINEFEED_            0x0A
#define _ASCII_CR_                  0x0D
#define _ASCII_DASH_                0x2A
#define _ASCII_SLASH_               0x2F
#define _ASCII_COLON_               0x3A
#define _ASCII_POINT_               0x2E
#define _ASCII_COMMA_               0x2C
#define _MAX_PRINTER_LINE_WIDTH_    0x1F
#define _LEFT_MARGIN_               0x01

#define PRINTER_LINEFEED(a)  \
WriteAuxPSoC(_g_linefeed, 0x01, a)

#define PRINTER_AUTOCUT(a)   \
WriteAuxPSoC(_g_autocut, 0x04, a)

#define PRINTER_INDENTLINE(a, b)    \
WriteAuxPSoC(_g_padcharacters, a, b)

#define PRINTER_SEPARATOR(a)                            \
PRINTER_LINEFEED(a);                                    \
WriteAuxPSoC(_g_separator, _MAX_PRINTER_LINE_WIDTH_, a);\
PRINTER_LINEFEED(a)

enum _PRINTER_PORTS_
{
    PRINTER_PORT1_SIDE1     = 0x01,//SERIAL PORT RS232 (J14)
    PRINTER_PORT2_SIDE1     = 0x02,//SERIAL PORT TTL (J16)
    
    PRINTER_PORT1_SIDE2     = 0x03,//SERIAL PORT RS232 (J15)
    PRINTER_PORT2_SIDE2     = 0x04,//SERIAL PORT TTL (J17)
};

enum _PRINTER_NAMED_LINES_
{
    PRN_DATE = 0x01,
    PRN_HOUR,
    PRN_PLATE,
    PRN_MILLEAGE,
    PRN_TRANSTYPE,
    PRN_PUMPPOS,
    PRN_INVNUMBER,
    PRN_BALANCE,
    PRN_COMPANY,
    PRN_ACCOUNT,
    PRN_VISITSDAY,
    PRN_VISITSWEEK,
    PRN_VISITSMONTH,
    PRN_VOLUMEDAY,
    PRN_VOLUMEWEEK,
    PRN_VOLUMEMONTH,
    PRN_MONEYDAY,
    PRN_MONEYWEEK,
    PRN_MONEYMONTH,
    PRN_ACCOUNTTYPE,
    PRN_PRODUCT,
    PRN_PRODPRICE,
    PRN_VOLUME,
<<<<<<< HEAD
    PRN_MONEY,
    PRN_ID,
    PRN_SIGNATURE,
    PRN_SERIAL
=======
    PRN_MONEY
>>>>>>> parent of cf6f4cd... Ultima version para pruebas 18.5
};

enum _PRINTER_VOLUME_UNIT_TYPE_
{
    PRN_VOLUNIT_US_GALLONS = 0x01,
    PRN_VOLUNIT_UK_GALLONS,
    PRN_VOLUNIT_LITERS,
    PRN_VOLUNIT_HAWAIIAN_GALLONS_GAS,
    PRN_VOLUNIT_HAWAIIAN_GALLONS_DIE
};

enum _PRINTER_PRICE_UNITS_
{
    PRN_MONEY_GALLONS = 0x01,
    PRN_MONEY_LITERS = 0x03
};

typedef struct
{
    uint8   _namedline;
    const char8  *_pstrline;    
} PrinterTemplate, *PNEAR_PRINTERTEMPLATEPTR;

typedef struct
{
    uint8 _printerportside1;
    uint8 _printerportside2;
    
} PrinterLayout, *PNEAR_PRINTERLAYOUTPTR;

extern PrinterLayout _g_printerlayout;
extern PrinterTemplate _g_printertemplate[];

char8* GetPrinterTemplateLine(uint8 lineid);
char8* GetPrinterVolumeTemplateTag(uint8 lineid);
char8* GetPrinterPriceTemplateTag(uint8 refid);

/* [] END OF FILE */
