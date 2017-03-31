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
#include "Printer.h"

PrinterTemplate _g_printertemplate[] =
{
    { PRN_DATE,         "Fecha      : " },
    { PRN_HOUR,         "Hora       : " },
    { PRN_PLATE,        "Placa      : " },
    { PRN_MILLEAGE,     "Kilometraje: " },
    { PRN_TRANSTYPE,    "Tipo       : " },
    { PRN_PUMPPOS,      "Posicion   : " },
    { PRN_PRODUCT,      "Producto   : " },
    { PRN_PRODPRICE,    "Ppu        : " },
    { PRN_VOLUME,       "Volumen    : " },
    { PRN_MONEY,        "Dinero     : " },
    { PRN_INVNUMBER,    "Consecutivo: " },
    { PRN_BALANCE,      "Saldo      : " },
    { PRN_COMPANY,      "Empresa    : " },
    { PRN_ACCOUNT,      "Cuenta     : " },
    { PRN_ACCOUNTTYPE,  "Tipo Cuenta: " },
    { PRN_VISITSDAY,    "Visitas Dia: " },
    { PRN_VISITSWEEK,   "Visitas Sem: " },
    { PRN_VISITSMONTH,  "Visitas Mes: " },
    { PRN_VOLUMEDAY,    "Volumen Dia: " },
    { PRN_VOLUMEWEEK,   "Volumen Sem: " },
    { PRN_VOLUMEMONTH,  "Volumen Mes: " },
    { PRN_MONEYDAY,     "Dinero Dia : " },
    { PRN_MONEYWEEK,    "Dinero Sem : " },
    { PRN_MONEYMONTH,   "Dinero Mes : " },
    { PRN_SIGNATURE,    "Firma      : " },
    { PRN_ID,           "Cedula     : " },
    { 0x00,             NULL },
};

PrinterTemplate _g_printervolumetemplate[] =
{
    { PRN_VOLUNIT_US_GALLONS,           " Gal" },//US
    { PRN_VOLUNIT_UK_GALLONS,           " Gal" },//UK
    { PRN_VOLUNIT_LITERS,               " L" },//Liters
    { PRN_VOLUNIT_HAWAIIAN_GALLONS_GAS, " Gal" },//Hawaiian Gallons of Gasoline
    { PRN_VOLUNIT_HAWAIIAN_GALLONS_DIE, " Gal" },//Hawaiian Gallons of Diesel
    { 0x00,                             NULL }
};

PrinterTemplate _g_printerpricetemplate[] =
{
    { PRN_MONEY_GALLONS,           " $/Gal" },//Money units per Gallon
    { PRN_MONEY_LITERS,            " $/L" },//Money units per Liter
    { 0x00,                        NULL }
};

char8* GetPrinterTemplateLine(uint8 lineid)
{
    char8* retval = NULL;
    PNEAR_PRINTERTEMPLATEPTR ptr = &_g_printertemplate[0x00];
    while(ptr->_namedline != 0x00)
    {
        if(ptr->_namedline == lineid)
        {
            retval = (char8*)ptr->_pstrline;
            break;
        }
        ptr++;
    }
    return retval;
}

char8* GetPrinterVolumeTemplateTag(uint8 lineid)
{
    char8* retval = NULL;
    PNEAR_PRINTERTEMPLATEPTR ptr = &_g_printervolumetemplate[0x00];
    while(ptr->_namedline != 0x00)
    {
        if(ptr->_namedline == lineid)
        {
            retval = (char8*)ptr->_pstrline;
            break;
        }
        ptr++;
    }
    return retval;
}

char8* GetPrinterPriceTemplateTag(uint8 refid)
{
    char8* retval = NULL;
    if(refid == PRN_MONEY_LITERS)
        retval = (char8*)_g_printerpricetemplate[0x01]._pstrline;
    else
        retval = (char8*)_g_printerpricetemplate[0x00]._pstrline;
        
    return retval;
}

/* [] END OF FILE */
