// Global libraries
#include <stdint.h>

// Nordic libraries
#include "ble_advdata.h"

// nrf5x-base libraries
#include "simple_ble.h"
#include "simple_adv.h"
#include "simple_timer.h"
#include "nrf_gpio.h"
#include "app_gpiote.h"

// Define constants about this beacon.
#define DEVICE_NAME "CAP_TOUCH"

// Manufacturer specific data setup
#define UMICH_COMPANY_IDENTIFIER 0x02E0

#define OUT_PIN 21
#define IN_PIN 24
#define LED 18
#define LED2 19
#define SIGNAL 2

#define PIR 5

volatile uint32_t touchCount;
volatile uint32_t touchTimer = 0;
volatile uint32_t PIRTimer = 0;
ble_advdata_manuf_data_t mandata;

app_gpiote_user_id_t gpiote_user;

// Intervals for advertising and connections
static simple_ble_config_t ble_config = {
    .platform_id       = 0x00,              // used as 4th octect in device BLE address
    .device_id         = DEVICE_ID_DEFAULT,
    .adv_name          = DEVICE_NAME,       // used in advertisements if there is room
    .adv_interval      = MSEC_TO_UNITS(500, UNIT_0_625_MS),
    .min_conn_interval = MSEC_TO_UNITS(500, UNIT_1_25_MS),
    .max_conn_interval = MSEC_TO_UNITS(1000, UNIT_1_25_MS)
};

static ble_uuid_t TEST_SERVICE_UUID = {0xBEAF, BLE_UUID_TYPE_BLE};

void test_touch (void *p_context) {	

	/*if(nrf_gpio_pin_read(PIR)) {
		PIRTimer = 200;
	} else {
		if(PIRTimer != 0) {
			PIRTimer--;
		}
	}*/
	if(PIRTimer != 0) {
		PIRTimer--;
	}

	if(PIRTimer) {
		nrf_gpio_pin_clear(LED2);
	} else {
		nrf_gpio_pin_set(LED2);
	}

	if(!touchTimer) {

		volatile uint32_t i = 0;

		nrf_gpio_pin_set(OUT_PIN);

		while(!nrf_gpio_pin_read(IN_PIN)) {
			i++;
		}

		if(i > 0x70 && !PIRTimer) {
			nrf_gpio_pin_clear(LED);
			nrf_gpio_pin_set(SIGNAL);
			for(volatile int j = 0; j < 100000; j++);
			nrf_gpio_pin_clear(SIGNAL);
			touchTimer = 400;
		} else {
			nrf_gpio_pin_set(LED);
		}

		nrf_gpio_pin_clear(OUT_PIN);
	} else {

		volatile uint32_t i = 0;

		nrf_gpio_pin_set(OUT_PIN);

		while(!nrf_gpio_pin_read(IN_PIN)) {
			i++;
		}

		if(i > 0x70) {
			touchTimer = 200;
		} else {
			touchTimer--;
		}

		nrf_gpio_pin_clear(OUT_PIN);

	}
}

void pir_interrupt_handler(uint32_t pins_l2h, uint32_t pins_h2l) {
	PIRTimer = 200;	
}


// main is essentially two library calls to setup all of the Nordic SDK
// API calls.
int main(void) {

    // Setup BLE
    simple_ble_init(&ble_config);


	//configure the capacitive touch pins
	nrf_gpio_cfg_output(OUT_PIN);
	nrf_gpio_cfg_input(IN_PIN, NRF_GPIO_PIN_NOPULL);

	//configure the LEDS
	nrf_gpio_cfg_output(LED);
	nrf_gpio_pin_set(LED);
	nrf_gpio_cfg_output(LED2);
	nrf_gpio_pin_set(LED2);

	//configure the output to the door
	nrf_gpio_cfg_output(SIGNAL);
	nrf_gpio_pin_clear(SIGNAL);
	
	//config as interrupt
	APP_GPIOTE_INIT(3);

	app_gpiote_user_register(&gpiote_user,
							1 << PIR,
							0x0,
							pir_interrupt_handler);

	nrf_gpio_cfg_input(PIR, NRF_GPIO_PIN_NOPULL);

	app_gpiote_user_enable(gpiote_user);

    // Setup our advertisement
    simple_adv_service_manuf_data(&TEST_SERVICE_UUID, &mandata);

	simple_timer_init();

	simple_timer_start (5, test_touch);

    while (1) {
        power_manage();
    }
}
