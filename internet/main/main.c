#include <stdio.h>
#include "nvs_flash.h" // wifi requires persistent memory. See NVS in memory tutorial for more
#include "esp_wifi.h"
#include "esp_event.h" // for the default event loop
#include "protocol_examples_common.h"
#include "esp_http_client.h"
#include "esp_log.h"

void quick_n_easy(void);
void http_client(void);
esp_err_t client_event(esp_http_client_event_t *evt);



void app_main(void) 
{
  // To get started, go over your your menuconfig either through gear in toolbar or idf.py & go to:
  //    Example Connection Configuration -> set up your SSID & Password 
}


/* LESSON 1 */

void quick_n_easy(void)
{ 
  // this method is not recommended for production, but rather a quick & easy way to get going
  // nonetheless these base 3 methods are all necessary to connect.
  nvs_flash_init();
  esp_netif_init(); // initialise network interface
  esp_event_loop_create_default();

  example_connect();
}


/* LESSON 2 */

// this callback function is where we'll see HTML data coming in from the webpage
esp_err_t client_event(esp_http_client_event_t *evt)
{
  if (evt->event_id == HTTP_EVENT_ON_DATA) { 
    // we're casting evt->data here because the data var is a void pointer owing to the possibility a user might be pulling in binary code etc
    // %.*s takes 2 params: printf("%.*s\n", str_len, str);
    printf("%.*s\n", evt->data_len, (char *)evt->data); 
  } 
  else if (evt->event_id == HTTP_EVENT_DISCONNECTED) { // check the enum for more info
    ESP_LOGE("ERROR", "Error: HTTP event disconnected");
    return ESP_FAIL;
  } else if(evt->event_id == HTTP_EVENT_ERROR){
    ESP_LOGE("ERROR", "Error executing client event");
    return ESP_FAIL;
  }
  return ESP_OK;
}

void http_client(void) {
  nvs_flash_init();
  esp_netif_init();
  esp_event_loop_create_default();

  example_connect();

  // note that if you use httpS, and don't supply the site with a cert, you'll get an error.
  // you could solve this by just using `http:// ...` instead. 
  //    or, you could go into component config -> TLS -> allow potentially insecure options -> skip server cert verif by default
  // the above IS NEVER OK FOR PROD IF YOU USE IT YOU DESERVE THE PAIN
  esp_http_client_config_t esp_http_client_config = {
      .url = "https://google.com", 
      .event_handler = client_event,
  };
  esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);
  esp_http_client_perform(client);
  esp_http_client_cleanup(client);
}