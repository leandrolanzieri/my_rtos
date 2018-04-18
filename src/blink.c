#include "my_rtos.h"
#include "my_rtos_events.h"
#include "sapi.h"
#include "blink.h"

uint8_t stack2[MY_RTOS_STACK_SIZE];

extern event_t ButtonEvent;

void blink(void *config) {
   blinkTaskData_t *myConfig = (blinkTaskData_t *)config;

   while(1) {
     MyRtos_EventWait(&ButtonEvent);
     gpioToggle(myConfig->led);
     MyRtos_DelayMs(500);
   }
}
