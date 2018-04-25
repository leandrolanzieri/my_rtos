#include "sapi.h"
#include "my_rtos.h"
#include "my_rtos_queues.h"
#include "task_button.h"

uint8_t stackButton[MY_RTOS_STACK_SIZE];

extern queue_t buttonQueue;


/**
 * @brief Esta tarea lee una tecla y aplica un anti rebote con delay.
 *        Cuando la tecla esta presionada se enciende un led y se envia
 *        un mensaje por cola a otra tarea indicando sobre que led debe
 *        actuar.
 * 
 * @param a None
 */
void taskButton(void *a) {
   while(1) {

      // Se lee la tecla
      if (!gpioRead(TEC1)) {
         // Se espera para evitar espurios
         MyRtos_DelayMs(30);

         if (!gpioRead(TEC1)) { // Se verifica nuevamente la tecla
            // Se enciende el led
            gpioWrite(LEDB, ON);

            // Se envia el mensaje por la cola
            MyRtos_QueueSend(&buttonQueue, LED2);
         } else { // Si ya no esta presionada se toma como espurio
            // Se apaga el led
            gpioWrite(LEDB, OFF);
         }
      } else {
         // Se apaga el led
         gpioWrite(LEDB, OFF);
      }

      // Se espera entre lecturas sucesivas
      MyRtos_DelayMs(5);
   }
 }
