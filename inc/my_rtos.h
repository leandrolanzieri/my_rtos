#ifndef MY_RTOS_H
#define MY_RTOS_H

#include "stdint.h"


// #define MY_RTOS_INIT_TASK(_entryPoint, _stack, _stackPointer, _stackSize) +1
// #define MY_RTOS_AMOUNT_OF_TASKS	0 MY_RTOS_TASKS
// #undef MY_RTOS_INIT_TASK
/***********************************************************************/

void MyRtos_StartOS(void);

#endif
