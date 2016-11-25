#include <stdint.h>
#include "adc.h"
#include "nrf_gpio.h"
#include "nucleum_logger_b.h"
#include "cvsense.h"

#define RESISTOR 0.12
#define SCALING (1.0/3.0)
//the voltage divider values
//low_res/(low_res+high_res)
#define DIVIDER (24.0/200.0)
#define REFERENCE 1.2
#define GAIN	25


static void disable_chip() {
    //pull SHDN to GND
	nrf_gpio_pin_clear(SHDN);
}

static uint8_t config_chip() {
    //pull SHDN up
	nrf_gpio_pin_set(SHDN);

    return 0;
}

uint8_t cvsense_init() {
	nrf_gpio_cfg_output(SWITCH);
	nrf_gpio_pin_clear(SWITCH);

	nrf_gpio_cfg_output(SHDN);
	nrf_gpio_pin_set(SHDN);

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

	adcConfig(ADC_CONFIG_RES_10bit, ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling,
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

	float volt = (((voltage/1024.0)*REFERENCE)*(1.0/SCALING))*(1.0/DIVIDER);

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

	float curr = ((((current/1024.0)*REFERENCE)*(1.0/SCALING)))/GAIN/RESISTOR;

	if(curr >= 0) {
		return curr;
	} else {
		return 0;
	}
}

