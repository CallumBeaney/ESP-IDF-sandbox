#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

EventGroupHandle_t evtGrp;

/// these are register bits -- macros representing individual bits in a binary number whereby the Nth bit is set i.e. the 0th bit is 1, so it's just 0x0...01; BIT1 is 0x0...02, then BIT2 is ...04, BIT3 ...08 and BIT4 ...10.
const int gotHttp = BIT0; /// alternatively consider this as (1<<0) 
const int gotBLE = BIT1; /// this as (1<<1) 

/// think of the event group as a formalised way of using bitmasking to control bits in a variable, using each bit as a flag to coordinate the program's progression. Much like how a singleton is essentially a global variable, it's an ensafed interface on a basic data structure.

void listenForHTTP(void *params)
{
  while (true)
  {
    xEventGroupSetBits(evtGrp, gotHttp); 
    printf("got http\n");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void listenForBluetooth(void *params)
{
  while (true)
  {
    xEventGroupSetBits(evtGrp, gotBLE);
    printf("got BLE\n");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void monitorEventGroup(void *params)
{
  while (true)
  { 
    /// Note here: he's ORing the bits set in the event group handler variable, but then the 4th param asks to wait for all as a way to ensure we want all to be set before we continue.
    xEventGroupWaitBits(evtGrp, gotHttp | gotBLE, true /* clear bits on exit? */, true /* wait for all bits? */, portMAX_DELAY);
    printf("received http and BLE\n");
  }
}

void app_main(void)
{
  evtGrp = xEventGroupCreate();
  xTaskCreate(&listenForHTTP, "get http", 2048, NULL, 1, NULL);
  xTaskCreate(&listenForBluetooth, "get BLE", 2048, NULL, 1, NULL);
  xTaskCreate(&monitorEventGroup, "do something with http", 2048, NULL, 1, NULL);
}