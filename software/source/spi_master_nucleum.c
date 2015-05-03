#include <stdint.h>
#include <stdbool.h>
#include "nucleum.h"
#include "spi_master_nucleum.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "nrf_soc.h"
#include "nrf51_bitfields.h"

#define NRF_SPI NRF_SPI1

void spi_config(uint32_t SPI_Freq, uint8_t SPI_ORDER, uint8_t SPI_CPOL, uint8_t SPI_CPHA) {
	NRF_SPI->PSELSCK 	= 	SPI_SCK_PIN;
	NRF_SPI->PSELMOSI 	= 	SPI_MOSI_PIN;
	NRF_SPI->PSELMISO 	= 	SPI_MISO_PIN;
	NRF_SPI->FREQUENCY	= 	SPI_Freq;
	NRF_SPI->CONFIG	= 	(uint32_t)(SPI_CPHA << SPI_CONFIG_CPHA_Pos) |
						(SPI_CPOL << SPI_CONFIG_CPOL_Pos) |
						(SPI_ORDER << SPI_CONFIG_ORDER_Pos);

	NRF_SPI->ENABLE = (SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);
	NRF_SPI->EVENTS_READY = 0;
}

void spi_write(uint8_t buf) {
	//clear the ready event

	NRF_SPI->TXD = buf;

	//wait until byte has transmitted
	while(NRF_SPI->EVENTS_READY == 0);

	uint8_t throw = NRF_SPI->RXD;

	NRF_SPI->EVENTS_READY = 0;
}

void spi_read(uint8_t* buf) {

	//clear ready event
	NRF_SPI->EVENTS_READY = 0;

	NRF_SPI->TXD = 0x00;

	//wait until byte has been received
	while(NRF_SPI->EVENTS_READY == 0);

	buf[0] = NRF_SPI->RXD;
}

