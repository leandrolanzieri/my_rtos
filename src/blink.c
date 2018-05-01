#include "my_rtos.h"
#include "my_rtos_events.h"
#include "my_rtos_queues.h"
#include "sapi.h"
#include "blink.h"

uint8_t stack2[MY_RTOS_STACK_SIZE];

// extern queue_t ledQueue;
//extern queue_t buttonQueue;
extern event_t buttonEvent;

void allLedsOff(void) {
   gpioWrite(LED1, OFF);
   gpioWrite(LED2, OFF);
   gpioWrite(LED3, OFF);
}


void blink(void *config) {
   blinkTaskData_t *myConfig = (blinkTaskData_t *)config;
   queueItem_t led;

   while(1) {
      // MyRtos_QueueReceive(&ledQueue, &led);
      // MyRtos_QueueReceive(&buttonQueue, &led);
      MyRtos_EventWait(&buttonEvent);

      gpioToggle(LED2);
      MyRtos_DelayMs(200);
   }
}