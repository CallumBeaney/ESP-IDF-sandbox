#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"

#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "sdkconfig.h"  

static const char *TAG = "store";
static const char *BASE_PATH = "/store";

void readWriteFunctionality(void);
void readOnlyFunctionality(void);
void write_file(char *path, char *content);
void read_file(char *path);

int main (void) {

}

void readWriteFunctionality(void)
{
    wl_handle_t wl_handle; // could be global if want
    
    /// Here's we're using 
    esp_vfs_fat_mount_config_t esp_vfs_fat_mount_config = {
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
        .max_files = 5,
        .format_if_mount_failed = true,
    };
    esp_vfs_fat_spiflash_mount_rw_wl(BASE_PATH, "storage", &esp_vfs_fat_mount_config, &wl_handle);

    read_file("/store/flash-loaded.txt");
    write_file("/store/text.txt", "Hello world!");
    read_file("/store/text.txt");

    esp_vfs_fat_spiflash_unmount_rw_wl(BASE_PATH, wl_handle);
}

void readOnlyFunctionality(void) {

    // To have RO functionality, you must using the following command in your main/CMakeLists.txt: 
    //     fatfs_create_rawflash_image(storage ../flash-loaded FLASH_IN_PROJECT)

    esp_vfs_fat_mount_config_t esp_vfs_fat_mount_config = {
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
        .max_files = 5,
        .format_if_mount_failed = true,
    };

    // FAT RO
    esp_vfs_fat_spiflash_mount_ro(BASE_PATH, "storage", &esp_vfs_fat_mount_config);

    read_file("/store/flash-loaded.txt");

    esp_vfs_fat_spiflash_unmount_ro(BASE_PATH, "storage");
}


void read_file(char *path)
{
    ESP_LOGI(TAG, "reading file %s", path);
    FILE *file = fopen(path, "r");
    char buffer[100];
    fgets(buffer, 99, file);
    fclose(file);
    ESP_LOGI(TAG, "file contains: %s", buffer);
}

void write_file(char *path, char *content)
{
    ESP_LOGI(TAG, "Writing \"%s\" to %s", content, path);
    FILE *file = fopen(path, "w");
    fputs(content, file);
    fclose(file);
}