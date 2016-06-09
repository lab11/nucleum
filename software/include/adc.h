#ifndef ADC_H
#define ADC_H

uint32_t getSample();
void adcConfig(uint8_t res, uint8_t pscale, uint8_t ref, uint8_t psel, 
                                                        uint8_t extref);

#endif
