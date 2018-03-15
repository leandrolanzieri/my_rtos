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
#include "board.h"

/*==================[macros and definitions]=================================*/
#define STACK_SIZE 512
/*==================[internal data declaration]==============================*/
static uint32_t pila1[STACK_SIZE/4];
static uint32_t pila2[STACK_SIZE/4];

/*==================[internal functions declaration]=========================*/

/** @brief hardware initialization function
 *	@return none
 */
static void initHardware(void);

/*==================[internal data definition]===============================*/
typedef void(*task_t)(void);

/*==================[external data definition]===============================*/
uint32_t sp2;
uint32_t sp1;

/*==================[internal functions definition]==========================*/

static void initHardware(void) {
   Board_Init();
   SystemCoreClockUpdate();
   SysTick_Config(SystemCoreClock / 1000);
}

void init_task(task_t task, uint32_t *stack, uint32_t *stackPointer, uint32_t stackLength) {
    // Point stack pointer to last unused position in stack. 8 positions used.
    *stackPointer = (uint32_t)(stack + stackLength - 8);

    // Indicate ARM/Thumb mode
    stack[stackLength - 1] = 1 << 24;

    // Program counter is the pointer to task
    stack[stackLength - 2] = (uint32_t)task;
}

void start_os(void) {

}

/*==================[external functions definition]==========================*/
void task1(void) {
   int i = 0;

   while(1) {
      i++;
   }
}

void task2(void) {
   int j = 0;

   while(1) {
      j++;
   }
}

int main(void) {
   initHardware();

   init_task(task1, pila1, &sp1, STACK_SIZE/4);
   init_task(task2, pila2, &sp2, STACK_SIZE/4);

   start_os();

   while (1) {
   }
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
