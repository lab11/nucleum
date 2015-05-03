#include "fm25l04b.h"
#include "nucleum.h"
#include "nrf_gpio.h"
#include "spi_master_nucleum.h"

void fm25l04b_init() {

	spi_config(SPI_FREQUENCY_FREQUENCY_M1, SPI_CONFIG_ORDER_MsbFirst, SPI_CONFIG_CPOL_ActiveHigh, SPI_CONFIG_CPHA_Leading);

  /* Set the HOLD_N and WP_N pins to outputs and high */
	nrf_gpio_cfg_output(FRAM_CS);
	nrf_gpio_cfg_output(FRAM_HOLD);
	nrf_gpio_cfg_output(FRAM_WP);
	nrf_gpio_pin_set(FRAM_CS);
	nrf_gpio_pin_set(FRAM_HOLD);
	nrf_gpio_pin_set(FRAM_WP);
}

/**
 * \brief         Read from the FRAM chip.
 * \param address The index of the byte to start reading from.
 * \param len     The number of bytes to read.
 * \param buf     A buffer to put the return data in.
 * \return        0 on success, -1 on error
 *
 *                Reads len bytes from the FRAM chip starting at address.
 */
int fm25l04b_read(uint16_t address, uint16_t len, uint8_t *buf)  {
  
  uint16_t i;
  uint16_t current_address = address;


  spi_config(SPI_FREQUENCY_FREQUENCY_M1, SPI_CONFIG_ORDER_MsbFirst, SPI_CONFIG_CPOL_ActiveHigh, SPI_CONFIG_CPHA_Leading);
  nrf_gpio_pin_clear(FRAM_CS);

  /* Send the READ command and the address to the FRAM */
  spi_write(FM25L04B_ADD_ADDRESS_BIT(current_address, FM25L04B_READ_COMMAND));
  spi_write(current_address & 0xFF);

  for (i=0; i<len; i++) {
    spi_read(&buf[i]);
  }

  nrf_gpio_pin_set(FRAM_CS);

  return 0;
}

/**
 * \brief         Write to the FRAM chip.
 * \param address The index of the byte to start writing to.
 * \param len     The number of bytes to write.
 * \param buf     A buffer of values to write.
 * \return        0 on success, -1 on error
 *
 *                Writes len bytes to the FRAM chip starting at address.
 */
int
fm25l04b_write(uint16_t address, uint16_t len, uint8_t *buf)
{
  uint16_t i;

  spi_config(SPI_FREQUENCY_FREQUENCY_M1, SPI_CONFIG_ORDER_MsbFirst, SPI_CONFIG_CPOL_ActiveHigh, SPI_CONFIG_CPHA_Leading);

  nrf_gpio_pin_clear(FRAM_CS);

  /* Send the WRITE ENABLE command to allow writing to the FRAM */
  spi_write(FM25L04B_WRITE_ENABLE_COMMAND);

  nrf_gpio_pin_set(FRAM_CS);
  nrf_gpio_pin_clear(FRAM_CS);

  /* Send the WRITE command and the address to the FRAM */
  spi_write(FM25L04B_ADD_ADDRESS_BIT(address, FM25L04B_WRITE_COMMAND));
  spi_write(address & 0xFF);

  /* Send the data to write */
  for(i=0; i<len; i++) {
    spi_write(buf[i]);
  }

  nrf_gpio_pin_set(FRAM_CS);

  return 0;
}
