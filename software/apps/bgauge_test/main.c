/*
 * Send an advertisement periodically while scanning for advertisements.
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_gpio.h"
#include "ble_advdata.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "ble_debug_assert_handler.h"
#include "led.h"
#include "nrf_drv_twi.h"
#include "multi_adv.h"
#include "eddystone.h"
#include "LTC2941.h"

#include "simple_ble.h"
#include "simple_adv.h"
#include "simple_timer.h"

#define LED0 19

#define LTC2941_ADDR 0x64
#define ADV_SWITCH_MS 1000
#define PHYSWEB_URL "j2x.us/bgauge"
#define BGAUGE_SERVICE 0x1D
#define UMICH_COMPANY_IDENTIFIER 0x02E0

static nrf_drv_twi_t i2c = NRF_DRV_TWI_INSTANCE(0);
static nrf_drv_twi_config_t i2c_config = {
  .scl = 1,
  .sda = 7,
  .frequency = NRF_TWI_FREQ_400K,
  .interrupt_priority = 0
};
static float bg_data;
static uint8_t mdata[1 + sizeof(bg_data)];

// Intervals for advertising and connections
static simple_ble_config_t ble_config = {
    .platform_id       = 0xd9,              // used as 4th octect in device BLE address
    .device_id         = DEVICE_ID_DEFAULT,
    .adv_name          = "bgauge",
    .adv_interval      = MSEC_TO_UNITS(500, UNIT_0_625_MS),
    .min_conn_interval = MSEC_TO_UNITS(500, UNIT_1_25_MS),
    .max_conn_interval = MSEC_TO_UNITS(1000, UNIT_1_25_MS)
};

void ble_error(uint32_t error_code) {
    led_on(LED0);
}

static void adv_config_eddystone () {
    eddystone_adv(PHYSWEB_URL, NULL);
}

static void adv_config_data () {
    ble_advdata_manuf_data_t mandata;
    mdata[0] = BGAUGE_SERVICE;

    memcpy(mdata+1, (uint8_t*) &bg_data, sizeof(float));

    mandata.company_identifier = UMICH_COMPANY_IDENTIFIER;
    mandata.data.p_data = mdata;
    mandata.data.size = 1 + sizeof(float);

    simple_adv_manuf_data(&mandata);
}

void sample_data() {
    int err_code = 0;
    uint8_t cmd = 0x02;
    uint8_t data[2];
    led_toggle(LED0);
    err_code = nrf_drv_twi_tx(&i2c, LTC2941_ADDR, &cmd, 1, true);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_twi_rx(&i2c, LTC2941_ADDR, data, 2);
    APP_ERROR_CHECK(err_code);

    bg_data = (((uint32_t) data[0] << 8) + data[1]) * 0.085;
}

int main(void) {
    uint32_t err_code = 0;
    
    led_init(LED0);
    led_off(LED0);
    
    // Init I2C interface
    err_code = nrf_drv_twi_init(&i2c, &i2c_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);
    nrf_drv_twi_enable(&i2c);
    
    uint8_t cmd[3];
    cmd[0] = ACC_CHRG_MSB;
    cmd[1] = 0x00;
    cmd[2] = 0x00;
    err_code = nrf_drv_twi_tx(&i2c, LTC2941_ADDR, cmd, 3, 0);
    APP_ERROR_CHECK(err_code);

    // Setup BLE
    simple_ble_init(&ble_config);

    multi_adv_init(ADV_SWITCH_MS);
    multi_adv_register_config(adv_config_eddystone);
    multi_adv_register_config(adv_config_data);
    
    multi_adv_start();

    simple_timer_start(250, sample_data);

    while (1) {
        power_manage();
    }
}
