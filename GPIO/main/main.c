#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"

#include "./interrupts/interrupts.h"


#define PIN_SWITCH 15
#define PIN_LED 2 // on board LED

void GPIO_input(void);


void app_main()
{
   
}


void pinConfigExample(void) 
{    
    // gpio_pad_select_gpio(PIN_SWITCH);
    // gpio_set_direction(PIN_SWITCH, GPIO_MODE_INPUT);
    // gpio_pulldown_en(PIN_SWITCH);
    // gpio_pullup_dis(PIN_SWITCH);
    // gpio_set_intr_type(PIN_SWITCH, GPIO_INTR_POSEDGE);
    
    /* or... you could do this: */
    gpio_config_t config = {
        .intr_type = GPIO_INTR_POSEDGE,
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = 1,
        .pull_up_en = 0,
        .pin_bit_mask = (1ULL << PIN_SWITCH) | (1ULL << 12) };
    gpio_config(&config);
}


void GPIO_input(void) {
    gpio_set_direction(PIN_LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_SWITCH, GPIO_MODE_INPUT);

    // enable on-board resistor that connects pin to GND when not set to HIGH i.e. when the switch is closed, it's HIGH, when it's open, it's LOW
    gpio_pulldown_en(PIN_SWITCH); 

    // disable on-board resistor that is always high when the switch is opened, but always connected to ground when closed. Note that it is important to disable pullups that you're not using.  
    gpio_pullup_dis(PIN_SWITCH);

    while (true)
    {   // get the push button's state and set the LED pin's state to that.
        // the delay is set to a single tick to avoid a watchdog timer interrupting this due to mistaking this for a (bad) infinite loop)
        int level = gpio_get_level(PIN_SWITCH);
        gpio_set_level(PIN_LED, level);
        vTaskDelay(1); // this delay is energy-inefficient, so we should use an interrupt that waits for an event rather than consume so many CPU cycles like this.
    }
}



uint32_t hall_sens_read(); /// you have to declare that you will be using this before you do so.
void hallSens() {   
    /// The ESP32 has a sensor that tracks magnetic radiation. It is unipolar.
    while (1) {
        printf("Hall Sens: %ld\n", hall_sens_read());
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}