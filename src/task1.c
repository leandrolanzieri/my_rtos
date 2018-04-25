#include "my_rtos.h"
#include "my_rtos_queues.h"
#include "sapi.h"
#include "task1.h"

uint8_t stack1[MY_RTOS_STACK_SIZE];

// extern queue_t ledQueue;

void task1(void *a) {   
   gpioToggle(LED1);
   while(1) {
      // MyRtos_QueueSend(&ledQueue, LED1);
      // MyRtos_DelayMs(500);
      // MyRtos_QueueSend(&ledQueue, LED2);
      // MyRtos_DelayMs(500);
      // MyRtos_QueueSend(&ledQueue, LED3);
      // MyRtos_DelayMs(500);
      gpioToggle(LED1);
      MyRtos_DelayMs(200);
   }
}