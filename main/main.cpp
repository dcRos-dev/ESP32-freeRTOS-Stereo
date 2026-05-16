#include <iostream>
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"

#include "SDCardManager.h" 
#include "WavPlayer.h"
#include "BleServer.h"


extern "C" {
    void app_main(void);
}


// Define dei pin SPI per MICRO SD
#define PIN_NUM_MISO GPIO_NUM_19
#define PIN_NUM_MOSI GPIO_NUM_23
#define PIN_NUM_CLK  GPIO_NUM_18
#define PIN_NUM_CS   GPIO_NUM_5


static const char* TAG = "MAIN";


SDCardManager* sdManager;
WavPlayer* wavPlayer;
BleServer* bleServer;
SemaphoreHandle_t playSem; 
SemaphoreHandle_t stopSem;



// TASK STEREO
void audio_task( void* pvParameters){
    for(;;){
        if(xSemaphoreTake(playSem, portMAX_DELAY) == pdTRUE){
            // Passiamo il file e il semaforo di stop che il player controllerà costantemente
            wavPlayer->play("/sdcard/audio.wav", stopSem);
        }
    }
}



void app_main(void) {
    vTaskDelay(pdMS_TO_TICKS(2000));
    ESP_LOGI(TAG, "Avvio sistema ESP32...");

    playSem = xSemaphoreCreateBinary();
    stopSem = xSemaphoreCreateBinary();

    // inizializzazione dell'hardware
    sdManager = new SDCardManager("/sdcard", PIN_NUM_MOSI, PIN_NUM_MISO, PIN_NUM_CLK, PIN_NUM_CS);
    wavPlayer = new WavPlayer();
    bleServer = new BleServer(playSem, stopSem);

    if (!sdManager->init()) {
        ESP_LOGE(TAG, "Inizializzazione sdManager fallita.");
        return;
    }

    if (!wavPlayer->init(16000)) {
        ESP_LOGE(TAG, "Errore DAC.");
        return; 
    }

    // Avviamo il Server BLE
    bleServer->init("ESP32_Stereo_Player");
    xTaskCreate(audio_task, "AudioTask", 4096, NULL, 5, NULL); 
    ESP_LOGI(TAG, "Sistema pronto con funzioni PLAY e STOP.");
}