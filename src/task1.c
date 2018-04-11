#include "my_rtos_task.h"
#include "sapi.h"
#include "task1.h"

uint8_t stack1[MY_RTOS_STACK_SIZE];

void task1(void *a) {
   uint32_t i = 0;
   gpioToggle(LED1);
   while(1) {
      for (i = 0; i < 500000; i++) {}
      gpioToggle(LED1);
   }
}