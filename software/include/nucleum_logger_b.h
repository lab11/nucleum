#ifndef NUCLEUM_H
#define NUCLEUM_H

#include <stdbool.h>

#include "nrf_gpio.h"

#ifndef DEVICE_NAME
#define DEVICE_NAME    "nucleum_logger"
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
#define SPI_CS_PIN		8
#define SD_ENABLE_PIN	13		//power gates sd card
#define CD_PIN			24		//the card detect pin for the SD card
#define RV3049_CS		28
#define FRAM_HOLD		14
#define FRAM_CS			15
#define FRAM_WP			16

#define VOLTAGE_SENSE 	4
#define CURRENT_SENSE	3
#define SWITCH			9
#define SHDN		    10

bool platform_init ();


#endif
