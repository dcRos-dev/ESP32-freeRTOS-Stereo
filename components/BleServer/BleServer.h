#pragma once

#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "NimBLEDevice.h"


class BleServer
{
private:
    SemaphoreHandle_t playSemaphor;
    SemaphoreHandle_t stopSemaphor;

    static const char* SERVICE_UUID;
    static const char* PLAY_CHARACTERISTIC_UUID;
    static const char* STOP_CHARACTERISTIC_UUID;


public:


    //Costruttore parametrizzato del server
    BleServer(SemaphoreHandle_t playSem, SemaphoreHandle_t stopSem);
    void init(const std::string& deviceName);
    ~BleServer();
};
