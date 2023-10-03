#include <stdio.h>
#include <string.h>
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "Memory"

void checkDynamicMemory(void);  /// DRAM & IRAM

void setTooMuchStackMemory(void);
void checkStackForSpecificTask(void);
void specificTask(void* param); 

void PSRAM(void);
void printMemory(void); 


void app_main()
{
  // checkDynamicMemory;
  // checkStackForSpecificTask();
  // setTooMuchStackMemory();
}


void printMemory() 
{
  /// "I generally write a function like this at the start of every program and use it before every single task"

  ESP_LOGI("memory","stack %d, total ram %d, internal memory %d, external memory %d\n",
  uxTaskGetStackHighWaterMark(NULL), heap_caps_get_free_size(MALLOC_CAP_8BIT),
  heap_caps_get_free_size(MALLOC_CAP_INTERNAL), heap_caps_get_free_size(MALLOC_CAP_SPIRAM)); 
}


void checkDynamicMemory(void) 
{
  /// N.B. Some ESP boards don't have non-static IRAM
  /// Use heap_caps_get_largest_free_block() to get block of free memory; heap frag prevents allocating via heap_caps_GFS()

  ESP_LOGI(TAG, "xPortGetFreeHeapSize %d = DRAM", xPortGetFreeHeapSize()); /// Alternative way to get heap size

  /// heap_caps_GFS() takes all regions capable of having the given capabilities allocated in them and adds up the free space they have.
  int dataRam = heap_caps_get_free_size(MALLOC_CAP_8BIT);
  int instructionRam = heap_caps_get_free_size(MALLOC_CAP_32BIT) - heap_caps_get_free_size(MALLOC_CAP_8BIT);

  ESP_LOGI(TAG, "DRAM \t\t %d (%d Kb)", dataRam, dataRam/1024);
  ESP_LOGI(TAG, "IRam \t\t %d (%d Kb)", instructionRam, instructionRam/1024);
}


void setTooMuchStackMemory(void) 
{
  // char buffer [2222222]; /// more memory than freeRTOS has in this stack
  // memset(&buffer, 1, sizeof(buffer)); /// initialise in memory
  /// PROBLEM: This will crash. The program will run and it will reload after a "panic"
  
  int stackMemory = uxTaskGetStackHighWaterMark(NULL); /// Also takes a taskHandler to check a specific task.
  ESP_LOGI(TAG, "Free memory on the stack:\t%d", stackMemory);
  
  char buffer [100];
  memset(&buffer, 1, sizeof(buffer));
  ESP_LOGI(TAG, "Free memory on the stack after buffer:\t%d", uxTaskGetStackHighWaterMark(NULL));
}



void checkStackForSpecificTask(void) 
{   
    int stackMemory = uxTaskGetStackHighWaterMark(NULL);
    ESP_LOGI(TAG, "initial stack space in host function = %d", stackMemory);
    xTaskCreate(&specificTask, "specific task", /*STACKDEPTH*/ 8000, NULL, 1, NULL);
    ESP_LOGI(TAG, "stack space in host function after executing specific task = %d", uxTaskGetStackHighWaterMark(NULL));
}

void specificTask(void* param) 
{
  ESP_LOGI(TAG, "stack space inside specific task = %d", uxTaskGetStackHighWaterMark(NULL));

  char buffer[5000]; /// even though 8000 bytes of stack space allocated, 7000+ allocated here will overflow.
  memset(&buffer, 1, sizeof(buffer));

  ESP_LOGI(TAG, "stack space inside specific task after memset = %d", uxTaskGetStackHighWaterMark(NULL));

  while(1) vTaskDelay(1000);
}



void PSRAM()
{
  /// This functionality board-dependent --- some don't have extended flash memory
  /// See the README for information on how to configure & compare results of a printMemory

  printMemory();
  char* buffer = heap_caps_malloc(10, MALLOC_CAP_SPIRAM); /// force malloc to heap
}