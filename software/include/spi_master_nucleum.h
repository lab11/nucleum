#ifndef SPI_MASTER_H
#define SPI_MASTER_H

#include <stdint.h>

//use SPI_CONFIG_ORDER_LsbFirest, SPI_CONFIG_CPOL_ActiveHigh, SPI_CONFIG_CPHA_Leading
void spi_config(uint32_t SPI_Freq, uint8_t SPI_ORDER, uint8_t SPI_CPOL, uint8_t SPI_CPHA);
void spi_write(uint8_t buf);
void spi_read(uint8_t* buf);

#endif
