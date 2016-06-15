#ifndef CVSENSE_H
#define CVSENSE_H

uint8_t 	cvsense_init();
uint16_t 	cvsense_get_raw_voltage();
uint16_t 	cvsense_get_raw_current();
void		cvsense_short_circuit();
void		cvsense_open_circuit();
float 		cvsense_get_current();
float 		cvsense_get_voltage();

#endif
