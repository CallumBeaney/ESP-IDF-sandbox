#include <stdio.h>
#include <string.h>

#include "esp_spiffs.h"
#include "esp_log.h"
#include "esp_err.h"

#include <dirent.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#include "SPIFFS.h"

#define TAG "SPIFFS"


void uploadWithSPIFFS(void)
{
  /// Note here nonconcordance of name of ../spiffs_dir and base_path. Is framework convention.
  esp_vfs_spiffs_conf_t config = {
      .base_path = "/spiffs", 
      .partition_label = NULL, /* if NULL, will use first partition it finds in the table CSV */
      .max_files = 5,  /*...that could be open at the same time. */
      .format_if_mount_failed = true,  /* If true, it will format the file system if it fails to mount. */
  };
  ESP_ERROR_CHECK(esp_vfs_spiffs_register(&config));

  /// Again, using framework convention defined by base_path
  FILE *file = fopen("/spiffs/sub/data.txt", "r");
  if(file == NULL) {
    ESP_LOGE(TAG,"could not open file");
  }
  else 
  {
    char line[256];
    while(fgets(line, sizeof(line), file) != NULL) {
      printf(line);
    }
    fclose(file);
  }
  esp_vfs_spiffs_unregister(NULL);
}



void loopOverAndAccessFiles(void) {
  /// Much of this setup is the same as the above function
   esp_vfs_spiffs_conf_t config = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
  };
  esp_vfs_spiffs_register(&config);

  DIR *dir = opendir("/spiffs");
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL)
  {
    /// After getting the full path of the file, get some basic stats/errorcheck
    char fullPath[300];
    sprintf(fullPath,"/spiffs/%s", entry->d_name);
    struct stat entryStat;
    if(stat(fullPath, &entryStat) == -1)
    {
      ESP_LOGE(TAG, "error getting stats for %s", fullPath);
    }
    else
    {
      /// Note the various types in the `stat` type struct.  
      ESP_LOGI(TAG,"full path = %s, file size = %ld", fullPath, entryStat.st_size);
    }
  }
  size_t total = 0, used = 0;
  esp_spiffs_info(NULL, &total, &used);
  ESP_LOGI(TAG, "total = %d, used = %d", total, used);
  
  /// Just print out the contents of that HTML file.
  FILE *file = fopen("/spiffs/index.html", "r");
  if(file == NULL)
  {
    ESP_LOGE(TAG,"could not open file");
  }
  else 
  {
    char line[256];
    while(fgets(line, sizeof(line), file) != NULL)
    {
      printf(line);
    }
    fclose(file);
  }
  esp_vfs_spiffs_unregister(NULL);
}




/// obvs curry nonsense like this in a dev environment where possible
void createReadWriteDelete /* aka CRUD! */(void) {
  /// imagine you want to update a file while working in the field e.g. some HTML when serving a webpage WITHOUT having to reflash the chip.
  ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true};

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK){
        if (ret == ESP_FAIL){
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND){
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    // Use POSIX and C standard library functions to work with files.
    // First create a file.
    ESP_LOGI(TAG, "Opening file");
    FILE *f = fopen("/spiffs/hello.txt", "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    fprintf(f, "Hello World!\n");
    fclose(f);
    ESP_LOGI(TAG, "File written");

    // Check if destination file exists before renaming
    struct stat st;
    if (stat("/spiffs/foo.txt", &st) == 0) { 
      /// stat checks if it exists. Then delete it if it exists
      unlink("/spiffs/foo.txt");
    }

    // Rename original file
    ESP_LOGI(TAG, "Renaming file");
    if (rename("/spiffs/hello.txt", "/spiffs/foo.txt") != 0) {
        ESP_LOGE(TAG, "Rename failed");
        return;
    }

    // Open renamed file for reading
    ESP_LOGI(TAG, "Reading file");
    f = fopen("/spiffs/foo.txt", "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
    char line[64];
    fgets(line, sizeof(line), f);
    fclose(f);
    // strip newline
    char *pos = strchr(line, '\n');
    if (pos)
    {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", line);

    // All done, unmount partition and disable SPIFFS
    esp_vfs_spiffs_unregister(NULL);
    ESP_LOGI(TAG, "SPIFFS unmounted");

}