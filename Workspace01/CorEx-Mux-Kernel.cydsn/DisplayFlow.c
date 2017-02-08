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
#include "Dispenser.h"
#include "Display.h"

void DisplayInputHandler(void *pparam);
void DisplayNumericInputHandler(void *pparam);
void DisplayIntegerInputHandler(void *pparam);
void DisplayMoneyInputHandler(void *pparam);
void DisplayVolumeInputHandler(void *pparam);
void DisplayInputPasswordHandler(void *pparam);
void DisplayIDInputHandler(void *pparam);
void DisplayProductSelectionInputHandler(void *pparam);
bool NonEmptyDataValidator(void *pparam);
bool SecureConfigurationValidator(void *pparam);
bool NonEmptyMoneyValidator(void *pparam);
bool NonEmpty15ModuloValidator(void *pparam);
void DrawProductLabels(void *pparam);
void DrawNumberOfCopies(void *pparam);
void DrawDateTime(void *pparam);
void CancelMessageDecorator(void *pparam);
void WorkShiftDecorator(void *pparam);
void AuthorizationMessageDecorator(void *pparam);
void AcceptedAuthorizationMessageDecorator(void *pparam);
void RejectedAuthorizationMessageDecorator(void *pparam);
void ProductNameSelectionDecorator(LPVOID pparam);
void MaskoutOptionDecorator(void *pparam);
void SequesterMessageSink(void *pparam);
void SaveNewPassword(void *pparam);
void ReleaseMessageSink(void *pparam);
void CleanMessageSink(void *pparam);
void InitiateiButtonScan(void *pparam);
void TerminateiButtonScan(void *pparam);
void SendAuthorizationRequest4Credit(LPVOID pparam);
void ValidateAuthorizationResponse4Credit(LPVOID pparam);
void ValidateHosePositionState(LPVOID pparam);
void KillHosePositionStateValidation(LPVOID pparam);
void InitiateUpdateDateTime(void *pparam);
void TerminateUpdateDateTime(void *pparam);
void InitiateHomeAnimation(void *pparam);
void TerminateHomeAnimation(void *pparam);
void InitiateWorkShiftValidation(void *pparam);
void RequestWorkShiftOperAuthorization(void *pparam);
void DisplayStationIDLoad(void *pparam);
void DisplayStationIDStore(void *pparam);
void DisplayStationIDDecorator(void *pparam);
void DisplayPumpIDOffsetLoad(void *pparam);
void DisplayPumpIDOffsetStore(void *pparam);
void DisplayPumpIDOffsetDecorator(void *pparam);
void PrintersWorking(void *pparam);
void EitherPrinterWorking(void *pparam);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
@Created By: HIJH
@Date: Septembre de 2016
The "DisplayFlow" structure allows to create a sophisticated "Screen Flow" for all of the
defined screens provided that the processed codes are consistent across all of the screens.
The structure manages multiple flow paths, timeouts, decorators, input capturers, input validators,
font and caret data etc.
*/
DisplayFlow _g_displayflow1[] = 
{
    //Display ID - Condition - { LIST OF CHILDREN DISPLAYS TO BE THROWN BASED ON THE MATCHING CODE SET }
    //The condition is the required value to throw the display (0xFF means no condition required. only the first)
    { DISPLAY_INICIO0, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x3A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_SELECCIONE_POSICION, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, //No state change data defined
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, //No state change data defined
        DrawHomeDateTime,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        InitiateHomeAnimation,
        TerminateHomeAnimation
    },
    { DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_INICIO0, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, //No state change data defined
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, //No state change data defined
        CancelMessageDecorator,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        CleanMessageSink
    },
    { DISPLAY_CANCELADO_X_PC, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_INICIO0, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, //No state change data defined
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, //No state change data defined
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_SELECCIONE_POSICION, DISPLAY_INICIO0, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { POSITIONA, POSITIONB, 0x7E, 0x94, 0xFF, 0xFF },
        { DISPLAY_FORMA_PAGO_DESEADA, DISPLAY_FORMA_PAGO_DESEADA, DISPLAY_INICIO0, DISPLAY_INICIO0, 0xFF, 0xFF }, 
        { DISPLAY_LESS_THAN_FOUR_POSITIONS, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_FORMA_PAGO_DESEADA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,//Display Decorator
        PNEAR_NULLPTR,//Display Input Handler
        ValidatePumpAvailability4Positions,//Display Validator
        { 0x0000, 0x0000, 0x00, {} },
        ///THIS INITIALIZER "SEQUESTERS" ONE MESSAGE FROM THE MESSAGE SINK AND USES ITS BUFFER AS A STORAGE FOR THE TRANSACTION
        SequesterMessageSink,//MUST BE EXECUTED AT LEAST ONE SCREEN BEFORE BEING VALIDATED SINCE THE ISR SIMULATES MULTITASKING WITH THE MAIN LOOP
        PNEAR_NULLPTR
    },
    #ifdef _PRESET_NO_HOSE_
    { DISPLAY_FORMA_PAGO_DESEADA, DISPLAY_SELECCIONE_POSICION, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0D, 0x0E, 0x45, 0x94, 0x7E, 0xFF },
        { DISPLAY_FORMA_PROGRAMACION, DISPLAY_ID_DIGITAL, DISPLAY_OPERACIONES, DISPLAY_INICIO0, DISPLAY_INICIO0, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        ValidatePumpAvailability21Positions,//Display Validator
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    #endif
    #ifdef _PRESET_HOSE_
    { DISPLAY_FORMA_PAGO_DESEADA, DISPLAY_SELECCIONE_POSICION, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0D, 0x0E, 0x45, 0x94, 0x7E, 0xFF },
        { DISPLAY_SELECCIONE_PRODUCTO4, DISPLAY_ID_DIGITAL, DISPLAY_OPERACIONES, DISPLAY_INICIO0, DISPLAY_INICIO0, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        ValidatePumpAvailability21Positions,//Display Validator
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    #endif
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //PATH TO PAYMENT WITH IBUTTON/TAG OR KEYBOARD
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    { DISPLAY_ID_DIGITAL, DISPLAY_FORMA_PAGO_DESEADA, 0x0E,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0xB6, 0x7E, 0x94, 0xFF, 0xFF, 0xFF },
        { DISPLAY_INTRODUZCA_KILOMETRAJE, DISPLAY_INICIO0, DISPLAY_INICIO0, 0xFF, 0xFF, 0xFF }, //DISPLAY_INTRODUZCA_VALOR, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_INTRODUZCA_KILOMETRAJE, DISPLAY_ID_DIGITAL, 0xB6,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0x94, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_ESPERANDO_ID, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        DisplayIntegerInputHandler,
        NonEmptyDataValidator,
        { 0x0032, 0x002A, 0x09, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_ESPERANDO_ID, DISPLAY_INTRODUZCA_KILOMETRAJE, 0x0C,
        DISPLAY_TIMEOUT_10SECS, //timeout limit (seconds)
        DISPLAY_ID_NO_RECONOCIDO, //display id to be thrown on timeout
        { 0x3B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { DISPLAY_IBUTTON_READ_ACCEPTED, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_ID_RECONOCIDO, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        InitiateiButtonScan,
        TerminateiButtonScan
    },
    { DISPLAY_ID_RECONOCIDO, DISPLAY_ESPERANDO_ID, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_SELECCIONE_PRODUCTO4, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_SELECCIONE_PRODUCTO4, DISPLAY_ID_RECONOCIDO, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x94, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_OPERACION_CANCELADA, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { DISPLAY_HOSE_ACTIVATED, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_FORMA_PROGRAMACION, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        ProductNameSelectionDecorator,
        DisplayProductSelectionInputHandler,
        PNEAR_NULLPTR,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0000, 0x0000, 0x00, { } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
    },
    { DISPLAY_FORMA_PROGRAMACION, DISPLAY_SELECCIONE_PRODUCTO4, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0F, 0x10, 0x43, 0x3B, 0xFF, 0xFF },
        { DISPLAY_DUMMY0, DISPLAY_DUMMY1, DISPLAY_DUMMY2, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_DUMMY0, DISPLAY_FORMA_PROGRAMACION, 0xFF,
        DISPLAY_TIMEOUT_100MSECS, //timeout limit (seconds)
        DISPLAY_INTRODUZCA_VALOR, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_INTRODUZCA_VALOR, DISPLAY_DUMMY0, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0x94, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_DUMMY2, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        DisplayMoneyInputHandler,
        NonEmptyMoneyValidator,
        { 0x0032, 0x002A, 0x08, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_DUMMY1, DISPLAY_FORMA_PROGRAMACION, 0xFF,
        DISPLAY_TIMEOUT_100MSECS, //timeout limit (seconds)
        DISPLAY_INTRODUZCA_VOLUMEN, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },    
    { DISPLAY_INTRODUZCA_VOLUMEN, DISPLAY_DUMMY1, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0x94, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_DUMMY2, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        DisplayVolumeInputHandler,
        NonEmptyDataValidator,
        { 0x0032, 0x002A, 0x08, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_DUMMY2, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_100MSECS, //timeout limit (seconds)
        DISPLAY_POR_FAVOR_ESPERE, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },    
    { DISPLAY_POR_FAVOR_ESPERE, DISPLAY_DUMMY2, 0xFF,
        DISPLAY_TIMEOUT_60SECS, //timeout limit (seconds)
        DISPLAY_TIEMPO_EXPIRADO, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { DISPLAY_CREDIT_AUTHORIZATION_ACCEPTED, DISPLAY_CREDIT_AUTHORIZATION_REJECTED, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_AUTORIZACION_ACEPTADA, DISPLAY_AUTORIZACION_RECHAZADA, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,//AuthorizationMessageDecorator,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        SendAuthorizationRequest4Credit,
        ValidateAuthorizationResponse4Credit
    },
    { DISPLAY_AUTORIZACION_ACEPTADA, DISPLAY_POR_FAVOR_ESPERE, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_INICIO0, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        AcceptedAuthorizationMessageDecorator,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        ReleaseMessageSink
    },
    { DISPLAY_AUTORIZACION_RECHAZADA, DISPLAY_POR_FAVOR_ESPERE, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_INICIO0, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        RejectedAuthorizationMessageDecorator,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_TIEMPO_EXPIRADO, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_INICIO0, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_ID_NO_RECONOCIDO, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_INICIO0, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //PATH TO PAYMENT IN CASH
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    #ifdef _PRESET_NO_HOSE_
    { DISPLAY_FORMA_PROGRAMACION, DISPLAY_FORMA_PAGO_DESEADA, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0F, 0x10, 0x43, 0x3B, 0xFF, 0xFF },
        { DISPLAY_INTRODUZCA_VALOR, DISPLAY_INTRODUZCA_VOLUMEN, DISPLAY_DIGITE_PLACA, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    #endif
    #ifdef _PRESET_HOSE_
    { DISPLAY_SELECCIONE_PRODUCTO4, DISPLAY_FORMA_PAGO_DESEADA, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x94, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_OPERACION_CANCELADA, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { DISPLAY_HOSE_ACTIVATED, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_DUMMY3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        ProductNameSelectionDecorator,
        DisplayProductSelectionInputHandler,
        PNEAR_NULLPTR,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0000, 0x0000, 0x00, { } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
    },
    { DISPLAY_DUMMY3, DISPLAY_SELECCIONE_PRODUCTO4, 0xFF,
        DISPLAY_TIMEOUT_100MSECS, //timeout limit (seconds)
        DISPLAY_FORMA_PROGRAMACION, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },    
    { DISPLAY_FORMA_PROGRAMACION, DISPLAY_DUMMY3, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0F, 0x10, 0x43, 0x3B, 0xFF, 0xFF },
        { DISPLAY_INTRODUZCA_VALOR, DISPLAY_INTRODUZCA_VOLUMEN, DISPLAY_DIGITE_PLACA, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    #endif    
    { DISPLAY_INTRODUZCA_VALOR, DISPLAY_FORMA_PROGRAMACION, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0x94, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_DIGITE_PLACA, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        DisplayMoneyInputHandler,
        NonEmptyMoneyValidator,
        { 0x0032, 0x002A, 0x08, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_INTRODUZCA_VOLUMEN, DISPLAY_FORMA_PROGRAMACION, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0x94, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_DIGITE_PLACA, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        DisplayVolumeInputHandler,
        NonEmptyDataValidator,
        { 0x0032, 0x002A, 0x08, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_DIGITE_PLACA, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0x94, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_DESEA_IMPRIMIR_RECIBO, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        DisplayInputHandler,
        NonEmptyDataValidator,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x004A, 0x002A, 0x0E, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_DESEA_IMPRIMIR_RECIBO, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x39, 0x38, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_SUBA_MANIJA, DISPLAY_SUBA_MANIJA, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        DrawNumberOfCopies,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0000, 0x0000, 0x00, { } },
        PNEAR_NULLPTR,
        ///ONCE THIS WINDOW IS CLOSED, THIS FINALIZER POSTS THE TRANSACTION TO THE PUMP!!
        ReleaseMessageSink
    },
    { DISPLAY_SUBA_MANIJA, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_INICIO0, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0000, 0x0000, 0x00, { } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // PATH TO CONFIGURATION OPTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    { DISPLAY_OPERACIONES, DISPLAY_FORMA_PAGO_DESEADA, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x46, 0x55, 0x94, 0x7E, 0xFF, 0xFF },
        { DISPLAY_OPCION_TURNO, DISPLAY_INGRESE_PASSWORD, DISPLAY_DESEA_IMPRIMIR_RECIBO,DISPLAY_OPERACION_CANCELADA, DISPLAY_INICIO0, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0000, 0x0000, 0x00, { } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_INGRESE_PASSWORD, DISPLAY_OPERACIONES, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0x94, 0x1D, 0x1E, 0xFF, 0xFF },
        { DISPLAY_PASSWORD_INVALIDO, DISPLAY_OPERACION_CANCELADA, DISPLAY_PASSWORD_VALIDO, DISPLAY_NUEVO_PASSWORD, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        DisplayInputPasswordHandler,
        SecureConfigurationValidator,
        { 0x0032, 0x002A, 0x05, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_PASSWORD_VALIDO, DISPLAY_INGRESE_PASSWORD, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_CONFIGURACIONES, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_PASSWORD_INVALIDO, DISPLAY_INGRESE_PASSWORD, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_INICIO0, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_NUEVO_PASSWORD, DISPLAY_INGRESE_PASSWORD, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_CONFIGURACIONES, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        DisplayIntegerInputHandler,
        NonEmptyDataValidator,
        { 0x0032, 0x002A, 0x04, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        PNEAR_NULLPTR,
        SaveNewPassword
    },
    { DISPLAY_CONFIGURACIONES, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x58, 0x82, 0x5A, 0x65, 0x94, 0x7E },
        { DISPLAY_CONFIGURAR_FECHA_HORA, DISPLAY_IDENTIFICADOR_ESTACION, DISPLAY_INTRODUZCA_VALOR, DISPLAY_AMBAS_IMPRESORAS_FUNCIONANDO, DISPLAY_OPERACION_CANCELADA, DISPLAY_INICIO0 }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0000, 0x0000, 0x00, { } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_CONFIGURAR_FECHA_HORA, DISPLAY_CONFIGURACIONES, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x7E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_CONFIGURACIONES, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        DrawDateTime,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0000, 0x0000, 0x00, { } },
        InitiateUpdateDateTime,
        TerminateUpdateDateTime
    },
    { DISPLAY_IDENTIFICADOR_ESTACION, DISPLAY_CONFIGURACIONES, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_CONFIGURACIONES, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        DisplayStationIDDecorator,
        DisplayIDInputHandler,
        NonEmptyDataValidator,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0032, 0x002A, 0x04, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        DisplayStationIDLoad,
        DisplayStationIDStore
    },
    { DISPLAY_INTRODUZCA_VALOR, DISPLAY_CONFIGURACIONES, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_CONFIGURACIONES, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        DisplayPumpIDOffsetDecorator,
        DisplayIDInputHandler,
        NonEmpty15ModuloValidator,
        { 0x0032, 0x002A, 0x02, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        DisplayPumpIDOffsetLoad,
        DisplayPumpIDOffsetStore
    },
    { DISPLAY_OPCION_TURNO, DISPLAY_OPERACIONES, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { DISPLAY_OPEN_WORK_SHIFT, DISPLAY_CLOSE_WORK_SHIFT, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_ABRIR_TURNO, DISPLAY_CERRAR_TURNO, 0xFF, 0xFF, 0xFF, 0xFF },
        WorkShiftDecorator,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0000, 0x0000, 0x00, { } },
        InitiateWorkShiftValidation,
        PNEAR_NULLPTR
    },
    { DISPLAY_ABRIR_TURNO, DISPLAY_OPCION_TURNO, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x47, 0x7E, 0x94, 0xFF, 0xFF, 0xFF },
        { DISPLAY_INTRODUZCA_CEDULA, DISPLAY_OPERACION_CANCELADA, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0000, 0x0000, 0x00, { } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_CERRAR_TURNO, DISPLAY_OPCION_TURNO, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x48, 0x7E, 0x94, 0xFF, 0xFF, 0xFF },
        { DISPLAY_INTRODUZCA_CEDULA, DISPLAY_OPERACION_CANCELADA, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0000, 0x0000, 0x00, { } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_INTRODUZCA_CEDULA, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_INGRESE_PASSWORD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        DisplayIntegerInputHandler,
        NonEmptyDataValidator,
        { 0x0032, 0x002A, 0x0A, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_INGRESE_PASSWORD, DISPLAY_INTRODUZCA_CEDULA, 0x0C,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_POR_FAVOR_ESPERE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        DisplayInputPasswordHandler,
        NonEmptyDataValidator,
        { 0x0032, 0x002A, 0x04, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_POR_FAVOR_ESPERE, DISPLAY_INGRESE_PASSWORD, 0xFF,
        DISPLAY_TIMEOUT_15SECS, //timeout limit (seconds)
        DISPLAY_TIEMPO_EXPIRADO, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { DISPLAY_WORK_SHIFT_OPERATOR_ACCEPTED, DISPLAY_WORK_SHIFT_OPERATOR_REJECTED, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_USUARIO_VALIDO, DISPLAY_USUARIO_INVALIDO, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,//AuthorizationMessageDecorator,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        RequestWorkShiftOperAuthorization,
        PNEAR_NULLPTR,
    },
    { DISPLAY_USUARIO_VALIDO, DISPLAY_POR_FAVOR_ESPERE, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_INICIO0, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_USUARIO_INVALIDO, DISPLAY_POR_FAVOR_ESPERE, 0xFF,
        DISPLAY_TIMEOUT_5SECS, //timeout limit (seconds)
        DISPLAY_INICIO0, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //PRINTERS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    { DISPLAY_AMBAS_IMPRESORAS_FUNCIONANDO, DISPLAY_CONFIGURACIONES, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x39, 0x38, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_CONFIGURACIONES, DISPLAY_IMPRESORA_NO_FUNCIONANDO, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PrintersWorking
    },
    { DISPLAY_IMPRESORA_NO_FUNCIONANDO, DISPLAY_AMBAS_IMPRESORAS_FUNCIONANDO, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x8C, 0x8D, 0x94, 0x7E, 0xFF, 0xFF },
        { DISPLAY_CONFIGURACIONES, DISPLAY_CONFIGURACIONES, DISPLAY_CONFIGURACIONES, DISPLAY_INICIO0, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        EitherPrinterWorking
    },
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //EO THE DISPLAY FLOW
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    { DISPLAY_NULL, DISPLAY_NULL, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DisplayFlow _g_displayflow2[] = 
{
    //Display ID - Condition - { LIST OF CHILDREN DISPLAYS TO BE THROWN BASED ON THE MATCHING CODE SET }
    //The condition is the required value to throw the display (0xFF means no condition required. only the first)
    { DISPLAY_INICIO0, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x3A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_SELECCIONE_POSICION, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, //No state change data defined
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, //No state change data defined
        DrawHomeDateTime,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        InitiateHomeAnimation,
        TerminateHomeAnimation
    },
    { DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_INICIO0, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, //No state change data defined
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, //No state change data defined
        CancelMessageDecorator,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        CleanMessageSink
    },
    { DISPLAY_CANCELADO_X_PC, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_INICIO0, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, //No state change data defined
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, //No state change data defined
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_SELECCIONE_POSICION, DISPLAY_INICIO0, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { POSITIONA, POSITIONB, 0x7E, 0x94, 0xFF, 0xFF },
        { DISPLAY_FORMA_PAGO_DESEADA, DISPLAY_FORMA_PAGO_DESEADA, DISPLAY_INICIO0, DISPLAY_INICIO0, 0xFF, 0xFF }, 
        { DISPLAY_LESS_THAN_FOUR_POSITIONS, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_FORMA_PAGO_DESEADA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,//Display Decorator
        PNEAR_NULLPTR,//Display Input Handler
        ValidatePumpAvailability4Positions,//Display Validator
        { 0x0000, 0x0000, 0x00, {} },
        ///THIS INITIALIZER "SEQUESTERS" ONE MESSAGE FROM THE MESSAGE SINK AND USES ITS BUFFER AS A STORAGE FOR THE TRANSACTION
        SequesterMessageSink,//MUST BE EXECUTED AT LEAST ONE SCREEN BEFORE BEING VALIDATED SINCE THE ISR SIMULATES MULTITASKING WITH THE MAIN LOOP
        PNEAR_NULLPTR
    },
    #ifdef _PRESET_NO_HOSE_
    { DISPLAY_FORMA_PAGO_DESEADA, DISPLAY_SELECCIONE_POSICION, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0D, 0x0E, 0x45, 0x94, 0x7E, 0xFF },
        { DISPLAY_FORMA_PROGRAMACION, DISPLAY_ID_DIGITAL, DISPLAY_OPERACIONES, DISPLAY_INICIO0, DISPLAY_INICIO0, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        ValidatePumpAvailability21Positions,//Display Validator
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    #endif
    #ifdef _PRESET_HOSE_
    { DISPLAY_FORMA_PAGO_DESEADA, DISPLAY_SELECCIONE_POSICION, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0D, 0x0E, 0x45, 0x94, 0x7E, 0xFF },
        { DISPLAY_SELECCIONE_PRODUCTO4, DISPLAY_ID_DIGITAL, DISPLAY_OPERACIONES, DISPLAY_INICIO0, DISPLAY_INICIO0, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        ValidatePumpAvailability21Positions,//Display Validator
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    #endif
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //PATH TO PAYMENT WITH IBUTTON/TAG OR KEYBOARD
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    { DISPLAY_ID_DIGITAL, DISPLAY_FORMA_PAGO_DESEADA, 0x0E,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0xB6, 0x7E, 0x94, 0xFF, 0xFF, 0xFF },
        { DISPLAY_INTRODUZCA_KILOMETRAJE, DISPLAY_INICIO0, DISPLAY_INICIO0, 0xFF, 0xFF, 0xFF }, //DISPLAY_INTRODUZCA_VALOR, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_INTRODUZCA_KILOMETRAJE, DISPLAY_ID_DIGITAL, 0xB6,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0x94, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_ESPERANDO_ID, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        DisplayIntegerInputHandler,
        NonEmptyDataValidator,
        { 0x0032, 0x002A, 0x09, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_ESPERANDO_ID, DISPLAY_INTRODUZCA_KILOMETRAJE, 0x0C,
        DISPLAY_TIMEOUT_10SECS, //timeout limit (seconds)
        DISPLAY_ID_NO_RECONOCIDO, //display id to be thrown on timeout
        { 0x3B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { DISPLAY_IBUTTON_READ_ACCEPTED, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_ID_RECONOCIDO, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        InitiateiButtonScan,
        TerminateiButtonScan
    },
    { DISPLAY_ID_RECONOCIDO, DISPLAY_ESPERANDO_ID, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_SELECCIONE_PRODUCTO4, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_SELECCIONE_PRODUCTO4, DISPLAY_ID_RECONOCIDO, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x94, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_OPERACION_CANCELADA, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { DISPLAY_HOSE_ACTIVATED, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_FORMA_PROGRAMACION, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        ProductNameSelectionDecorator,
        DisplayProductSelectionInputHandler,
        PNEAR_NULLPTR,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0000, 0x0000, 0x00, { } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
    },
    { DISPLAY_FORMA_PROGRAMACION, DISPLAY_SELECCIONE_PRODUCTO4, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0F, 0x10, 0x43, 0x3B, 0xFF, 0xFF },
        { DISPLAY_DUMMY0, DISPLAY_DUMMY1, DISPLAY_DUMMY2, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_DUMMY0, DISPLAY_FORMA_PROGRAMACION, 0xFF,
        DISPLAY_TIMEOUT_100MSECS, //timeout limit (seconds)
        DISPLAY_INTRODUZCA_VALOR, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_INTRODUZCA_VALOR, DISPLAY_DUMMY0, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0x94, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_DUMMY2, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        DisplayMoneyInputHandler,
        NonEmptyMoneyValidator,
        { 0x0032, 0x002A, 0x08, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_DUMMY1, DISPLAY_FORMA_PROGRAMACION, 0xFF,
        DISPLAY_TIMEOUT_100MSECS, //timeout limit (seconds)
        DISPLAY_INTRODUZCA_VOLUMEN, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },    
    { DISPLAY_INTRODUZCA_VOLUMEN, DISPLAY_DUMMY1, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0x94, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_DUMMY2, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        DisplayVolumeInputHandler,
        NonEmptyDataValidator,
        { 0x0032, 0x002A, 0x08, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_DUMMY2, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_100MSECS, //timeout limit (seconds)
        DISPLAY_POR_FAVOR_ESPERE, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },    
    { DISPLAY_POR_FAVOR_ESPERE, DISPLAY_DUMMY2, 0xFF,
        DISPLAY_TIMEOUT_60SECS, //timeout limit (seconds)
        DISPLAY_TIEMPO_EXPIRADO, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { DISPLAY_CREDIT_AUTHORIZATION_ACCEPTED, DISPLAY_CREDIT_AUTHORIZATION_REJECTED, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_AUTORIZACION_ACEPTADA, DISPLAY_AUTORIZACION_RECHAZADA, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,//AuthorizationMessageDecorator,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        SendAuthorizationRequest4Credit,
        ValidateAuthorizationResponse4Credit
    },
    { DISPLAY_AUTORIZACION_ACEPTADA, DISPLAY_POR_FAVOR_ESPERE, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_INICIO0, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        AcceptedAuthorizationMessageDecorator,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        ReleaseMessageSink
    },
    { DISPLAY_AUTORIZACION_RECHAZADA, DISPLAY_POR_FAVOR_ESPERE, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_INICIO0, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        RejectedAuthorizationMessageDecorator,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_TIEMPO_EXPIRADO, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_INICIO0, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_ID_NO_RECONOCIDO, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_INICIO0, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //PATH TO PAYMENT IN CASH
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    #ifdef _PRESET_NO_HOSE_
    { DISPLAY_FORMA_PROGRAMACION, DISPLAY_FORMA_PAGO_DESEADA, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0F, 0x10, 0x43, 0x3B, 0xFF, 0xFF },
        { DISPLAY_INTRODUZCA_VALOR, DISPLAY_INTRODUZCA_VOLUMEN, DISPLAY_DIGITE_PLACA, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    #endif
    #ifdef _PRESET_HOSE_
    { DISPLAY_SELECCIONE_PRODUCTO4, DISPLAY_FORMA_PAGO_DESEADA, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x94, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_OPERACION_CANCELADA, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { DISPLAY_HOSE_ACTIVATED, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_DUMMY3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        ProductNameSelectionDecorator,
        DisplayProductSelectionInputHandler,
        PNEAR_NULLPTR,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0000, 0x0000, 0x00, { } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
    },
    { DISPLAY_DUMMY3, DISPLAY_SELECCIONE_PRODUCTO4, 0xFF,
        DISPLAY_TIMEOUT_100MSECS, //timeout limit (seconds)
        DISPLAY_FORMA_PROGRAMACION, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },    
    { DISPLAY_FORMA_PROGRAMACION, DISPLAY_DUMMY3, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0F, 0x10, 0x43, 0x3B, 0xFF, 0xFF },
        { DISPLAY_INTRODUZCA_VALOR, DISPLAY_INTRODUZCA_VOLUMEN, DISPLAY_DIGITE_PLACA, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    #endif    
    { DISPLAY_INTRODUZCA_VALOR, DISPLAY_FORMA_PROGRAMACION, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0x94, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_DIGITE_PLACA, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        DisplayMoneyInputHandler,
        NonEmptyMoneyValidator,
        { 0x0032, 0x002A, 0x08, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_INTRODUZCA_VOLUMEN, DISPLAY_FORMA_PROGRAMACION, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0x94, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_DIGITE_PLACA, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        DisplayVolumeInputHandler,
        NonEmptyDataValidator,
        { 0x0032, 0x002A, 0x08, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_DIGITE_PLACA, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0x94, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_DESEA_IMPRIMIR_RECIBO, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        DisplayInputHandler,
        NonEmptyDataValidator,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x004A, 0x002A, 0x0E, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_DESEA_IMPRIMIR_RECIBO, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x39, 0x38, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_SUBA_MANIJA, DISPLAY_SUBA_MANIJA, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        DrawNumberOfCopies,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0000, 0x0000, 0x00, { } },
        PNEAR_NULLPTR,
        ///ONCE THIS WINDOW IS CLOSED, THIS FINALIZER POSTS THE TRANSACTION TO THE PUMP!!
        ReleaseMessageSink
    },
    { DISPLAY_SUBA_MANIJA, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_INICIO0, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0000, 0x0000, 0x00, { } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // PATH TO CONFIGURATION OPTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    { DISPLAY_OPERACIONES, DISPLAY_FORMA_PAGO_DESEADA, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x46, 0x55, 0x94, 0x7E, 0xFF, 0xFF },
        { DISPLAY_OPCION_TURNO, DISPLAY_INGRESE_PASSWORD, DISPLAY_OPERACION_CANCELADA, DISPLAY_INICIO0, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0000, 0x0000, 0x00, { } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_INGRESE_PASSWORD, DISPLAY_OPERACIONES, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0x94, 0x1D, 0x1E, 0xFF, 0xFF },
        { DISPLAY_PASSWORD_INVALIDO, DISPLAY_OPERACION_CANCELADA, DISPLAY_PASSWORD_VALIDO, DISPLAY_NUEVO_PASSWORD, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        DisplayInputPasswordHandler,
        SecureConfigurationValidator,
        { 0x0032, 0x002A, 0x05, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_PASSWORD_VALIDO, DISPLAY_INGRESE_PASSWORD, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_CONFIGURACIONES, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_PASSWORD_INVALIDO, DISPLAY_INGRESE_PASSWORD, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_INICIO0, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_NUEVO_PASSWORD, DISPLAY_INGRESE_PASSWORD, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_CONFIGURACIONES, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        DisplayIntegerInputHandler,
        NonEmptyDataValidator,
        { 0x0032, 0x002A, 0x04, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        PNEAR_NULLPTR,
        SaveNewPassword
    },
    { DISPLAY_CONFIGURACIONES, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x58, 0x82, 0x5A, 0x65, 0x94, 0x7E },
        { DISPLAY_CONFIGURAR_FECHA_HORA, DISPLAY_IDENTIFICADOR_ESTACION, DISPLAY_INTRODUZCA_VALOR, DISPLAY_AMBAS_IMPRESORAS_FUNCIONANDO, DISPLAY_OPERACION_CANCELADA, DISPLAY_INICIO0 }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0000, 0x0000, 0x00, { } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_CONFIGURAR_FECHA_HORA, DISPLAY_CONFIGURACIONES, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x7E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_CONFIGURACIONES, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        DrawDateTime,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0000, 0x0000, 0x00, { } },
        InitiateUpdateDateTime,
        TerminateUpdateDateTime
    },
    { DISPLAY_IDENTIFICADOR_ESTACION, DISPLAY_CONFIGURACIONES, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_CONFIGURACIONES, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        DisplayStationIDDecorator,
        DisplayIDInputHandler,
        NonEmptyDataValidator,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0032, 0x002A, 0x04, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        DisplayStationIDLoad,
        DisplayStationIDStore
    },
    { DISPLAY_INTRODUZCA_VALOR, DISPLAY_CONFIGURACIONES, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_CONFIGURACIONES, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        DisplayPumpIDOffsetDecorator,
        DisplayIDInputHandler,
        NonEmpty15ModuloValidator,
        { 0x0032, 0x002A, 0x02, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        DisplayPumpIDOffsetLoad,
        DisplayPumpIDOffsetStore
    },
    { DISPLAY_OPCION_TURNO, DISPLAY_OPERACIONES, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { DISPLAY_OPEN_WORK_SHIFT, DISPLAY_CLOSE_WORK_SHIFT, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_ABRIR_TURNO, DISPLAY_CERRAR_TURNO, 0xFF, 0xFF, 0xFF, 0xFF },
        WorkShiftDecorator,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0000, 0x0000, 0x00, { } },
        InitiateWorkShiftValidation,
        PNEAR_NULLPTR
    },
    { DISPLAY_ABRIR_TURNO, DISPLAY_OPCION_TURNO, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x47, 0x7E, 0x94, 0xFF, 0xFF, 0xFF },
        { DISPLAY_INTRODUZCA_CEDULA, DISPLAY_OPERACION_CANCELADA, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0000, 0x0000, 0x00, { } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_CERRAR_TURNO, DISPLAY_OPCION_TURNO, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x48, 0x7E, 0x94, 0xFF, 0xFF, 0xFF },
        { DISPLAY_INTRODUZCA_CEDULA, DISPLAY_OPERACION_CANCELADA, DISPLAY_OPERACION_CANCELADA, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        //InputBox default X and Y input positions, maximum number of characters and FontData info
        { 0x0000, 0x0000, 0x00, { } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_INTRODUZCA_CEDULA, 0xFF, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_INGRESE_PASSWORD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        DisplayIntegerInputHandler,
        NonEmptyDataValidator,
        { 0x0032, 0x002A, 0x0A, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_INGRESE_PASSWORD, DISPLAY_INTRODUZCA_CEDULA, 0x0C,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x0C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_POR_FAVOR_ESPERE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        DisplayInputPasswordHandler,
        NonEmptyDataValidator,
        { 0x0032, 0x002A, 0x04, { 0x04, 0x10, DISP_TEXT_OPAQUE, 0xFFFF, 0x0000 } },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_POR_FAVOR_ESPERE, DISPLAY_INGRESE_PASSWORD, 0xFF,
        DISPLAY_TIMEOUT_15SECS, //timeout limit (seconds)
        DISPLAY_TIEMPO_EXPIRADO, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { DISPLAY_WORK_SHIFT_OPERATOR_ACCEPTED, DISPLAY_WORK_SHIFT_OPERATOR_REJECTED, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_USUARIO_VALIDO, DISPLAY_USUARIO_INVALIDO, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,//AuthorizationMessageDecorator,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        RequestWorkShiftOperAuthorization,
        PNEAR_NULLPTR,
    },
    { DISPLAY_USUARIO_VALIDO, DISPLAY_POR_FAVOR_ESPERE, 0xFF,
        DISPLAY_TIMEOUT_2SECS, //timeout limit (seconds)
        DISPLAY_INICIO0, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    { DISPLAY_USUARIO_INVALIDO, DISPLAY_POR_FAVOR_ESPERE, 0xFF,
        DISPLAY_TIMEOUT_5SECS, //timeout limit (seconds)
        DISPLAY_INICIO0, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    },
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //PRINTERS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    { DISPLAY_AMBAS_IMPRESORAS_FUNCIONANDO, DISPLAY_CONFIGURACIONES, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x39, 0x38, 0xFF, 0xFF, 0xFF, 0xFF },
        { DISPLAY_CONFIGURACIONES, DISPLAY_IMPRESORA_NO_FUNCIONANDO, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PrintersWorking
    },
    { DISPLAY_IMPRESORA_NO_FUNCIONANDO, DISPLAY_AMBAS_IMPRESORAS_FUNCIONANDO, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0x8C, 0x8D, 0x94, 0x7E, 0xFF, 0xFF },
        { DISPLAY_CONFIGURACIONES, DISPLAY_CONFIGURACIONES, DISPLAY_CONFIGURACIONES, DISPLAY_INICIO0, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        EitherPrinterWorking
    },
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //EO THE DISPLAY FLOW
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    { DISPLAY_NULL, DISPLAY_NULL, 0xFF,
        DISPLAY_TIMEOUT_NOTIMEOUT, //timeout limit (seconds)
        DISPLAY_NULL, //display id to be thrown on timeout
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        PNEAR_NULLPTR,
        { 0x0000, 0x0000, 0x00, {} },
        PNEAR_NULLPTR,
        PNEAR_NULLPTR
    }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* [] END OF FILE */
