#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "nrf_gpiote.h"
#include "nrf51_bitfields.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
//#include "ble_conn_params.h"
#include "app_scheduler.h"
#include "softdevice_handler.h"
//#include "app_timer.h"
#include "bleConfig.h"
#include "ble_co.h"
#include "lmp91000.h"
#include "lps25h.h"
#include "led.h"
#include "pins.h"
#include "queue.h"
//#include "ble_bas.h"
//#include "ble_auth.h"

//module-private variable declarations

//Security requirements for this application.
static ble_gap_sec_params_t             m_sec_params;
//current connection handle
static uint16_t 		m_conn_handle = BLE_CONN_HANDLE_INVALID; 
static ble_co_t			m_co;
//static ble_auth_t	m_auth;
//static ble_bas_t	m_bas;
//static ble_bas_init_t m_bas_init;

static ble_advdata_t advdata;
static ble_gap_adv_params_t m_adv_params;
//static app_timer_id_t notifyTimer;

#define APP_BEACON_INFO_LENGTH 	0x09
#define APP_ADV_DATA_LENGTH		0x07
#define APP_DEVICE_TYPE			0x01
#define APP_BEACON_DATA			0x42, 0x68, 0x86, 0x34, 0x56
#define APP_ID					0x43

#define PRESSURE_REBASE_PRESCALER 	4095 //125ms intervals
#define PRESSURE_REBASE_TIME		60*8 //60 seconds * 8 ticks/second
#define PRESSURE_THRESHOLD			1000 //PA

static uint8_t m_beacon_info[APP_BEACON_INFO_LENGTH] =
{
	APP_DEVICE_TYPE,
	APP_ADV_DATA_LENGTH,
	APP_BEACON_DATA,
	APP_ID
};

typedef enum {
	NONE = 0,
	ADVERTISING,
	CONNECTED,
	SLEEPING,
} BLE_STATE;

volatile BLE_STATE bleState = NONE;
volatile BLE_STATE nextBleState = ADVERTISING;

//module-private function declaration
void assert_nrf_callback(uint16_t, const uint8_t*);
static void service_error_handler(uint32_t);
static void timers_init(void);
static void timer_start(void);
static void gap_params_init(void);
static void services_init(void);
static void sec_params_init(void);
static void power_manage(void);
static void scheduler_init(void);
static void ble_stack_init(void);
static void sys_evt_dispatch(uint32_t);
static void on_ble_evt(ble_evt_t * p_ble_evt);
static void advertising_init(void);
//static void conn_params_init(void);

//void protocol_write_handler(ble_auth_t* auth, uint8_t protocol);
//void len_write_handler(ble_auth_t* auth, uint16_t len);
//void data_write_handler(ble_auth_t* auth, uint8_t* dataArray);
//void start_write_handler(ble_auth_t* auth, uint8_t start);
//void pass_write_handler(ble_auth_t* auth, uint8_t pass);
//void retry_write_handler(ble_auth_t* auth, uint8_t retry);

static void advertisingStart();
static void advertisingStop();
static void connectedStart();
static void connectedStop();
static void sleepStart();
static void sleepStop();
static void bleInit();

//global function implementation
void appService(void) {

	//power_manage();

	switch(bleState) {
		case NONE:
			if(nextBleState == ADVERTISING) {
				advertisingStart();
				bleState = ADVERTISING;
			}
		break;
		case ADVERTISING:
			if(nextBleState == CONNECTED) {
				advertisingStop();
				connectedStart();
				bleState = CONNECTED;	
			} else if(nextBleState == SLEEPING) {
				advertisingStop();
				sleepStart();
				bleState = SLEEPING;
			}
		break;
		case CONNECTED:
			if(nextBleState == ADVERTISING) {
				connectedStop();
				advertisingStart();
				bleState = ADVERTISING;
			} else if(nextBleState == SLEEPING) {
				connectedStop();
				sleepStart();
				bleState = SLEEPING;
			}
		break;
		case SLEEPING:
			if(nextBleState == ADVERTISING) {
				sleepStop();
				advertisingStart();
				bleState = ADVERTISING;	
			} else {
				sleepStart();
			}
		break;
	}
}

void advertisingStop(void) {
	
}

void setRTC1(void) {

	NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_RC << CLOCK_LFCLKSRC_SRC_Pos);
	NRF_CLOCK->TASKS_LFCLKSTART = 1;
	while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0); 
  	NRF_RTC1->PRESCALER = PRESSURE_REBASE_PRESCALER; //1kHz frequency
	NRF_RTC1->EVENTS_COMPARE[0] = 0;
	NRF_RTC1->CC[0] = PRESSURE_REBASE_TIME;
	NRF_RTC1->EVTENSET = RTC_EVTENSET_COMPARE0_Msk;
	NRF_RTC1->INTENSET = RTC_INTENSET_COMPARE0_Msk;
	NRF_RTC1->TASKS_CLEAR = 1;
	NRF_RTC1->TASKS_START = 1;
	NVIC_EnableIRQ(RTC1_IRQn);
}

static void setGPIOTE(void) {
	
	nrf_gpiote_event_config(0, PINT, NRF_GPIOTE_POLARITY_LOTOHI);
	NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_IN0_Msk;
	NRF_GPIOTE->EVENTS_IN[0] = 0;
	NVIC_EnableIRQ(GPIOTE_IRQn);
}

void sleepStart(void) {
	ledOff(LED_1);
	ledOff(LED_3);

	//put gas to sleep
	setGasInactive();

	//put pressure to sleep and set threshold
	uint32_t currPress = getPressure();
	currPress += PRESSURE_THRESHOLD;
	setPressureThreshold(currPress);
	setPressureInactive();

	//setup RTC1 interrupt and pressure interrupts
	setRTC1();
	setGPIOTE();

	//WAITFORINTERRUPTS
	__WFI();
}

void RTC1_IRQHandler() {
	if(NRF_RTC1->EVENTS_COMPARE[0] != 0) {
		NVIC_DisableIRQ(RTC1_IRQn);
	}
}

void GPIOTE_IRQHandler() {

	if(NRF_GPIOTE->EVENTS_IN[0] != 0) {
		nextBleState = ADVERTISING;
		NVIC_DisableIRQ(RTC1_IRQn);
		NVIC_DisableIRQ(GPIOTE_IRQn);
	}
}

void sleepStop(void) {
	setGasActive();
	setPressureActive();
}

void connectedStart(void) {
}

void connectedStop(void) {
	
}

void appInit() {
	gasInit();
	pressureInit();
	ledInit();
	bleInit();
}

void bleInit() {
    timers_init();
    ble_stack_init();
    scheduler_init();
    gap_params_init();
    advertising_init();
    services_init();
    //conn_params_init();
    sec_params_init();
}

void advertisingStart(void) {
	
	ledOn(LED_1);
	ledOn(LED_3);

	//start advertising
    uint32_t             err_code;
    err_code = sd_ble_gap_adv_start(&m_adv_params);
    APP_ERROR_CHECK(err_code);
}


//module-private implementations

//softdevice assertion callback
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

//service error callback
static void service_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
} 

//initialize advertising
static void advertising_init(void)
{
    uint32_t      err_code;
    uint8_t       flags =  BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;


    //ble_uuid_t adv_uuids[] = {{CO_UUID_SERVICE, m_co.uuid_type}}; 

    // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
	advdata.include_appearance		= true;
    advdata.flags.size              = sizeof(flags);
    advdata.flags.p_data            = &flags;
    //advdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    //advdata.uuids_complete.p_uuids  = adv_uuids;

    err_code = ble_advdata_set(&advdata, NULL);
    APP_ERROR_CHECK(err_code);

	memset(&m_adv_params, 0, sizeof(m_adv_params));

	m_adv_params.type				= BLE_GAP_ADV_TYPE_ADV_IND;
	m_adv_params.p_peer_addr		= NULL; 
	m_adv_params.fp					= BLE_GAP_ADV_FP_ANY; 
    m_adv_params.interval    		= APP_ADV_INTERVAL;
    m_adv_params.timeout     		= APP_ADV_TIMEOUT_IN_SECONDS;
}

//initialize timers

void notifyHandler(void* p_context) {
	static uint8_t i = 0;
	static bool notifyPress = false;
	queuePush(getGasSample());	
	
	if(!(i%10)) {
		ble_co_on_gas_change(&m_co, convertSampleToPPM((uint32_t)queueAverage()));
		//ble_co_on_press_change(&m_co, getPressure());
		notifyPress = true;
	} else if (notifyPress) {
		ble_co_on_press_change(&m_co, getPressure());
		notifyPress = false;
	}
	i++;
}

static void timers_init(void)
{
    // Initialize timer module, making it use the scheduler
    //APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);

    
	//uint32_t err_code = app_timer_create(&notifyTimer, APP_TIMER_MODE_REPEATED, notifyHandler);
    //APP_ERROR_CHECK(err_code); 
}

static void timer_start(void)
{
	//uint32_t err_code = app_timer_start(notifyTimer, NOTIFY_RATE, NULL);
	//APP_ERROR_CHECK(err_code);
}


//gap name/apperance/connection parameters
static void gap_params_init(void)
{
    uint32_t                err_code;
    //ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;
	
	sd_ble_gap_tx_power_set(4);
	
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    //err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_COMPUTER);
    //APP_ERROR_CHECK(err_code);

    //memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    //gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    //gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    //gap_conn_params.slave_latency     = SLAVE_LATENCY;
    //gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    //err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    //APP_ERROR_CHECK(err_code);
}


//init services
static void services_init(void)
{
	uint32_t err_code;
	ble_co_init_t co_init;

	memset(&co_init, 0, sizeof(co_init));

	co_init.evt_handler = NULL;

	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&co_init.co_gas_attr_md.cccd_write_perm);
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&co_init.co_gas_attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&co_init.co_gas_attr_md.write_perm);

	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&co_init.co_press_attr_md.cccd_write_perm);
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&co_init.co_press_attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&co_init.co_press_attr_md.write_perm);

	err_code = ble_co_init(&m_co, &co_init);
	APP_ERROR_CHECK(err_code);

	/*ble_auth_init_t authInit;
	authInit.protocol_write_handler = protocol_write_handler;
	authInit.len_write_handler 		= len_write_handler;
	authInit.data_write_handler 	= data_write_handler;
	authInit.start_write_handler 	= start_write_handler;
	authInit.pass_write_handler 	= pass_write_handler;
	authInit.retry_write_handler 	= retry_write_handler;*/

	//err_code = ble_auth_init(&m_auth, &authInit);
	//APP_ERROR_CHECK(err_code);

	//err_code = ble_bas_init(&m_bas, &m_bas_init);
	//APP_ERROR_CHECK(err_code);
}


//init security
static void sec_params_init(void)
{
    m_sec_params.timeout      = SEC_PARAM_TIMEOUT;
    m_sec_params.bond         = SEC_PARAM_BOND;
    m_sec_params.mitm         = SEC_PARAM_MITM;
    m_sec_params.io_caps      = SEC_PARAM_IO_CAPABILITIES;
    m_sec_params.oob          = SEC_PARAM_OOB;
    m_sec_params.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    m_sec_params.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
}

//connection parameters event handler callback
/*
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;

    if(p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}

//connection parameters error callback
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

//initialize connection parameters

static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}*/



//ble event callback
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t                         err_code;
    static ble_gap_evt_auth_status_t m_auth_status;
    ble_gap_enc_info_t *             p_enc_info;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle,
                                                   BLE_GAP_SEC_STATUS_SUCCESS,
                                                   &m_sec_params);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0);
            APP_ERROR_CHECK(err_code);
            break;
        case BLE_GAP_EVT_AUTH_STATUS:
            m_auth_status = p_ble_evt->evt.gap_evt.params.auth_status;
            break;
        case BLE_GAP_EVT_SEC_INFO_REQUEST:
            p_enc_info = &m_auth_status.periph_keys.enc_info;
            if (p_enc_info->div == 
							p_ble_evt->evt.gap_evt.params.sec_info_request.div)
            {
                err_code = sd_ble_gap_sec_info_reply(m_conn_handle, 
															p_enc_info, NULL);
                APP_ERROR_CHECK(err_code);
            }
            else
            {
                // No keys found for this device
                err_code = sd_ble_gap_sec_info_reply(m_conn_handle, NULL, NULL);
                APP_ERROR_CHECK(err_code);
            }
            break;
        case BLE_GAP_EVT_TIMEOUT:
            if (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISEMENT)
            {
                //err_code = sd_power_system_off();
                //APP_ERROR_CHECK(err_code);
				nextBleState = SLEEPING;
            }
            break;
        default:
            break;
    }
}


//dispatching a BLE stack event to all modules with a BLE stack event handler.
//This function is called from the scheduler in the main loop after a BLE stack
//event has been received.
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    on_ble_evt(p_ble_evt);
    //ble_conn_params_on_ble_evt(p_ble_evt);
    /*
    YOUR_JOB: Add service ble_evt handlers calls here, like, for example:
    ble_bas_on_ble_evt(&m_bas, p_ble_evt);
    */

	ble_co_on_ble_evt(&m_co, p_ble_evt);
}


//for dispatching a system event to interested modules.
//is called from the System event interrupt handler after a system
//event has been received.
static void sys_evt_dispatch(uint32_t sys_evt)
{
    
}

//ble event interrupt
static void ble_stack_init(void)
{
    uint32_t err_code;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_RC_250_PPM_8000MS_CALIBRATION, 
																		false);

    // Enable BLE stack 
    ble_enable_params_t ble_enable_params;
    memset(&ble_enable_params, 0, sizeof(ble_enable_params));
    ble_enable_params.gatts_enable_params.service_changed = 
											IS_SRVC_CHANGED_CHARACT_PRESENT;
    err_code = sd_ble_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);
    
    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);
    
    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}


static void scheduler_init(void)
{
    //APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}


static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

/*
void protocol_write_handler(ble_auth_t* auth, uint8_t protocol){

}
void len_write_handler(ble_auth_t* auth, uint16_t len){

}
void data_write_handler(ble_auth_t* auth, uint8_t* dataArray){

}
void start_write_handler(ble_auth_t* auth, uint8_t start){

}
void pass_write_handler(ble_auth_t* auth, uint8_t pass){

}
void retry_write_handler(ble_auth_t* auth, uint8_t retry){

}*/

