#include "sapi.h"
#include "main.h"
#include "my_rtos.h"
#include "my_rtos_queues.h"
#include "my_rtos_vectors.h"
#include "task_button.h"

uint8_t stackButton[MY_RTOS_STACK_SIZE];

extern event_t buttonEvent;

void taskButton(void *a) {
   MyRtos_AttachIRQ(PIN_INT0_IRQn, buttonInterrupt);

   while(1) {
      MyRtos_DelayMs(5);
   }
 }

void buttonInterrupt(void) {

   if (Chip_PININT_GetFallStates(LPC_GPIO_PIN_INT) & PININTCH(CH0_PININT)) {
      Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(CH0_PININT));
      gpioWrite(LEDB, ON);
   } else {
      Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(CH0_PININT));
      gpioWrite(LEDB, OFF);
   }

   MyRtos_EventSend(&buttonEvent);
}
