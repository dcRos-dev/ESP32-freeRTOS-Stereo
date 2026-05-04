#include <iostream>
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"

#include "SDCardManager.h" 
#include "WavPlayer.h"

extern "C" {
    void app_main(void);
}


// Define dei pin SPI per MICRO SD
#define PIN_NUM_MISO GPIO_NUM_19
#define PIN_NUM_MOSI GPIO_NUM_23
#define PIN_NUM_CLK  GPIO_NUM_18
#define PIN_NUM_CS   GPIO_NUM_5

// Define del button pin
#define PIN_BUTTON GPIO_NUM_32

static const char* TAG = "MAIN";


SDCardManager* sdManager;
WavPlayer* wavPlayer;
SemaphoreHandle_t sem;



// TASK STEREO
void audio_task( void* pvParameters){
    for(;;){
        if(xSemaphoreTake(sem,portMAX_DELAY) == pdTRUE){
            wavPlayer ->play("/sdcard/audio.wav");
        }
    }
}


// TASK DEL PULSANTE
void button_task(void* pvParameters){
    gpio_set_direction(PIN_BUTTON, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PIN_BUTTON, GPIO_PULLUP_ONLY);


    vTaskDelay(pdMS_TO_TICKS(50));

    int last_state = gpio_get_level(PIN_BUTTON);

    for(;;){
        int current_state = gpio_get_level(PIN_BUTTON);


        if (last_state == 1 && current_state == 0) {
            ESP_LOGI(TAG, "Pulsante premuto. ");
            xSemaphoreGive(sem);     
            // Debounce
            vTaskDelay(pdMS_TO_TICKS(300)); 
        }

        last_state = current_state;
            
        // Risposo minimo per non mandare in blocco la CPU (10ms)
        vTaskDelay(pdMS_TO_TICKS(10));
    }   
}


void app_main(void) {
    vTaskDelay(pdMS_TO_TICKS(2000));
    ESP_LOGI(TAG, "Avvio sistema ESP32...");

    sem = xSemaphoreCreateBinary();

    // inizializzazione dell'hardware
    sdManager = new SDCardManager("/sdcard", PIN_NUM_MOSI, PIN_NUM_MISO, PIN_NUM_CLK, PIN_NUM_CS);
    wavPlayer = new WavPlayer();

    if (!sdManager -> init()) {
        ESP_LOGE(TAG, "Inizializzazione sdManager fallita.");
        return;
    }

    if (!wavPlayer->init(16000)) {
        ESP_LOGE(TAG, "Errore DAC.");
        return; // Usciamo
    }

    xTaskCreate(audio_task, "AudioTask", 4096, NULL, 5, NULL); // Priorità alta per l'audio!
    xTaskCreate(button_task, "ButtonTask", 2048, NULL, 2, NULL); // Priorità bassa per il pulsante
    
    ESP_LOGI(TAG, "Sistema pronto.");
}