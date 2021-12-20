/* GPIO Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "chip_info.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "input_read_sensor.h"


#define BLINK_GPIO 18
#define CONFIG_BLINK_LED_RMT_CHANNEL 2

static const char *TAG = "Main";
/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/


static bool s_led_state = true;


static led_strip_t *pStrip_a;

static void blink_led(void)
{
    /* If the addressable LED is enabled */
    if (s_led_state) {
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        pStrip_a->set_pixel(pStrip_a, 0, 16, 16, 16);
        /* Refresh the strip to send data */
        pStrip_a->refresh(pStrip_a, 100);
    } else {
        /* Set all LED off to clear all pixels */
        pStrip_a->clear(pStrip_a, 50);
    }
}

static void configure_led(void)
{
    /* LED strip initialization with the GPIO and pixels number*/
    pStrip_a = led_strip_init(CONFIG_BLINK_LED_RMT_CHANNEL, BLINK_GPIO, 1);
    /* Set all LED off to clear all pixels */
    pStrip_a->clear(pStrip_a, 50);
}


 

void blinky(void *pvParameter)
{
     while(1) {
        ESP_LOGD(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        blink_led();
        /* Toggle the LED state */
        s_led_state = !s_led_state;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

 

void app_main(void)
{
    print_chip_info();

    /* Configure input sensor and attach handler */
    config_input_snesor();

    /* Configure the peripheral according to the LED type */
    configure_led();

    xTaskCreate(&blinky, "blinky", 2048,NULL,5,NULL );

    while(1) {
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}