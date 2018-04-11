#ifndef MY_RTOS_H
#define MY_RTOS_H

#include "stdint.h"
#include "my_rtos_user_tasks.h"

#ifndef MY_RTOS_TASKS
#error "User 'MY_RTOS_TASKS' macro to define tasks"
#endif

// #define MY_RTOS_INIT_TASK(_entryPoint, _stack, _stackPointer, _stackSize) +1
// #define MY_RTOS_AMOUNT_OF_TASKS	0 MY_RTOS_TASKS
// #undef MY_RTOS_INIT_TASK
/***********************************************************************/

void MyRtos_StartOS(void);

#endif
