#ifndef BRD_CONFIG_HG
#define BRD_CONFIG_HG

//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_gpio.h"
#include "em_cmu.h"

//***********************************************************************************
// defined files
//***********************************************************************************

// LED 0 pin is
#define LED_RED_PORT       gpioPortD
#define LED_RED_PIN        8u
#define LED_RED_DEFAULT    false   // Default false (0) = off, true (1) = on
#define LED_RED_GPIOMODE   gpioModePushPull

// LED 1 pin is
#define LED_GREEN_PORT       gpioPortD
#define LED_GREEN_PIN        9u
#define LED_GREEN_DEFAULT    false // Default false (0) = off, true (1) = on
#define LED_GREEN_GPIOMODE   gpioModePushPull

#define MCU_HFXO_FREQ			cmuHFRCOFreq_26M0Hz


// GPIO pin setup
//#define STRONG_DRIVE

#ifdef STRONG_DRIVE
	#define LED_RED_DRIVE_STRENGTH		gpioDriveStrengthStrongAlternateStrong
	#define LED_GREEN_DRIVE_STRENGTH	gpioDriveStrengthStrongAlternateStrong
#else
	#define LED_RED_DRIVE_STRENGTH		gpioDriveStrengthWeakAlternateWeak
	#define LED_GREEN_DRIVE_STRENGTH	gpioDriveStrengthWeakAlternateWeak
#endif

// LETIMER PWM Configuration

#define   PWM_ROUTE_0     LETIMER_ROUTELOC0_OUT0LOC_LOC17
#define   PWM_ROUTE_1     LETIMER_ROUTELOC0_OUT1LOC_LOC16

//ThunderboardRGB LED pin definitions
#define RGB_ENABLE_PORT   gpioPortJ
#define RGB_ENABLE_PIN    14
#define RGB0_PORT         gpioPortI
#define RGB0_PIN          0
#define RGB1_PORT         gpioPortI
#define RGB1_PIN          1
#define RGB2_PORT         gpioPortI
#define RGB2_PIN          2
#define RGB3_PORT         gpioPortI
#define RGB3_PIN          3
#define RGB_RED_PORT      gpioPortD
#define RGB_RED_PIN       11
#define RGB_GREEN_PORT    gpioPortD
#define RGB_GREEN_PIN     12
#define RGB_BLUE_PORT     gpioPortD
#define RGB_BLUE_PIN      13
#define RGB_DEFAULT_OFF     false
#define COLOR_DEFAULT_OFF   false
#define RED_RGB_LOC       TIMER_ROUTELOC0_CC0LOC_LOC19
#define GREEN_RGB_LOC     TIMER_ROUTELOC0_CC1LOC_LOC19
#define BLUE_RGB_LOC      TIMER_ROUTELOC0_CC2LOC_LOC19

#define SI1133_SCL_PORT                  gpioPortC
#define SI1133_SCL_PIN                    4u
#define SI1133_SDA_PORT                  gpioPortC
#define SI1133_SDA_PIN                    5u
#define SI1133_SENSOR_EN_PORT            gpioPortF
#define SI1133_SENSOR_EN_PIN               9u

#define SI1133_DRIVE_STRENGTH          gpioDriveStrengthWeakAlternateWeak
#define SI1133_GPIO_MODE               gpioModePushPull
#define SI1133_DEFAULT                 true
#define SI1133_I2C_GPIO_MODE           gpioModeWiredAnd
#define SI1133_I2C_DEFAULT             true


#define   SCL_ROUTE     I2C_ROUTELOC0_SCLLOC_LOC17
#define   SDA_ROUTE     I2C_ROUTELOC0_SDALOC_LOC17

#define   LEUART_TX_PORT      gpioPortF
#define   LEUART_TX_PIN       3u
#define   LEUART_RX_PORT      gpioPortF
#define   LEUART_RX_PIN       4u

#define   LEUART_TX_STRENGTH    gpioDriveStrengthStrongAlternateWeak
#define   LEUART_TX_GPIOMODE    gpioModePushPull
#define   LEUART_RX_GPIOMODE    gpioModeInput

#define   LEUART_TR_DEFAULT   true

#define HM10_LEUART0      LEUART0
#define HM10_BAUDRATE     9600
#define HM10_DATABITS     leuartDatabits8
#define HM10_ENABLE       leuartEnable
#define HM10_PARITY       leuartNoParity
#define HM10_REFFREQ      0
#define HM10_STOPBITS     leuartStopbits1
#define LEUART0_TX_ROUTE  LEUART_ROUTELOC0_TXLOC_LOC27
#define LEUART0_RX_ROUTE  LEUART_ROUTELOC0_RXLOC_LOC27

//***********************************************************************************
// function prototypes
//***********************************************************************************

#endif
