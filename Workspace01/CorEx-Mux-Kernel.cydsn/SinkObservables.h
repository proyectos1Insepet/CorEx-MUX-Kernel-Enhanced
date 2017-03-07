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

enum _SINK_OBSERVABLES_
{
///**********************************************
///OBSERVABLE IDENTIFIERS FOR THE UART SUBSCRIBER    
    UART_RF_RECEPTION = 0x01,    
    UART_RF_TEST,
    UART_RF_AUTONOMOUS_MONITOR,
///**********************************************
///SINK MESSAGES FROM CONTROL MODULES
    SINK_UPDATE_COUNTERS,
    SINK_RELEASE_BY_ID,
    SINK_RELEASE_BY_PTR,
///**********************************************
///DISPENSER BLOCK
    DISPENSER_UPDATE_PUMP_TRANSACTION_TIMEOUTS,
    //DISPENSER_UPDATE_STATE1,
    //DISPENSER_UPDATE_STATE2,
    DISPENSER_ACQUIRE_PUMP_STATE,
    DISPENSER_ACQUIRE_PUMP_HOSE,
    DISPENSER_EXECUTE_CASH_TRANSACTION,
    DISPENSER_EXECUTE_CREDIT_TRANSACTION,
    DISPENSER_UNLOCK_EOT_PUMPS,
    DISPENSER_LOAD_EEPROM_SETTINGS,
    DISPENSER_ENUMERATE_VISIBLE_PUMPS,
    DISPENSER_ACQUIRE_CONFIGURATION_INFORMATION,
    DISPENSER_CONFIGURE_PRICES,
    DISPENSER_RESTORE_PRICES,
    DISPENSER_CONFIGURE_SINGLE_PRICE,
    DISPENSER_ACQUIRE_TOTALS,
    DISPENSER_PUMP_HANDLER,
    DISPENSER_PUMP_BEEP_WARNING,
///**********************************************
///PRINTER
    PRINTER11_JOB,//This is for the printer spooler on side 1 port1
    PRINTER12_JOB,//This is for the printer spooler on side 1 port2
    PRINTER21_JOB,//This is for the printer spooler on side 2 port1
    PRINTER22_JOB,//This is for the printer spooler on side 2 port2
    PRINTER1_GENERIC_JOB,
    PRINTER2_GENERIC_JOB,
    PRINTER1_GENERIC_JOB_LOGO,
    PRINTER2_GENERIC_JOB_LOGO,
///**********************************************
    DISPLAY_UPDATE_PUMP_STATE,
///DISPLAY1
    DISPLAY1_RECEPTION,
    DISPLAY1_MESSAGE,
    DISPLAY1_UPDATE_DATETIME,
    DISPLAY1_ANIMATE_HOME,
///**********************************************
///DISPLAY2
    DISPLAY2_RECEPTION,
    DISPLAY2_MESSAGE,
    DISPLAY2_UPDATE_DATETIME,
    DISPLAY2_ANIMATE_HOME,
///**********************************************
///IBUTTON
    IBUTTON1_SCAN,
    IBUTTON2_SCAN,
///**********************************************
///GLOBAL TIMEOUT SERVICES
    GLOBAL_TICK_NOTIFICATION,
///**********************************************
///TESTING
    TOGGLE_SCREEN_AND_WRITE_TEST,
///**********************************************
///WATCHDOG RESET SERVICES
    WATCHDOG_STARTUP
};

/* [] END OF FILE */
