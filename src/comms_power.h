//--------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### COMM.H #####################################
//--------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _COMM_H_
#define _COMM_H_

#include "switches_answers.h"

// Module Exported Types Constants and Macros ----------------------------------


// Module Exported Functions ---------------------------------------------------
resp_t Comms_Power_Send_Bright (unsigned char * p_ch);
resp_t Comms_Power_Send_Current_Conf (unsigned char new_current);
void Comms_Power_Timeouts (void);
void Comms_Power_Update (void);


#endif    /* _COMM_H_ */

//--- end of file ---//
