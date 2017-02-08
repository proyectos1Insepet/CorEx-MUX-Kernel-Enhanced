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
#include "GlobalDefinitions.h"

PTR_LOOP _g_ptr_LOOP;
LOOPContainer _g_loop_container;

//@Created By: HIJH
//@Septembre de 2016
void loop_push(PTR_LOOP _func)
{
    if(_g_loop_container._index < _MAX_LOOP_POINTERS)
        _g_loop_container._ptrarr[_g_loop_container._index++] = _func;
}

//@Created By: HIJH
//@Septembre de 2016
void loop_invoke()
{
    FOR(uint8_t ndx = 0, ndx < _g_loop_container._index, ndx++)
    {
        _g_ptr_LOOP = _g_loop_container._ptrarr[ndx];
        if(_g_ptr_LOOP)
             _g_ptr_LOOP();
        else
            break;
    }
}


/* [] END OF FILE */
