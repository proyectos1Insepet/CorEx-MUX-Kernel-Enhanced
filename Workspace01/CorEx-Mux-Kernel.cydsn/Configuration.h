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

//////////////////////////////////////////////////////////////////////////////
//**************************** LANGUAGE DEFINITIONS **************************
#define _LANGUAGE_SPANISH_
#undef  _LANGUAGE_ENGLISH_
#undef  _LANGUAGE_PORTUGUESE_
#undef  _LANGUAGE_FRENCH_

//////////////////////////////////////////////////////////////////////////////
//**************************** LOCALE DEFINITIONS **************************
#define _L_COLOMBIA_
#undef  _L_ECUADOR_
#undef  _L_PERU_
#undef  _L_BRASIL_
#undef  _L_COSTARICA_
#undef  _L_URUGUAY_

//////////////////////////////////////////////////////////////////////////////
//THESE LANGUAGE SPECIFIC RESOURCE BLOCKS ARE USED FOR LANGUAGE DEFINITION AND CUSTOMIZATION
//OF THE OUTPUT SOURCES LIKE DISPLAYS AND PRINTERS
#ifdef _LANGUAGE_SPANISH_
#endif
#ifdef _LANGUAGE_ENGLISH_
#endif
#ifdef _LANGUAGE_PORTUGUESE_
#endif
#ifdef _LANGUAGE_FRENCH_
#endif

//////////////////////////////////////////////////////////////////////////////
//**************************** PRINTER TYPE DEFINITION ****************************
//#define PRINTER_TYPE_PANEL
#define PRINTER_TYPE_KIOSK

//////////////////////////////////////////////////////////////////////////////
//**************************** CLIENT DEFINITIONS *********************************
#define  _CLIENT_BIOMAX_
//#define  _CLIENT_BRIO_
//#define  _CLIENT_CENCOSUD_
//#define  _CLIENT_ECOPETROL_
//#define  _CLIENT_ESSO_
//#define  _CLIENT_EXITO_
//#define  _CLIENT_GULF_
//#define  _CLIENT_MINEROIL_
//#define  _CLIENT_MOBIL_
//#define  _CLIENT_PETROBRAS_
//#define  _CLIENT_PLUS_
//#define  _CLIENT_TERPEL_
//#define  _CLIENT_TEXACO_
//#define  _CLIENT_ZEUS_
//#define  _CLIENT_PETROMIL_
//#define  _CLIENT_TREBOL_
//#define  _CLIENT_PUMA_ //No esta para kiosko
//#define _NO_LOGO_

#ifdef _NO_LOGO_
    #define  _CLIENT_TERPEL_
#endif

//This is the activated logo array based on the CLIENT DEFINITION
extern uint16 _g_stationidentifier;

#undef _DEBUG_

/* [] END OF FILE */
