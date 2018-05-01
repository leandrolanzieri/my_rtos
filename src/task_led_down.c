#include "my_rtos.h"
#include "my_rtos_queues.h"
#include "sapi.h"
#include "task_led_down.h"

uint8_t stackLedDown[MY_RTOS_STACK_SIZE];

extern event_t ledDownEvent;
extern osTicks_t downTime;

void taskLedDown(void *a) {   
   while(1) {
      MyRtos_EventWait(&ledDownEvent);
      
      gpioWrite(LED1, ON);
      MyRtos_DelayMs(downTime);
      gpioWrite(LED1, OFF);
   }
}