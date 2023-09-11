#include <stdio.h>
#include <string.h> // memset, strlen
#include "math.h" 
#include "helpers.h"

#include "esp_system.h"
#include "esp_random.h"
#include "esp_log.h"

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h" // delays etc
#include "freertos/task.h"

void app_main(void);

void delayDemonstration_vTaskDelayUntil(TickType_t);
void delayDemonstration_vTaskDelay(int(*)(void));
int  getDicerollNumber(void);

void logInfo(void);
void blinker(gpio_num_t PIN);
void captureInput(void);


void app_main(void)
{
  // delayDemonstration_vTaskDelay(getDicerollNumber);
  // delayDemonstration_vTaskDelayUntil(10);
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

void delayDemonstration_vTaskDelayUntil(TickType_t freq) {
  // This is preferable for fixed frequency execution using abs rather than the relative time at which calling task should unblock
  TickType_t lastWokeTime = xTaskGetTickCount();
  int i = 0;
  while(1) {
    ESP_LOGI("LOOP", "in loop %d", i++);
    vTaskDelayUntil(&lastWokeTime, freq); /// Block after each log
  }
}

void delayDemonstration_vTaskDelay(int (*getNewNumber)(void)) {
  for (;;)
  {
    int number = getNewNumber();
    ESP_LOGI("DICE", "random # :  %d", number);
    
    // vTaskDelay() specifies a time at which the task wishes to unblock relative to the time at which vTaskDelay() is called.
    vTaskDelay(/* 1000 == 1s */ 500 / portTICK_PERIOD_MS /* portTICK is defined at compile */);   
    vTaskDelay(pdMS_TO_TICKS(500)); 
    
    /// "I think the only compelling reason for using pdMS...() is that the macro does the multiplication first and can be used with tick rates higher than 1000Hz without a division by zero error."
    // pdMS_TO_TICKS() usage requires configTICK_RATE_HZ <= 1000
  }
}

int getDicerollNumber(void) {
  return abs((int)esp_random()) % 6 + 1;
}

void logInfo(void)
{
  // esp_log_level_set("LOG", ESP_LOG_INFO);
  // Does not override menuconfig minimum set level
  //  e.g. if menuconfig is set to INFO, setting above to ESP_LOG_VERBOSE is nothing doing.
  ESP_LOGE("LOG", "This is an error");
  ESP_LOGW("LOG", "This is a warning");
  ESP_LOGI("LOG", "This is an info");
  ESP_LOGD("LOG", "This is a Debug");
  ESP_LOGV("LOG", "This is Verbose");

  int number = 0;
    // esp_log_level_set("TAG 2", ESP_LOG_VERBOSE);
  ESP_LOGE("TAG 2", "This is an error %d", number++);
  ESP_LOGW("TAG 2", "This is a warning %d", number++);
  ESP_LOGI("TAG 2", "This is an info %d", number++);
  ESP_LOGD("TAG 2", "This is a Debug %d", number++);
  ESP_LOGV("TAG 2", "This is Verbose %d", number++);
}