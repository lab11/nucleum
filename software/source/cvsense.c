#include <stdint.h>
#include "spi_master_nucleum.h"
#include "adc.h"
#include "nrf_gpio.h"
#include "nucleum_logger.h"
#include "cvsense.h"

#define RESISTOR 0.12
#define SCALING (2.0/3.0)
#define REFERENCE 1.2
#define VOLTAGE 3.3			//probably should be sensed
#define GAIN	10.0
#define OFFSET 0.0147

#define CALIBRATION_OFFSET -0.02

static float vcc;

static void sense_vcc() {
	cvsense_open_circuit();

	adcConfig(ADC_CONFIG_RES_10bit, ADC_CONFIG_INPSEL_AnalogInputTwoThirdsPrescaling,  
                         ADC_CONFIG_REFSEL_VBG,                                  
                         1 << CURRENT_SENSE,                                          
                         ADC_CONFIG_EXTREFSEL_None);

	uint32_t vccSample = getSample();

	vcc = (((((vccSample/1024.0)*REFERENCE)*(1.0/SCALING))-(GAIN*OFFSET))*2.0);

}

static void disable_chip() { 
	spi_config(SPI_FREQUENCY_FREQUENCY_M1, SPI_CONFIG_ORDER_LsbFirst, SPI_CONFIG_CPOL_ActiveHigh, SPI_CONFIG_CPHA_Leading);

	nrf_gpio_pin_clear(PGA_CS);
	spi_write(0x98);
	nrf_gpio_pin_set(PGA_CS);

	for(volatile int j = 0; j < 1000; j++);
}

static uint8_t config_chip() {

	//set the gains and stuff over spi
	spi_config(SPI_FREQUENCY_FREQUENCY_M1, SPI_CONFIG_ORDER_LsbFirst, SPI_CONFIG_CPOL_ActiveHigh, SPI_CONFIG_CPHA_Leading);

	nrf_gpio_pin_clear(PGA_CS);
	spi_write(0x18);
	nrf_gpio_pin_set(PGA_CS);

	for(volatile int j = 0; j < 1000; j++);

	nrf_gpio_pin_clear(PGA_CS);
	spi_write(0x03);
	nrf_gpio_pin_set(PGA_CS);

	for(volatile int j = 0; j < 1000; j++);

	return 0;
}

uint8_t cvsense_init() {
	nrf_gpio_cfg_output(SWITCH);
	nrf_gpio_pin_clear(SWITCH);

	nrf_gpio_cfg_output(PGA_CS);
	nrf_gpio_pin_set(PGA_CS);

	config_chip();
	sense_vcc();

	disable_chip();

	return 0;
}


uint16_t cvsense_get_raw_voltage() {
    adcConfig(ADC_CONFIG_RES_10bit, ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling,  
                         ADC_CONFIG_REFSEL_VBG,                                  
                         1 << VOLTAGE_SENSE,                                          
                         ADC_CONFIG_EXTREFSEL_None);
	return getSample();
}

uint16_t cvsense_get_raw_current() {
	config_chip();

	adcConfig(ADC_CONFIG_RES_10bit, ADC_CONFIG_INPSEL_AnalogInputTwoThirdsPrescaling,  
                         ADC_CONFIG_REFSEL_VBG,                                  
                         1 << CURRENT_SENSE,                                          
                         ADC_CONFIG_EXTREFSEL_None);

	uint16_t curr = getSample();

	for(volatile int j = 0; j < 1000; j++);

	disable_chip();

	return curr;
}

void cvsense_short_circuit() {
	nrf_gpio_pin_set(SWITCH);
}

void cvsense_open_circuit() {
	nrf_gpio_pin_clear(SWITCH);
}

float cvsense_get_voltage() {
	cvsense_open_circuit();
	
	uint16_t voltage = cvsense_get_raw_voltage();

	float volt = (voltage/1024.0)*1.2*3*2;

	if(volt >= 0) {
		return volt;
	} else {
		return 0;
	}
}

float cvsense_get_current() {
	cvsense_short_circuit();

	for(volatile int i = 0; i < 1000000; i++);
	uint32_t current = cvsense_get_raw_current();

	cvsense_open_circuit();
	
	float curr = ((((current/1024.0)*REFERENCE)*(1.0/SCALING))-vcc/2.0-GAIN*OFFSET)/-GAIN/RESISTOR;

	if(curr >= 0) {
		return curr;
	} else {
		return 0;
	}
}

