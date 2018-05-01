#include "my_rtos.h"
#include "my_rtos_queues.h"
#include "sapi.h"
#include "task_led_down.h"

uint8_t stackLedUp[MY_RTOS_STACK_SIZE];

extern event_t ledUpEvent;
extern osTicks_t upTime;

void taskLedUp(void *a) {   
   while(1) {
      MyRtos_EventWait(&ledUpEvent);
      
      gpioWrite(LED2, ON);
      MyRtos_DelayMs(upTime);
      gpioWrite(LED2, OFF);
   }
}