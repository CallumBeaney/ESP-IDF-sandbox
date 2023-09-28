#ifndef TIMER_H
#define TIMER_H

#include "freertos/timers.h"
void freeRTOStimerDemonstration(void);
void on_timer(TimerHandle_t xTimer);

void hiResTimer(void);
void timer_callback(void *args);

#endif