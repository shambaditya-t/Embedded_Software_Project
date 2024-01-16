/**
 * @file app.c
 * @author Shambaditya Tarafder
 * @date   9/2/2021
 * @brief  This file basically constructs the letimer struct,uf,comp0,comp1 and sets up the
 *         various application peripheral.
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "app.h"
#include "LEDs_thunderboard.h"
#include <stdio.h>

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************
//static int colorLED=0;
 uint32_t x = 3;
 uint32_t y =0;

//***********************************************************************************
// Private functions
//***********************************************************************************



static void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route);

//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *This is the setup for the LETIMER peripheral setup
 *
 * @details
 *This function makes call to the cmu_open() ,gpio_open(),scheduler_open(),
 * sleep_(),rgb_init() function for the
 *initial setup.then called the letimer_pwm_open() function and starts the
 *LETIMER0
 *
 * @note
 *This function is for setting up and intializing the application peripherals
 *only
 ******************************************************************************/


void app_peripheral_setup(void){
  cmu_open();
  gpio_open();
  scheduler_open();
  sleep_open();
  rgb_init();
 // si1133_i2c_open();
  ble_open(BLE_TX_DONE_CB,BLE_RX_DONE_CB);
  sleep_block_mode(SYSTEM_BLOCK_EM);
  app_letimer_pwm_open(PWM_PER, PWM_ACT_PER, PWM_ROUTE_0, PWM_ROUTE_1);
  letimer_start(LETIMER0, true);  //This command will initiate the start of the LETIMER0
  add_scheduled_event(BOOT_UP_CB);
}

/***************************************************************************//**
 * @brief
 *This function is used for setting up the struct
 *
 * @details
 *This function is used for setting up the structures and data for the
 *app_letimer_pwm_open() function
 *
 * @note
 *Setting up the LETIMER and the pwm function
 *
 * @param[in]
 *PWM periods in seconds , initialized by float
 *
 * @param[in]
 *PWM active period in seconds, initialized by float
 ******************************************************************************/

void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route){
  // Initializing LETIMER0 for PWM operation by creating the
  // letimer_pwm_struct and initializing all of its elements
  // APP_LETIMER_PWM_TypeDef is defined in letimer.h
  APP_LETIMER_PWM_TypeDef   pwm;

  pwm.enable = false;
  pwm.debugRun = false;
  pwm.active_period = act_period;
  pwm.period = period;
  pwm.out_pin_0_en = false;
  pwm.out_pin_1_en = false;
  pwm.out_pin_route0 = out0_route;
  pwm.out_pin_route1 = out1_route;
  pwm.comp0_irq_enable = false;
  pwm.comp0_cb = LETIMER0_COMP0_CB;
  pwm.comp1_irq_enable = true;
  pwm.comp1_cb = LETIMER0_COMP1_CB;
  pwm.uf_irq_enable = true;
  pwm.uf_cb = LETIMER0_UF_CB;


 letimer_pwm_open(LETIMER0, &pwm);
}


/***************************************************************************//**
 * @brief
 *This is the setup for handling UF interrupts.
 *
 * @details
 *This function sets the interrupts and then functions basic operation of adding , dividing etc
 *and calls the ble_write function for it to appear on the terminal
 *
 *
 *
 *
 * @note
 *This function is for setting up and initializing the UF interrupts
 *
 ******************************************************************************/

void scheduled_letimer0_uf_cb (void){

/*  EFM_ASSERT(!(get_scheduled_events() & LETIMER0_UF_CB));
  if(colorLED == 0){
       leds_enabled(RGB_LED_1,COLOR_RED,false);
       colorLED++;

    }
  else if(colorLED ==1){
        leds_enabled(RGB_LED_1,COLOR_GREEN,false);
        colorLED++;
    }
  else if (colorLED ==2){
        leds_enabled(RGB_LED_1,COLOR_BLUE,false);
        colorLED = 0;
    }
*/

//  si1133_request_result( SI1133_REG_READ_CB );
  float z;
  x=x+ADD_THREE;
  y=y+ADD_ONE;
  z=(float)x/y;
  char send[CHAR_SEND];
  sprintf(send, "z = %1.1f \n", z);
  ble_write(send);


}

/***************************************************************************//**
 * @brief
 *This is the setup for the comp0 interuppts
 *
 * @details
 *Not Applicable as we are not using this function at this moment
 *
 *
 *
 *
 * @note
 *Calling this function would place this function in an EFM False loop.
 *
 ******************************************************************************/

void scheduled_letimer0_comp0_cb (void){

 EFM_ASSERT(false);
}

/***************************************************************************//**
 * @brief
 * This is the setup for the comp1 interrupts
 *
 * @details
 * This function makes a call to the read function with passed in desired value
 *
 *@note
 *
 *
 ******************************************************************************/

void scheduled_letimer0_comp1_cb (void){

 /* if(colorLED ==0){
   leds_enabled(RGB_LED_1,COLOR_RED,true);


  }
  if(colorLED == 1){
      leds_enabled(RGB_LED_1,COLOR_GREEN,true);

  }
  if(colorLED == 2){
      leds_enabled(RGB_LED_1,COLOR_BLUE,true);

  }*/

 // si1133_read(SI1133_REG_READ_CB);
 //   si1133_send_force_cmd();

}

/***************************************************************************//**
 * @brief
 *   This function checks if the desired value matches with the partID.
 *
 * @details
 *   This function checks if the desired value matches with the partID(function)
 *   If successful,then Green LED is turned on. If not then the RED LED is turned on
 *   indication that there's something wrong
 *
 * @note
 *   Proper checks and value should be implemented.
 *
 ******************************************************************************/

/*void scheduled_si1133_cb(void) {


  uint32_t value = si1133_pass_ID();
  if(value == CHECK_VAL) {
      leds_enabled(RGB_LED_1,COLOR_GREEN,true);
  }
  if(value != CHECK_VAL){
      leds_enabled(RGB_LED_1,COLOR_RED,true);
  }


}*/

/***************************************************************************//**
 * @brief
 *  This function basically performs the white light operation.
 *
 * @details
 *   This function checks if the desired value matches with the pass_ID(function)/register
 *   If the value is less than 20 then the blue led of RGB LED_1 lights up else if
 *   its greater than or equal to 20 then it turns off
 *
 *
 *
 * @note
 *   Proper checks and value should be implemented.
 *
 *
 ******************************************************************************/

void si1133_white_op(void){

    uint32_t value = si1133_pass_ID();
    if(value < SENSE_VAL) {
        leds_enabled(RGB_LED_1,COLOR_BLUE,true);
    }
    else if(value >= SENSE_VAL){
        leds_enabled(RGB_LED_1,COLOR_BLUE,false);
    }

}

/***************************************************************************//**
 * @brief
 *  This function basically handles the boot up event
 *
 * @details
 *   This function was initially used to set the connection name and then it would
 *   print hello world on the terminal and call the letimer_start function
 *
 *
 *
 ******************************************************************************/

void scheduled_boot_up_cb(void){
//  EFM_ASSERT(get_scheduled_events() & BOOT_UP_CB);
#ifdef BLE_TEST_ENABLED
  EFM_ASSERT(ble_test("Sam"));
  timer_delay(DELAY);
#endif
  ble_write("\nHello World\n");
  letimer_start(LETIMER0, true);
}

/***************************************************************************//**
 * @brief
 *  Not currently in use
 *
 *
 ******************************************************************************/

void scheduled_ble_tx_done_cb(void) {
// Not in use
}



