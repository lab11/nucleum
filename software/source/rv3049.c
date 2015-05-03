#include "spi_master_nucleum.h"
#include "nrf_gpio.h"
#include "nucleum.h"
#include "rv3049.h"

// Check that the application was compiled with the RTC constants for
// initialization
#ifndef RTC_SECONDS
#error "To use the RTC you must compile with RTC_ initial values."
#endif

uint8_t rv3049_binary_to_bcd (uint8_t binary) {
  uint8_t out = 0;

  if (binary >= 40) {
    out |= 0x40;
    binary -= 40;
  }
  if (binary >= 20) {
    out |= 0x20;
    binary -= 20;
  }
  if (binary >= 10) {
    out |= 0x10;
    binary -= 10;
  }
  out |= binary;
  return out;
}

void rv3049_init() {

	nrf_gpio_cfg_output(RV3049_CS);
	nrf_gpio_pin_set(RV3049_CS);

  	spi_config(SPI_FREQUENCY_FREQUENCY_M1, SPI_CONFIG_ORDER_LsbFirst, SPI_CONFIG_CPOL_ActiveHigh, SPI_CONFIG_CPHA_Leading);

  // Write the initial values
  {
    rv3049_time_t start_time = {RTC_SECONDS, RTC_MINUTES, RTC_HOURS,
                                RTC_DAYS,    RTC_WEEKDAY, RTC_MONTH,
                                RTC_YEAR};
    rv3049_set_time(&start_time);
  }
}

int rv3049_read_time(rv3049_time_t* time) {
  	uint8_t buf[8];
  	int i;

  	spi_config(SPI_FREQUENCY_FREQUENCY_M1, SPI_CONFIG_ORDER_MsbFirst, SPI_CONFIG_CPOL_ActiveHigh, SPI_CONFIG_CPHA_Leading);

	nrf_gpio_pin_set(RV3049_CS);

  	// Tell the RTC we want to read the clock
  	spi_write(RV3049_SET_READ_BIT(RV3049_PAGE_ADDR_CLOCK));

  	// Read a null byte here. Not exactly sure why.
  	//spi_read(&buf[0]);

  	// Then actually read the clock
  	for (i=0; i<RV3049_READ_LEN_TIME; i++) {
    	spi_read(&buf[i]);
  	}

	nrf_gpio_pin_clear(RV3049_CS);

  	// Convert the values
  	time->seconds = BCD_TO_BINARY(buf[0]);
  	time->minutes = BCD_TO_BINARY(buf[1]);
  	time->hours   = BCD_TO_BINARY((buf[2])&0x3F);
  	time->days    = BCD_TO_BINARY(buf[3]);
  	time->weekday = buf[4];
  	time->month   = buf[5];
  	time->year    = BCD_TO_BINARY(buf[6])+2000;

  return 0;
}

int rv3049_set_time(rv3049_time_t* time) {
  	uint8_t buf[8];
  	int i;

  	buf[0] = rv3049_binary_to_bcd(time->seconds);
  	buf[1] = rv3049_binary_to_bcd(time->minutes);
  	buf[2] = rv3049_binary_to_bcd(time->hours); // 24 hour mode
  	buf[3] = rv3049_binary_to_bcd(time->days);
  	buf[4] = time->weekday;
  	buf[5] = time->month;
  	buf[6] = rv3049_binary_to_bcd(time->year - 2000);

  	spi_config(SPI_FREQUENCY_FREQUENCY_M1, SPI_CONFIG_ORDER_MsbFirst, SPI_CONFIG_CPOL_ActiveHigh, SPI_CONFIG_CPHA_Leading);

	nrf_gpio_pin_set(RV3049_CS);

  	// Signal a write to the clock
  	spi_write(RV3049_SET_WRITE_BIT(RV3049_PAGE_ADDR_CLOCK));

  	// Write the clock values
  	for (i=0; i<RV3049_WRITE_LEN_TIME; i++) {
    	spi_write(buf[i]);
  	}

	nrf_gpio_pin_clear(RV3049_CS);

  	return 0;
}
