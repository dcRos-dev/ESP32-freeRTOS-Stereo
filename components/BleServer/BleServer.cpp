#include "BleServer.h"
#include "esp_log.h"


// Define del tag per ESP LOG
static const char* TAG = "BLE_SERVER";

// Codici UUID generati casualmente con Online UUID Generator
const char* BleServer::SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const char* BleServer::PLAY_CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
const char* BleServer::STOP_CHARACTERISTIC_UUID = "c2a8b94f-1234-459e-8fcc-fa07361b26bc";


class PlayCallbacks: public NimBLECharacteristicCallbacks {
    SemaphoreHandle_t _s;
public:
    PlayCallbacks(SemaphoreHandle_t s) : _s(s) {}
    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue == "PLAY" || rxValue == "1") {
            ESP_LOGI(TAG, "Ricevuto comando: PLAY");
            xSemaphoreGive(_s);
        }
    }
};


class StopCallbacks: public NimBLECharacteristicCallbacks {
    SemaphoreHandle_t _s;
public:
    StopCallbacks(SemaphoreHandle_t s) : _s(s) {}
    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue == "STOP" || rxValue == "0") {
            ESP_LOGI(TAG, "Ricevuto comando: STOP");
            xSemaphoreGive(_s); 
        }
    }
};



BleServer::BleServer(SemaphoreHandle_t playSem, SemaphoreHandle_t stopSem) : playSemaphor(playSem) , stopSemaphor(stopSem){}

void BleServer::init(const std::string& deviceName) {
    NimBLEDevice::init(deviceName);
    NimBLEServer* pServer = NimBLEDevice::createServer();
    NimBLEService* pService = pServer->createService(SERVICE_UUID);
    
   NimBLECharacteristic* pPlayChar = pService->createCharacteristic(
        PLAY_CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::WRITE
    );
    pPlayChar->setCallbacks(new PlayCallbacks(playSemaphor));


    NimBLECharacteristic* pStopChar = pService->createCharacteristic(
        STOP_CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::WRITE
    );
    pStopChar->setCallbacks(new StopCallbacks(stopSemaphor));



    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->start();
    
    ESP_LOGI(TAG, "Server BLE avviato con controlli PLAY e STOP.");
}