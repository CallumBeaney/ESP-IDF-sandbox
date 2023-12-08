#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "freertos/FreeRTOS.h" // delays etc
#include "freertos/task.h" // vTaskDelay

#include "esp_sleep.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/rtc_io.h" // allows us to use pins while sleeping


// note that you must account for the specific chip for this include.
#include "esp32s3/rom/uart.h"
// #include "esp32/rom/uart.h" // uart_tx_wait_idle() 

#include "light.h"


void lightSleepWithTimerHasStartupPausingError(void) {
  // When you run this code, it will pause at onload, and *then* print the first printf. this is because the sleep command inside the timer executes before the printf is able to complete.
  esp_sleep_enable_timer_wakeup(5000000);
  printf("printf1\n");

  int64_t before = esp_timer_get_time();
  esp_light_sleep_start();
  int64_t after = esp_timer_get_time();  

  printf("napped for %lld\n", (after - before) / 1000);
}


void lightSleepWithTimer(void) {
  esp_sleep_enable_timer_wakeup(5000000); // microseconds
  printf("going for a nap\n");
  uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM); /// this prevents the fallthrough problem

  int64_t before = esp_timer_get_time();
  esp_light_sleep_start();
  int64_t after = esp_timer_get_time();
  printf("napped for %lld\n", (after - before) / 1000); // to ms
}


void lightSleepWithGPIO(void) {

  gpio_set_direction(GPIO_NUM_0, GPIO_MODE_INPUT); // pin 0 -- often pinned to the BOOT button on devboards

  gpio_wakeup_enable(GPIO_NUM_0, (gpio_int_type_t) GPIO_INTR_LOW_LEVEL); // when GPIO hits a low val, wake up
  esp_sleep_enable_gpio_wakeup();
  esp_sleep_enable_timer_wakeup(5000000); // microseconds
  
  while(true) {
    /// If the pin goes low, this while loop will just run straight back into the sleep. 
    //  rtc_() can check the pin while it's asleep
    if (rtc_gpio_get_level(GPIO_NUM_0) == 0) {
      printf("please release button\n");
      do {
        vTaskDelay(pdMS_TO_TICKS(10));
      } while (rtc_gpio_get_level(GPIO_NUM_0) == 0);
    }

    printf("going for a nap\n");
    uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);

    int64_t before = esp_timer_get_time();
    esp_light_sleep_start();
    int64_t after = esp_timer_get_time();

    esp_sleep_wakeup_cause_t causeOfWakeup = esp_sleep_get_wakeup_cause();

    // obvs in real world scenario consider switch st etc.
    printf("napped for %lld, reason was %s\n", (after - before) / 1000,
               causeOfWakeup == ESP_SLEEP_WAKEUP_TIMER ? "timer" : "button");
  }
}