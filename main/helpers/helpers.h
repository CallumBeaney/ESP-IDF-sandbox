#ifndef HELPERS_H // looks for whether this particular define has ever been declared
#define HELPERS_H

#include "freertos/FreeRTOS.h" // delays etc

void delayDemonstration_vTaskDelayUntil(TickType_t);
void delayDemonstration_vTaskDelay(int(*)(void));
int  getDicerollNumber(void);

#endif