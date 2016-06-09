#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "nordic_common.h"
#include "nrf.h"
#include "adc.h"
#include "app_timer.h"
#include "nrf_gpio.h"
#include "rv3049.h"

#define MAX_SAMPLE 25000

//timer stuff
#define TIMER_PRESCALER 	1
#define MAX_TIMERS 			1
#define TIMER_QUEUE_SIZE 	5
#define SAMPLE_RATE			APP_TIMER_TICKS(10000, TIMER_PRESCALER)

#define VOLTAGE_PIN 1 
#define CURRENT_PIN 2 
#define SWITCH_PIN  3

static void sample(void* p_context);
static void writeWord(uint32_t* address, uint32_t value);
static void adcConfigVoltage(void);
static void adcConfigCurrent(void);
static void configCurrent(void);
static void configVoltage(void);

//local variables
APP_TIMER_DEF(sampleTimer);
static uint32_t numSamples = 0;
volatile static bool done = false;
static uint32_t* addr;
static uint32_t pg_size;
static uint32_t pg_num;

static void sample (void* p_context) {

	rv3049_time_t time;
	rv3049_read_time(&time);
	
	configVoltage();
	uint32_t voltage = getSample();

	configCurrent();
	uint32_t current = getSample();

	uint32_t currVolt = (current << 16) + (voltage & 0x0000ffff);

	uint32_t combinedTime = ((uint32_t)time.days << 24) + 
							((uint32_t)time.hours << 16) + 
							((uint32_t)time.minutes << 8) + 
							(uint32_t)time.seconds;

	writeWord(++addr, combinedTime);
	writeWord(++addr, currVolt);

	numSamples++;

	if(numSamples >= MAX_SAMPLE)
		done = true;
}

static void writeWord(uint32_t* address, uint32_t value) {
	
	NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }

    *address = value;

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }

    // Turn off flash write enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
}

static void clockInit(void) {
	NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART = 1;

    // Wait for the low frequency clock to start
    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {}
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
}

static void storageInit(void) {
	pg_size = NRF_FICR->CODEPAGESIZE;
	//pg_num = NRF_FICR->CODESIZE-1;
	pg_num = 0x4000/pg_size;

	addr = (uint32_t*)(pg_size * pg_num) -1;
}


static void adcConfigVoltage(void) {
	adcConfig(ADC_CONFIG_RES_10bit, ADC_CONFIG_INPSEL_AnalogInputNoPrescaling,  
						ADC_CONFIG_REFSEL_VBG,                      
						1 << VOLTAGE_PIN,                                  
						ADC_CONFIG_EXTREFSEL_None);
}

static void adcConfigCurrent(void) {

	adcConfig(ADC_CONFIG_RES_10bit, ADC_CONFIG_INPSEL_AnalogInputNoPrescaling,  
						ADC_CONFIG_REFSEL_VBG,                      
						1 << CURRENT_PIN,                                  
						ADC_CONFIG_EXTREFSEL_None);
}

static void configVoltage(void) {
	adcConfigVoltage();

	//set GPIO to low
	nrf_gpio_pin_clear(SWITCH_PIN);
}

static void configCurrent(void) {
	adcConfigCurrent();

	//set GPIO to high
	nrf_gpio_pin_set(SWITCH_PIN);
}

int main(void)
{
    // Initialize
	clockInit();

	APP_TIMER_INIT(	TIMER_PRESCALER,
					TIMER_QUEUE_SIZE,
					NULL);

	uint32_t err_code;
	err_code = app_timer_create(&sampleTimer,APP_TIMER_MODE_REPEATED,sample);
	APP_ERROR_CHECK(err_code);

	storageInit();
	nrf_gpio_cfg_output(SWITCH_PIN);
	rv3049_init();

	err_code = app_timer_start(sampleTimer, SAMPLE_RATE, NULL);
	APP_ERROR_CHECK(err_code);

    // Enter main loop
    while(true);
    {
		if(done) {
			app_timer_stop(sampleTimer);
		}
    }
}

