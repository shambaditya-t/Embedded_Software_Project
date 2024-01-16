/**
 * @file SI1133.c
 * @author Shambaditya Tarafder
 * @date   10/16/2021
 * @brief  This file contains the SI1133 related functions/operations
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************

#include "SI1133.h"
#include "em_i2c.h"

//***********************************************************************************
// Private variables
//***********************************************************************************

static uint32_t data;
uint32_t byte = 1;
bool Write_Read = true;
int counter = 0;

/***************************************************************************//**
 * @brief
 *  This function basically requests the read from the SI1133 sensor.
 *
 * @details
 *   This function utilizes only in the read mode from the flowchart
 *
 *
 * @note
 *   Starts the read operation from the sensor only
 *
 *
 * @param[in] SI1133_LIGHT_READ_CB
 *   Sets the event to be scheduled upon completion of the I2C operation
 *
 *
 ******************************************************************************/

void si1133_read(uint32_t SI1133_LIGHT_READ_CB) {
  i2c_start(I2C1,SI1133_ADDRESS, SI1133_PART, Write_Read, &data, byte,SI1133_LIGHT_READ_CB,counter);
}

/***************************************************************************//**
 * @brief
 *   This function is for passing the SI1133 part ID
 *
 * @details
 *   function will be used to pass up the Si1133 Part ID read result
 *   which is saved in the result SI1133 static/private variable
 *
 *
 * @note
 *   The Part ID response should be 51 decimal or 0x33 hexidecimal.
 *
 ******************************************************************************/

uint32_t si1133_pass_ID(void){
  return data;
}

/***************************************************************************//**
 * @brief
 *   This function initiates the SI1133 sensor
 *
 * @details
 *   This STRUCT contains the information to complete the set-up of the I2C external devices.
 *
 *
 * @note
 *   Time delay must be incorporated to take into account the delays
 *
 ******************************************************************************/

void si1133_i2c_open(){
  timer_delay(POR);
  I2C_OPEN_STRUCT i2cOpen;

  i2cOpen.enable = true;
  i2cOpen.master = true;
  i2cOpen.out_pin_scl_en = true;
  i2cOpen.out_pin_sda_en = true;
  i2cOpen.out_pin_scl_route = SCL_ROUTE;
  i2cOpen.out_pin_sda_route = SDA_ROUTE;
  i2cOpen.freq = I2C_FREQ_FAST_MAX;
  i2cOpen.clhr = i2cClockHLRAsymetric;

  i2c_open(I2C1, &i2cOpen);
}


