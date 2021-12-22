
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "mqtt_client.h"

#include "chip_info.h"
#include "led_strip.h"
#include "config_board.h"
#include "door_state_enum.h"
#include "home_wifi.h"

#define ALERT_THRESHOLD_US 10000000LL //10sec

#define MQTT_PUBLISH_STATE "/garage/door/state"
#define MQTT_PUBLISH_EVENT "/garage/door/event"
#define MQTT_SUBSCRIBE_CMD "/garage/door/command"
#define MQTT_HOST "192.168.1.203"


static const char *TAG = "Main";
static xQueueHandle gpio_evt_queue = NULL;

static esp_mqtt_client_handle_t mqtt_client;

DoorState door_state_model;
int64_t lastTimeMsStateMOVING;

void notify_mqtt_changeState(DoorState door_state_model)
{
    int msg_id = esp_mqtt_client_publish(mqtt_client, MQTT_PUBLISH_STATE, doorStateToString(door_state_model), 0, 1, 0);
    ESP_LOGI(TAG, "Sent publish successful, (change state to %s) msg_id=%d", doorStateToString(door_state_model), msg_id);
}

void change_state_model(DoorState state)
{
    ESP_LOGI(TAG, "Door changes state %s to %s", doorStateToString(door_state_model), doorStateToString(state));
    door_state_model = state;
    change_led_color(door_state_model);
    notify_mqtt_changeState(door_state_model);
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
                change_state_model(ALERT);
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
                change_state_model(OPEN);
            }
            else if (io_num == GPIO_INPUT_IO_2 && gpio_get_level(io_num) == 1)
            {
                change_state_model(CLOSE);
            }
            else
            {
                change_state_model(MOVING);
                lastTimeMsStateMOVING = esp_timer_get_time();
            }
        }
    }
}

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_publish(client, MQTT_PUBLISH_EVENT, "MQTT_EVENT_CONNECTED", 0, 1, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, MQTT_SUBSCRIBE_CMD, 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        if (memcmp(event->data, "OPEN", 4) == 0)
        {
            ESP_LOGI(TAG, "TODO: OPENNING DOOR ...");
        }
        else if (memcmp(event->data, "CLOSE", 5) == 0)
        {
            ESP_LOGI(TAG, "TODO: CLOSING DOOR ...");
        }
        else
        {
            notify_mqtt_changeState(door_state_model);
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void app_main(void)
{

    print_chip_info();

    ESP_LOGI(TAG, "***** Initialize NVS *****");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "***** Initialize WIFI *****");
    wifi_init_sta();

    ESP_LOGI(TAG, "***** Initialize MQTT *****");
    esp_mqtt_client_config_t mqtt_cfg = {
        .host = MQTT_HOST,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);

    ESP_LOGI(TAG, "***** Initialize GPIO *****");
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

    ESP_LOGI(TAG, "***** Initialize LED *****");
    /* Configure the peripheral according to the LED type */
    configure_led();

    ESP_LOGI(TAG, "***** Initialize STATE MODEL *****");
    if (gpio_get_level(GPIO_INPUT_IO_1) == 0 && gpio_get_level(GPIO_INPUT_IO_2) == 0)
    {
        ESP_LOGI(TAG, "STARTUP: Door is not close or not open ... in mvt ?");
        lastTimeMsStateMOVING = esp_timer_get_time();
        change_state_model(MOVING);
    }
    else if (gpio_get_level(GPIO_INPUT_IO_1) == 1)
    {
        ESP_LOGI(TAG, "STARTUP: Door is open");
        change_state_model(OPEN);
    }
    else if (gpio_get_level(GPIO_INPUT_IO_2) == 1)
    {
        ESP_LOGI(TAG, "STARTUP: Door is close");
        change_state_model(OPEN);
    }
    else
    {
        change_state_model(UNKNOWN);
    }

    ESP_LOGI(TAG, "***** Initialize ALARM TASK *****");
    xTaskCreate(alarm_check, "alarm_check", 2048, NULL, 9, NULL);

    for (;;)
    {
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}