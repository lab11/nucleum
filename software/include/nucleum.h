#ifndef NUCLEUM_H
#define NUCLEUM_H

#include <stdbool.h>

#include "nrf_gpio.h"

#ifndef DEVICE_NAME
#define DEVICE_NAME    "nucleum"
#endif

#define LED_START      18
#define LED_0          18
#define LED_1          19
#define LED_2          20
#define LED_STOP       20


// This pin is mapped to the FTDI chip to all the device to enter the
// bootloader mode.
#define BOOTLOADER_CTRL_PIN  13
#define BOOTLOADER_CTRL_PULL NRF_GPIO_PIN_PULLUP


#define I2C_SCL_PIN   1
#define I2C_SDA_PIN   7

#define RX_PIN_NUMBER  11
#define TX_PIN_NUMBER  12
#define CTS_PIN_NUMBER 0
#define RTS_PIN_NUMBER 0
#define HWFC           false

//#define SPIS_MISO_PIN  29    // SPI MISO signal.
//#define SPIS_CSN_PIN   1     // SPI CSN signal.
//#define SPIS_MOSI_PIN  11    // SPI MOSI signal.
//#define SPIS_SCK_PIN   9     // SPI SCK signal.

#define SPIM0_SCK_PIN       0     /**< SPI clock GPIO pin number. */
#define SPIM0_MOSI_PIN      30     /**< SPI Master Out Slave In GPIO pin number. */
#define SPIM0_MISO_PIN      29     /**< SPI Master In Slave Out GPIO pin number. */
//#define SPIM0_SS_PIN        1u     /**< SPI Slave Select GPIO pin number. */

//#define SPIM1_SCK_PIN       16u     /**< SPI clock GPIO pin number. */
//#define SPIM1_MOSI_PIN      18u     /**< SPI Master Out Slave In GPIO pin number. */
//#define SPIM1_MISO_PIN      17u     /**< SPI Master In Slave Out GPIO pin number. */
//#define SPIM1_SS_PIN        19u     /**< SPI Slave Select GPIO pin number. */

// serialization APPLICATION board


bool platform_init ();


#endif
