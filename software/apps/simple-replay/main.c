// Global libraries
#include <stdint.h>

// Nordic libraries
#include "ble_advdata.h"

// nrf5x-base libraries
#include "simple_ble.h"
#include "simple_adv.h"
#include "multi_adv.h"
#include "eddystone.h"
#include "simple_timer.h"
#include "nrf_gpio.h"
#include "led.h"
#include "nucleum_logger.h"
#include "replay.h"
#include "rv3049.h"
#include "simple_logger.h"

#define ADV_SWITCH_MS 1000
#define PHYSWEB_URL "goo.gl/qtn9V9"
#define CV_SENSE_SERVICE 0x17
#define UMICH_COMPANY_IDENTIFIER 0x02E0

typedef struct {
	float volt;
	float curr;
	float pow;
} __attribute__((packed)) cv_data_t;

cv_data_t cv_data;
uint8_t mdata[1 + sizeof(cv_data_t)];

volatile uint8_t new_data = 0;

// Intervals for advertising and connections
static simple_ble_config_t ble_config = {
    .platform_id       = 0x00,              // used as 4th octect in device BLE address
    .device_id         = DEVICE_ID_DEFAULT,
    .adv_name          = "Solar",       // used in advertisements if there is room
    .adv_interval      = MSEC_TO_UNITS(500, UNIT_0_625_MS),
    .min_conn_interval = MSEC_TO_UNITS(500, UNIT_1_25_MS),
    .max_conn_interval = MSEC_TO_UNITS(1000, UNIT_1_25_MS)
};

static void adv_config_eddystone () {
	eddystone_adv(PHYSWEB_URL, NULL);
}

static void adv_config_data() {
	ble_advdata_manuf_data_t mandata;

	mdata[0] = CV_SENSE_SERVICE;

	memcpy(mdata+1, (uint8_t*) &cv_data, sizeof(cv_data_t));

	mandata.company_identifier = UMICH_COMPANY_IDENTIFIER;
	mandata.data.p_data = mdata;
	mandata.data.size = 1 + sizeof(cv_data_t);
		
	simple_adv_manuf_data(&mandata);
}

int main(void) {
	
	//initalize peripherals
	//rv3049_init();
	platform_init();
	replay_init();

    // Setup BLE
    simple_ble_init(&ble_config);

	multi_adv_init(ADV_SWITCH_MS);
	multi_adv_register_config(adv_config_eddystone);
	multi_adv_register_config(adv_config_data);

	multi_adv_start();

	replay_set_voltage(10);
	replay_set_current(10);

    while (1) {
        power_manage();
    }
}
