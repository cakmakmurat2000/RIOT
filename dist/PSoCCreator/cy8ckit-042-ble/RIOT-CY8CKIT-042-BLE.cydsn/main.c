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
#include <project.h>
#include <stdbool.h>
#include <stdio.h>
#include "thread.h"

bool initialize_RIOT(void)
{
    static bool riot_kernel_started = false;
    
    if (riot_kernel_started)
    {
        /* RIOT kernel was started before no need to do anything */
        return false;
    }
    else
    {
        /* For the first time, initialize board and RIOT kernel. */
        riot_kernel_started = true;
        
        /* Initialize Board */
        board_init();
        
        /* Initialize Kernel to start RIOT */
        kernel_init();
        
        return true;
    }
}

int main()
{
    CyGlobalIntEnable;

    /* 
     * Initialize RIOT to start RIOT.
     *
     * Should be located at the beginning of main func
     */
    if (initialize_RIOT() == true)
    {
        return 0;
    }

    /* Application code */
}
