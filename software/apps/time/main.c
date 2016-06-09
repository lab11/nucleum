/* Send an advertisement periodically
 */

#include <stdbool.h>
#include <stdint.h>
#include "boards.h"
#include "ble_advdata.h"
#include "led.h"
#include "rv3049.h"
#include "nrf_delay.h"

#include "simple_ble.h"
#include "simple_adv.h"
#include "simple_timer.h"

static rv3049_time_t time;

// Intervals for advertising and connections                                    
static simple_ble_config_t ble_config = {                                       
    .platform_id       = 0x80,              // used as 4th octect in device BLE address
    .device_id         = DEVICE_ID_DEFAULT,                                     
    .adv_name          = DEVICE_NAME,       // used in advertisements if there is room
    .adv_interval      = MSEC_TO_UNITS(500, UNIT_0_625_MS),                     
    .min_conn_interval = MSEC_TO_UNITS(500, UNIT_1_25_MS),                      
    .max_conn_interval = MSEC_TO_UNITS(1000, UNIT_1_25_MS)                      
};

static ble_advdata_manuf_data_t ble_advdata; 

static void handler(void* p_context) {
	rv3049_read_time(&time);
	time.hours -=2000;
	time.hours <<= 8;

	ble_advdata.data.p_data = (uint8_t *)(&time);
	ble_advdata.data.size = sizeof(time);

	simple_adv_manuf_data(&ble_advdata);
}

int main(void)
{

	simple_ble_init(&ble_config);

	simple_timer_init();

	rv3049_init();

	rv3049_read_time(&time);
	time.hours -=2000;
	time.hours <<= 8;

	ble_advdata.data.p_data = (uint8_t *)(&time);
	ble_advdata.data.size = sizeof(time);

	simple_adv_manuf_data(&ble_advdata);

	simple_timer_start(2000, handler);

    while (1) { 
		power_manage();
    }
}

