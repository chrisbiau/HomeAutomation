#pragma once

#include "led_strip.h"

#define GPIO_INPUT_IO_1     1
#define GPIO_INPUT_IO_2     2
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_1) | (1ULL<<GPIO_INPUT_IO_2))

#define BLINK_GPIO 18
#define CONFIG_BLINK_LED_RMT_CHANNEL 2

void config_input_gpio(void);
led_strip_t * configure_led(void);