#ifndef INTERRUPTS_H
#define INTERRUPTS_H

static void IRAM_ATTR gpio_isr_handler(void *args);
void buttonPushedTask(void *params);
void interrupt(void);

#endif