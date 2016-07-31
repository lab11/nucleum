#include <stdint.h>
#include "spi_master_nucleum.h"
#include "nrf_gpio.h"
#include "nucleum_logger.h"
#include "replay.h"

#define MAX_VOLTAGE 20

#define V_TAP1_CS 3
#define V_TAP2_CS 2

#define C_TAP_CS 9

uint8_t replay_init() {

	nrf_gpio_cfg_output(V_TAP1_CS);
	nrf_gpio_pin_set(V_TAP1_CS);
	nrf_gpio_cfg_output(V_TAP2_CS);
	nrf_gpio_pin_set(V_TAP2_CS);
	nrf_gpio_cfg_output(C_TAP_CS);
	nrf_gpio_pin_set(C_TAP_CS);

	return 0;
}

void replay_set_voltage(float voltage) {

	//calculate the position of the taps against the max voltage
	float ratio = voltage/MAX_VOLTAGE;
	
	uint16_t taps = (uint16_t)(ratio*512.0);

	uint8_t tap1 = taps/2;
	uint8_t tap2 = taps-tap1;

	spi_config(SPI_FREQUENCY_FREQUENCY_M1, SPI_CONFIG_ORDER_MsbFirst, SPI_CONFIG_CPOL_ActiveHigh, SPI_CONFIG_CPHA_Leading);

	//set the first pot
	nrf_gpio_pin_clear(V_TAP1_CS);
	spi_write(0x00);
	spi_write(128);
	nrf_gpio_pin_set(V_TAP1_CS);
	for(volatile int i = 0; i < 1000; i++);

	//set the second pot
	nrf_gpio_pin_clear(V_TAP2_CS);
	spi_write(0x00);
	spi_write(128);
	nrf_gpio_pin_set(V_TAP2_CS);
	for(volatile int i = 0; i < 1000; i++);
}

void replay_set_current(float current) {

	spi_config(SPI_FREQUENCY_FREQUENCY_M1, SPI_CONFIG_ORDER_MsbFirst, SPI_CONFIG_CPOL_ActiveHigh, SPI_CONFIG_CPHA_Leading);

	nrf_gpio_pin_clear(C_TAP_CS);
	spi_write(0x00);
	spi_write(0x80);
	spi_write(0x00);
	nrf_gpio_pin_set(C_TAP_CS);
}

