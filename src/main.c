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
// #include "board.h"
#include "sapi.h"
#include "my_rtos.h"

/*==================[macros and definitions]=================================*/
/*==================[internal data declaration]==============================*/
static uint8_t pila1[MY_RTOS_STACK_SIZE];
static uint8_t pila2[MY_RTOS_STACK_SIZE];

/*==================[internal functions declaration]=========================*/

/** @brief hardware initialization function
 *	@return none
 */
static void initHardware(void);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/
uint32_t sp2;
uint32_t sp1;

/*==================[internal functions definition]==========================*/

static void initHardware(void) {
   // Board_Init();
   boardConfig();
   SystemCoreClockUpdate();
   SysTick_Config(SystemCoreClock / 1000);
}


/*==================[external functions definition]==========================*/
void task1(void) {
   uint32_t i = 0;

   while(1) {
      for (i = 0; i < 1000000; i++) {}
      // gpioToggle(LEDR);
   }
}

void task2(void) {
   uint32_t j = 0;

   while(1) {
     for (j = 0; j < 1000000; j++) {}
     // gpioToggle(LEDB);
   }
}

int main(void) {
   initHardware();

   MyRtos_InitTask(task1, pila1, &sp1, MY_RTOS_STACK_SIZE);
   MyRtos_InitTask(task2, pila2, &sp2, MY_RTOS_STACK_SIZE);

   MyRtos_StartOS();

   while (1) {
   }
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
