#include "my_rtos.h"
#include "sapi.h"
#include "task1.h"

uint8_t stack1[MY_RTOS_STACK_SIZE];

void task1(void *a) {   
   gpioToggle(LED1);
   while(1) {
      MyRtos_DelayMs(500);
      gpioToggle(LED1);
   }
}