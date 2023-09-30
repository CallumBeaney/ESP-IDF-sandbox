// C stuff
#include <stdio.h>
#include <string.h> // memset, strlen
#include "math.h" 

// When you setup the CMakeLists.txt for custom folder/files, you need to run `idf.py build` to make the main.c accept the #include
#include "delay.h"
#include "tasks.h"
#include "mutex.h"
#include "queue.h"
#include "sema.h"
#include "../components/externComponents/extern.h"

// ESP stuff
#include "esp_system.h"
#include "esp_random.h"
#include "esp_log.h"

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h" // delays etc
#include "freertos/task.h"

void app_main(void);
void captureInput(void);
void blinker(gpio_num_t PIN);


void app_main(void)
{
  // logInfo();
  // delayDemonstration_vTaskDelay(getDicerollNumber);
  // delayDemonstration_vTaskDelayUntil(10);

  // handleTwoTasksConcurrently();
  // runTwoTasksWithHandlers();

  // mutex();
  // makeQueue();

  // blinker(2);
  // captureInput();
}

void captureInput(void) {
  esp_log_level_set("LOG", ESP_LOG_DEBUG); // causes other error messages to be visible i.e. recall that baud rate issues were reported by the board iff this was set here.

  char c = 0;
  char inputString[100];
  memset(inputString, 0, sizeof(inputString)); // arr no longer filled with random vals

  while(c != '\n') {
    c = getchar(); // getchar() is non-blocking
    
    ESP_LOGD("LOG", "Char: %c", c);
    if (c != 0xff) {  
      // Because non-blocking, while a user isn't typing, 0xff returned.
      inputString[strlen(inputString)] = c;
      printf("%c", c);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }

  printf("Input string: [%s] \n", inputString);
}

void blinker(gpio_num_t PIN) {
  esp_err_t result = gpio_set_direction(PIN, GPIO_MODE_OUTPUT);
  if (result != ESP_OK) {
    ESP_LOGE("PIN ERROR", "Failed to set GPIO Pin %d direction with an error of %s", PIN, esp_err_to_name(result));
    vTaskDelete(NULL);
  }
  ESP_LOGI("PINFO", "Pin %d directionset result: %s", PIN, esp_err_to_name(result));

  uint32_t isOn = 0;
  for (;;) 
  {
    isOn = !isOn; 
    esp_err_t result = gpio_set_level(PIN, isOn);
    if (result != ESP_OK) {
      ESP_LOGE("GPIO ERROR", "Failed to set GPIO Pin %d level with an error of %s", PIN, esp_err_to_name(result));
      vTaskDelete(NULL);
    }
    ESP_LOGI("PINFO", "Pin %d levelset result: %s", PIN, esp_err_to_name(result));

    vTaskDelay(3000 / portTICK_PERIOD_MS);   
  }
} 
