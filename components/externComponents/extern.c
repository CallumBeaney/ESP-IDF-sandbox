#include <stdio.h>
#include "driver/gpio.h"

void externFunction(void) {
  int i = 0;
  i++;
  printf("extern!");
}
