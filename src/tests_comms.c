//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS_COMMS.C #########################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "comms_power.h"

//helper modules
#include "tests_ok.h"
// #include "tests_mock_usart.h"

#include <stdio.h>
#include <string.h>


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Auxialiary Functions -------------------------------------------------


// Module Functions for testing ------------------------------------------------
void Test_Comms (void);
    

// Module Functions ------------------------------------------------------------


int main(int argc, char *argv[])
{

    Test_Comms ();
    
}


void Test_Comms (void)
{
    printf("Test Comms -- Send_Bright\n");

    unsigned char dmx_data[2] = { 0 };

    printf(" -- min on bright\n");

    dmx_data[0] = 1;
    dmx_data[1] = 0;    
    printf("dmx0: %d dmx1: %d\n", dmx_data[0] ,dmx_data[1]);
    Comms_Power_Send_Bright(dmx_data);

    dmx_data[0] = 1;
    dmx_data[1] = 1;
    printf("dmx0: %d dmx1: %d\n", dmx_data[0] ,dmx_data[1]);
    Comms_Power_Send_Bright(dmx_data);

    dmx_data[0] = 255;
    dmx_data[1] = 1;
    printf("dmx0: %d dmx1: %d\n", dmx_data[0] ,dmx_data[1]);
    Comms_Power_Send_Bright(dmx_data);

    dmx_data[0] = 255;
    dmx_data[1] = 0;
    printf("dmx0: %d dmx1: %d\n", dmx_data[0] ,dmx_data[1]);
    Comms_Power_Send_Bright(dmx_data);

    dmx_data[0] = 255;
    dmx_data[1] = 254;
    printf("dmx0: %d dmx1: %d\n", dmx_data[0] ,dmx_data[1]);
    Comms_Power_Send_Bright(dmx_data);

    dmx_data[0] = 255;
    dmx_data[1] = 255;
    printf("dmx0: %d dmx1: %d\n", dmx_data[0] ,dmx_data[1]);
    Comms_Power_Send_Bright(dmx_data);
    
    // printf(" -- all brights 0 - 255\n");
    // for (int i = 0; i < 255; i++)
    // {
    //     dmx_data[0] = (unsigned char) i;
    //     Comms_Power_Send_Bright(dmx_data);
    // }
    
}


// Module Mocked Functions -----------------------------------------------------
void Usart2Send(char * msg)
{
    int len = strlen(msg);
    printf ("sent %d: %s", len, msg);
    
}


unsigned char Usart2HaveData (void)
{
    return 0;
}


void Usart2HaveDataReset (void)
{

}


void Usart2ReadBuffer (void)
{

}

//--- end of file ---//


