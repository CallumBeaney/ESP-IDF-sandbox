#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "esp_sleep.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/rtc_io.h" // allows us to use pins while sleeping
#include "esp32s3/rom/uart.h" // note that you must account for the specific chip for this include.


RTC_DATA_ATTR int timesWokenUp = 0;
gpio_num_t BUTTON_1   = GPIO_NUM_0; // 例えだけです
gpio_num_t BUTTON_2 = GPIO_NUM_5; // 上記と同じ



void deepSleepTimer(void) {
  esp_sleep_enable_timer_wakeup(5 * 1000000); // 5 * 1 microsecond
  printf("going to sleep. woken up %d times\n", timesWokenUp++);

  esp_deep_sleep_start(); // the ESP will essentially reset with this. The program acts as if it gets reset, but this doesn't mean you can't store information

  /// the MC will as such never get here; these will never print
  printf("waking up\n");
  vTaskDelay(pdMS_TO_TICKS (1000));
}


void buttonWithEXT0_noOtherOperations(void) {
  /// say we've got a button hooked to a pin and out to ground. We're going to enable the PU-resistor on it, bringing the pin high while the button is open (unpressed), and then, When we press the button, it's going to be connected to ground, so it will bring that line to zero:

  /*
    __________            __________  1
              \          /              
               \________/
              ^                       0 
         button press                   */

  rtc_gpio_deinit(BUTTON_1); 

  // the only difference between rtc_ & gpio_pullup_en() in the GPIO dir is that this `rtc` prefix indicates we can use this even while the ESP is asleep
  rtc_gpio_pullup_en(BUTTON_1); 
  rtc_gpio_pulldown_dis(BUTTON_1); // good practice

  /* This function uses external wakeup feature of RTC_IO peripheral. It will work only if RTC peripherals are kept on during sleep. This feature can monitor any pin which is an RTC IO. */
  esp_sleep_enable_ext0_wakeup(BUTTON_1, 0 /* input level which will trigger wakeup (0=low, 1=high) */);
  printf("going to sleep. woken up %d times\n", timesWokenUp++);

  esp_deep_sleep_start();
}



void buttonWithEXT0_otherOperations(void) {
  // see: buttonWithEXT0_noOtherOperations()
  // now, when we run that code, we're sending the line low when the button's pushed. 
  // But we might want to do something with the pin as a general GPIO while the button's high. Everything below `/// sleep handling` will interrupt with that. So...

  rtc_gpio_deinit(BUTTON_1);

  /* 
      Now, within here, you can use BUTTON pin as a GPIO.
      When you get down to sleep handling, it will set it up as an RTC pin.
  */

  /// sleep handling
  rtc_gpio_deinit(BUTTON_1); 
  rtc_gpio_pullup_en(BUTTON_1); 
  rtc_gpio_pulldown_dis(BUTTON_1); // good practice
  esp_sleep_enable_ext0_wakeup(BUTTON_1, 0 /* input level which will trigger wakeup (0=low, 1=high) */);
  printf("going to sleep. woken up %d times\n", timesWokenUp++);

  esp_deep_sleep_start();
}



void buttonsWithEXT1(void) {
  /// imagine you have 2 buttons hooked up to pins, and then to ground. When you run this code, the chip will go to sleep, until both buttons are pressed simultaneously, like an AND switch. 

  rtc_gpio_deinit(BUTTON_1);
  rtc_gpio_deinit(BUTTON_2);

  /* 
      general gpio functions using the pins go here
  */

  /// one of the primary differences between ext1 and ext0 is ext0 turns on RTC peripherals, whereas ext1 does not. This means we must configure the power domain of the RTC manually. We use esp_sleep_pd_config() to do this (check the README.md here).  
  /// There are 2 params to this function. The first concerns the power domain used. A power domain is a grouping of devices or circuits that share a common power supply. Remember now that we are concerned with RTC. 
  /// Well, when we use ext1, the RTC's power domain, aliased as ESP_PD_DOMAIN_RTC_PERIPH (i.e. RTC IO, sensors and ULP co-processor) is turned off. So we switch it back on by passing that as the first param.
  /// Then, we choose ESP_PD_OPTION_ON which will keep the power domain enabled in sleep mode
  esp_sleep_pd_domain_t domain = ESP_PD_DOMAIN_RTC_PERIPH;
  esp_sleep_pd_option_t option = ESP_PD_OPTION_ON;
  esp_sleep_pd_config(domain, option);

      /* You could invert this setup from having a 2-button AND onswitch, to an OR switch that wakes the chip up, by disabling the pullups and enabling the pulldowns, and then changing `mode` to ESP_EXT1_WAKEUP_ANY_HIGH */

  rtc_gpio_pullup_en(BUTTON_1);
  rtc_gpio_pulldown_dis(BUTTON_1);
  rtc_gpio_pullup_en(BUTTON_2);
  rtc_gpio_pulldown_dis(BUTTON_2);

  esp_sleep_ext1_wakeup_mode_t mode = ESP_EXT1_WAKEUP_ANY_LOW; // see this typedef for more info

  // build a mask from each button. uint64_t is just a typedef of an unsigned long long
  // The 1ULL operand represents the value 1 as an unsigned 64-bit integer. Shifting this value by BUTTON_1 positions sets the BUTTON_1th bit to 1 and leaves all other bits unchanged. Similarly, shifting 1 by BUTTON_2 positions sets the BUTTON_2th bit to 1.
  uint64_t mask = (1ULL << BUTTON_1) | (1ULL << BUTTON_2); 

  esp_sleep_enable_ext1_wakeup(mask, mode);
  printf("going to sleep. woken up %d\n", timesWokenUp++);

  esp_deep_sleep_start();
}


void hibernation(void) {

  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  /// TODO: find out what's wrong with these ifndefs in the new IDF ver's `esp_sleep_pd_domain_t` enum
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_enable_timer_wakeup(5 * 1000000);
  printf("going to sleep. woken up %d\n", timesWokenUp++);

  esp_deep_sleep_start();
}