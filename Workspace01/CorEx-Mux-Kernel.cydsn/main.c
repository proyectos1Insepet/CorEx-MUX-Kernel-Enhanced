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
#include <project.h>

#include "GlobalDefinitions.h"

#pragma long_calls
/*
@Created by HIJH
@Date: Août 31 de 2016

****** GLOBAL CONVENTIONS ****** 

- Global variables (those that have no parent "module") are preceded by an underscore "_"
- Global constants are preceded by an underscore "_" and defined in UPPERCASE as a RULE
- MACROS are some preceded by an underscore "_" and others don't. They are as a RULE defined in UPPERCASE
- Global variables and types are prefixed with "_g_" to imply that are "Global"

There is a global loop handler that invokes all of the handled methods in the order they where pushed 
onto the Loop Queue. Take this into account in order to avoid weird behaviors.

As an example on how to push a method that will be running in the loop, one would make the following call:

GLOBAL_LOOP_PUSH(YourMethodName);

Include the header file "globaldefinitions.h" to get access to this macro.

The preferred place to push your method is during the initialization phase, so put the initialization call
in the desired order according to the loop invocation order.

*/

int main()
{    
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    //Setting the bus clock freq as reference for CyDelay functions
    CyDelayFreq(0u);
    /* 
    Place your initialization/startup code here (e.g. MyInst_Start()) 
    ******************************************************************
    Take into account the GLOBAL LOOP order to make the push calls.
    ******************************************************************    
    */
        
    //First initialization delay
    INITITALIZATION_DELAY0;
    //All of the initializers all invoked from the file "Initializers.c" located in the "Framework" folder.
    GlobalInitializer();
    //Second initialization delay
    INITITALIZATION_DELAY1;
    //Infinite loop over the whole registered handlers
    LOOP_FOREVER;
}

/* [] END OF FILE */
