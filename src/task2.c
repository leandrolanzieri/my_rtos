#include "my_rtos_task.h"
#include "sapi.h"
#include "task2.h"

uint8_t stack2[MY_RTOS_STACK_SIZE];

void task2(void *a) {
   uint32_t j = 0;

   while(1) {
     for (j = 0; j < 1000000; j++) {}
     gpioToggle(LED2);
   }
}
