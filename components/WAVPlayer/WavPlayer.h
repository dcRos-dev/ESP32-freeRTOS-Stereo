#pragma once
#include <string>
#include "driver/dac_continuous.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

class WavPlayer
{
private:
    // Usiamo un handle Digital To Analogic per gestire l'audio
    // Continous perché è uno stream ininterrotto per riprodurre la musica
    dac_continuous_handle_t dac_handle;
    bool isInitialized;

public:
    WavPlayer(/* args */);
    ~WavPlayer();

    // Init dello stream inidicando il sample_rate 
    bool init(uint32_t sample_rate = 16000);


    // Passiamo la reference dell filePath della canzone da riprodurre
    // Cosi evitiamo di allocare memoria inutile copiando il path per intero
    // Update 16/05/26: aggiungo anche stopSemaphor per implementare il service ble per stoppare la musica
    void play(const std::string& filePath, SemaphoreHandle_t stopSemaphore);
};

