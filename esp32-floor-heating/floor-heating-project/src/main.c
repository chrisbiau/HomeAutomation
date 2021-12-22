#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include "cJSON.h"

#include "home_wifi.h"
#include "ds18b20.h"

// Temp Sensors are on GPIO15
#define TEMP_BUS 15
#define digitalWrite gpio_set_level
#define NB_TEMP_SENSORS 5

#define MQTT_PUBLISH_SENSOR "/floor/heating/sensor"
#define MQTT_PUBLISH_EVENT "/floor/heating/event"
#define MQTT_HOST "192.168.1.203"

typedef struct sensor_info
{
	uint16_t hash;	   /**< Hash adress of sensor */
	int index;		   /**< Index of sensor */
	float temperature; /**< temperature of sensor */
} sensor_info;

static const char *TAG = "Main";
static esp_mqtt_client_handle_t mqtt_client;
static xQueueHandle mqtt_pub_evt_queue = NULL;

DeviceAddress tempSensors[NB_TEMP_SENSORS];

void getTempAddresses(DeviceAddress *tempSensorAddresses)
{
	unsigned int numberFound = 0;
	reset_search();
	// search for NB_TEMP_SENSORS addresses on the oneWire protocol
	while (search(tempSensorAddresses[numberFound], true))
	{
		numberFound++;
		if (numberFound == NB_TEMP_SENSORS)
			break;
	}

	return;
}

// a simple hash function for 1wire address to reduce id to two bytes
uint16_t simpleAddrHash(DeviceAddress a)
{
	return ((a[1] ^ a[2] ^ a[3] ^ a[4] ^ a[5] ^ a[6]) << 8) + a[7];
}

void publish_mqtt_task(void *arg)
{
	cJSON *root;
	sensor_info sensor;
	char hashStr[7];
	char topic[sizeof(MQTT_PUBLISH_SENSOR) + sizeof(hashStr) + 2];
	for (;;)
	{
		if (xQueueReceive(mqtt_pub_evt_queue, &sensor, portMAX_DELAY))
		{
			root = cJSON_CreateObject();
			snprintf(hashStr, sizeof(hashStr), "0x%04x", sensor.hash);
			cJSON_AddStringToObject(root, "h", hashStr);
			cJSON_AddNumberToObject(root, "i", sensor.index);
			cJSON_AddNumberToObject(root, "t", sensor.temperature);

			snprintf(topic, sizeof(topic), "%s/%s", MQTT_PUBLISH_SENSOR, hashStr);
			int msg_id = esp_mqtt_client_publish(mqtt_client, topic, cJSON_PrintUnformatted(root), 0, 1, 0);
			ESP_LOGI(TAG, "sent topic %s publish successful msg_id=%d",topic, msg_id);
			cJSON_Delete(root);
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
		ESP_LOGI(TAG, "MQTT Other event id:%d", event->event_id);
		break;
	}
}

void read_temp_task(void *arg)
{
	float temp;
	uint16_t hash;
	;
	for (;;)
	{
		ds18b20_requestTemperatures();
		for (int i = 0; i < NB_TEMP_SENSORS; i++)
		{
			temp = ds18b20_getTempC((DeviceAddress *)tempSensors[i]);
			hash = simpleAddrHash(tempSensors[i]);
			if (temp <= -196.0)
			{
				ESP_LOGE(TAG, "Failed to read from DS18B20 sensor  Hash: 0x%04x index: %d value: %0.1fCd", hash, i, temp);
			}else{
				ESP_LOGI(TAG, " Temperature: Hash: 0x%04x index: %d value: %0.1fC", hash, i, temp);
			}
			sensor_info sensor = {.hash = hash, .index = i, .temperature = temp};
			xQueueSend(mqtt_pub_evt_queue, &sensor, (TickType_t)10);
		}
		vTaskDelay(10000 / portTICK_RATE_MS); // 1sec
	}
}

void app_main(void)
{

	ESP_LOGI(TAG, "***** Initialize NVS *****");
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
	ESP_LOGI(TAG, "***** Initialize DS18B20 *****");

	ds18b20_init(TEMP_BUS);
	getTempAddresses(tempSensors);
	ds18b20_setResolution(tempSensors, NB_TEMP_SENSORS, 10);
	for (int i = 0; i < NB_TEMP_SENSORS; i++)
	{
		uint16_t hash = simpleAddrHash(tempSensors[i]);
		ESP_LOGI(TAG, "Hash = 0x%04x ; Index %d: ; Adress: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", hash, i, tempSensors[i][0], tempSensors[i][1], tempSensors[i][2], tempSensors[i][3], tempSensors[i][4], tempSensors[i][5], tempSensors[i][6], tempSensors[i][7]);
	}

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

	// create a queue to handle gpio event from isr
	mqtt_pub_evt_queue = xQueueCreate(10, sizeof(sensor_info));

	ESP_LOGI(TAG, "***** Initialize PERIODIC READ TASK *****");
	xTaskCreate(read_temp_task, "read_temp_task", 2048, NULL, 1, NULL);
	xTaskCreate(publish_mqtt_task, "publish_mqtt_task", 2048, NULL, 10, NULL);

	for (;;)
	{
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}