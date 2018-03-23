#ifndef MY_RTOS_H
#define MY_RTOS_H

#include "stdint.h"

/***********************************************************************/
#define MY_RTOS_STACK_SIZE  512

#define MY_RTOS_INITIAL_xPSR    (1 << 24)

/***********************************************************************/
typedef void(*task_t)(void);

/***********************************************************************/
void MyRtos_InitTask(task_t task, uint32_t *stack, uint32_t *stackPointer, uint32_t stackLength);

void MyRtos_StartOS(void);


#endif