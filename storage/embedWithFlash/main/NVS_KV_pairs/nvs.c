#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "nvs.h"

#define TAG "NVS"
#define NVS_PARTITION "NVSPartitionNameInCSVConfig"


void simple_NVS_dataHandling (void) {

  // What this specific "val" logic does is just check how many times "val" has been accessed by the program; remember that each reboot unless some condition changes, so this would increment the value in they key "val" until stopped. 

  // vTaskDelay(1000 / portTICK_PERIOD_MS); 
  /// If you don't begin with this delay, you'll notice that your first print output for Val will be "1". This is because in `idf.py erase_flash flash monitor`, your Flash runs, and the application starts running -- Monitor then kicks in and resets, meaning it already incremented from the basestate 0.

  ESP_ERROR_CHECK(nvs_flash_init());

  nvs_handle_t handle;
  ESP_ERROR_CHECK(nvs_open("store", NVS_READWRITE, &handle)); /// note 1st param for namespace alias

  nvs_stats_t nvsStats;
  nvs_get_stats(NULL, &nvsStats); /// for default partition, NULL is fine

  int32_t val = 0; 
  esp_err_t result = nvs_get_i32(handle, "val", &val); /// nvs_get_ has many specifiable types inc blobs
  switch (result)
  {
    case ESP_ERR_NVS_NOT_FOUND:
      ESP_LOGE(TAG, "Value not set yet");
      break;
    case ESP_OK:
      ESP_LOGI(TAG, "Value is %ld", val);
      break;
    default:
      ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(result));
      break;
  }
  val++;
  ESP_ERROR_CHECK(nvs_set_i32(handle, "val", val));
  ESP_ERROR_CHECK(nvs_commit(handle));
  nvs_close(handle);
}


void handling_NVS_partitionedData (void) {
  ESP_ERROR_CHECK(nvs_flash_init_partition(NVS_PARTITION));

  nvs_handle_t handle;
  ESP_ERROR_CHECK(nvs_open_from_partition(NVS_PARTITION, "nameSpaceAlias", NVS_READWRITE, &handle));

  nvs_stats_t nvsStats;
  nvs_get_stats("customPartitionAliasStoredInCSVConfig", &nvsStats);

  ESP_LOGI(TAG, "used: %d, free: %d, total: %d, namespace count: %d", nvsStats.used_entries,
           nvsStats.free_entries, nvsStats.total_entries, nvsStats.namespace_count);

  /// After here you can R/W to the KV pairs.
}


void handleOtherFileComponents (void) {
  /// SEE: main/files/ directory; main/component.mk
  
  /// As with e.g. IFNDEF macros with header files, this assembly conversion command takes the . in your file (_binary_{your_file}_start) and converts it to an underscore.
  /// _start gives us the start of the memory address
  extern const unsigned char indexHtml[] asm("_binary_index_html_start");
  printf("html = %s\n", indexHtml);

  extern const unsigned char sample[] asm("_binary_sample_txt_start");
  printf("sample = %s\n", sample);

  /// These are marked as chars, but they really function as just pointers here; that's why you can do your imgEnd-imgStart thing -- you're just subtracting memory addresses from each other.

  /// see component.mk -- we have to find the start & end of flash-embedded components not marked as text files (i.e. TXT, HTML, CSS). 
  extern const unsigned char imgStart[] asm("_binary_pinout_jpg_start");
  extern const unsigned char imgEnd[] asm("_binary_pinout_jpg_end");
  const unsigned int imgSize = imgEnd - imgStart;
  printf("img size is %d\n", imgSize);
 
}



typedef struct cat_struct {
  char name[30];
  int age;
  int id;
} Cat;

void handleStructsInNVS (void) 
{
  vTaskDelay (1000 / portTICK_PERIOD_MS);
  ESP_ERROR_CHECK(nvs_flash_init_partition(NVS_PARTITION));
  nvs_handle handle;
  ESP_ERROR_CHECK(nvs_open_from_partition(NVS_PARTITION, "cat_store", NVS_READWRITE, &handle));

  char catKey [15]; /// Note that the key param in nvs_get functions has a max length of 50 bytes.
  Cat cat;
  
  /// Set structs in nonvol storage
  for (int i = 0; i < 5; i++)
  {
    sprintf(catKey, "cat_%d", i);
    Cat newCat;
    sprintf(newCat.name,"Mr cat %d", i);
    newCat.age = i + 2;
    newCat.id = i;
    ESP_ERROR_CHECK(nvs_set_blob(handle, catKey, (void *) &newCat, sizeof(Cat) ));
    ESP_ERROR_CHECK(nvs_commit(handle));
  }

  /// Not get them
  size_t catSize = sizeof(Cat); /// note this difference in pointer usage between getter & setter functions
  for (int i = 0; i < 5; i++) {
    sprintf(catKey, "cat_%d", i);
    esp_err_t result = nvs_get_blob(handle, catKey, (void *) &cat, &catSize);

    switch (result)
    {
    case ESP_ERR_NOT_FOUND:
    case ESP_ERR_NVS_NOT_FOUND: // pay attn to this errortype; they expand to different hex commands
      ESP_LOGE(TAG, "Value not set yet");
      break;
    case ESP_OK:
      ESP_LOGI(TAG, "cat name: %s, age %d, id %d", cat.name, cat.age, cat.id);
      break;
    default:
      ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(result));
      break;
    }
  }
  nvs_close(handle);
}
