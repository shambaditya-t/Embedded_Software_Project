/**
 * @file scheduler.c
 * @author Shambaditya Tarafder
 * @date   9/23/2021
 * @brief  Setting up the scheduler file
 *
 */


#include "scheduler.h"
#include "em_assert.h"
#include "em_core.h"
#include "em_emu.h"

//***********************************************************************************
// Private variables
//***********************************************************************************

static unsigned int event_scheduled;

/***************************************************************************//**
 * @brief
 *   This function sets up the schedular portion
 *
 * @details
 *   Opens the scheduler functionality by resetting the static /
 *   private variable event_scheduled to 0.
 *
 *
 * @note
 *   Before doing anything with the scheduler this first must first be initialized
 *
 *
 ******************************************************************************/

void scheduler_open(void){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  event_scheduled = 0;
  CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *   Adds a new event to the scheduler event
 *
 * @details
 *   ORs a new event, the input argument,into the existing state of the private
 *   (static) variable event_scheduled
 *
 * @note
 *   Should be called only when adding an event
 *
 *
 * @param[in] event
 *   Its is basically a 32 bit integer of the event to be added to the scheduler
 *
 *
 ******************************************************************************/

void add_scheduled_event(uint32_t event){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  event_scheduled |= event;
  CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *   Removes an event of the scheduler event
 *
 * @details
 *    Removes the event, the input argument, from the existing state of the
 *    private(static)variable event_scheduled.
 *
 *
 * @note
 *   Function should only be called when removing an event
 *
 * @param[in] event
 *   Its is basically a 32 bit integer of the event to be removed
 *
 *
 ******************************************************************************/

void remove_scheduled_event(uint32_t event){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  event_scheduled &= ~event;
  CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *   Function which returns the events_scheduled
 *
 * @details
 *  Returns the current state of the private (static) variable event_scheduled.
 *
 * @note
 *
 * @return
 *  Returns the event_scheduled as an integer
 *
 ******************************************************************************/

uint32_t get_scheduled_events(void){
  return event_scheduled;
}
