//***********************************************************************************
// Include files

/**
 * @file LEDs_thunderboard.c
 * @author Shambaditya Tarafder
 * @date   9/23/2021
 * @brief  Initializing and operation on the RGB leds
 *
 */



//***********************************************************************************
#include "LEDs_thunderboard.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************
bool	rgb_enabled_status;

//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *This is the setup for initializing the RGB in the thunderboard sense 2
 *
 * @details
 *This function initially initialized the status of the RGB as flase and clear
 *out the various pins and ports associated with it
 *
 *
 *
 * @note
 *This function is for initializing the RGB LED only
 *
 ******************************************************************************/

void rgb_init(void) {
	rgb_enabled_status = false;
  GPIO_PinOutClear(RGB0_PORT,RGB0_PIN);
  GPIO_PinOutClear(RGB1_PORT,RGB1_PIN);
  GPIO_PinOutClear(RGB2_PORT,RGB2_PIN);
  GPIO_PinOutClear(RGB3_PORT,RGB3_PIN);
  GPIO_PinOutSet(RGB_ENABLE_PORT,RGB_ENABLE_PIN);
}

/***************************************************************************//**
 * @brief
 *When calling this function the with the requisite input arguments it basically
 *shut off and on certain LED(red,green or blue)from the require location.
 *
 * @details
 *When the UF and the comp1 interrupts call this function it is for enabling and
 * disabling the RGB leds on a certain location based on the input arguments
 *which causes the it to rotate in the required from in sync.
 *
 *
 * @note
 *This function is for enabling and disabling RGB leds at a certain location
 *
 ******************************************************************************/

void leds_enabled(uint32_t leds, uint32_t color, bool enable){

    if ((color & COLOR_RED) && enable) {
      GPIO_PinOutSet(RGB_RED_PORT,RGB_RED_PIN);
    } else if ((color & COLOR_RED) && !enable) GPIO_PinOutClear(RGB_RED_PORT,RGB_RED_PIN);

    if ((color & COLOR_GREEN) && enable) {
      GPIO_PinOutSet(RGB_GREEN_PORT,RGB_GREEN_PIN);
    } else if ((color & COLOR_GREEN) && !enable)  GPIO_PinOutClear(RGB_GREEN_PORT,RGB_GREEN_PIN);

    if ((color & COLOR_BLUE) && enable) {
      GPIO_PinOutSet(RGB_BLUE_PORT,RGB_BLUE_PIN);
    } else if ((color & COLOR_BLUE) && !enable)  GPIO_PinOutClear(RGB_BLUE_PORT,RGB_BLUE_PIN);

		if ((leds & RGB_LED_0) && enable) {
			GPIO_PinOutSet(RGB0_PORT,RGB0_PIN);
		} else if ((leds & RGB_LED_0) && !enable) GPIO_PinOutClear(RGB0_PORT,RGB0_PIN);

		if ((leds & RGB_LED_1) && enable) {
			GPIO_PinOutSet(RGB1_PORT,RGB1_PIN);
		} else if ((leds & RGB_LED_1) && !enable)  GPIO_PinOutClear(RGB1_PORT,RGB1_PIN);

		if ((leds & RGB_LED_2) && enable) {
			GPIO_PinOutSet(RGB0_PORT,RGB2_PIN);
		} else if ((leds & RGB_LED_2) && !enable)  GPIO_PinOutClear(RGB2_PORT,RGB2_PIN);

		if ((leds & RGB_LED_3) && enable) {
			GPIO_PinOutSet(RGB3_PORT,RGB3_PIN);
		} else if ((leds & RGB_LED_3) && !enable)  GPIO_PinOutClear(RGB3_PORT,RGB3_PIN);

}

