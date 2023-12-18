#include <stdio.h>
#include "esp_wifi.h"
#include "nvs_flash.h"

#define MAX_APs 20

char *getAuthModeName(wifi_auth_mode_t wifi_auth_mode)
{
    switch (wifi_auth_mode)
    {
    case WIFI_AUTH_OPEN:
        return "WIFI_AUTH_OPEN";
    case WIFI_AUTH_WEP:
        return "WIFI_AUTH_WEP";
    case WIFI_AUTH_WPA_PSK:
        return "WIFI_AUTH_WPA_PSK";
    case WIFI_AUTH_WPA2_PSK:
        return "WIFI_AUTH_WPA2_PSK";
    case WIFI_AUTH_WPA_WPA2_PSK:
        return "WIFI_AUTH_WPA_WPA2_PSK";
    case WIFI_AUTH_WPA2_ENTERPRISE:
        return "WIFI_AUTH_WPA2_ENTERPRISE";
    case WIFI_AUTH_WPA3_PSK:
        return "WIFI_AUTH_WPA3_PSK";
    case WIFI_AUTH_WPA2_WPA3_PSK:
        return "WIFI_AUTH_WPA2_WPA3_PSK";
    case WIFI_AUTH_WAPI_PSK:
        return "WIFI_AUTH_WAPI_PSK";
    case WIFI_AUTH_OWE:
        return "WIFI_AUTH_OWE";
    case WIFI_AUTH_MAX:
        return "WIFI_AUTH_MAX";
    }
    return "UNKNOWN";
}

void app_main(void)
{
    nvs_flash_init();

    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT(); // check this struct -- beefy! this macro simplifies
    esp_wifi_init(&wifi_init_config);
    esp_wifi_start();

    wifi_scan_config_t wifi_scan_config = {
      // these params you can change to e.g. filter out certain networks & e.g. only get those SSIDs you care about.
      .show_hidden = true,
      .channel = 13
    };
    esp_wifi_scan_start(&wifi_scan_config, true);

    wifi_ap_record_t wifi_records[MAX_APs]; // you create arr with max number of records allowed on the ESP32
    uint16_t max_record = MAX_APs; // you assign that max val to this tracking index 

    // you get the AP list results found in the last scan & pass to the tracking index the actual max.
    // now you can use this index to traverse the records list as below.
    esp_wifi_scan_get_ap_records(&max_record, wifi_records); 

    printf("Found %d access points:\n", max_record);
    printf("\n");
    printf("               SSID              | Channel | RSSI |   Auth Mode \n");
    printf("----------------------------------------------------------------\n");
    for (int i = 0; i < max_record; i++)
    {
        printf("%32s | %7d | %4d | %12s\n", (char *)wifi_records[i].ssid,
               wifi_records[i].primary, wifi_records[i].rssi,
               getAuthModeName(wifi_records[i].authmode));
    }
    printf("----------------------------------------------------------------\n");
}