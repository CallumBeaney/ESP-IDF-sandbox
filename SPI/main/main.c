#include <stdio.h>
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"

#include "uart_queue/uart_queue.h"

#define TAG "UART"

#define TXD_PIN 4
#define RXD_PIN 5

#define RX_BUF_SIZE 1024

void UART_shortedPins(void);


void app_main(void)
{

}




/// most basic example
void UART_shortedPins(void) {

  // say you connect TXD_PIN & RXD_PIN to one another with a single connector wire, effectively shorting them. One is your TX, the other your RX (obviously!). Here is set up a very basic send/receiving action with no CTS/CTR pins involved.
  /*
             ___
            │   │
    ┌───────┴───▼──────┐
    │ oooooooooooooooo │
    │                 [[===== 
    │ oooooooooooooooo │
    └──────────────────┘
  */

  uart_config_t uart_config = {
    .baud_rate = 9600, // ofc can be much higher on ESP32
    .data_bits = UART_DATA_8_BITS, // transfer a set of 8 bits at a time
    .parity = UART_PARITY_DISABLE, // stored in the data frame, tells RX if error in data bits. 
    .stop_bits = UART_STOP_BITS_1, // decides when frame complete
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE // this param for en/disable CTS RTS pins
  };
  uart_port_t uartNumber = UART_NUM_1; // ESP32 has 3 UARTS, 0 usu. for flashing/monitoring. 
  uart_param_config(uartNumber, &uart_config);
  uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  
  int TX_BUFSIZE = 0;
  int QUEUESIZE = 0;
  int interruptFlags = 0;
  uart_driver_install(UART_NUM_1, RX_BUF_SIZE, TX_BUFSIZE, QUEUESIZE, NULL /*queue handler*/,
  interruptFlags);


  char message[] = "ping";
  printf("sending: %s\n", message);
  uart_write_bytes(UART_NUM_1, message, sizeof(message));

  char incoming_message[RX_BUF_SIZE]; // buffer to hold message sent from Tx pin
  memset(incoming_message, 0 , sizeof(incoming_message));
  uart_read_bytes(UART_NUM_1, (uint8_t *) incoming_message, RX_BUF_SIZE, pdMS_TO_TICKS(500));
  printf("received: %s\n", incoming_message);

  // note the choice to cast the incoming message above -- it expects a byte array, not a char array.
  // pay attention to your TickType_t ticks_to_wait param in uart_read_bytes() -- too fast, and Rx won't get from Tx.
}
