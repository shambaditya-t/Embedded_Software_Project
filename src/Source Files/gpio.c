/**
 * @file gpio.c
 * @author Shambaditya Tarafder
 * @date   9/23/2021
 * @brief  GPIO ports used for enabling/disabling LED
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "gpio.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************


//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 * the GPIO is basically used for the LEDs
 *
 * @details
 *The function uses the CMU_ClockEnable to set up the clock for the GPIO
 *to be used for the pinmode and driver strength.
 *
 * @note
 *This function doesnt return anything and should be utilized while
 *and setting up the GPIO drivers etc.
 *
 ******************************************************************************/

void gpio_open(void){

  CMU_ClockEnable(cmuClock_GPIO, true);

	// Configure LED pins
	GPIO_DriveStrengthSet(LED_RED_PORT, LED_RED_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED_RED_PORT, LED_RED_PIN, LED_RED_GPIOMODE, LED_RED_DEFAULT);

	GPIO_DriveStrengthSet(LED_GREEN_PORT, LED_GREEN_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED_GREEN_PORT, LED_GREEN_PIN, LED_GREEN_GPIOMODE, LED_GREEN_DEFAULT);

	//Set RGB LED gpiopin configurations
	GPIO_PinModeSet(RGB_ENABLE_PORT, RGB_ENABLE_PIN, gpioModePushPull, RGB_DEFAULT_OFF);
	GPIO_PinModeSet(RGB0_PORT, RGB0_PIN, gpioModePushPull, RGB_DEFAULT_OFF);
	GPIO_PinModeSet(RGB1_PORT, RGB1_PIN, gpioModePushPull, RGB_DEFAULT_OFF);
	GPIO_PinModeSet(RGB2_PORT, RGB2_PIN, gpioModePushPull, RGB_DEFAULT_OFF);
	GPIO_PinModeSet(RGB3_PORT, RGB3_PIN, gpioModePushPull, RGB_DEFAULT_OFF);
	GPIO_PinModeSet(RGB_RED_PORT, RGB_RED_PIN, gpioModePushPull, COLOR_DEFAULT_OFF);
	GPIO_PinModeSet(RGB_GREEN_PORT, RGB_GREEN_PIN, gpioModePushPull, COLOR_DEFAULT_OFF);
	GPIO_PinModeSet(RGB_BLUE_PORT, RGB_BLUE_PIN, gpioModePushPull, COLOR_DEFAULT_OFF);

	GPIO_DriveStrengthSet(SI1133_SENSOR_EN_PORT,SI1133_DRIVE_STRENGTH);
	GPIO_PinModeSet(SI1133_SENSOR_EN_PORT,SI1133_SENSOR_EN_PIN,SI1133_GPIO_MODE,SI1133_DEFAULT);

	GPIO_PinModeSet(SI1133_SCL_PORT,SI1133_SCL_PIN,SI1133_I2C_GPIO_MODE,SI1133_I2C_DEFAULT);
	GPIO_PinModeSet(SI1133_SDA_PORT,SI1133_SDA_PIN,SI1133_I2C_GPIO_MODE,SI1133_I2C_DEFAULT);

  GPIO_DriveStrengthSet(LEUART_TX_PORT, LEUART_TX_STRENGTH);
  GPIO_PinModeSet(LEUART_TX_PORT, LEUART_TX_PIN, LEUART_TX_GPIOMODE, LEUART_TR_DEFAULT);
  GPIO_PinModeSet(LEUART_RX_PORT, LEUART_RX_PIN, LEUART_RX_GPIOMODE, LEUART_TR_DEFAULT);



}
