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
#include "SinkManager.h"
#include "Display.h"

//@Created By: HIJH
//@Septembre de 2016
DisplayScreenBufferMap _g_buffscrlut[] = 
{
//THIS IS THE MEMORY LAYOUT FOR PAYMENT IN CASH
    { DISPLAY_FORMA_PAGO_DESEADA,       0x0000, 0x02 }, //Two bytes allocated for the payment option
    { DISPLAY_SELECCIONE_POSICION,      0x0002, 0x02 }, //Two bytes allocated for the pump selection
    { DISPLAY_FORMA_PROGRAMACION,       0x0004, 0x02 }, //Two bytes allocated for the type of PRESET
    { DISPLAY_SELECCIONE_PRODUCTO3,     0x0006, 0x02 }, //Two bytes allocated for the product type (HOSE)
    { DISPLAY_SELECCIONE_PRODUCTO4,     0x0006, 0x02 }, //Two bytes allocated for the product type (HOSE)
    //Both have the same offfset and length since they are mutually exclusive
    { DISPLAY_INTRODUZCA_VALOR,         0x0008, 0x10 }, //Sixteen bytes allocated for the money
    { DISPLAY_INTRODUZCA_VOLUMEN,       0x0008, 0x10 }, //Sixteen bytes allocated for the volume
    { DISPLAY_DIGITE_PLACA,             0x0018, 0x08 }, //Eight bytes allocated for the car's plate
    { DISPLAY_DESEA_IMPRIMIR_RECIBO,    0x0020, 0x02 }, //Two bytes allocated for the invoice print option
    { DISPLAY_INTRODUZCA_KILOMETRAJE,   0x0022, 0x09 }, //Nine bytes allocated for the car's mileage
    { DISPLAY_ESPERANDO_ID,             0x002B, 0x08 }, //Eight bytes allocated for the iButton serial identifier
    { DISPLAY_SUBA_MANIJA,              0x0033, 0x02 }, //Two bytes allocated for the Hose identifier
    { DISPLAY_PRECIO_POR_UNIDAD,        0x0035, 0x06 },
    { DISPLAY_SIDE_DUMMY_DISPLAY,       0x003B, 0x02 },
    { DISPLAY_INTRODUZCA_CEDULA,        0x003D, 0x0A },
    { DISPLAY_INGRESE_PASSWORD,         0x0047, 0x08 },
    { 0x00, 0x0000, 0x00 }
//THIS IS THE MEMORY LAYOUT FOR PAYMENT IN CREDIT
};

//@Created By: HIJH
//@Septembre de 2016
uint16 GetBufferIndexFromDisplayID(uint8 id)
{
    uint16 retval = 0x0000;
    DisplayScreenBufferMap *ptr = _g_buffscrlut;
    while(ptr->_refscrid != 0x00)
    {
        if(ptr->_refscrid == id)
        {
            retval = ptr->_bufferpos;
            break;
        }
        ptr++;
    }    
    return retval;
}

//@Created By: HIJH
//@Septembre de 2016
uint8 GetBufferLengthFromDisplayID(uint8 id)
{
    uint8 retval = 0x00;
    DisplayScreenBufferMap *ptr = _g_buffscrlut;
    while(ptr->_refscrid != 0x00)
    {
        if(ptr->_refscrid == id)
        {
            retval = ptr->_bufferlength;
            break;
        }
        ptr++;
    }    
    return retval;
}

uint8 GetPumpIndexFromDisplay(LPVOID pparam)
{
    uint8 retval = 0;
    PDISPLAYLAYOUTPTR pdisplay = (PDISPLAYLAYOUTPTR)pparam;
    if(pdisplay->_dispid == DISPLAY1)
    {
        switch((uint8)((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SELECCIONE_POSICION)])
        {
            case POSITIONA:
            {
                retval = 0x00;
                break;
            }
            case POSITIONB:
            {
                retval = 0x02;
                break;
            }
        }
    }
    else if(pdisplay->_dispid == DISPLAY2)
    {
        switch((uint8)((PSINKMESSAGEPTR)pdisplay->_psequesteredmessagesink)->_buffer[GetBufferIndexFromDisplayID(DISPLAY_SELECCIONE_POSICION)])
        {
            case POSITIONA:
            {
                retval = 0x03;
                break;
            }
            case POSITIONB:
            {
                retval = 0x01;
                break;
            }
        }
    }
    
    return retval;
}

/* [] END OF FILE */
