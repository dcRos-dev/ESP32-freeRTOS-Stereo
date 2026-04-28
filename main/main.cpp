#include <iostream>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "SDCardManager.h" // Includiamo solo l'header della nostra classe

extern "C" {
    void app_main(void);
}

#define PIN_NUM_MISO GPIO_NUM_19
#define PIN_NUM_MOSI GPIO_NUM_23
#define PIN_NUM_CLK  GPIO_NUM_18
#define PIN_NUM_CS   GPIO_NUM_5

static const char *TAG = "MAIN";

void app_main(void) {
    vTaskDelay(pdMS_TO_TICKS(2000));
    ESP_LOGI(TAG, "Avvio sistema ESP32...");

    // Creazione dell'oggetto passando i pin hardware al costruttore
    SDCardManager sdManager("/sdcard", PIN_NUM_MOSI, PIN_NUM_MISO, PIN_NUM_CLK, PIN_NUM_CS);

    if (sdManager.init()) {
        sdManager.listFiles();
    } else {
        ESP_LOGE(TAG, "Inizializzazione fallita. Sistema in stop.");
    }
}