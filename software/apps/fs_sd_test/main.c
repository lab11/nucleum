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

//filesystem files
#include "mem-ffs.h"

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

void fs_heartbeat(void* p_context) {
	if(ffs_10ms_timer) {
		ffs_10ms_timer--;
	}
}

// API calls.
int main(void) {

    // Setup BLE
    simple_ble_init(&ble_config);

    // Setup our advertisement
    simple_adv_service_manuf_data(&TEST_SERVICE_UUID, &mandata);

	simple_timer_init();

	simple_timer_start (10, fs_heartbeat);

	platform_init();

	ffs_init();

	//lets test this file system

    while (1) {
		ffs_process();

		if(ffs_is_card_available() && !already_written) {
			FFS_FILE* fpoint = ffs_fopen("test_directory/test_file.txt","w");
			if(fpoint) {
				if(ffs_fputs("writing to my test file in a dir",fpoint) >= 0) {
					ffs_fflush(fpoint);
					ffs_fclose(fpoint);
					led_on(LED_1);
					already_written = 1;
				} else {

				}
			}
		}

        //power_manage();
    }
}
