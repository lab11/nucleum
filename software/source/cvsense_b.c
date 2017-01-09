#include <stdint.h>
#include "adc.h"
#include "nrf_gpio.h"
#include "nucleum_logger_b.h"
#include "cvsense.h"

//The actual value of the resistor is .06
//.0725 is a value derived from a guess-and-check calibration
//The boards have an additional systematic resistance in their measurement circuit
#define RESISTOR 0.0725
#define SCALING (1.0/3.0)
//the voltage divider values
//low_res/(low_res+high_res)
#define DIVIDER (24.0/200.0)
#define REFERENCE 1.2
#define GAIN	50
#define OFFSET 0.0


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

//	for(volatile int j = 0; j < 4000000; j++);

	cvsense_short_circuit();

	for(volatile int j = 0; j < 1000000; j++);

	adcConfig(ADC_CONFIG_RES_10bit, ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling,
                         ADC_CONFIG_REFSEL_VBG,
                         1 << CURRENT_SENSE,
                         ADC_CONFIG_EXTREFSEL_None);

	uint16_t curr = getSample();

	cvsense_open_circuit();

	for(volatile int j = 0; j < 1000; j++);

//	disable_chip();

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

	uint32_t current = cvsense_get_raw_current();


	float curr = ((((current/1024.0)*REFERENCE)*(1.0/SCALING))-OFFSET)/GAIN/RESISTOR;

	if(curr >= 0) {
		return curr;
	} else {
		return 0;
	}
}

