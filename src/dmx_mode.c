//-------------------------------------------------
// #### DEXEL 2CH PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DMX_MODE.C ###############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "dmx_mode.h"
#include "dmx_lcd_menu.h"
#include "parameters.h"
#include "temperatures.h"
#include "lcd.h"

// #include "hard.h"

#include <stdio.h>
#include <string.h>


// Private Types Constants and Macros ------------------------------------------
typedef enum {
    DMX_MODE_INIT = 0,
    DMX_MODE_RUNNING

} dmx_mode_e;


#define DMX_PKT_TYPE    0
#define DMX_CLR_CH1    1
#define DMX_CLR_CH2    2

#define TT_MENU_TIMEOUT    30000
#define TT_DMX_RECEIVING    1000

// variables re-use
#define dmx_mode_state    mode_state
#define dmx_mode_enable_menu_timer    mode_effect_timer


// Externals -------------------------------------------------------------------
extern volatile unsigned char dmx_buff_data[];


// -- externals re-used
extern unsigned char mode_state;
extern volatile unsigned short mode_effect_timer;


//del main para dmx
extern volatile unsigned char Packet_Detected_Flag;
extern volatile unsigned short DMX_channel_selected;

extern parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------
unsigned char dmx_end_of_packet_update = 0;


//-- timers del modulo --------------------
volatile unsigned short dmx_mode_dmx_receiving_timer = 0;
volatile unsigned short dmx_mode_show_temp_timer = 0;
unsigned char showing_temp = 0;


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void DMXMode_UpdateTimers (void)
{
    if (dmx_mode_enable_menu_timer)
        dmx_mode_enable_menu_timer--;

    if (dmx_mode_dmx_receiving_timer)
        dmx_mode_dmx_receiving_timer--;

    if (dmx_mode_show_temp_timer)
        dmx_mode_show_temp_timer--;
    
}


void DMXModeReset (void)
{
    dmx_mode_state = DMX_MODE_INIT;
}


#define timer_address    dmx_mode_enable_menu_timer
unsigned char dmx_address_show = 0;
unsigned char last_dmx_ch1 = 0;
unsigned char last_dmx_ch2 = 0;
unsigned char idle_pckt_cnt = 0;
resp_t DMXMode (unsigned char * ch_val, sw_actions_t action)
{
    resp_t resp = resp_continue;
    unsigned char update_anyway = 0;
    
    switch (dmx_mode_state)
    {
    case DMX_MODE_INIT:
        DMXLcdMenuReset();
        DMXLcdMenu_ChangeAddressReset();
        dmx_address_show = 1;
        dmx_mode_state++;

        // force the first update
        last_dmx_ch1 = ~(dmx_buff_data[DMX_CLR_CH1]);
        break;

    case DMX_MODE_RUNNING:

        //update del dmx - generalmente a 40Hz -
        //desde el dmx al sp
        if (Packet_Detected_Flag)
        {
            Packet_Detected_Flag = 0;

            //le aviso al menu que se estan recibiendo paquetes dmx
            dmx_mode_dmx_receiving_timer = TT_DMX_RECEIVING;

            if (dmx_buff_data[DMX_PKT_TYPE] == 0x00)    //dmx packet
            {                
                //update the colors channels
                *(ch_val + 0) = dmx_buff_data[DMX_CLR_CH1];
                *(ch_val + 1) = dmx_buff_data[DMX_CLR_CH2];

                if ((last_dmx_ch1 != dmx_buff_data[DMX_CLR_CH1]) ||
                    (last_dmx_ch2 != dmx_buff_data[DMX_CLR_CH2]))
                {
                    last_dmx_ch1 = dmx_buff_data[DMX_CLR_CH1];
                    last_dmx_ch2 = dmx_buff_data[DMX_CLR_CH2];
                    dmx_end_of_packet_update = 2;    //por lo que demora el update pido dos seguidos
                    resp = resp_change;
                }
                else if (idle_pckt_cnt > 40)
                {
                    idle_pckt_cnt = 0;
                    update_anyway = 1;
                }
                else
                    idle_pckt_cnt++;
                
                break;
            }
        }

        if (dmx_end_of_packet_update)
        {
            dmx_menu_data_t dmx_st;
            dmx_st.dmx_first_chnl = &mem_conf.dmx_first_channel;
            dmx_st.channels_mode = mem_conf.channels_operation_mode;
            dmx_st.pchannels = ch_val;

            if (dmx_address_show)
                dmx_st.show_addres = 1;
            else
                dmx_st.show_addres = 0;

            resp = DMXLcdMenu(&dmx_st);

            if (resp == resp_finish)
            {
                if (dmx_end_of_packet_update)
                    dmx_end_of_packet_update--;
            }
        }

	// show the current temp every 10 secs for 2 secs
	if ((Temp_Sensor_Present_Get()) &&
	     (!dmx_mode_show_temp_timer))
	{
	    if (showing_temp)
	    {
		showing_temp = 0;
		dmx_mode_show_temp_timer = 10000;

		Lcd_SetDDRAM(16 - 3);
		Lcd_senddata('D');
		Lcd_senddata('m');
		Lcd_senddata('x');
	    }
	    else
	    {
		char s_temp [20];
		showing_temp = 1;
		dmx_mode_show_temp_timer = 2000;
	    
		sprintf(s_temp, "%dC", Temp_TempToDegreesExtended_Last_Filtered ());

		Lcd_SetDDRAM(16 - 3);
		Lcd_senddata(*(s_temp + 0));
		Lcd_senddata(*(s_temp + 1));
		Lcd_senddata(*(s_temp + 2));
	    }
	}
        break;

    default:
        dmx_mode_state = DMX_MODE_INIT;
        break;
    }

    
    //look for a change in address if we are not changing colors
    // if (resp != resp_change)
    if (dmx_end_of_packet_update == 0)        
    {
        dmx_menu_address_data_t dmx_addr_st;
        dmx_addr_st.dmx_address = mem_conf.dmx_first_channel;
        dmx_addr_st.dmx_channels_qtty = mem_conf.dmx_channel_quantity;
        dmx_addr_st.actions = action;
        dmx_addr_st.timer = (unsigned short *) &timer_address;
        dmx_addr_st.address_show = &dmx_address_show;
        resp = DMXLcdMenu_ChangeAddress(&dmx_addr_st);

        if (resp == resp_change)
        {
            // printf("something change dmx_address_show: %d timer: %d\n",
            //        dmx_address_show,
            //        timer_address);

            // change the DMX address
            DMX_channel_selected = dmx_addr_st.dmx_address;
            mem_conf.dmx_first_channel = DMX_channel_selected;
        
            // force a display update
            dmx_end_of_packet_update = 1;
        }

        if (resp == resp_finish)
        {
            //end of changing ask for a memory save
            resp = resp_need_to_save;

            // force a display update
            dmx_end_of_packet_update = 1;            
        }

        if (update_anyway)
        {
            update_anyway = 0;
            resp = resp_change;
        }
    }
    
    return resp;
            
}


unsigned short DMXGetPacketTimer (void)
{
    return dmx_mode_dmx_receiving_timer;
}

//--- end of file ---//
