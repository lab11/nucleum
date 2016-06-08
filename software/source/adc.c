#include <stdint.h>
#include "nrf.h"
#include "adc.h"


uint32_t getSample() {
    NRF_ADC->TASKS_START = 1;

    while(!NRF_ADC->EVENTS_END);
    

    NRF_ADC->EVENTS_END = 0;
    NRF_ADC->TASKS_STOP = 1;

    return NRF_ADC->RESULT;
}

void adcConfig(uint8_t res, uint8_t pscale, uint8_t ref, uint8_t psel,
                                                            uint8_t extref) {
    NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Disabled;
    NRF_ADC->INTENCLR = 1;
    NRF_ADC->CONFIG = (res << ADC_CONFIG_RES_Pos) |
                        (pscale << ADC_CONFIG_INPSEL_Pos) |
                        (ref << ADC_CONFIG_REFSEL_Pos) |
                        (psel << ADC_CONFIG_PSEL_Pos) |
                        (extref << ADC_CONFIG_EXTREFSEL_Pos);

    NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;
}

