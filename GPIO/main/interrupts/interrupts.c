#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "interrupts.h"

#define PIN_SWITCH 15

QueueHandle_t interruptQueue;

/* 
    In this code you create a global queue, create a task loop that listens for an update to that queue, and add a handler to a GPIO pin that has sends a message to this queue when the button connected to it is pressed.
*/ 

void interrupt()
{
    // gpio_pad_select_gpio(PIN_SWITCH);
    gpio_set_direction(PIN_SWITCH, GPIO_MODE_INPUT);
    gpio_pulldown_en(PIN_SWITCH);
    gpio_pullup_dis(PIN_SWITCH);
    gpio_set_intr_type(PIN_SWITCH, GPIO_INTR_POSEDGE); // rising edge of signal going high

    interruptQueue = xQueueCreate(10, sizeof(int));
    xTaskCreate(buttonPushedTask, "buttonPushedTask", 2048, NULL, 1, NULL);

    gpio_install_isr_service(0); // Interrupt Service Routine
    gpio_isr_handler_add(PIN_SWITCH, gpio_isr_handler, (void *)PIN_SWITCH);
}


static void IRAM_ATTR gpio_isr_handler(void *args) {
    /// IRAM_ATTR is a compiler directive that indicates that this code will be run from dynamic memory
    /// this is particular to interrupts, and ideally one exits from this code as soon as possible, and to avoid doing any logic within this code.
    int pinNumber = (int)args;
    xQueueSendFromISR(interruptQueue, &pinNumber, NULL);
}


void buttonPushedTask(void *params)
{
    int pinNumber, count = 0;
    while (true)
    {
        if (xQueueReceive(interruptQueue, &pinNumber, portMAX_DELAY))
        {   
            //  here you're handling bouncing!
            gpio_isr_handler_remove(pinNumber); // remove the given GPIO pin's handler as defined in interrupt()
            
            // wait for user to stop pressing button 
            do { 
                vTaskDelay(20 / portTICK_PERIOD_MS);
            } while(gpio_get_level(pinNumber) == 1);

            // do function work
            printf("GPIO %d was pressed %d times. The state is %d\n", pinNumber, count++, gpio_get_level(PIN_SWITCH));
        
            // re-enable interrupt
            gpio_isr_handler_add(pinNumber, gpio_isr_handler, (void *)pinNumber);
        }
    }
}