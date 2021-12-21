#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "driver/gpio.h"
#include "config_board.h"
 



#define ESP_INTR_FLAG_DEFAULT 0

led_strip_t *pStrip_a;

void config_input_gpio(void){
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};

    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO1/2 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //change gpio interrupt type for one pin
    gpio_set_intr_type(GPIO_INPUT_IO_1, GPIO_INTR_ANYEDGE);
    gpio_set_intr_type(GPIO_INPUT_IO_2, GPIO_INTR_ANYEDGE);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
}


void configure_led(void)
{
    /* LED strip initialization with the GPIO and pixels number*/
    pStrip_a = led_strip_init(CONFIG_BLINK_LED_RMT_CHANNEL, BLINK_GPIO, 1);
    /* Set all LED off to clear all pixels */
    pStrip_a->clear(pStrip_a, 50);
}


void change_led_color(DoorState door_state_t)
{
    switch (door_state_t)
    {
    case OPEN:
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        pStrip_a->set_pixel(pStrip_a, 0, 0, 0, 255);
        break;
    case CLOSE:
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        pStrip_a->set_pixel(pStrip_a, 0, 0, 255, 0);
        break;
    case MOVING:
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        pStrip_a->set_pixel(pStrip_a, 0, 50, 50, 50);
        break;
    case ALARM:
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        pStrip_a->set_pixel(pStrip_a, 0, 255, 0, 0);
        break;
    default:
    case UNKNOWN:
        /* Set all LED off to clear all pixels */
        pStrip_a->clear(pStrip_a, 50);
        break;
    }

    /* Refresh the strip to send data */
    pStrip_a->refresh(pStrip_a, 100);
}
