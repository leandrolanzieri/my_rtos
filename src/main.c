/* Copyright 2016, Pablo Ridolfi
 * All rights reserved.
 *
 * This file is part of Workspace.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/** @brief This is a simple blink example.
 */

/** \addtogroup blink Bare-metal blink example
 ** @{ */

/*==================[inclusions]=============================================*/

#include "main.h"
#include "sapi.h"
#include "my_rtos.h"
#include "my_rtos_events.h"
#include "my_rtos_queues.h"
#include "my_rtos_vectors.h"
#include "ciaaUART.h"

#include "task_led_down.h"
#include "task_led_up.h"
#include "task_button_down.h"
#include "task_button_up.h"

/*==================[macros and definitions]=================================*/
/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/** @brief hardware initialization function
 *	@return none
 */
static void initHardware(void);

/*==================[internal data definition]===============================*/

event_t buttonDownEvent, buttonUpEvent, ledDownEvent, ledUpEvent;
bool buttonsPressed = false;
osTicks_t downTime = 0, upTime = 0;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void initHardware(void) {
   boardConfig();

   ciaaUARTInit();

   /* External Interrupt for CH0 */
   Chip_SCU_GPIOIntPinSel(CH0_PININT, CH0_GPIO_PORT, CH0_GPIO_PIN);
   Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(CH0_PININT));
   Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(CH0_PININT));
   Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(CH0_PININT));
   Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(CH0_PININT));

   /* External Interrupt for CH1 */
   Chip_SCU_GPIOIntPinSel(CH1_PININT, CH1_GPIO_PORT, CH1_GPIO_PIN);
   Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(CH1_PININT));
   Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(CH1_PININT));
   Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(CH1_PININT));
   Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(CH1_PININT));
}

/**
 * @brief Funcion que se llama con las interrupciones del canal 0 de GPIO.
 *        Dependiendo del flanco que la produjo libera un evento u otro.
 * 
 */
void button1Interrupt(void) {

   if (Chip_PININT_GetFallStates(LPC_GPIO_PIN_INT) & PININTCH(CH0_PININT)) {
      Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(CH0_PININT));
      MyRtos_EventSend(&buttonDownEvent);
   } else {
      Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(CH0_PININT));
      MyRtos_EventSend(&buttonUpEvent);
   }
}


/**
 * @brief Funcion que se llama con las interrupciones del canal 1 de GPIO.
 *        Dependiendo del flanco que la produjo libera un evento u otro.
 * 
 */
void button2Interrupt(void) {

   if (Chip_PININT_GetFallStates(LPC_GPIO_PIN_INT) & PININTCH(CH1_PININT)) {
      Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(CH1_PININT));
      MyRtos_EventSend(&buttonDownEvent);
   } else {
      Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(CH1_PININT));
      MyRtos_EventSend(&buttonUpEvent);
   }
}


/**
 * @brief Reporta los tiempos entre los eventos de los pulsadores. Recibe el 
 *        el orden de los botones y el tiempo y los imprime con el formato
 *        requerido por UART USB.
 * 
 * @param firstButton Primer boton que produjo el evento
 * @param time Tiempo entre eventos
 */
void reportTimes(int firstButton, osTicks_t time) {
   dbgPrint("{");

   if (firstButton == TEC1) {
      dbgPrint("0:1:");
   } else {
      dbgPrint("1:0:");
   }
   
   dbgPrint(intToString(time));
   dbgPrint("}\r\n");
}

/*==================[external functions definition]==========================*/

int main(void) {
   // Se inicialza el hardware
   initHardware();

   // Se registran las funciones para las interrupciones de GPIO
   MyRtos_AttachIRQ(PIN_INT0_IRQn, button1Interrupt);
   MyRtos_AttachIRQ(PIN_INT1_IRQn, button2Interrupt);

   // Se incializan los eventos
   MyRtos_EventInit(&buttonDownEvent); // Boton presionado
   MyRtos_EventInit(&buttonUpEvent); // Boton liberado
   MyRtos_EventInit(&ledDownEvent); // Encender led de tiempo desc.
   MyRtos_EventInit(&ledUpEvent); // Encender led de tiempo asc.

   // Se lanza el OS
   MyRtos_StartOS();

   while (1) {}
}


// Se registran las tareas
#define MY_RTOS_TASKS                                                                   \
       MY_RTOS_INIT_TASK(taskLedDown, (uint32_t *)stackLedDown, MY_RTOS_STACK_SIZE, 0, 0) \
       MY_RTOS_INIT_TASK(taskLedUp, (uint32_t *)stackLedUp, MY_RTOS_STACK_SIZE, 0, 0) \
       MY_RTOS_INIT_TASK(taskButtonDown, (uint32_t *)stackButtonDown, MY_RTOS_STACK_SIZE, 0, 0) \
       MY_RTOS_INIT_TASK(taskButtonUp, (uint32_t *)stackButtonUp, MY_RTOS_STACK_SIZE, 0, 0) \


taskControl_t MyRtos_TasksList[] = {
   MY_RTOS_TASKS
   MY_RTOS_LAST_TASK
};

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
