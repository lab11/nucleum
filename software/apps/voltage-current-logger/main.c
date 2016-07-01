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
#include "cvsense.h"
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

void log_data(void* p_context) {
	new_data = 1;
}

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

static void blink_red() {
	for(uint8_t i = 0; i < 200; i++) {
		led_on(LED_0);
		led_off(LED_0);
	}
}

static void blink_blue() {
	for(uint8_t i = 0; i < 200; i++) {
		led_on(LED_1);
		led_off(LED_1);
	}
}

static void blink_green() {
	for(uint8_t i = 0; i < 200; i++) {
		led_on(LED_2);
		led_off(LED_2);
	}
}

int main(void) {
	
	//initalize peripherals
	cvsense_init();
	rv3049_init();
	platform_init();

    // Setup BLE
    simple_ble_init(&ble_config);

	multi_adv_init(ADV_SWITCH_MS);
	multi_adv_register_config(adv_config_eddystone);
	multi_adv_register_config(adv_config_data);

	//setup timer
	simple_timer_init();

	//setup logger
	simple_logger_init("stest2.csv","a");
	simple_logger_log_header("%s,%s,%s,%s\n","Time","Voc","Isc","Estimated_Power");

	//start the timer
	simple_timer_start (10000, log_data);

	multi_adv_start();

    while (1) {

		if(new_data) {

			rv3049_time_t time;
			rv3049_read_time(&time);

			float voltage = cvsense_get_voltage();
			cv_data.volt = voltage;

			float current = cvsense_get_current();
			cv_data.curr = current;

			cv_data.pow = current*voltage*0.7;

			blink_blue();
			if(simple_logger_log("06/%d %d:%d:%d,%f,%f,%f\n",time.days,time.hours,time.minutes,time.seconds,
																voltage,current,voltage*current*0.7)) {
				blink_red();
			} else {
				blink_green();
			}

			new_data = 0;
		}

        power_manage();
    }
}
