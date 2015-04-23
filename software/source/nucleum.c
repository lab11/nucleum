#include <stdbool.h>

#include "nucleum.h"

#include "led.h"

bool platform_init () {
	bool err = 0;

	led_init(LED_0);
	led_init(LED_1);
	led_init(LED_2);

	return err;
}

