#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/// Tasks are normally implemented as an infinite loop; the function which implements the task must never attempt to return or exit. Tasks can, however, delete themselves: https://www.freertos.org/a00126.html

void task1(void * params) // note is a pointer to void; allows parameter type flexibility with casting as below
{
  int index = 0;
  while (1)
  {
    printf("1:%i - %s\n", index++, (char *) params);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    if (index == 100) {vTaskDelete( NULL );}
  }
}

void task2(void * params) 
{
  int index = 0;
  while (1)
  {
    printf("2:%i - %s\n", index++, (char *) params);    
    vTaskDelay(2500 / portTICK_PERIOD_MS);
  }
}

void handleTwoTasksConcurrently(void) {

  int SECOND_CORE = 1;
  // xTaskCreate (address of task function, task alias, amount of stackmem allocated within the task, optional parameters, priority where it chooses the highest priority rank, task handle)
   xTaskCreate(&task1, "temperature reading", 2048, "this is task1 string", 2, NULL);
   xTaskCreatePinnedToCore(&task2, "humidity reading", 2048, "...and this is task2 string", 2, NULL, SECOND_CORE); // not advised for wifi/bluetooth stuff
}