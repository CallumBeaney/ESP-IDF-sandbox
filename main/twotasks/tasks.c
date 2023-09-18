#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* READ BOTTOM -> TOP */

/// xTaskCreate (address of task function, task alias, amount of stackmem allocated within the task, optional parameters, priority where it chooses the highest priority rank, task handle)
/// Tasks are normally implemented as an infinite loop; the function which implements the task must never attempt to return or exit. Tasks can, however, delete themselves: https://www.freertos.org/a00126.html

//// Here is a guide to running two tasks + also using handlers

static TaskHandle_t receiverHandler = NULL; /// globalise just for convenience here

void sender(void * params) {

  /// The number of xTNG you use will be reflected the return of a pdFALSE; 3 instances in here will be rendered in the order of 3, 2, 1. 
  /// Remember: pdFALSE commands blocking; the most recently sent command will print first out the stack.

  /// xTaskNotify (with no "give") OTOH is 

  while (1) {
    // xTaskNotifyGive(receiverHandler);
    // vTaskDelay(6000 / portTICK_PERIOD_MS);

    /*  xTN allows better control flow
      eSetBits may also be used for the 3rd "eAction" param
      pair this with bitshifting for the 2nd ulValue param  */
    xTaskNotify(receiverHandler, (1<<0), eSetValueWithOverwrite);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    xTaskNotify(receiverHandler, (1<<2), eSetValueWithOverwrite);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    xTaskNotify(receiverHandler, (1<<4), eSetValueWithOverwrite);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

}

void receiver(void * params) {

  uint state;
  while (1) 
  { /*  ulTaskNotifyTake(
          boolean that says "do not reset" if false,
          delay period [here blocks until xTaskNotifyGive actuated; forthi no vTaskdelay needed],
        );  */
    // int countTake = ulTaskNotifyTake(pdFALSE, portMAX_DELAY); 
    // printf("received notification: %d\n", countTake);
    
  /*  xTaskNotifyWait(ulBitsToClearOnEntry,ulBitsToClearOnExit,pulNotificationValue,xTicksToWait)
      This allows you to pass simple state parameters around.
      Let's say you want to clear the first 4 bits -- use xTaskNotifyWait(0xffffffff, 0, &state, portMAX_DELAY); 
  */ 
    xTaskNotifyWait(0, 0, &state /* this is receiving the 2nd ulValue param of xTaskNotify */, portMAX_DELAY); 
    if (state != 16) {
      printf("received state: %d\n", state);
    } else {
      printf("state is 16 (1<<4) \n");
    }
  }

}

void runTwoTasksWithHandlers(void)
{
  xTaskCreate(&receiver, "receiver", 2048, NULL, 2, &receiverHandler);
  xTaskCreate(&sender, "sender", 2048, NULL, 2, NULL);
}

// __________________________________________________________________


//// Here is a guide just to running two tasks concurrently

void task1(void * params) // note is a pointer to void; allows parameter type flexibility with casting as below
{
  int index = 0;
  while (1)
  {
    printf("Checking index...\n");
    index++;

    printf("1:%i - %s\n", index++, (char *) params);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    // if (index == 100) {vTaskDelete( NULL );}
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
  /// xTaskCreate (address of task function, task alias, amount of stackmem allocated within the task, optional parameters, priority where it chooses the highest priority rank, task handle)
   xTaskCreate(&task1, "temperature reading", 2048, "this is task1 string", 2, NULL);
   xTaskCreatePinnedToCore(&task2, "humidity reading", 2048, "...and this is task2 string", 2, NULL, SECOND_CORE); // not advised for wifi/bluetooth stuff
}