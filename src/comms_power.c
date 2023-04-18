//--------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### COMMS_POWER.C ##############################
//--------------------------------------------------

// Includes --------------------------------------------------------------------
#include "comms_power.h"
#include "usart.h"
#include "switches_answers.h"

#include <string.h>
#include <stdio.h>


// Private Types Constants and Macros ------------------------------------------
#define COMM_FLAG_OK    0x0001

#define comms_flag_ok_check    (comms_flags & COMM_FLAG_OK)
#define comms_flag_ok_set     (comms_flags |= COMM_FLAG_OK)
#define comms_flag_ok_reset    (comms_flags &= ~(COMM_FLAG_OK))

// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
volatile unsigned short comms_timeout = 0;
unsigned short comms_flags = 0;


// Module Private Functions ----------------------------------------------------
void Comms_Power_Rx_Messages (char * msg);


// Module Functions ------------------------------------------------------------
resp_t Comms_Power_Send_Bright (unsigned char * p_ch)
{
    char buff [35] = { 0 };
    unsigned short calc = 0;
    unsigned char bright = 0;
    unsigned char temp0 = 0;
    unsigned char temp1 = 0;
                    
    // backup and bright temp calcs
    // ch0 the bright ch1 the temp
    bright = *(p_ch + 0);
    temp0 = 255 - *(p_ch + 1);
    temp1 = 255 - temp0;
        
    calc = temp0 * bright;
    calc >>= 8;    // to 255
    temp0 = (unsigned char) calc;
        
    calc = temp1 * bright;
    calc >>= 8;    // to 255
    temp1 = (unsigned char) calc;

    sprintf(buff, "ch1 %03d ch2 %03d sum %03d\n",
            temp0,
            temp1,
            temp0 + temp1);
    Usart2Send(buff);
    
    return resp_ok;
}


resp_t Comms_Power_Send_Current_Conf (unsigned char new_current)
{
    char buff [35] = { 0 };

    sprintf(buff, "current config %d\n", new_current);
    for (int i = 0; i < 5; i++)
    {
        Usart2Send(buff);
        comms_timeout = 100;
        comms_flag_ok_reset;

        while (comms_timeout)
        {
            if (comms_flag_ok_check)
            {
                comms_flag_ok_reset;
                return resp_ok;
            }

            Comms_Power_Update();
        }
    }

    return resp_timeout;
}


// #define COMM_INIT    0
// #define COMM_SEND_CMD    1
// #define COMM_WAIT_ANSWER    2
// unsigned char comm_power_send_cmd_state = 0;
// unsigned char comm_power_send_cmd_cnt = 0;
// resp_t Comms_Power_Send_Current_Conf2 (unsigned char new_current)
// {
//     resp_t resp = resp_continue;
//     char buff [35] = { 0 };

//     switch (comm_power_send_cmd_state)
//     {
//     case COMM_INIT:
//         comm_power_send_cmd_cnt = 250;
//         comm_power_send_cmd_state++;
//         break;

//     case COMM_SEND_CMD:
//         sprintf(buff, "current config %d\n", new_current);
//         Usart2Send(buff);
//         comms_timeout = 100;
//         comms_flag_ok_reset;
//         comm_power_send_cmd_state++;
//         break;
        
//     case COMM_WAIT_ANSWER:
//         if (comms_flag_ok_check)
//         {
//             comms_flag_ok_reset;
//             comm_power_send_cmd_state = COMM_INIT;
//             return resp_ok;
//         }

//         if (!comms_timeout)
//         {
//             if (comm_power_send_cmd_cnt)
//             {
//                 comm_power_send_cmd_cnt--;
//                 comm_power_send_cmd_state = COMM_SEND_CMD;
//             }
//             else
//             {
//                 comm_power_send_cmd_state = COMM_INIT;
//                 return resp_timeout;
//             }
//         }
//         break;
//     }

//     return resp_timeout;
// }


void Comms_Power_Timeouts (void)
{
    if (comms_timeout)
        comms_timeout--;
    
}


void Comms_Power_Update (void)
{
    char buff [30];
    
    if (Usart2HaveData())
    {
        Usart2HaveDataReset();
        Usart2ReadBuffer((unsigned char *)buff, 30);

        Comms_Power_Rx_Messages (buff);
    }        
}


void Comms_Power_Rx_Messages (char * msg)
{
    if (!strncmp(msg, "ok", sizeof ("ok") - 1))
    {
        comms_flag_ok_set;
    }

}

//--- end of file ---//
