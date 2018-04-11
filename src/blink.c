#include "my_rtos.h"
#include "sapi.h"
#include "blink.h"

uint8_t stack2[MY_RTOS_STACK_SIZE];

void blink(void *config) {
   blinkTaskData_t *myConfig = (blinkTaskData_t *)config;

   while(1) {
     MyRtos_DelayMs(myConfig->delay);
     gpioToggle(myConfig->led);
   }
}
