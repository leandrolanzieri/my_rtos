#include "sapi.h"
#include "main.h"
#include "my_rtos.h"
#include "my_rtos_events.h"

#include "task_button_up.h"

uint8_t stackButtonUp[MY_RTOS_STACK_SIZE];

extern event_t buttonUpEvent, ledDownEvent, ledUpEvent;

extern bool buttonsPressed;

osTicks_t upTime;

/**
 * @brief Tarea que detecta eventos de liberado en los botones. Cuenta el tiempo
 *        entre los flancos ascendentes considerando un anti-rebote.
 *        Luego reporta el tiempo entre los eventos.
 * 
 * @param a 
 */
void taskButtonUp(void *a) {
   int firstButton, secondButton;
   osTicks_t time = 0;

   while(1) {

      // Se espera un evento de liberado
      MyRtos_EventWait(&buttonUpEvent);

      // Si los botones estaban presionados se continua
      if (buttonsPressed == true) {
         
         // Se determina el primer boton
         firstButton = gpioRead(TEC1) ? TEC1 : TEC2;

         // Por descarte el segundo
         secondButton = firstButton == TEC1 ? TEC2 : TEC1;
         
         // Se inicia la cuenta de tiempo
         time = MyRtos_GetSystemTicks();
         
         // Se espera para el rebote
         MyRtos_DelayMs(20);
         
         // Si el boton sigue liberado se continua
         if (gpioRead(firstButton)) {

            // Se espera el segundo evento de liberado
            MyRtos_EventWait(&buttonUpEvent);

            // Se espera para el rebote
            MyRtos_DelayMs(20);

            // Si el segundo boton sigue liberado se reporta el tiempo
            if (gpioRead(secondButton)) {

               // Se marca que los botones estan liberados
               buttonsPressed = false;

               upTime = MyRtos_GetSystemTicks() - time;

               // Se reporta el tiempo
               reportTimes(firstButton, upTime);
               
               MyRtos_EventSend(&ledDownEvent);

               MyRtos_EventSend(&ledUpEvent);
            }
         }
      }
   }
 }
