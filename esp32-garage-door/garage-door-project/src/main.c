/* GPIO Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "chip_info.h"
#include "led_strip.h"
#include "config_board.h"
#include "door_state_enum.h"
#include "home_wifi.h"

#define ALERT_THRESHOLD_US  10000000LL //10sec

static const char *TAG = "Main";
static xQueueHandle gpio_evt_queue = NULL;


DoorState door_state_model;
int64_t lastTimeMsStateMOVING;

void changeStateModel(DoorState state)
{
    ESP_LOGI(TAG, "Door changes state %s to %s", doorStateToString(door_state_model), doorStateToString(state));
    door_state_model = state;
    change_led_color(door_state_model);
    //TODO: NOTIFY MQTT ...
}

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void alarm_check(void *arg)
{
    int64_t delta;
    for (;;)
    {
        if (door_state_model == MOVING)
        {
            delta = esp_timer_get_time() - lastTimeMsStateMOVING;
            if (delta > ALERT_THRESHOLD_US)
            {
                ESP_LOGI(TAG, "LAST Time in MVT: %lld  us, delta time in us: %lld  us", lastTimeMsStateMOVING, delta);
                changeStateModel(ALARM);
            }
        }
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void main_state_machine(void *arg)
{
    uint32_t io_num;
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            ESP_LOGI(TAG, "GPIO[%d] intr, val: %d", io_num, gpio_get_level(io_num));
            if (io_num == GPIO_INPUT_IO_1 && gpio_get_level(io_num) == 1)
            {
                changeStateModel(OPEN);
            }
            else if (io_num == GPIO_INPUT_IO_2 && gpio_get_level(io_num) == 1)
            {
                changeStateModel(CLOSE);
            }
            else
            {
                changeStateModel(MOVING);
                lastTimeMsStateMOVING = esp_timer_get_time();
            }
        }
    }
}

void app_main(void)
{

     print_chip_info();

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_init_sta();

   

    /* Configure input sensor and attach handler */
    config_input_gpio();
    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(main_state_machine, "main_state_machine", 2048, NULL, 10, NULL);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void *)GPIO_INPUT_IO_1);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_2, gpio_isr_handler, (void *)GPIO_INPUT_IO_2);

    /* Configure the peripheral according to the LED type */
    configure_led();

    //Init model
    if (gpio_get_level(GPIO_INPUT_IO_1) == 0 && gpio_get_level(GPIO_INPUT_IO_2) == 0)
    {
        ESP_LOGI(TAG, "STARTUP: Door is not close or not open ... in mvt ?");
        lastTimeMsStateMOVING = esp_timer_get_time();
        changeStateModel(MOVING);
    }else if (gpio_get_level(GPIO_INPUT_IO_1) == 1)
    {
        ESP_LOGI(TAG, "STARTUP: Door is open"); 
        changeStateModel(OPEN);
    }
    else if (gpio_get_level(GPIO_INPUT_IO_2) == 1)
    {
        ESP_LOGI(TAG, "STARTUP: Door is close"); 
        changeStateModel(OPEN);
    }
    else{   
        changeStateModel(UNKNOWN);
    }

    //Run alarm task
    xTaskCreate(alarm_check, "alarm_check", 2048, NULL, 10, NULL);

    while (1)
    {
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}