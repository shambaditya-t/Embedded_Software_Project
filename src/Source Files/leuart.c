/**
 * @file leuart.c
 * @author Shambaditya Tarafder/K.Graham
 * @date   11/6/2021
 * @brief Contains all the functions of the LEUART peripheral
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************

//** Standard Library includes
#include <string.h>

//** Silicon Labs include files
#include "em_gpio.h"
#include "em_cmu.h"

//** Developer/user include files
#include "leuart.h"
#include "scheduler.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// private variables
//***********************************************************************************
uint32_t	rx_done_evt;
uint32_t	tx_done_evt;
bool		leuart0_tx_busy;

typedef enum {
  ENABLE_TRANSMIT,
  TRANSMIT_DATA,
  END_TRANSMIT,
} DEFINED_STATES;

typedef enum {
  STARTFRAME,
  RECEIVE,
  SIGFRAME
}LEUART_RX_STATE;

static LEUART_STATE_MACHINE leuart_state;
static RX_LEUART_STATE_MACHINE leuart_rx_state;

/***************************************************************************//**
 * @brief LEUART driver
 * @details
 *  This module contains all the functions to support the driver's state
 *  machine to transmit a string of data across the LEUART bus.  There are
 *  additional functions to support the Test Driven Development test that
 *  is used to validate the basic set up of the LEUART peripheral.  The
 *  TDD test for this class assumes that the LEUART is connected to the HM-18
 *  BLE module.  These TDD support functions could be used for any TDD test
 *  to validate the correct setup of the LEUART.
 *
 ******************************************************************************/

//***********************************************************************************
// Private functions
//***********************************************************************************
static void leuart_txbl(LEUART_STATE_MACHINE *leuart_state);
static void leuart_txc(LEUART_STATE_MACHINE *leuart_state);
static void leuart_startf(RX_LEUART_STATE_MACHINE *leuart_state);
static void leuart_rxdatav(RX_LEUART_STATE_MACHINE *leuart_state);
static void leuart_sigf(RX_LEUART_STATE_MACHINE *leuart_state);

//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *   Sets up the leuart bus
 *
 * @details
 *   This function basically initializes the leuart bus,it also initializes and sets
 *   up the clock frequencies,interrupts and the leuart struct.
 *
 * @note
 *   This function is just for setting up the structs etc. not operating on it
 *
 *
 * @param[in] leuart
 *   It is pointing address of the leuart peripheral being used.
 *
 * @param[in] leuart_settings
 *   It is the data structure that will be used for modification etc.
 *
 *
 ******************************************************************************/

void leuart_open(LEUART_TypeDef *leuart, LEUART_OPEN_STRUCT *leuart_settings){

  LEUART_Init_TypeDef leuart_init;


  if(leuart == LEUART0) {
      CMU_ClockEnable(cmuClock_LEUART0, true);
    } else {
      EFM_ASSERT(false);
    }

  if(!(leuart->STARTFRAME & 0x01)){
      leuart->STARTFRAME = 0x01;
      while(leuart->SYNCBUSY);
      EFM_ASSERT(leuart->STARTFRAME & 0x01);
      leuart->STARTFRAME = 0x00;
      while(leuart->SYNCBUSY);
    }

  leuart_init.baudrate = leuart_settings->baudrate;
  leuart_init.databits = leuart_settings->databits;
  leuart_init.parity = leuart_settings->parity;
  leuart_init.refFreq = false;
  leuart_init.stopbits = leuart_settings->stopbits;
  leuart_init.enable = leuart_settings->enable;
  tx_done_evt = leuart_settings->tx_done_evt;
  rx_done_evt = leuart_settings->rx_done_evt;

  LEUART_Init(leuart, &leuart_init);
  while(leuart->SYNCBUSY);

  leuart->ROUTELOC0 = leuart_settings->rx_loc | leuart_settings->tx_loc;
  leuart->ROUTEPEN = (leuart_settings->rx_pin_en*leuart_settings->rx_en) | (leuart_settings->tx_pin_en*leuart_settings->tx_en);

  leuart->CMD = LEUART_CMD_CLEARTX | LEUART_CMD_CLEARRX;

  while(leuart->SYNCBUSY);

  LEUART_Enable(leuart, leuart_settings->enable);
  while(leuart->SYNCBUSY);

  if(leuart_settings->tx_en){
    leuart->CMD = LEUART_CMD_TXEN;
    while(!(leuart->STATUS & LEUART_STATUS_TXENS));
    EFM_ASSERT(leuart->STATUS & LEUART_STATUS_TXENS);
  }
  if(leuart_settings->rx_en){
      leuart->CMD = LEUART_CMD_RXEN;
      while(!(leuart->STATUS & LEUART_STATUS_RXENS));
      EFM_ASSERT(leuart->STATUS & LEUART_STATUS_RXENS);
  }

  if(leuart == LEUART0) {
      NVIC_EnableIRQ(LEUART0_IRQn);
    }
    else {
      EFM_ASSERT(false);
    }
  //week's checkpoint
  leuart_rx_state.leuart = leuart;
  leuart_rx_state.callback = rx_done_evt;
  leuart_rx_state.length = ZERO;
  leuart_rx_state.state = STARTFRAME;

  leuart_rx_state.leuart->STARTFRAME = '#';
  leuart_rx_state.leuart->SIGFRAME = '!';

  //Enabling the interrupts
  leuart_rx_state.leuart->IEN |= LEUART_IEN_STARTF;
//  leuart_rx_state.leuart->IEN |= LEUART_IEN_SIGF;
//  leuart_rx_state.leuart->IEN |= LEUART_IEN_RXDATAV;

  leuart->CTRL |= LEUART_CTRL_SFUBRX;
  while(leuart->SYNCBUSY);
  leuart->CMD = LEUART_CMD_RXBLOCKEN;
  while(leuart->SYNCBUSY);

  leuart_test();



}

/***************************************************************************//**
 * @brief
 *   This function is the IRQ handler for LEUART0
 *
 * @details
 *   When the function is called it basically points/calls the interrupts based on
 *   the required specification
 *
 * @note
 *   It handles/calls the interrupts for TXBL and TXC
 *
 ******************************************************************************/

void LEUART0_IRQHandler(void){
  int int_flag = LEUART0->IF & LEUART0->IEN;
    LEUART0->IFC = int_flag;

    if(int_flag & LEUART_IF_TXBL) {
      leuart_txbl(&leuart_state);
    }
    if(int_flag & LEUART_IF_TXC) {
      leuart_txc(&leuart_state);
    }
    if(int_flag & LEUART_IF_STARTF){
        leuart_startf(&leuart_rx_state);
      }
    if(int_flag & LEUART_IF_RXDATAV){
        leuart_rxdatav(&leuart_rx_state);
      }
    if(int_flag & LEUART_IF_SIGF){
        leuart_sigf(&leuart_rx_state);
      }
}

/***************************************************************************//**
 * @brief
 *   This function is called by the leuart interrupt handler whenever TXBL is encountered
 *
 * @details
 *   This function defines the TXBL behavior for the state machine
 *
 *
 * @note
 *   Shouldn't encounter EFM Assert.If it does then something must have gone wrong
 *
 *
 * @param[in] i2c
 *   The leuart SM currently in use
 *
 *
 ******************************************************************************/

static void leuart_txbl(LEUART_STATE_MACHINE *leuart_state){
  switch(leuart_state->state) {
    case ENABLE_TRANSMIT:
      leuart_state->state = TRANSMIT_DATA;
      break;
    case TRANSMIT_DATA:
      leuart_state->leuart->TXDATA = leuart_state->string[leuart_state->count];
      leuart_state->count++;
      if (leuart_state->count == leuart_state->length) {
        leuart_state->leuart->IEN &= ~LEUART_IF_TXBL;
        leuart_state->leuart->IFC = LEUART_IFC_TXC;
        leuart_state->leuart->IEN = LEUART_IEN_TXC;
        leuart_state->state = END_TRANSMIT;
      }
      break;
    case END_TRANSMIT:
      EFM_ASSERT(false);
      break;
    default:
      EFM_ASSERT(false);
      break;
  }
}

/***************************************************************************//**
 * @brief
 *   This function is called by the leuart interrupt handler whenever TXC is encountered
 *
 * @details
 *   This function defines the TXC behavior for the state machine
 *
 *
 * @note
 *   Shouldn't encounter EFM Assert.If it does then something must have gone wrong
 *
 *
 * @param[in] i2c
 *   The leuart SM currently in use
 *
 *
 ******************************************************************************/

static void leuart_txc(LEUART_STATE_MACHINE *leuart_state){
  switch(leuart_state->state) {
    case ENABLE_TRANSMIT:
      EFM_ASSERT(false);
      break;
    case TRANSMIT_DATA:
      EFM_ASSERT(false);
      break;
    case END_TRANSMIT:
      leuart_state->leuart->IEN &= ~LEUART_IEN_TXC;
      add_scheduled_event(leuart_state->callback);
      sleep_unblock_mode(LEUART_TX_EM);
      leuart_state->busy = false;
      leuart_state->state = ENABLE_TRANSMIT;
      break;
    default:
      EFM_ASSERT(false);
      break;
  }
}


static void leuart_startf(RX_LEUART_STATE_MACHINE *leuart_state){
  switch(leuart_state->state){
    case STARTFRAME:{

      leuart_state->state = RECEIVE;

      leuart_state->length = 0;

      leuart_state->string[leuart_state->length] = leuart_state->leuart->RXDATA;

      leuart_state->length++;

      leuart_state->leuart->IEN |= LEUART_IEN_SIGF;
      leuart_state->leuart->CMD = LEUART_CMD_RXBLOCKDIS;
      leuart_state->leuart->IEN |= LEUART_IEN_RXDATAV;
    break;
    }
    case RECEIVE:{
      leuart_state->length = 0;
    break;
    }
    case SIGFRAME:{
      EFM_ASSERT(false);
    break;
    }
    default:{
      EFM_ASSERT(false);
    break;
    }
  }
}

static void leuart_rxdatav(RX_LEUART_STATE_MACHINE *leuart_state){
  switch(leuart_state->state){
    case STARTFRAME:{

      EFM_ASSERT(false);
    break;
    }
    case RECEIVE:{
      leuart_state->string[leuart_state->length] = leuart_state->leuart->RXDATA;
      leuart_state->length++;
    break;
    }
    case SIGFRAME:{

      EFM_ASSERT(false);
    break;
    }
    default:{
      EFM_ASSERT(false);
    break;
    }
  }
}

static void leuart_sigf(RX_LEUART_STATE_MACHINE *leuart_state){
  switch(leuart_state->state){
    case STARTFRAME:{

      EFM_ASSERT(false);
    break;
    }
    case RECEIVE:{
      leuart_state->state = SIGFRAME;
      leuart_state->leuart->IEN &= ~LEUART_IEN_SIGF;
      leuart_state->leuart->IEN &= ~LEUART_IEN_RXDATAV;
      leuart_state->leuart->CMD = LEUART_CMD_RXBLOCKEN;
      leuart_state->string[leuart_state->length] = '\0';
      leuart_state->length++;
      leuart_state->state = STARTFRAME;
      add_scheduled_event(rx_done_evt);
    break;
    }
    case SIGFRAME:{
      EFM_ASSERT(false);
    break;
    }
    default:{
      EFM_ASSERT(false);
    break;
    }
  }
}



/***************************************************************************//**
 * @brief
 *   Sets up the leuart state machine for operation
 *
 * @details
 *   This function basically initially block the EM and initializes the leuart
 *   struct and the correct state for thr operations to begin
 *
 * @note
 *   This function is just for setting up the structs etc. not operating on it
 *
 *
 * @param[in] string
 *   The string which is to be sent to the module
 *
 * @param[in] string_len
 *   The length of the string
 *
 *
 ******************************************************************************/



void leuart_start(LEUART_TypeDef *leuart, char *string, uint32_t string_len){

    while(leuart->SYNCBUSY);

    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_CRITICAL();
    while(leuart_state.busy == true);
    sleep_block_mode(LEUART_TX_EM);

    leuart_state.callback = tx_done_evt;
    leuart_state.count = 0;
    leuart_state.length = string_len;
    leuart_state.leuart = leuart;
    leuart_state.state = TRANSMIT_DATA;
    strcpy(leuart_state.string, string);
    leuart_state.busy = true;

    leuart->IEN = LEUART_IEN_TXBL;
    CORE_EXIT_CRITICAL();


}

/***************************************************************************//**
 * @brief
 *  This function is basically used when TX is busy
 *
 * @details
 *   This function is basically used when TX is busy and also returns the same
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 ******************************************************************************/

bool leuart_tx_busy(LEUART_TypeDef *leuart){

    return (leuart->SYNCBUSY);

}

/***************************************************************************//**
 * @brief
 *   LEUART STATUS function returns the STATUS of the peripheral for the
 *   TDD test
 *
 * @details
 * 	 This function enables the LEUART STATUS register to be provided to
 * 	 a function outside this .c module.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @return
 * 	 Returns the STATUS register value as an uint32_t value
 *
 ******************************************************************************/

uint32_t leuart_status(LEUART_TypeDef *leuart){
	uint32_t	status_reg;
	status_reg = leuart->STATUS;
	return status_reg;
}

/***************************************************************************//**
 * @brief
 *   LEUART CMD Write sends a command to the CMD register
 *
 * @details
 * 	 This function is used by the TDD test function to program the LEUART
 * 	 for the TDD tests.
 *
 * @note
 *   Before exiting this function to update  the CMD register, it must
 *   perform a SYNCBUSY while loop to ensure that the CMD has by synchronized
 *   to the lower frequency LEUART domain.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @param[in] cmd_update
 * 	 The value to write into the CMD register
 *
 ******************************************************************************/

void leuart_cmd_write(LEUART_TypeDef *leuart, uint32_t cmd_update){

	leuart->CMD = cmd_update;
	while(leuart->SYNCBUSY);
}

/***************************************************************************//**
 * @brief
 *   LEUART IF Reset resets all interrupt flag bits that can be cleared
 *   through the Interrupt Flag Clear register
 *
 * @details
 * 	 This function is used by the TDD test program to clear interrupts before
 * 	 the TDD tests and to reset the LEUART interrupts before the TDD
 * 	 exits
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 ******************************************************************************/

void leuart_if_reset(LEUART_TypeDef *leuart){
	leuart->IFC = 0xffffffff;
}

/***************************************************************************//**
 * @brief
 *   LEUART App Transmit Byte transmits a byte for the LEUART TDD test
 *
 * @details
 * 	 The BLE module will respond to AT commands if the BLE module is not
 * 	 connected to the phone app.  To validate the minimal functionality
 * 	 of the LEUART peripheral, write and reads to the LEUART will be
 * 	 performed by polling and not interrupts.
 *
 * @note
 *   In polling a transmit byte, a while statement checking for the TXBL
 *   bit in the Interrupt Flag register is required before writing the
 *   TXDATA register.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @param[in] data_out
 *   Byte to be transmitted by the LEUART peripheral
 *
 ******************************************************************************/

void leuart_app_transmit_byte(LEUART_TypeDef *leuart, uint8_t data_out){
	while (!(leuart->IF & LEUART_IF_TXBL));
	leuart->TXDATA = data_out;
}


/***************************************************************************//**
 * @brief
 *   LEUART App Receive Byte polls a receive byte for the LEUART TDD test
 *
 * @details
 * 	 The BLE module will respond to AT commands if the BLE module is not
 * 	 connected to the phone app.  To validate the minimal functionality
 * 	 of the LEUART peripheral, write and reads to the LEUART will be
 * 	 performed by polling and not interrupts.
 *
 * @note
 *   In polling a receive byte, a while statement checking for the RXDATAV
 *   bit in the Interrupt Flag register is required before reading the
 *   RXDATA register.
 *
 * @param[in] leuart
 *   Defines the LEUART peripheral to access.
 *
 * @return
 * 	 Returns the byte read from the LEUART peripheral
 *
 ******************************************************************************/

uint8_t leuart_app_receive_byte(LEUART_TypeDef *leuart){
	uint8_t leuart_data;
	while (!(leuart->IF & LEUART_IF_RXDATAV));
	leuart_data = leuart->RXDATA;
	return leuart_data;
}

/***************************************************************************//**
 * @brief
 *   This function is used for checking the functionality of the Read of the Leuart
 *   by using the loopback functionality.
 *
 *
 * @details
 *   This function is basically for test driven development using the loopback to talk to itself.
 *   This is used for verifying things as to if everything's working perfectly or not.The input string
 *   to be used includes some string before the startframe,the string/data after it , the sigframe and
 *   data/string after that. The leuart RX program should basically only pass  the startframe,the string/data
 *   in-between and the sigframe. A problem or error in the application layer cannot be tested within this
 *   driver code,and it may become a basis of a test escape.
 *   I'm not sure as to why we are using the C library strcmp instead of for loop. One reason might that
 *   it will catch strings without NULL character.A while loop can also be used but I have not included
 *   that here.
 *
 *
 * @note
 *
 *   The loopback must be enabled and the rxblocken must be handled carefully
 *
 *
 ******************************************************************************/

void leuart_test(void){


    LEUART_TypeDef * leuart = leuart_rx_state.leuart;
   // set IEN to 0
    uint32_t save_IEN;
    save_IEN = leuart->IEN;
    leuart->IEN = ZERO;

    //making it atomic
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_CRITICAL();

    //enable loopback using the CTRL Register
    leuart->CTRL |= LEUART_CTRL_LOOPBK ;
    while(leuart->SYNCBUSY);

    EFM_ASSERT(leuart->STATUS & LEUART_STATUS_RXBLOCK);

      char local_startf, local_sigf;
      local_startf = leuart->STARTFRAME;
      local_sigf = leuart->SIGFRAME;

      //Test case :  if its not local_startf as in sending anything other than #

      leuart->TXDATA = ~local_startf;
      timer_delay(TIME_DELAY_SHORT);
      EFM_ASSERT(!(leuart->IF & LEUART_IF_RXDATAV));

      //Test case : to see if the startframe behaves as expected

      leuart->TXDATA = local_startf;
      timer_delay(TIME_DELAY_SHORT);
      EFM_ASSERT(leuart->IF & LEUART_IF_RXDATAV);
      EFM_ASSERT(local_startf == leuart->RXDATA);

      //Test case : to check if sigframe behaves as expected

      leuart->TXDATA = local_sigf;
      timer_delay(TIME_DELAY_SHORT);
      EFM_ASSERT(leuart->IF & LEUART_IF_SIGF);
      EFM_ASSERT(local_sigf == leuart->RXDATA);

      CORE_EXIT_CRITICAL();

      // clearing the flags
      leuart->IFC = leuart->IF;
      leuart->IEN = save_IEN;


      leuart->CMD = LEUART_CMD_RXBLOCKEN;
      while(leuart->SYNCBUSY);

 // Test Case : This is for testing and making sure the state machine is implemented correctly
      char test_str[] = "123";
      char  input_str[CHAR_SIZE]  ;
      uint32_t length;

      input_str[ZERO] = ZERO;

      strcat(input_str,"abc");
      length = strlen(input_str);
      input_str[length] = leuart->STARTFRAME;
      input_str[length+ONE] = ZERO;
      strcat(input_str, test_str);
      length = strlen(input_str);
      input_str[length] = leuart->SIGFRAME;
      input_str[length+ONE] = ZERO;
      strcat(input_str, "xyz");

      char corr_str[CHAR_SIZE] ;

      corr_str[ZERO] = leuart->STARTFRAME;
      corr_str[ONE] = ZERO;

      strcat(corr_str,test_str);

      length = strlen(corr_str);
      corr_str[length] = leuart->SIGFRAME;

      corr_str[length+ONE] = ZERO;

    //  char result_str[CHAR_SIZE];



      int str_len = strlen(input_str);

      // sending the input string to leuart_start()

      leuart_start(leuart, input_str, str_len);

      while(leuart_tx_busy(leuart));

      timer_delay(TIME_DELAY_LONG);

 //     strcpy(result_str,leuart_rx_state.string);  //copying into result from the buffer

  /*    for(size_t i = ZERO;i< strlen(corr_str);i++){


         EFM_ASSERT(corr_str[i] == result_str[i]); //checking
      }*/

      EFM_ASSERT(strcmp(leuart_rx_state.string, corr_str) == 0); // using the c library : strcmp to compare the result


      // have to clear the event as well

      EFM_ASSERT((leuart->STATUS & LEUART_STATUS_RXBLOCK)); // Check if RX is blocked
      leuart->CTRL &= ~LEUART_CTRL_LOOPBK; //disable loopback
      while(leuart->SYNCBUSY);

      remove_scheduled_event(rx_done_evt);










}
