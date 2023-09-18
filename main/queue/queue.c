#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/// Essentially a singleton with an array. Can be used to pass or receive data between distinct operations.
/// Follow "SEQ" messages for priority notes.

QueueHandle_t queueDS;


void listenForHTTP(void *params)
{
  int count = 0;
  while (true)
  {
    count++;
    long ok = xQueueSend(queueDS, &count, 1000 / portTICK_PERIOD_MS); /// SEQ: 1
    /// if the number of xQueueSend operations to a given handler (1st param) is greater than the number assigned at instantiation, it will not segfault; fail messages will be returned.
    if (ok)
    {
      printf("listener:\tSUCCESS adding message to queue\n"); /// SEQ: 3
    } 
    else 
    {
      printf("listener:\tFAILURE to add message to queue\n");
    }
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void queueDataProcessor(void *params)
{
  while (true)
  {
    int rxInt;
    /// xQR(global var; addr of local variable to pass to from global var; delay period)
    /// while the queue is empty, xQR sleeps -- does not consume cycles
    if (xQueueReceive(queueDS, &rxInt /* the value passed by xQueueSend */, 5000 / portTICK_PERIOD_MS)) /// SEQ: 2
    {
      printf("processor:\tdoing something with http %d\n", rxInt);
    }
  }
}

void makeQueue(void)
{
  queueDS = xQueueCreate(3, sizeof(int));
  xTaskCreate(&listenForHTTP, "get http", 2048, NULL, 2, NULL); /// SEQ: if you wish for SEQ 1&3 to become SEQ 1&2, raise the priority of this xTC.
  xTaskCreate(&queueDataProcessor, "do something with http", 2048, NULL, 1, NULL);
}