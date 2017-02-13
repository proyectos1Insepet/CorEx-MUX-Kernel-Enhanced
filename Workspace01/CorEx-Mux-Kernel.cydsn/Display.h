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

#define _MAX_DECIMALS_          1
#define _MAX_DISPLAY_PATHS_     6

/////////////////////////////////////////////////////////////////////////////////////////////////
///THIS IS MEANT ONLY TO CAPTURE THE SCREEN IDS
/////////////////////////////////////////////////////////////////////////////////////////////////  
//#define _DISPLAY_AV_SCREENS_
#undef _DISPLAY_AV_SCREENS_
/////////////////////////////////////////////////////////////////////////////////////////////////  

enum _AVAILABLE_DISPLAYS_
{
    DISPLAY_INICIO0 = 0x01,
    DISPLAY_INICIO1,
    DISPLAY_INICIO2,

    DISPLAY_FORMA_PAGO_DESEADA = 0x04,
    DISPLAY_FORMA_PROGRAMACION = 0x05,
    DISPLAY_INTRODUZCA_VALOR = 0x06,
    DISPLAY_INTRODUZCA_VALOR2 = 0x0F,
    DISPLAY_SELECCIONE_PRODUCTO3 = 0x7E,
    DISPLAY_SELECCIONE_PRODUCTO4 = 0x86,
    DISPLAY_SUBA_MANIJA = 0x07,
    DISPLAY_DESPACHANDO = 0x08,
    DISPLAY_BAJE_MANIJA = 0x09,
    DISPLAY_DIGITE_PLACA = 0x0A,
    DISPLAY_DESEA_IMPRIMIR_RECIBO = 0x0B,
    
    DISPLAY_INGRESE_PASSWORD    = 0x25,
    DISPLAY_PASSWORD_VALIDO    = 0x26,
    DISPLAY_PASSWORD_INVALIDO    = 0x27,
    DISPLAY_NUEVO_PASSWORD    = 0x2F,
    
    DISPLAY_TIEMPO_EXPIRADO     = 0x2C,
    
    DISPLAY_GRACIAS_VUELVA_PRONTO = 0x0C,

    DISPLAY_ID_DIGITAL = 0x85,
    DISPLAY_ID_NO_RECONOCIDO = 0x1C,//0x11,
    DISPLAY_ESPERANDO_ID = 0x12,
    DISPLAY_ID_RECONOCIDO = 0x13,
    
    DISPLAY_INTRODUZCA_CEDULA = 0x24,
    DISPLAY_INTRODUZCA_VOLUMEN = 0x0D,
    DISPLAY_INTRODUZCA_KILOMETRAJE = 0x0E,
    DISPLAY_OPERACIONES     = 0x83, //antes 0x22
    DISPLAY_CONFIGURACIONES = 0x5A,
    DISPLAY_COPIA_RECIBO = 0x31,    
    DISPLAY_IMPRIMIENDO_RECIBO = 0x35,
    
    DISPLAY_SELECCIONE_POSICION = 0x2E,
    DISPLAY_OPERACION_CANCELADA = 0x8C,
    DISPLAY_ESPERANDO_AUTORIZACION = 0x96,
    DISPLAY_POR_FAVOR_ESPERE = 0x39,
    DISPLAY_AUTORIZACION_ACEPTADA = 0x97,
    DISPLAY_AUTORIZACION_RECHAZADA = 0x98,
    
    DISPLAY_CONFIGURAR_FECHA_HORA    = 0x3E,
    DISPLAY_HORA    = 0x3F,
    DISPLAY_FECHA   = 0x40,
    
    DISPLAY_OPCION_TURNO = 0x8D,
    DISPLAY_ABRIR_TURNO = 0x23,
    DISPLAY_CERRAR_TURNO = 0x2A,
    DISPLAY_CANCELADO_X_PC = 0x2B,
    
    DISPLAY_USUARIO_VALIDO = 0x3C,
    DISPLAY_USUARIO_INVALIDO = 0x41,
    
    DISPLAY_AMBAS_IMPRESORAS_FUNCIONANDO = 0x6B,
    DISPLAY_IMPRESORA_NO_FUNCIONANDO = 0x67,
    
    DISPLAY_IDENTIFICADOR_ESTACION   = 0x64,
    DISPLAY_PRECIO_POR_UNIDAD   = 0x65,
    
    DISPLAY_DUMMY0              = 0x99,
    DISPLAY_DUMMY1              = 0x9A,
    DISPLAY_DUMMY2              = 0x9B,
    DISPLAY_DUMMY3              = 0x9C,
    
    DISPLAY_SIDE_DUMMY_DISPLAY   = 0xFA,
    DISPLAY_NULL = 0xFF
};

enum _DISPLAY_STATES_
{
    DISPLAY_NO_STATE = 0x00,
    
    DISPLAY_CREDIT_AUTHORIZATION_ACCEPTED = 0xA0,
    DISPLAY_CREDIT_AUTHORIZATION_REJECTED,
    DISPLAY_IBUTTON_READ_ACCEPTED,
    DISPLAY_HOSE_ACTIVATED,

    DISPLAY_LESS_THAN_FOUR_POSITIONS,
    
    DISPLAY_OPEN_WORK_SHIFT,
    DISPLAY_CLOSE_WORK_SHIFT,
    
    DISPLAY_WORK_SHIFT_OPERATOR_ACCEPTED,
    DISPLAY_WORK_SHIFT_OPERATOR_REJECTED,
    
    DISPLAY_NEW_PASSWORD,
    DISPLAY_INVALID_PASSWORD,
    DISPLAY_VALID_PASSWORD,
};

typedef enum
{
    DISPLAY1,
    DISPLAY2
} DISPLAY;

typedef enum
{
    POSITIONA = 0x56,//Facing the screen this is the one on the LEFT either side
    POSITIONB = 0x57//Facing the screen this is the one on the RIGHT either side
} PUMPPOSITION;

enum _DISPLAY_TEXT_OPACITY_
{
    DISP_TEXT_OPAQUE = 0xC5,
    DISP_TEXT_TRANSP = 0x85
};

typedef struct
{
    uint8  _size;
    uint8  _face;
    uint8  _opaquebackground;
    uint16 _backcolor;
    uint16 _forecolor;
} FontData;

typedef void (*Decorator)(void *pparam);
typedef bool (*InputValidator)(void *pparam);
typedef void (*InputHandler)(void *pparam);
typedef void (*Initializer)(void *pparam);
typedef void (*Finalizer)(void *pparam);

typedef Decorator       FPTRDECORATOR;
typedef InputValidator  FPTRINPUTVALIDATOR;
typedef InputHandler    FPTRINPUTHANDLER;
typedef Initializer     FPTRINITIALIZER;
typedef Finalizer       FPTRFINALIZER;

typedef struct
{
    uint8 _refchar;
    uint8 _mappedto;
} DisplayCharacterMap;

typedef struct
{
    uint8  _refscrid;
    uint16 _bufferpos;
    uint8  _bufferlength;
} DisplayScreenBufferMap;

typedef struct
{
    uint16 _xdefpos;//Default X input position
    uint16 _ydefpos;//Default Y input position
    uint8 _maxchars;//Maximum number of characters
    
    FontData _font;
} DisplayInputBox;

//This structure comprises the core support for the display flow model for all of the screens. 
//It allows for the "fine grained" configuration of the paths of the display flow
typedef struct 
{
    //Screen identifier
    uint8 _scrid;
    //Parent screen identifier to allow multiple paths
    uint8 _parentscrid;
    //Parent code
    uint8 _parentcode;
    //Maximum time to generate timeout (leave as zero if not timing out)
    uint16 _responsetimeoutlimit;
    //The screen will be thrown on timeout
    uint8 _timeoutscrid;
    //When one of the codes of "_pathcodes" is posted from the current screen, the matching display
    //code in "_pathnodes" will be used to throw the next screen
    uint8 _pathcodes[_MAX_DISPLAY_PATHS_];
    uint8 _pathnodes[_MAX_DISPLAY_PATHS_];
    //When the external state changes and matches one of the states predefined in "_statecodes", the matching
    //screen in "_statenodes" will be thrown
    uint8 _statecodes[_MAX_DISPLAY_PATHS_];
    uint8 _statenodes[_MAX_DISPLAY_PATHS_];
    //This function pointer can perform extra decoration and will be executed in case its pointer is different from null
    Decorator _decorator;
    //This function pointer can perform input capture into a buffer. It needs to translate the input codes into
    //the required ASCII information.
    InputHandler _inputhandler;
    //This function pointer can perform final validation of the captured data
    InputValidator _inputvalidator;
    //If this screen has an InputBox, this is the default position for text input inside the inputbox
    DisplayInputBox _inputboxinfo;
    //This function pointer can perform some sort of initialization tasks
    Initializer _initializer;
    //This function pointer can perform some sort of initialization tasks
    Finalizer _finalizer;
} DisplayFlow, *PDISPLAYFLOWPTR;

#define _DISPLAY_BUFFER_SIZE_       16

typedef struct
{
    uint16 _xpos;
    uint16 _ypos;
    uint8 _bufferindex;
    char8 _buffer[_DISPLAY_BUFFER_SIZE_];
    
} DisplayBuffer;

typedef struct
{
    uint8  _dispid;
    uint8  _sinktarget;
    uint8  _nextscrid;
    uint8  _currscrcode;
    uint8  _currentstate;
    uint8  _statechangescrid;
    uint16 _timeoutreftick;
    uint16  _timeoutmultiplier;
    uint8  _timeoutscrid;
    
    uint8 _val0;
    uint8 _val1;
    
    bool _starting;
    
    DisplayFlow *_pcurrflow;
    DisplayFlow *_preferenceflow;
    DisplayBuffer _bufferinfo;
    
    void *_psequesteredmessagesink;
    void *_pdata;
    
    bool (*DisplayValidator)(void *pparam);
    
} DisplayLayout, *PDISPLAYLAYOUTPTR;

typedef struct
{
    uint8 _currindex;
    bool  _colontoggler;
    uint8 _timeoutmultiplier;
    char8 _g_homeanimation[4];
    
} DisplayHomeAnimation;

extern DisplayHomeAnimation _g_homeanimarray[];
extern DisplayScreenBufferMap _g_buffscrlut[];
extern DisplayCharacterMap _g_charmap[];
extern DisplayFlow _g_displayflow1[];
extern DisplayFlow _g_displayflow2[];
extern DisplayLayout _g_display1;
extern DisplayLayout _g_display2;

void Display1Logic(void *pdata);
void Display2Logic(void *pdata);
PDISPLAYLAYOUTPTR GetDisplayFromPumpID(uint8 pumpid);

void ParseDisplayFlow(void *pparam);
void PrepareDisplayStructure(void *pparam);
char8 ParseDisplayStream(char* pbuffer, uint8 buffersize);
Decorator DisplayFlowDecorator(void *pparam);
InputHandler DisplayFlowInputHandler(void *pparam);
InputValidator DisplayFlowInputValidator(void *pparam);
Initializer DisplayInitializer(void *pparam);
Finalizer DisplayFinalizer(void *pparam);
uint16 DisplayOutputString(uint16 xpos, uint16 ypos, char *pbuffer, char8 *pstring, uint16 buffersize, FontData fontdata);
void DrawHomeDateTime(void *pparam);
char8 UnmapCode(uint8 code);
void StoreTransactionData(void *pparam);
uint16 GetBufferIndexFromDisplayID(uint8 id);
uint8 GetBufferLengthFromDisplayID(uint8 id);
uint8 GetPumpIndexFromDisplay(void *pparam);
void GetProductNames(uint8 display, uint8 side, uint8 product, char8 *pbuffer);
void iButtonScreenNotification(void *pparam);
bool ValidatePumpAvailability4Positions(void *pparam);
bool ValidatePumpAvailability21Positions(void *pparam);
void GetPumpsAvailability(void *pparam, uint8 *pvalues);
void GetPumpsPrintState(void *pparam, bool *pvalues);
void PrintLastP1(void *pparam); ///Para incluir impresion

#define DISPLAY_TIMEOUT_NOTIMEOUT     0
#define DISPLAY_TIMEOUT_100MSECS      100
#define DISPLAY_TIMEOUT_2SECS         2000
#define DISPLAY_TIMEOUT_5SECS         5000
#define DISPLAY_TIMEOUT_10SECS        10000
#define DISPLAY_TIMEOUT_15SECS        15000
#define DISPLAY_TIMEOUT_20SECS        20000
#define DISPLAY_TIMEOUT_30SECS        30000
#define DISPLAY_TIMEOUT_60SECS        60000


/* [] END OF FILE */
