#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include <netdb.h>


char *get_wifi_disconnection_string(wifi_err_reason_t wifi_err_reason);


// ___________ globals ___________

static char *TAG = "WIFI CONNECT";

static esp_netif_t *esp_netif;

static EventGroupHandle_t wifi_events;
static int CONNECTED = BIT0;
static int DISCONNECTED = BIT1;

int disconnection_err_count = 0;
static bool attempt_reconnect = false;


// ___________ functions begin ___________

/// @brief this is called when an event is pushed to the queue, using the event_Handler_Register functions below. f12 into the `esp_event_handler_t` expected by these functions, and you'll see that their handler param is a FP returning void, with the below params. 
/// @param event_handler_arg 
/// @param event_base e.g. `WIFI_EVENT`, `IP_EVENT` etc. An `esp_event_base_t` for the category of events you wish to limit your scope to.
/// @param event_id this is the enum ID of the actual event detected by the registered event handler
void event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    // See the diagrams linked in `wifi_init.c`. each time this handler is called by `wifi_connect_sta()`, assuming successful procedure, the current case->event id will progress down this switch statement. Once it gets the IP, we can change the event group handler's bits & confirm in our wifi_init.c->init_main() that we've connected by means of returning an errortype OK.

    switch (event_id) 
    {
        case WIFI_EVENT_STA_START:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
            esp_wifi_connect();
            break;
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
            disconnection_err_count = 0;
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
        {
            wifi_event_sta_disconnected_t *wifi_event_sta_disconnected = event_data;
            ESP_LOGW(TAG, "DISCONNECTED %d: %s", wifi_event_sta_disconnected->reason,
                    get_wifi_disconnection_string(wifi_event_sta_disconnected->reason));
            
            if (attempt_reconnect == true) {
                if (wifi_event_sta_disconnected->reason == WIFI_REASON_NO_AP_FOUND ||
                    wifi_event_sta_disconnected->reason == WIFI_REASON_ASSOC_LEAVE ||
                    wifi_event_sta_disconnected->reason == WIFI_REASON_AUTH_EXPIRE  )
                {
                    if (disconnection_err_count++ < 5) {
                        vTaskDelay(pdMS_TO_TICKS(5000));
                        esp_wifi_connect();
                        break;
                    } else {
                        ESP_LOGE(TAG, "WIFI retries exceeded");
                    }
                }
            }
            xEventGroupSetBits(wifi_events, DISCONNECTED);
            break;
        }
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
            xEventGroupSetBits(wifi_events, CONNECTED);
            break;
        default:
            break;
    }
}


esp_err_t wifi_connect_sta(char *ssid, char *password, int timeout)
{   
    attempt_reconnect = true;
    wifi_events = xEventGroupCreate();
    esp_netif = esp_netif_create_default_wifi_sta();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    // for static ip:
    // ESP_ERROR_CHECK(esp_netif_dhcpc_stop(esp_netif));
    // esp_netif_ip_info_t ip_info;
    // ip_info.ip.addr = ipaddr_addr("192.168.43.150");
    // ip_info.gw.addr = ipaddr_addr("192.168.43.1");
    // ip_info.netmask.addr = ipaddr_addr("255.255.255.0");
    // ESP_ERROR_CHECK(esp_netif_set_ip_info(esp_netif, &ip_info));

    // note `wifi_config_t` is a union
    wifi_config_t wifi_config = {}; // remember: every variable in this instance is null from using empty {}
    
    // copy params into config struct:  note max arrlen of sta.ssid & sta.password; you should be errchecking fn inputs!
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1 /* remember: nullterm */);
    strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1); 

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    EventBits_t result = xEventGroupWaitBits(wifi_events, CONNECTED | DISCONNECTED, true, false, pdMS_TO_TICKS(timeout));
    if (result == CONNECTED) { 
        return ESP_OK;
    }
    return ESP_FAIL;
}


void wifi_connect_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default()); // this is the infrastructure for everything else.

    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config)); 

    // See also e.g. WIFI_EVENT_SCAN_DONE in wifi_event_t
    // ESP_EVENT_ANY_ID says "notify me about any/all of (param1 "event base") WIFI_EVENT category activity.
    // the final param per docs are additional params to pass into event handler; these would be passed in as `void *event_handler_arg` as in our event_handler() function above
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, NULL));

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event_handler, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
}


void wifi_connect_ap(const char *ssid, const char *pass)
{
    esp_netif = esp_netif_create_default_wifi_ap();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    wifi_config_t wifi_config = {}; // all struct vars set to 0
    strncpy((char *)wifi_config.ap.ssid, ssid, sizeof(wifi_config.ap.ssid) - 1);
    strncpy((char *)wifi_config.ap.password, pass, sizeof(wifi_config.ap.password) - 1);

    // PSK = preSharedKey i.e. the type of encryption used. 
    // WPA 2 is the current standard, plain WPA is old & less safe. WPA 3 is not supported on all setups.
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK; 
    wifi_config.ap.max_connection = 4;
    wifi_config.ap.beacon_interval = 100; // how often the ESP32 beacons as an access point, in ms
    wifi_config.ap.channel = 1; // ESP32 is a 2.4gHz device -- Europe standard is channels 1-11, elsewhere 1-13.

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    // ESP should now be accessible to other devices to connect, giving network access to them.
}


void wifi_disconnect(void)
{
    attempt_reconnect = false;
    esp_wifi_stop();
    esp_netif_destroy(esp_netif);
}