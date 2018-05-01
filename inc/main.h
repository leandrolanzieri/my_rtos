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

#ifndef _MAIN_H_
#define _MAIN_H_

/** \addtogroup blink Bare-metal blink example
 ** @{ */

/*==================[inclusions]=============================================*/
#include "my_rtos.h"
/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/** delay in milliseconds */
#define DELAY_MS 500

/** led number to toggle */
#define LED 0

#define CH0_LED 0
#define CH1_LED 3
#define CH2_LED 4
#define CH3_LED 5

#define CH0_PININT 0
#define CH1_PININT 1
#define CH2_PININT 2
#define CH3_PININT 3

#define CH0_PIN_INT_IRQn PIN_INT0_IRQn
#define CH1_PIN_INT_IRQn PIN_INT1_IRQn
#define CH2_PIN_INT_IRQn PIN_INT2_IRQn
#define CH3_PIN_INT_IRQn PIN_INT3_IRQn

#define CH0_GPIO_PORT   0
#define CH0_GPIO_PIN    4
#define CH1_GPIO_PORT   0
#define CH1_GPIO_PIN    8
#define CH2_GPIO_PORT   0
#define CH2_GPIO_PIN    9
#define CH3_GPIO_PORT   1
#define CH3_GPIO_PIN    9

#define CH0_IRQ_HANDLER GPIO0_IRQHandler
#define CH1_IRQ_HANDLER GPIO1_IRQHandler
#define CH2_IRQ_HANDLER GPIO2_IRQHandler
#define CH3_IRQ_HANDLER GPIO3_IRQHandler

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** @brief main function
 * @return main function should never return
 */
int main(void);

void button1Interrupt(void);

void button2Interrupt(void);

void reportTimes(int firstButton, osTicks_t time);
/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif /* #ifndef _MAIN_H_ */
