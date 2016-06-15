// Global libraries
#include <stdint.h>

// Nordic libraries
#include "ble_advdata.h"

// nrf5x-base libraries
#include "simple_ble.h"
#include "simple_adv.h"
#include "simple_timer.h"
#include "nrf_gpio.h"
#include "led.h"
#include "nucleum_logger.h"
#include "cvsense.h"
#include "rv3049.h"

#include "simple_logger.h"


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

void log_data(void* p_context) {

	rv3049_time_t time;
	rv3049_read_time(&time);

	float voltage = cvsense_get_voltage();

	cvsense_short_circuit();
	for(volatile int i = 0; i < 1000000; i++);
	float current = cvsense_get_current();

	cvsense_open_circuit();

	simple_logger_log("06/%d %d:%d:%d,%f,%f,%f\n",time.days,time.hours,time.minutes,time.seconds,
													voltage,current,voltage*current*0.7);
}


int main(void) {
	
	//initalize peripherals
	cvsense_init();
	rv3049_init();
	platform_init();

    // Setup BLE
    simple_ble_init(&ble_config);
    simple_adv_service_manuf_data(&TEST_SERVICE_UUID, &mandata);

	//setup timer
	simple_timer_init();

	//setup logger
	simple_logger_init("stest2.csv","a");
	simple_logger_log_header("%s,%s,%s,%s\n","Time","Voc","Isc","Estimated_Power");

	//start the timer
	simple_timer_start (10000, log_data);

    while (1) {
		simple_logger_update();
        //power_manage();
    }
}
