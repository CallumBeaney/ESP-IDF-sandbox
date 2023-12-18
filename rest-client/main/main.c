#include <stdio.h>
#include "esp_http_client.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "wifi_connect.h"

typedef struct chunk_payload_t {
    uint8_t *buffer;
    int buffer_index;
} chunk_payload_t;

static const char *TAG = "REST";


void app_main(void)
{
  ESP_ERROR_CHECK(nvs_flash_init());
  wifi_connect_init();
  ESP_ERROR_CHECK(wifi_connect_sta("SSID", "password", 10000));
  fetch_quote();
}


void fetch_quote() {
  chunk_payload_t chunk_payload = {0};

  esp_http_client_config_t esp_http_client_config = {
    .url = "http://quotes.rest/qod",
    .method = HTTP_METHOD_GET,
    .event_handler = on_client_data,
    .user_data = &chunk_payload};
  esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);
  esp_http_client_set_header(client, "Content-Type", "application/json");
  esp_err_t err = esp_http_client_perform(client);
  
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "HTTP GET status = %d, result = %s\n", esp_http_client_get_status_code(client),  chunk_payload.buffer);
  } else {
    ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
  }
  
  if(chunk_payload.buffer != NULL) {
      free(chunk_payload.buffer); 
  }
  esp_http_client_cleanup(client);
  wifi_disconnect();
}


// see: https://learnesp32.com/videos/internet-rest-client/chunking-data
esp_err_t on_client_data(esp_http_client_event_t *evt) {
  switch (evt->event_id) {
    case HTTP_EVENT_ON_DATA: 
    { /*   
        1. Access chunk payload: The code extracts the chunk_payload_t structure from the evt->user_data field of the event structure. This structure contains the buffer where received data chunks are accumulated.
        2. Reallocate buffer: If the current buffer is not large enough to accommodate the new data chunk, the realloc() function is used to increase the buffer's size. This ensures that the received data can be stored without overflowing.
        3. Copy data to buffer: The received data chunk is copied to the appropriate position within the reallocated buffer. The memcpy() function is used for efficient byte-wise copying.
        4. Update buffer index: The buffer_index member of the chunk_payload_t structure is incremented to reflect the new position in the buffer. This keeps track of the offset where the next data chunk should be written.
        5. Set null terminator: A null terminator is inserted at the end of the buffer to indicate the end of the accumulated data. This will allow the received data to be treated as a string or a sequence of bytes.
      */
     
      chunk_payload_t *chunk_payload = evt->user_data;
      chunk_payload->buffer = realloc(chunk_payload->buffer, chunk_payload->buffer_index + evt->data_len + 1);
      memcpy(&chunk_payload->buffer[chunk_payload->buffer_index],(uint8_t *) evt->data, evt->data_len );
      chunk_payload->buffer_index = chunk_payload->buffer_index + evt->data_len;
      chunk_payload->buffer[chunk_payload->buffer_index] = 0;

      printf("buffer******** %s\n",chunk_payload->buffer);
    }
    break;

    default:
      break;
  }
  return ESP_OK;
}






/// these don't use chunking of REST return data -- they're just to give an easy entrypoint to revisiting above if necessary.

esp_err_t onClientData_simple(esp_http_client_event_t *data) 
{
  switch (data->event_id) {
    case HTTP_EVENT_ON_DATA:
      ESP_LOGI(TAG, "Length=%d", data->data_len);
      printf("%.*s\n", data->data_len, (char *)data->data);
      break;
    default:
      break;
  }
    return ESP_OK;
}


void fetch_quote_simple(void) 
{
  esp_http_client_method_t method = HTTP_METHOD_GET;
  http_event_handle_cb callback = onClientData_simple;  // function pointer

  esp_http_client_config_t config = {
    .url = "http://quotes.rest/qod", // https://quotes.rest/qod?language=en
    .method = method,
    .event_handler = callback,
  };
  esp_http_client_handle_t client = esp_http_client_init(&config); 
  
  esp_http_client_set_header(client, "Content-Type", "application/json"); // set a K:V pair
  esp_err_t err = esp_http_client_perform(client);

  if (err == ESP_OK) {
    ESP_LOGI(TAG, "HTTP GET status = %d, content_length = %d", (int)esp_http_client_get_status_code(client), (int)esp_http_client_get_content_length(client)); // note int64_t / int problems with ESP_LOG lib here
  } else {
    ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
  }
  
  esp_http_client_cleanup(client); // "Don't call this function if you intend to transfer more files, re-using handles is a key to good performance with esp_http_client."
  wifi_disconnect();   

}
