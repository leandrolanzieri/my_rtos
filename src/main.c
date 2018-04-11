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
#include "my_rtos_task.h"

/*==================[macros and definitions]=================================*/
/*==================[internal data declaration]==============================*/
static uint8_t stack1[MY_RTOS_STACK_SIZE];
static uint8_t stack2[MY_RTOS_STACK_SIZE];
static uint8_t stackButton[MY_RTOS_STACK_SIZE];


/*==================[internal functions declaration]=========================*/

/** @brief hardware initialization function
 *	@return none
 */
static void initHardware(void);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void initHardware(void) {
   boardConfig();
}


/*==================[external functions definition]==========================*/
void task1(void *a) {
   uint32_t i = 0;
   gpioToggle(LED1);
   while(1) {
      for (i = 0; i < 500000; i++) {}
      gpioToggle(LED1);
   }
}

void task2(void *a) {
   uint32_t j = 0;

   while(1) {
     for (j = 0; j < 1000000; j++) {}
     gpioToggle(LED2);
   }
}

void taskButton(void *a) {
   while(1) {
      gpioWrite(LEDG, !gpioRead(TEC1));

   }
 }

#define MY_RTOS_TASKS                                                             \
       MY_RTOS_INIT_TASK(task1, (uint32_t *)stack1, MY_RTOS_STACK_SIZE, 0),          \
       MY_RTOS_INIT_TASK(task2, (uint32_t *)stack2, MY_RTOS_STACK_SIZE, 0),          \
       MY_RTOS_INIT_TASK(taskButton, (uint32_t *)stackButton, MY_RTOS_STACK_SIZE, 0),\
       MY_RTOS_LAST_TASK

taskControl_t MyRtos_TasksList[] = {
      MY_RTOS_TASKS
};

int main(void) {
   initHardware();

   MyRtos_StartOS();

   while (1) {}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
