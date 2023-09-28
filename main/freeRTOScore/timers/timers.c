#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_timer.h" // For IDF 4 use #include "esp_system.h"


void on_timer(TimerHandle_t xTimer)
{
  printf("time hit %lld\n", esp_timer_get_time() / 1000); /// μs --> ms

}

void freeRTOStimerDemonstration(void)
{
  printf("app started %lld\n", esp_timer_get_time() / 1000 ); 
  /// xTC(alias, howlong before fire, repeat/auto-reload?, settable ID, callback fn)
  TimerHandle_t xTimer = xTimerCreate("my timer", pdMS_TO_TICKS(1000), true, NULL, on_timer); 
  xTimerStart(xTimer, 0); // instantiation alone won't run it; must be manually started

  /// xTimer is run on an idle task, so do not want any interrupts in here. Good time to fire semaphores 
}