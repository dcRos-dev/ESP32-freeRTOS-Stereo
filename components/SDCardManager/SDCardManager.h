#pragma once


#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "hal/gpio_types.h"


class SDCardManager
{
private:
    // salviamo la sdmmc_card della microSD
    // e il mount_point nel VFS
    sdmmc_card_t *card;
    const char *mount_point;

    // SD Reader è basato su SPI
    // quindi definisco SPI prot. pins: 
    gpio_num_t mosi_pin;
    gpio_num_t miso_pin;
    gpio_num_t clk_pin;
    gpio_num_t cs_pin;


public:
    SDCardManager(const char *mountPath, gpio_num_t mosi, gpio_num_t miso, gpio_num_t clk, gpio_num_t cs );
    ~SDCardManager();

    bool init();
    void listFiles();
};

