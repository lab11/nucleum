/*
 * Test UART print
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf_gpio.h"
#include "ble_advdata.h"
#include "boards.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "ble_debug_assert_handler.h"
#include "app_uart.h"
#include "led.h"

#include "simple_ble.h"
#include "simple_adv.h"


#define RX_PIN_NUMBER 11
#define TX_PIN_NUMBER 12

#define UART_RX_BUF_SIZE 64
#define UART_TX_BUF_SIZE 64


// Intervals for advertising and connections
static simple_ble_config_t ble_config = {
    .platform_id       = 0x40,              // used as 4th octect in device BLE address
    .device_id         = DEVICE_ID_DEFAULT,
    .adv_name          = "UARTTEST",       // used in advertisements if there is room
    .adv_interval      = MSEC_TO_UNITS(500, UNIT_0_625_MS),
    .min_conn_interval = MSEC_TO_UNITS(500, UNIT_1_25_MS),
    .max_conn_interval = MSEC_TO_UNITS(1000, UNIT_1_25_MS)
};

void myuart_error_handle (app_uart_evt_t * p_event) {
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR) {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    } else if (p_event->evt_type == APP_UART_FIFO_ERROR) {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}

void init_uart () {
    uint32_t err_code;
    const app_uart_comm_params_t comm_params = {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_DISABLED,
        false,
        UART_BAUDRATE_BAUDRATE_Baud38400
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       myuart_error_handle,
                       APP_IRQ_PRIORITY_LOW,
                       err_code);

    APP_ERROR_CHECK(err_code);
}


int main (void) {
    uint32_t err_code;

    led_init(LED_0);
    led_init(LED_1);
    led_init(LED_2);

    // Setup BLE
    simple_ble_init(&ble_config);

    // Advertise because why not
    simple_adv_only_name();

    led_on(LED_0);

    // We want UART
    init_uart();

    // Printf should work
    printf("Print a string.\n");

    // Also can do individual characters
    app_uart_put('C');
    app_uart_put('\n');

    // log_rtt_init();
    // log_rtt_printf(0, "rtt works??\n");



    while (1) {
        power_manage();
    }
}
