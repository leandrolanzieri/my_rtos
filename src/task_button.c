#include "sapi.h"
#include "task_button.h"

uint8_t stackButton[MY_RTOS_STACK_SIZE];

extern event_t ButtonEvent;

void taskButton(void *a) {
   while(1) {
      if (!gpioRead(TEC1)) {
         MyRtos_DelayMs(30);
         if (!gpioRead(TEC1)) {
            gpioWrite(LEDB, ON);
            MyRtos_EventSend(&ButtonEvent);
         } else {
            gpioWrite(LEDB, OFF);
         }
      } else {
         gpioWrite(LEDB, OFF);
      }
      MyRtos_DelayMs(5);
   }
 }
