#ifndef REPLAY_H
#define REPLAY_H

uint8_t 	replay_init();
void		replay_set_voltage(float);
void		replay_set_current(float);
uint8_t		replay_get_mode();
uint16_t	replay_get_dial();

#endif
