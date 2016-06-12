// Global libraries
#include <stdint.h>

// Nordic libraries
#include "ble_advdata.h"

// nrf5x-base libraries
#include "simple_ble.h"
#include "simple_adv.h"
#include "simple_timer.h"

#include "nrf_gpio.h"

// Define constants about this beacon.
#define DEVICE_NAME "CAP_TOUCH"

// Manufacturer specific data setup
#define UMICH_COMPANY_IDENTIFIER 0x02E0

#define OUT_PIN 21
#define IN_PIN 24
#define LED 18
#define SIGNAL 2

uint32_t touchCount;
uint32_t touchTimer = 0;
ble_advdata_manuf_data_t mandata;

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
	if(!touchTimer) {

		volatile uint32_t i = 0;

		nrf_gpio_pin_set(OUT_PIN);

		while(!nrf_gpio_pin_read(IN_PIN)) {
			i++;
		}

		if(i > 0x70) {
			nrf_gpio_pin_clear(LED);
			nrf_gpio_pin_set(SIGNAL);
			for(volatile int j = 0; j < 1000; j++);
			nrf_gpio_pin_clear(SIGNAL);
			touchTimer = 100;
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
			touchTimer = 50;
		} else {
			touchTimer--;
		}

		nrf_gpio_pin_clear(OUT_PIN);

	}
}


// main is essentially two library calls to setup all of the Nordic SDK
// API calls.
int main(void) {

    // Setup BLE
    simple_ble_init(&ble_config);

    mandata.company_identifier = UMICH_COMPANY_IDENTIFIER;
    mandata.data.p_data = &touchCount;
    mandata.data.size   = 4;

	nrf_gpio_cfg_output(OUT_PIN);
	nrf_gpio_cfg_input(IN_PIN, NRF_GPIO_PIN_NOPULL);

	nrf_gpio_cfg_output(LED);
	nrf_gpio_pin_set(LED);

	nrf_gpio_cfg_output(SIGNAL);
	nrf_gpio_pin_clear(SIGNAL);


    // Setup our advertisement
    simple_adv_service_manuf_data(&TEST_SERVICE_UUID, &mandata);

	simple_timer_init();

	simple_timer_start (20, test_touch);

    while (1) {
        power_manage();
    }
}
