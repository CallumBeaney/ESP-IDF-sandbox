#include <stdio.h>
#include "math.h" 

#include "esp_system.h"
#include "esp_random.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "DELAY" 

void app_main(void);

void logInfo(void);
int  getDicerollNumber(void);
void delay_vTaskDelay(int (*getNewNumber)(void));
void delay_vTaskDelayUntil(TickType_t);


void app_main(void)
{
  delay_vTaskDelay(getDicerollNumber);
  // delay_vTaskDelayUntil(10);

}

void delay_vTaskDelayUntil(TickType_t freq ) {
  // This is preferable for fixed frequency execution using abs rather than the relative time at which calling task should unblock
  TickType_t lastWokeTime = xTaskGetTickCount();
  int i = 0;
  while(1) {
    ESP_LOGI(TAG, "in loop %d", i++);
    vTaskDelayUntil(&lastWokeTime, freq); /// Block after each log
  }
}

void delay_vTaskDelay(int (*getNewNumber)(void)) {
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