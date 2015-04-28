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

#define SPI_SCK_PIN     0     /**< SPI clock GPIO pin number. */
#define SPI_MOSI_PIN    30     /**< SPI Master Out Slave In GPIO pin number. */
#define SPI_MISO_PIN    29     /**< SPI Master In Slave Out GPIO pin number. */
#define RV3049_CS		28		
#define FRAM_HOLD		14
#define FRAM_CS			15


bool platform_init ();


#endif
