//***********************************************************************************
// Include files
//***********************************************************************************

#ifndef HEADER_FILES_SI1133_H_
#define HEADER_FILES_SI1133_H_

/* Silicon Labs include statements */

#include "em_cmu.h"
#include "em_assert.h"
#include "cmu.h"
#include "gpio.h"
#include "i2c.h"
#include "brd_config.h"
#include "HW_delay.h"
#include "em_i2c.h"

//***********************************************************************************
// defined files
//***********************************************************************************

#define POR 25
#define SI1133_ADDRESS  0x55
#define SI1133_PART           0x00

//***********************************************************************************
// global variables
//***********************************************************************************

void si1133_i2c_open();
void si1133_read(uint32_t SI1133_LIGHT_READ_CB);
uint32_t si1133_pass_ID(void);

#endif /* HEADER_FILES_SI1133_H_ */
