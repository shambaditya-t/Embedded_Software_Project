/**
 * @file sleep_routines.c
 * @author Shambaditya Tarafder
 * @date   9/23/2021
 * @brief  Setting up the sleep routines and modes
 *
 */




/**************************************************************************
 * @file sleep_routines.c
 * @section License*
 *  <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************
 *Permission is granted to anyone to use this software for any purpose,
 *including commercial applications, and to alter it and redistribute it
 *freely, subject to the following restrictions:
 *1. The origin of this software must not be mis-represented; you must not
 *claim that you wrote the original software.
 *2. Altered source versions must be plainly marked as such, and must not be
 *misrepresented as being the original software.
 *3. This notice may not be removed or altered from any source distribution.

 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 **************************************************************************/


#include "sleep_routines.h"

//***********************************************************************************
// Private variables
//***********************************************************************************

static int lowest_energy_mode[MAX_ENERGY_MODES];


/***************************************************************************//**
 * @brief
 *   This function initializes the sleep routines
 *
 * @details
 *   initialize the sleep_routines static / private array, lowest_energy_mode[],
 *   to all zeroes
 *
 *
 * @note
 *   Before doing anything with the the sleep modes and routines this function
 *   must be called and initialized
 *
 *
 ******************************************************************************/


void sleep_open(void){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  for (int i = 0; i < MAX_ENERGY_MODES; i++) {
    lowest_energy_mode[i] = 0;
  }
  CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *   This function sets up appropriate sleep mode depending on the situation
 *
 * @details
 *    Function that will enter the appropriate sleep Energy Mode based on the
 *    first non-zero array element in lowest_energy_mode[].
 *
 *
 *
 * @note
 *   It can maximum enter the EM3 sleep mode
 *
 *
 ******************************************************************************/


void enter_sleep(void){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  if (lowest_energy_mode[EM0] > 0) {
  }
  else if (lowest_energy_mode[EM1] > 0) {
  }
  else if (lowest_energy_mode[EM2] > 0) {
    EMU_EnterEM1();
  }
  else if (lowest_energy_mode[EM3] > 0) {
    EMU_EnterEM2(true);
  }
  else {
    EMU_EnterEM3(true);
  }
  CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *   This function basically allows the processor to sleep when no peripherals
 *   are active
 *
 * @details
 *   Utilized to release the processor from going into a sleep mode with a
 *   peripheral that is no longer active.
 *
 * @note
 *   It will decrement the associated array element in lowest_energy_mode[] by one.
 *
 *
 * @param[in] EM
 *   Its is basically a 32 bit integer of the EM that should be released
 *
 *
 ******************************************************************************/


void sleep_unblock_mode(uint32_t EM){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  if(lowest_energy_mode[EM]> 0){
      lowest_energy_mode[EM]--;
  }
  EFM_ASSERT(lowest_energy_mode[EM] >= 0);
  CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *   This function basically prevents the processor from sleeping when peripherals
 *   are active.
 *
 * @details
 *   Utilized by a peripheral to prevent the MightyGecko from going into that
 *   sleep mode while the peripheral is active.
 *
 * @note
 *  It will incrementthe associated array element in lowest_energy_mode[] by one.
 *
 *
 * @param[in] EM
 *   Its is basically a 32 bit integer of the EM that should be prevented from
 *   sleep.
 *
 *
 ******************************************************************************/


void sleep_block_mode(uint32_t EM) {
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  lowest_energy_mode[EM]++;
  EFM_ASSERT(lowest_energy_mode[EM] < 5);
  CORE_EXIT_CRITICAL();
}

