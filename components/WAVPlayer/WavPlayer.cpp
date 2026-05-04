#include "WavPlayer.h"
#include "esp_log.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"


static const char* TAG = "WAV_PLAYER";

WavPlayer::WavPlayer(): isInitialized(false) {}

WavPlayer::~WavPlayer(){
    if(isInitialized){
        dac_continuous_disable(dac_handle);
        dac_continuous_del_channels(dac_handle);
    }
}


bool WavPlayer::init(uint32_t sample_rate){
    ESP_LOGI(TAG, "Inizializzazione DAC ");

    //configurazione del dac


    dac_continuous_config_t dac_cfg = {};

    // uso il canale 0 perché ho collegato la cassa al pin 25
    dac_cfg.chan_mask = DAC_CHANNEL_MASK_CH0;

    // Parametri del DMA
    dac_cfg.desc_num = 4;
    dac_cfg.buf_size = 2048;

    // Definizione della frequenza di aggiornamento e offset del volume
    dac_cfg.freq_hz = sample_rate;
    dac_cfg.offset = 0;

    dac_cfg.clk_src = DAC_DIGI_CLK_SRC_APLL;
    dac_cfg.chan_mode = DAC_CHANNEL_MODE_SIMUL;


    // Prova ad allocare ed abilitare il canale DAC
    if(dac_continuous_new_channels(&dac_cfg, &dac_handle) != ESP_OK) return false;
    if(dac_continuous_enable(dac_handle) != ESP_OK) return false;

    isInitialized = true;
    ESP_LOGI(TAG, " Inizializzazione completata ");
    return true;
}



void WavPlayer::play(const std::string& filePath){

    if(!isInitialized) return;

    ESP_LOGI(TAG, "Riproduzione: %s", filePath.c_str());


    FILE* f = fopen(filePath.c_str(), "rb");
    if(f == NULL){
        ESP_LOGE(TAG, "Impossibile aprire il file");
        return;
    }

    // Il formato WAV ha un'intestazione di 44 bit.
    // Dunque é necessario saltarli
    fseek(f,44,SEEK_SET);

    uint8_t audio_buffer[1024];
    size_t bytes_read;
    size_t bytes_written;


    // Leggiamo ciclicamente dalla microSD pezzi della traccia da riprodurre
    while (true) {
        bytes_read = fread(audio_buffer, 1, sizeof(audio_buffer), f);
        
        if (bytes_read == 0) {
            break; 
        }

        dac_continuous_write(dac_handle, audio_buffer, bytes_read, &bytes_written, portMAX_DELAY);
    }

    fclose(f);
    ESP_LOGI(TAG, "Riproduzione terminata");
}