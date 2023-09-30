#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

SemaphoreHandle_t mutexBus;

void writeToBus(char *message)
{
  /// imagine this is e.g. an i2c bus
  printf(message);
}

void task1(void *params)
{
  while (true)
  {
    printf("reading temperature \n");
    if (xSemaphoreTake(mutexBus, 1000 / portTICK_PERIOD_MS))
    {
      /// once acquired, this thread will be locked until it is manually released
      /// were you to not release, this printf would appear once and only once.
      writeToBus("temperature is 25c\n");
      xSemaphoreGive(mutexBus); 
    }
    else 
    {
      /// the semaphore take attempt timed out
      printf("writing temperature timed out \n");
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void task2(void *params)
{
  while (true)
  {
    printf("reading humidity\n");
    if (xSemaphoreTake(mutexBus, 1000 / portTICK_PERIOD_MS))
    {
      writeToBus("humidity is 50 \n");
      xSemaphoreGive(mutexBus);
    }
    else
    {
      printf("writing humidity timed out \n");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void mutex(void)
{
  mutexBus = xSemaphoreCreateMutex(); // If creating a binary semaphor, need different fn(). See sema.c
  xTaskCreate(&task1, "temperature reading", 2048, NULL, 2, NULL);
  xTaskCreate(&task2, "humidity reading", 2048, NULL, 2, NULL);
}