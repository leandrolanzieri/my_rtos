#ifndef MY_RTOS_TASK_H
#define MY_RTOS_TASK_H

#include "stdint.h"

#define MY_RTOS_INIT_TASK(_entryPoint, _stack, _stackSize, _parameter) \
	{.entryPoint = _entryPoint, .stack = _stack, .stackPointer = 0, .stackSize = _stackSize, .state = TASK_READY, .initialParameter = _parameter}

#define MY_RTOS_LAST_TASK {0,0,0,0,0}

/***********************************************************************/
#define MY_RTOS_STACK_SIZE  512

#define MY_RTOS_INITIAL_xPSR    (1 << 24)

#define MY_RTOS_EXC_RETURN (0xFFFFFFF9)

#define MY_RTOS_ACTUAL_TASK_NONE -1

#define MY_RTOS_MAX_TASKS  8


/***********************************************************************/
typedef void(*task_t)(void *);

typedef enum {
   TASK_ERROR = 0,
   TASK_READY = 1,
   TASK_RUNNING = 2,
   TASK_BLOCKED = 3,
   TASK_HALTED = 4
} taskState_t;

typedef struct {
   task_t entryPoint;
   uint32_t *stack;
   uint32_t stackPointer;
   uint32_t stackSize;
   taskState_t state;
   void* initialParameter;
} taskControl_t;

#endif