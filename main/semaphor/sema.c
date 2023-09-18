#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

SemaphoreHandle_t binarySemaphor;

void httpListener(void * params)
{
  while (1) {
    printf("listener:\t received message\n");
    xSemaphoreGive(binarySemaphor);
    printf("listner:\t  processed message\n"); /// if the associated xTaskCreate has a priority greater than httpProcessor's xTaskCreate, this print statement will be executed BEFORE the above xSemaphorGive's taker function can actuate its own functionality. 
    vTaskDelay(5000 /* 5s */ / portTICK_PERIOD_MS); 
  }
}

void httpProcessor (void * params) {
  while (1) {
    xSemaphoreTake(binarySemaphor, portMAX_DELAY); // non-blocking; sleeps until another function Gives() it the sema
    printf("processor:\t  processing message\n");
  }
}

void semaphorDemonstration(void) {
  binarySemaphor = xSemaphoreCreateBinary();
  xTaskCreate(&httpListener, "get http", 2048, NULL, 1 /* lowest priority lvl */, NULL);
  xTaskCreate(&httpProcessor, "do something with http", 2048, NULL, 1, NULL);
}