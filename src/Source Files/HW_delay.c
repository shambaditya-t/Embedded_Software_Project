/**
 * @file HW_delay.c
 * @author K Graham
 * @date   10/16/2021
 * @brief  This file contains the POR related functions/operations
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************

//** Silicon Labs Include Files

//** User Include Files
#include "HW_delay.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// private variables
//***********************************************************************************


//***********************************************************************************
// Private functions Prototypes
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************
/***************************************************************************//**
 * @brief
 *   This function is for setting up the time delay.
 *
 * @details
 *   This delay time is referred to as Power-On-Reset(POR) or Power up Time delay.
 *   Used to account for the delay
 *
 * @note
 *   We should use the worst case time delay and the time delay shouldn't be
 *   optimized out.
 *
 *@param[in] ms_delay
 *  The time delay value
 *
 ******************************************************************************/
void timer_delay(uint32_t ms_delay){
	uint32_t timer_clk_freq = CMU_ClockFreqGet(cmuClock_HFPER);
	uint32_t delay_count = ms_delay *(timer_clk_freq/1000) / 1024;
	CMU_ClockEnable(cmuClock_TIMER0, true);
	TIMER_Init_TypeDef delay_counter_init = TIMER_INIT_DEFAULT;
		delay_counter_init.oneShot = true;
		delay_counter_init.enable = false;
		delay_counter_init.mode = timerModeDown;
		delay_counter_init.prescale = timerPrescale1024;
		delay_counter_init.debugRun = false;
	TIMER_Init(TIMER0, &delay_counter_init);
	TIMER0->CNT = delay_count;
	TIMER_Enable(TIMER0, true);
	while (TIMER0->CNT != 00);
	TIMER_Enable(TIMER0, false);
	CMU_ClockEnable(cmuClock_TIMER0, false);
}

