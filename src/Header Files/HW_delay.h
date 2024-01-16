/*
 * HW_delay.h
 *
 *  Created on: Apr 19, 2020
 *      Author: kgraham
 */

#ifndef SRC_HW_DELAY_H_
#define SRC_HW_DELAY_H_

/* Silicon Labs include statements */

#include "em_timer.h"
#include "em_cmu.h"

//***********************************************************************************
// function prototypes
//***********************************************************************************

void timer_delay(uint32_t ms_delay);

#endif /* SRC_HW_DELAY_H_ */
