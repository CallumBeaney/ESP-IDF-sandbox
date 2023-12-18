#include <stdio.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "protocol_examples_common.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <time.h>
#include "esp_sntp.h" // ESP time handling lib

#define TAG "NTP TIME"

SemaphoreHandle_t got_time_semaphore;

void print_time()
{
    time_t now = 0;
    time(&now);
    struct tm *time_info = localtime(&now);

    char time_buffer[50];
    strftime(time_buffer, sizeof(time_buffer), "%c", time_info); // `string from time`
    ESP_LOGI(TAG, "************ %s ***********", time_buffer);
}

void on_got_time(struct timeval *tv)
{
    printf("on got callback %lld\n", tv->tv_sec);
    print_time();

    xSemaphoreGive(got_time_semaphore);
}

void NTP_main(void)
{
  got_time_semaphore = xSemaphoreCreateBinary();

  setenv("TZ", "AEST-10AEDT-11,M10.5.0/02:00:00,M4.1.0/03:00:00", 1); // sets a global environment variable. See OPNODE timezones
  tzset(); // makes TZ set persistent

  printf("first time print\n");
  print_time(); // this'll give us a 1970 time here, but only the 1st time. After the esp_restart() below the time changes will be persistent -- deep sleep & soft reboots don't reset it; pressing the RESET button will.

  // now connect to the internet
  nvs_flash_init();
  esp_netif_init();
  esp_event_loop_create_default();
  example_connect();

  // now, let's try getting the real current time from a server
  esp_sntp_init();
  esp_sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED); 
  esp_sntp_setservername(0, "pool.ntp.org"); // reach out to this server & get the time. 
  esp_sntp_set_time_sync_notification_cb(on_got_time); // how we know when setServerName() done retrieving time? use this setter

  // now, wait until the semaphore give is carried out in the callback above
  xSemaphoreTake(got_time_semaphore, portMAX_DELAY /*wait forever*/);

  for (int i = 0; i < 5; i++)
  {
      print_time();
      vTaskDelay(pdMS_TO_TICKS(1000));
  }

  esp_restart();
}