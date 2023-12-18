// now we're trying to make an interface for handling events -- the IDF functionality to e.g. get an IP addr cannot interrupt our app code directly, and so instead we must listen for events and change our behaviour accordingly. 
// SEE: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/wifi.html#esp32-wi-fi-programming-model
// Note the relationship between application code, and the wifi API on the IDF's side, and the event return of both.
// NOW SEE: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/wifi.html#esp32-wi-fi-station-general-scenario
// Note how the middle event loop is sent data from the WIFI task, which can then be passed to the App task (user's code) via this event task
#include <stdio.h>
#include "nvs_flash.h"
#include "wifi_connect.h"

void init_main(void) {
    nvs_flash_init();
    wifi_connect_init();

    while (true)
    {

        wifi_connect_ap("myEsp32AP", "password");
        vTaskDelay(pdMS_TO_TICKS(10000));
        wifi_disconnect();

        wifi_connect_sta("POCO", "password", 10000);
        vTaskDelay(pdMS_TO_TICKS(20000));
        wifi_disconnect();
    }
}