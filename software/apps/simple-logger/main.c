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

#include "simple_logger.h"


ble_advdata_manuf_data_t mandata;
bool already_written = 0;

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
	simple_logger_log("%d,%d,%d,%d\n",1,2,3,4);
}


int main(void) {

    // Setup BLE
    simple_ble_init(&ble_config);

    // Setup our advertisement
    simple_adv_service_manuf_data(&TEST_SERVICE_UUID, &mandata);

	simple_timer_init();

	platform_init();

	simple_logger_init("t_file1.csv","a");
	simple_logger_log_header("%s,%s,%s,%s\n","Time","Voc","Isc","Estimated_Power");

	simple_timer_start (100, log_data);

    while (1) {
		simple_logger_update();
        //power_manage();
    }
}
