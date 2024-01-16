//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef APP_HG
#define APP_HG

/* System include statements */


/* Silicon Labs include statements */
#include "em_cmu.h"
#include "em_assert.h"

/* The developer's include statements */
#include "cmu.h"
#include "gpio.h"
#include "letimer.h"
#include "brd_config.h"
#include "scheduler.h"
#include "LEDs_thunderboard.h"
#include "SI1133.h"
#include "HW_delay.h"
#include "ble.h"


//***********************************************************************************
// defined files
//***********************************************************************************
#define   PWM_PER         2.0   // PWM period in seconds
#define   PWM_ACT_PER     0.002 // PWM active period in seconds

#define LETIMER0_COMP0_CB        0x00000001   //0b0001
#define LETIMER0_COMP1_CB        0x00000002   //0b0010
#define LETIMER0_UF_CB           0x00000004      //0b0100
#define SI1133_LIGHT_READ_CB     0X00000008
#define SI1133_REG_READ_CB       0X00000010
#define BOOT_UP_CB               0x00000020
#define BLE_TX_DONE_CB           0x00000040
#define BLE_RX_DONE_CB           0x00000080
#define CHECK_VAL                51
#define SENSE_VAL                20
#define SYSTEM_BLOCK_EM          EM3
#define DELAY                    2000
#define CHAR_SEND                25
#define ADD_THREE                3
#define ADD_ONE                  1

//#define BLE_TEST_ENABLED
//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void app_peripheral_setup(void);
void scheduled_letimer0_uf_cb (void);
void scheduled_letimer0_comp0_cb (void);
void scheduled_letimer0_comp1_cb (void);
void scheduled_si1133_cb(void);
void si1133_white_op(void);
void scheduled_boot_up_cb(void);
void scheduled_boot_up_cb(void);
void scheduled_ble_tx_done_cb(void);

#endif
