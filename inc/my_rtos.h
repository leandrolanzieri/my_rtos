#ifndef MY_RTOS_H
#define MY_RTOS_H

#include "stdint.h"

/***********************************************************************/
#define MY_RTOS_STACK_SIZE  512

#define MY_RTOS_INITIAL_xPSR    (1 << 24)

#define MY_RTOS_EXC_RETURN (0xFFFFFFF9)

#define MY_RTOS_ACTUAL_TASK_NONE -1

#define MY_RTOS_MAX_TASKS  8

/***********************************************************************/
typedef void(*task_t)(void);

/***********************************************************************/
bool MyRtos_InitTask(task_t task, uint32_t *stack, uint32_t stackLength);

void MyRtos_StartOS(void);


#endif