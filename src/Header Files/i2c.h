//***********************************************************************************
// Include files
//***********************************************************************************

#ifndef HEADER_FILES_I2C_H_
#define HEADER_FILES_I2C_H_

/* System include statements */

#include "stdbool.h"

/* Silicon Labs include statements */
#include "em_i2c.h"
#include "em_assert.h"
#include "sleep_routines.h"
#include "scheduler.h"

//***********************************************************************************
// defined files
//***********************************************************************************

#define I2C_EM_BLOCK  EM2
#define I2C_READ    1
#define I2C_WRITE   0

//***********************************************************************************
// global variables
//***********************************************************************************

typedef struct {

  volatile bool    enable;
  bool    master;
  uint32_t refFreq;
  uint32_t freq;
  I2C_ClockHLR_TypeDef clhr;
  uint32_t   out_pin_scl_route;
  uint32_t   out_pin_sda_route;
  uint32_t   out_pin_scl_en;
  uint32_t   out_pin_sda_en;

}I2C_OPEN_STRUCT;

typedef struct {

    uint32_t        state;
    I2C_TypeDef     *I2Cn;
    uint32_t        slave_add;
    uint32_t        slave_reg;
    bool            write_read;
    uint32_t        *data_add;
    uint32_t        bytes;
    bool            busy;
    uint32_t        cb;
    int             counter;



}I2C_STATE_MACHINE;

//***********************************************************************************
// function prototypes
//***********************************************************************************

void i2c_start(I2C_TypeDef *i2c, uint32_t slave_add, uint32_t slave_reg, bool write_read, uint32_t *data_add, uint32_t bytes, uint32_t cb,int counter);
void I2C0_IRQHandler(void);
void I2C1_IRQHandler(void);
void i2c_open(I2C_TypeDef *i2c, I2C_OPEN_STRUCT *i2c_open);


#endif /* HEADER_FILES_I2C_H_ */
