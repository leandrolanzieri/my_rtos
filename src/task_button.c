#include "sapi.h"
#include "task_button.h"

uint8_t stackButton[MY_RTOS_STACK_SIZE];

void taskButton(void *a) {
   while(1) {
      gpioWrite(LEDG, !gpioRead(TEC1));
   }
 }
