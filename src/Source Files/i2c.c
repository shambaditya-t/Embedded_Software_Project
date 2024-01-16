/**
 * @file i2c.c
 * @author Shambaditya Tarafder
 * @date   10/16/2021
 * @brief  This file contains the i2c related functions
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************

#include "em_i2c.h"
#include "i2c.h"
#include "em_cmu.h"


//***********************************************************************************
// Private variables
//***********************************************************************************

typedef enum {
  Start_CMD,
  Reg_CMD,
  Wait_Read,
  Process_Sense,
  Stop,
} DEFINED_STATES;


I2C_STATE_MACHINE i2c0_sm;
I2C_STATE_MACHINE i2c1_sm;

//***********************************************************************************
// Private functions
//***********************************************************************************

static void i2c_bus_reset(I2C_TypeDef *i2c);
static void i2c_ack_sm(I2C_STATE_MACHINE *i2c);
static void i2c_nack_sm(I2C_STATE_MACHINE *i2c);
static void i2c_mstop_sm(I2C_STATE_MACHINE *i2c);
static void i2c_rxdatav_sm(I2C_STATE_MACHINE *i2c);

/***************************************************************************//**
 * @brief
 *   Sets up the i2c bus
 *
 * @details
 *   This function basically initializes the i2c bus,it also initializes and sets
 *   up the clock frequencies,interrupts and the i2c struct.
 *
 * @note
 *   This function is just for setting up the structs etc. not operating on it
 *
 *
 * @param[in] i2c
 *   It is pointing address of the i2c peripheral being used.[i2c0 or i2c1]
 *
 * @param[in] i2c_open
 *   It is the data structure that will be used for modification etc.
 *
 *
 ******************************************************************************/
void i2c_open(I2C_TypeDef *i2c, I2C_OPEN_STRUCT *i2c_open) {
  i2c0_sm.busy = false;
  i2c1_sm.busy = false;
  I2C_Init_TypeDef i2c_values;
if(i2c == I2C0) {
    CMU_ClockEnable(cmuClock_I2C0, true);
  } else if (i2c == I2C1) {
    CMU_ClockEnable(cmuClock_I2C1, true);
  } else {
    EFM_ASSERT(false);
  }

  if ((i2c->IF & 0x01) == 0) {
    i2c->IFS = 0x01;
    EFM_ASSERT(i2c->IF & 0x01);
    i2c->IFC = 0x01;
  } else {
    i2c->IFC = 0x01;
    EFM_ASSERT(!(i2c->IF & 0x01));
  }

  i2c_values.clhr = i2c_open->clhr;
  i2c_values.enable = i2c_open->enable;
  i2c_values.freq = i2c_open->freq;
  i2c_values.master = i2c_open->master;
  i2c_values.refFreq = i2c_open->refFreq;

  I2C_Init(i2c, &i2c_values);

  i2c->ROUTELOC0 = i2c_open->out_pin_scl_route | i2c_open->out_pin_sda_route;
  i2c->ROUTEPEN = (I2C_ROUTEPEN_SCLPEN*i2c_open->out_pin_scl_en) | (I2C_ROUTEPEN_SDAPEN*i2c_open->out_pin_sda_en);

  i2c_bus_reset(i2c);

    i2c->IFC = I2C_IF_ACK;
    i2c->IEN |= I2C_IF_ACK;
    i2c->IEN |= I2C_IF_NACK;
    i2c->IEN |= I2C_IF_MSTOP;
    i2c->IEN |= I2C_IF_RXDATAV;

    if(i2c == I2C0) {
      NVIC_EnableIRQ(I2C0_IRQn);
    } else if (i2c == I2C1) {
      NVIC_EnableIRQ(I2C1_IRQn);
    }
}

/***************************************************************************//**
 * @brief
 *   This function resets the i2c
 *
 * @details
 *   This function basically reset the I2C state machines of the MightyGecko I2C peripheral as well as reset the I2C state machines of the
 *   external I2C devices such as the Si1133
 *
 *
 * @note
 *   Ensure the proper procedure for resetting
 *
 *
 * @param[in] i2c
 *   It is pointing address of the i2c peripheral being used.[i2c0 or i2c1]
 *
 * @param[in] i2c_open
 *   It is the data structure that will be used for modification etc. presently
 *
 *
 ******************************************************************************/

void i2c_bus_reset(I2C_TypeDef * i2c) {

  if(i2c->STATE & I2C_STATE_BUSY){
  i2c->CMD = I2C_CMD_ABORT;
  while(i2c->STATE & I2C_STATE_BUSY);
  }

  uint32_t state = i2c->IEN;
  i2c->IEN = false;
  i2c->IFC = i2c->IF;
  i2c->CMD = I2C_CMD_CLEARTX;
  i2c->CMD = I2C_CMD_START | I2C_CMD_STOP;
  while(!(i2c->IF & I2C_IF_MSTOP));
  i2c->IFC = i2c->IF;
  i2c->CMD = I2C_CMD_ABORT;
  i2c->IEN = state;
}

/***************************************************************************//**
 * @brief
 *   This function is the IRQ handler for I2C1
 *
 * @details
 *   When the function is called it basically points/calls the interrupts based on
 *   the required specification
 *
 * @note
 *   It handles/calls the interrupts for ACK,NACK,MSTOP,RXDATAV
 *
 ******************************************************************************/

void I2C1_IRQHandler(void) {

  int int_flag = I2C1->IF;
  I2C1->IFC = int_flag;

  if(int_flag & I2C_IF_ACK) {
    i2c_ack_sm(&i2c1_sm);
  }
  if(int_flag & I2C_IF_NACK) {
    i2c_nack_sm(&i2c1_sm);
  }
  if(int_flag & I2C_IF_MSTOP) {
    i2c_mstop_sm(&i2c1_sm);
  }
  if(int_flag & I2C_IF_RXDATAV) {
    i2c_rxdatav_sm(&i2c1_sm);
  }
}

/***************************************************************************//**
 * @brief
 *   This function is the IRQ handler for I2C0
 *
 * @details
 *   When the function is called it basically points/calls the interrupts based on
 *   the required specification
 *
 * @note
 *   It handles/calls the interrupts for ACK,NACK,MSTOP,RXDATAV
 *
 ******************************************************************************/


void I2C0_IRQHandler(void) {

  int int_flag = I2C0->IF;
  I2C0->IFC = int_flag;

  if(int_flag & I2C_IF_ACK) {
    i2c_ack_sm(&i2c0_sm);
  }
  if(int_flag & I2C_IF_NACK) {
    i2c_nack_sm(&i2c0_sm);
  }
  if(int_flag & I2C_IF_MSTOP) {
    i2c_mstop_sm(&i2c0_sm);
  }
  if(int_flag & I2C_IF_RXDATAV) {
    i2c_rxdatav_sm(&i2c0_sm);
  }
}

/***************************************************************************//**
 * @brief
 *   This function is called by the i2c interrupt handler whenever ACK is encountered
 *
 * @details
 *   This function defines the ACK behavior for the state machine
 *
 *
 * @note
 *   Shouldn't encounter EFM Assert.If it does then something must have gone wrong
 *
 *
 * @param[in] i2c
 *   The i2c SM currently in use
 *
 *
 ******************************************************************************/

static void i2c_ack_sm(I2C_STATE_MACHINE *i2c){
  switch(i2c->state){
    case Start_CMD:
      i2c->state = Reg_CMD;
      i2c->I2Cn->TXDATA = i2c->slave_reg;
      break;
    case Reg_CMD:
      i2c->state = Wait_Read;
      i2c->I2Cn->CMD = I2C_CMD_START;
      i2c->I2Cn->TXDATA = (i2c->slave_add << 1) | I2C_READ;
      break;
    case Wait_Read:
      i2c->state = Process_Sense;
      break;
    case Process_Sense:
      EFM_ASSERT(false);
      break;
    case Stop:
      EFM_ASSERT(false);
      break;
    default :
      EFM_ASSERT(false);
      break;
  }
}

/***************************************************************************//**
 * @brief
 *   This function is called by the i2c interrupt handler whenever NACK is encountered
 *
 * @details
 *   This function defines the NACK or not available behavior for the state machine
 *
 *
 * @note
 *   Shouldn't encounter EFM Assert.If it does then something must have gone wrong
 *
 *
 * @param[in] i2c
 *   The i2c SM currently in use
 *
 *
 ******************************************************************************/

static void i2c_nack_sm(I2C_STATE_MACHINE *i2c){
  switch(i2c->state){
    case Start_CMD:
      EFM_ASSERT(false);
      break;
    case Reg_CMD:
      EFM_ASSERT(false);
      break;
    case Wait_Read:
      EFM_ASSERT(false);
      break;
    case Process_Sense:
      EFM_ASSERT(false);
      break;
    case Stop:
      EFM_ASSERT(false);
      break;
    default :
      EFM_ASSERT(false);
      break;
  }
}

/***************************************************************************//**
 * @brief
 *   This function is called by the i2c interrupt handler whenever MSTOP is encountered
 *
 * @details
 *   This function defines the MSTOP behavior for the state machine
 *
 *
 * @note
 *   Shouldn't encounter EFM Assert.If it does then something must have gone wrong
 *
 *
 * @param[in] i2c
 *   The i2c SM currently in use
 *
 *
 ******************************************************************************/

static void i2c_mstop_sm(I2C_STATE_MACHINE *i2c){
  switch(i2c->state) {
  case Start_CMD:
    EFM_ASSERT(false);
    break;
  case Reg_CMD:
    EFM_ASSERT(false);
    break;
  case Wait_Read:
    EFM_ASSERT(false);
    break;
  case Process_Sense:
    EFM_ASSERT(false);
    break;
  case Stop:
    sleep_unblock_mode(EM2);
    add_scheduled_event(i2c->cb);
    i2c->busy = false;
    break;
  default:
    EFM_ASSERT(false);
    break;
  }
}

/***************************************************************************//**
 * @brief
 *   This function is called by the i2c interrupt handler whenever RXDATAV is encountered
 *
 * @details
 *   This function defines the RXDATAV behavior for the state machine when data becomes
 *   available and follows the logic defined in the flowchart
 *
 * @note
 *   Shouldn't encounter EFM Assert.If it does then something must have gone wrong
 *
 *
 * @param[in] i2c
 *   The i2c SM currently in use
 *
 *
 ******************************************************************************/

static void i2c_rxdatav_sm(I2C_STATE_MACHINE *i2c){
  switch(i2c->state) {
  case Start_CMD:
    EFM_ASSERT(false);
    break;
  case Reg_CMD:
    EFM_ASSERT(false);
    break;
  case Wait_Read:
    EFM_ASSERT(false);
    break;
  case Process_Sense:
 //   EFM_ASSERT(false);
    i2c->data_add[i2c->counter] = i2c->I2Cn->RXDATA;
    i2c->bytes--;
    i2c->counter++;
    if (i2c->bytes > 0){

        i2c->I2Cn->CMD = I2C_CMD_ACK;
    }
    if (i2c->bytes == 0){
            i2c->state = Stop;
            i2c->I2Cn->CMD = I2C_CMD_NACK;
            i2c->I2Cn->CMD = I2C_CMD_STOP;
        }
    break;
  case Stop:
    EFM_ASSERT(false);
    break;
  default:
    EFM_ASSERT(false);
    break;
  }
}

/***************************************************************************//**
 * @brief
 *   Sets up the i2c state machine for operation
 *
 * @details
 *   Initializes a private STRUCTin i2c that will keep state of the progress of the I2C operation.
 *   This state information will be stored in a static STRUCT
 *   in i2c.c of type I2C_STATE_MACHINE.  In your case, the read of the
 *   Si1133Part ID register
 *
 *
 * @note
 *   This function should called when starting the i2c operation
 *
 *
 * @param[in] i2c
 *   It is pointing address of the i2c peripheral being used.[i2c0 or i2c1]
 *
 * @param[in] slave_add
 *   Address of the peripheral
 *
 * @param[in] slave_reg
 *   The partID to be sent to the peripheral
 *
 * @param[in] write_read
 *   Boolean value for read/write op.
 *
 * @param[in] data_add
 *  address of the read(in this case) data
 *
 * @param[in] bytes
 *  The number of bytes required for operation
 *
 * @param[in] cb
 *  this is the callback event
 *
 * @param[in] counter
 *  To keep track while looping through
 *
 ******************************************************************************/

void i2c_start(I2C_TypeDef *i2c, uint32_t slave_add, uint32_t slave_reg, bool write_read, uint32_t *data_add, uint32_t bytes, uint32_t cb,int counter) {

  I2C_STATE_MACHINE *i2c_sm_pt;
  if(i2c == I2C0){
      i2c_sm_pt = &i2c0_sm;
  }
  if(i2c == I2C1){
      i2c_sm_pt = &i2c1_sm;
    }

  while(i2c_sm_pt->busy);
  EFM_ASSERT((i2c->STATE & _I2C_STATE_STATE_MASK) == I2C_STATE_STATE_IDLE);
  sleep_block_mode(I2C_EM_BLOCK);


    i2c_sm_pt->state = Start_CMD;
    i2c_sm_pt->I2Cn = i2c;
    i2c_sm_pt->slave_add = slave_add;
    i2c_sm_pt->slave_reg = slave_reg;
    i2c_sm_pt->write_read = write_read;
    i2c_sm_pt->data_add = data_add;
    i2c_sm_pt->bytes = bytes;
    i2c_sm_pt->counter = counter;

    i2c_sm_pt->I2Cn->CMD = I2C_CMD_START;
    i2c_sm_pt->I2Cn->TXDATA = (i2c_sm_pt->slave_add << 1) | I2C_WRITE;
    i2c_sm_pt->busy = true;
    i2c_sm_pt->cb = cb;
}
