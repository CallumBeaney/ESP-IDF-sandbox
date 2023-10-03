#include "esp_random.h"
#include "freertos/FreeRTOS.h" // delays etc
#include "freertos/task.h"
#include "esp_log.h"

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


