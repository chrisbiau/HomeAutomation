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
#include "config_board.h"

#include "freertos/queue.h"

static const char *TAG = "Main";
/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/

enum DoorState
{
    UNKNOWN = 0, /**< Unknown door state */
    OPEN,        /**< Door is open */
    CLOSE,       /**< Door is close */
    INMVT,       /**< Door is in mouvemnt */
    ALARM        /**< Door alarm not in open or not in close */
}door_state_model;

static led_strip_t *pStrip_a;
static xQueueHandle gpio_evt_queue = NULL;


static void change_led_color(enum DoorState door_state_t)
{
    switch (door_state_t)
    {
    case OPEN:

        ESP_LOGI(TAG, "OPEN");
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        pStrip_a->set_pixel(pStrip_a, 0, 0, 0, 255);
        break;
    case CLOSE:
        ESP_LOGI(TAG, "CLOSE");
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        pStrip_a->set_pixel(pStrip_a, 0, 0, 255, 0);
        break;
    case INMVT:
        ESP_LOGI(TAG, "INMVT");
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        pStrip_a->set_pixel(pStrip_a, 0, 50, 50, 50);
        break;
    case ALARM:
        ESP_LOGI(TAG, "ALARM");
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        pStrip_a->set_pixel(pStrip_a, 0, 255, 0, 0);
        break;
    default:
    case UNKNOWN:
        /* Set all LED off to clear all pixels */
        ESP_LOGI(TAG, "UNKNOWN");
        pStrip_a->clear(pStrip_a, 50);
        break;
    }

    /* Refresh the strip to send data */
    pStrip_a->refresh(pStrip_a, 100);
}

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_log(void *arg)
{
    uint32_t io_num;
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            ESP_LOGI(TAG, "GPIO[%d] intr, val: %d", io_num, gpio_get_level(io_num));
            
            if(io_num == GPIO_INPUT_IO_1 &&  gpio_get_level(io_num) ==1 ){
                door_state_model = OPEN;
            }else if (io_num == GPIO_INPUT_IO_2 &&  gpio_get_level(io_num) ==1 ){
                door_state_model = CLOSE;
            }else{
                 door_state_model = INMVT;
            }
            change_led_color(door_state_model);
        }
    }
}

void app_main(void)
{
    print_chip_info();

    /* Configure input sensor and attach handler */
    config_input_gpio();
    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(gpio_task_log, "gpio_task_log", 2048, NULL, 10, NULL);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void *)GPIO_INPUT_IO_1);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_2, gpio_isr_handler, (void *)GPIO_INPUT_IO_2);

    ESP_LOGI(TAG, "StartUp state value of GPIO%d, state: %d", GPIO_INPUT_IO_1, gpio_get_level(GPIO_INPUT_IO_1));
    ESP_LOGI(TAG, "StartUp state value of GPIO%d, state: %d", GPIO_INPUT_IO_2, gpio_get_level(GPIO_INPUT_IO_2));

    /* Configure the peripheral according to the LED type */
    pStrip_a = configure_led();

    change_led_color(UNKNOWN);

    while (1)
    {
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}