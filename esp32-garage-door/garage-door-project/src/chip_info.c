#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"

static const char *TAG = "chipInfo";

void print_chip_info(){
        /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    ESP_LOGI(TAG,"This is ESP32 chip with %d CPU cores, WiFi%s%s, silicon revision %d",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "",
            chip_info.revision);
    ESP_LOGI(TAG,"%dMB %s flash", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
    ESP_LOGI(TAG,"Minimum free heap size: %d bytes", esp_get_minimum_free_heap_size());

}
