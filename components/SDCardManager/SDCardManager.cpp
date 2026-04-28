#include "SDCardManager.h"

#include <iostream>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include "esp_log.h"
#include "driver/spi_master.h"


static const char *TAG = "SD_MANAGER";

SDCardManager:: SDCardManager(const char *mountPath, gpio_num_t mosi, gpio_num_t miso, gpio_num_t clk, gpio_num_t cs ) 
: card(nullptr), mount_point(mountPath), mosi_pin(mosi), miso_pin(miso), clk_pin (clk), cs_pin(cs) {}


SDCardManager::~SDCardManager(){
    // Prima di distruggere l'oggetto SDCardManager
    // è necessario smontare il percorso della microSD
    // se presente
    if(card != nullptr){
        esp_vfs_fat_sdcard_unmount(mount_point, card);
        ESP_LOGI(TAG, "Scheda SD smontata");
    }

}

bool SDCardManager::init() {
    ESP_LOGI(TAG, "Inizializzazione SPI e montaggio SD...");

    // CORREZIONE 1: Inizializziamo tutto a 0 per evitare l'errore dei campi mancanti
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {}; 
    mount_config.format_if_mount_failed = false;
    mount_config.max_files = 5;
    mount_config.allocation_unit_size = 16 * 1024;

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.max_freq_khz = 5000;

    spi_bus_config_t bus_cfg = {};
    bus_cfg.mosi_io_num = mosi_pin;
    bus_cfg.miso_io_num = miso_pin;
    bus_cfg.sclk_io_num = clk_pin;
    bus_cfg.quadwp_io_num = -1;
    bus_cfg.quadhd_io_num = -1;
    bus_cfg.max_transfer_sz = 4000;

    esp_err_t ret = spi_bus_initialize((spi_host_device_t)host.slot, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Errore inizializzazione bus SPI.");
        return false;
    }

    // CORREZIONE 2: Lasciamo solo UNA dichiarazione di slot_config
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = cs_pin;
    slot_config.host_id = (spi_host_device_t)host.slot;

    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Errore montaggio filesystem. Controlla i pin e la formattazione (FAT32).");
        return false;
    }

    ESP_LOGI(TAG, "SD montata con successo. Info carta:");
    sdmmc_card_print_info(stdout, card);
    return true;
}


void SDCardManager::listFiles() {
    ESP_LOGI(TAG, "Contenuto della directory %s:", mount_point);
    
    struct dirent *entry;
    DIR *dir = opendir(mount_point);

    if (dir == nullptr) {
        ESP_LOGE(TAG, "Impossibile aprire la directory.");
        return;
    }

    bool hasFiles = false;
    while ((entry = readdir(dir)) != nullptr) {
        hasFiles = true;
        if (entry->d_name[0] == '.') continue;
        
        std::cout << " - " << entry->d_name;
        
        if (entry->d_type == DT_DIR) {
            std::cout << " [CARTELLA]\n";
        } else {
            std::cout << " [FILE]\n";
        }
    }
    
    if (!hasFiles) {
        std::cout << "La SD è vuota.\n";
    }

    closedir(dir);
}

