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
@Created by: HIJH
@Date: Septembre 2 de 2016
*/
#include <project.h>
#include "SinkManager.h"
#include "UARTManager.h"

#include "Eeprom.h"

/*
@Created by: HIJH
@Date: Aôut 30 de 2016

This is the unique function to be called in order to initialize the RF networking stack
*/
void RFSinkRegistration();
void DispenserSinkRegistration();
void DisplaySinkRegistration();
void PrinterSinkRegistration();
void iButtonSinkRegistration();

void WatchDogInitialize();

void InitTimers();
void InitTest();

void GlobalInitializer()
{
    RF_Physical_Start();
    
    Printer_Start();
    Display1_Start();
    Display2_Start();
    Dispenser_Start();
    I2C_Bus_Start();
    
    InitTimers();
    
    SinkManagerRegistration();
    WatchDogInitialize();
    InitUARTManager();
    
    UARTSinkRegistration();
    RFSinkRegistration();
    DisplaySinkRegistration();
    PrinterSinkRegistration();
    DispenserSinkRegistration();
    iButtonSinkRegistration();

}

/* [] END OF FILE */
