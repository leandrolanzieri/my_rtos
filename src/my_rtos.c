#include <string.h>

#include "my_rtos.h"

/**
 * @brief Register a new task to the OS scheduler.
 * 
 * @param task Pointer to the task function
 * @param stack Pointer to the stack of the task
 * @param stackPointer Pointer to the 'stack pointer' of the task
 * @param stackLength Length of the stack of the task, expressed in bytes
 */
void MyRtos_InitTask(task_t task, uint32_t *stack, uint32_t *stackPointer, uint32_t stackLength) {

   // Initialize stack with 0
   memset(stack, 0, MY_RTOS_STACK_SIZE);

   // Point stack pointer to last unused position in stack. 8 positions used.
   *stackPointer = (uint32_t)(stack + (stackLength / 4) - 8);

   // Indicate ARM/Thumb mode in PSR registers
   stack[(stackLength / 4) - 1] = MY_RTOS_INITIAL_xPSR;

   // Program counter is the pointer to task
   stack[(stackLength / 4) - 2] = (uint32_t)task;

   // LR - Link Register. Could be use to point to a return hook. 0 for now.
   stack[(stackLength / 4) - 3] = 0;

   // R0 - First parameter passed to the task. 0 for now.
   stack[(stackLength / 4) - 8] = 0;
}


/**
 * @brief Starts the OS execution.
 * 
 */
void MyRtos_StartOS(void) {

}