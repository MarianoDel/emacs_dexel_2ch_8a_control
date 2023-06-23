//----------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### HARD.C #######################################
//----------------------------------------------------

// Includes --------------------------------------------------------------------
#include "hard.h"
#include "stm32g0xx.h"
#include "switches_answers.h"


// Module Private Types Constants and Macros -----------------------------------
#define SWITCHES_TIMER_RELOAD    5
#define SWITCHES_THRESHOLD_FULL	1000    //5 segundos
#define SWITCHES_THRESHOLD_HALF	50    //250 ms
#define SWITCHES_THRESHOLD_MIN	10    //50 ms
#define SWITCHES_THRESHOLD_MIN_FAST    2    //10 ms


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
//for timers or timeouts
volatile unsigned char switches_timer = 0;


// Module Functions ------------------------------------------------------------
void HARD_Timeouts (void)
{
    if (switches_timer)
        switches_timer--;
}


unsigned short sw_up_cntr = 0;
unsigned short sw_dwn_cntr = 0;
unsigned short sw_sel_cntr = 0;
unsigned char sw_up_cont_cntr = 0;
unsigned char sw_dwn_cont_cntr = 0;
resp_sw_t Check_SW_UP (void)
{
    resp_sw_t sw = SW_NO;

    if (sw_up_cont_cntr >= 5)
    {
        sw_up_cont_cntr = 5;

        if (sw_up_cntr > SWITCHES_THRESHOLD_MIN_FAST)
        {
            sw_up_cntr -= SWITCHES_THRESHOLD_MIN_FAST;
            sw = SW_MIN;
        }
    }
    else if (sw_up_cntr > SWITCHES_THRESHOLD_MIN)
    {
        sw_up_cntr -= SWITCHES_THRESHOLD_MIN;
        sw = SW_MIN;
        sw_up_cont_cntr++;
    }

    return sw;    
}


resp_sw_t Check_SW_DWN (void)
{
    resp_sw_t sw = SW_NO;

    if (sw_dwn_cont_cntr >= 5)
    {
        sw_dwn_cont_cntr = 5;

        if (sw_dwn_cntr > SWITCHES_THRESHOLD_MIN_FAST)
        {
            sw_dwn_cntr -= SWITCHES_THRESHOLD_MIN_FAST;
            sw = SW_MIN;
        }
    }
    else if (sw_dwn_cntr > SWITCHES_THRESHOLD_MIN)
    {
        sw_dwn_cntr -= SWITCHES_THRESHOLD_MIN;
        sw = SW_MIN;
        sw_dwn_cont_cntr++;
    }

    return sw;    
}


resp_sw_t Check_SW_SEL (void)
{
    resp_sw_t sw = SW_NO;
    
    if (sw_sel_cntr > SWITCHES_THRESHOLD_FULL)
        sw = SW_FULL;
    else if (sw_sel_cntr > SWITCHES_THRESHOLD_HALF)
        sw = SW_HALF;
    else if (sw_sel_cntr > SWITCHES_THRESHOLD_MIN)
    {
        // sw_sel_cntr -= SWITCHES_THRESHOLD_MIN;
        sw = SW_MIN;
    }

    return sw;    
}


void UpdateSwitches (void)
{
    if (!switches_timer)
    {
        if (SW_UP)
            sw_up_cntr++;
        else if (sw_up_cntr > 50)
            sw_up_cntr -= 50;
        else if (sw_up_cntr > 10)
            sw_up_cntr -= 5;
        else if (sw_up_cntr)
            sw_up_cntr--;
        else
            sw_up_cont_cntr = 0;

        if (SW_DWN)
            sw_dwn_cntr++;
        else if (sw_dwn_cntr > 50)
            sw_dwn_cntr -= 50;
        else if (sw_dwn_cntr > 10)
            sw_dwn_cntr -= 5;
        else if (sw_dwn_cntr)
            sw_dwn_cntr--;
        else
            sw_dwn_cont_cntr = 0;

        if (SW_SEL)
            sw_sel_cntr++;
        else if (sw_sel_cntr > 50)
            sw_sel_cntr -= 50;
        else if (sw_sel_cntr > 10)
            sw_sel_cntr -= 5;
        else if (sw_sel_cntr)
            sw_sel_cntr--;
        
        switches_timer = SWITCHES_TIMER_RELOAD;
    }       
}


sw_actions_t CheckActions (void)
{
    sw_actions_t sw = selection_none;
    
    if (Check_SW_UP () > SW_NO)
        sw = selection_up;

    if (Check_SW_DWN () > SW_NO)
        sw = selection_dwn;

    resp_sw_t s_sel = SW_NO;
    s_sel = Check_SW_SEL ();
    
    if (s_sel > SW_HALF)
        sw = selection_back;
    else if (s_sel > SW_NO)
        sw = selection_enter;
    
    return sw;
    
}


#if defined HARDWARE_VERSION_2_0
char hardware_version [] = {"Hrd 2.0"};
#else
#error "No hard version selected on hard.c"
#endif
char * HARD_GetHardwareVersion (void)
{
    return hardware_version;
}

#if defined FIRMWARE_VERSION_2_1
char software_version [] = {"Soft 2.1"};
#elif defined FIRMWARE_VERSION_2_0
char software_version [] = {"Soft 2.0"};
#else
#error "No soft version selected on hard.c"
#endif

char * HARD_GetSoftwareVersion (void)
{
    return software_version;
}


//--- end of file ---//
