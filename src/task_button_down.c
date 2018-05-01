#include "sapi.h"
#include "main.h"
#include "my_rtos.h"
#include "my_rtos_events.h"

#include "task_button_down.h"

uint8_t stackButtonDown[MY_RTOS_STACK_SIZE];

extern event_t buttonDownEvent;

extern bool buttonsPressed;

extern osTicks_t downTime;

/**
 * @brief Tarea que detecta eventos de pulsado en los botones. Cuenta el tiempo
 *        entre los flancos descendentes considerando un anti-rebote.
 *        Luego reporta el tiempo entre los eventos.
 * 
 * @param a 
 */
void taskButtonDown(void *a) {
   int firstButton, secondButton;
   osTicks_t time = 0;

   while(1) {

      // Se espera un evento de pulsado
      MyRtos_EventWait(&buttonDownEvent);

      // Si los botones no estaban presionados se continua
      if (buttonsPressed == false) {
         
         // Se determina el primer boton
         firstButton = !gpioRead(TEC1) ? TEC1 : TEC2;

         // Por descarte el segundo
         secondButton = firstButton == TEC1 ? TEC2 : TEC1;
         
         // Se inicia la cuenta de tiempo
         time = MyRtos_GetSystemTicks();
         
         // Se espera para el rebote
         MyRtos_DelayMs(20);
         
         // Si el boton sigue presionado se continua
         if (!gpioRead(firstButton)) {

            // Se espera el segundo evento de pulsado
            MyRtos_EventWait(&buttonDownEvent);

            // Se espera para el rebote
            MyRtos_DelayMs(20);

            // Si el segundo boton sigue presionado se reporta el tiempo
            if (!gpioRead(secondButton)) {

               // Se marca que los botones estan presionados
               buttonsPressed = true;

               // Se calcula el tiempo que paso
               downTime = MyRtos_GetSystemTicks() - time;

               // Se reporta el tiempo
               reportTimes(firstButton, downTime);
            }
         }
      }
   }
 }
