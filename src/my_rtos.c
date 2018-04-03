#include <string.h>
#include "chip.h"
#include "my_rtos.h"



static task_t tasks[MY_RTOS_MAX_TASKS];

static uint32_t stackPointers[MY_RTOS_MAX_TASKS];

static uint32_t addedTasks = 0;

/**
 * @brief Register a new task to the OS scheduler.
 * 
 * @param task Pointer to the task function
 * @param stack Pointer to the stack of the task
 * @param stackPointer Pointer to the 'stack pointer' of the task
 * @param stackLength Length of the stack of the task, expressed in bytes
 */
bool MyRtos_InitTask(task_t task, uint32_t *stack, uint32_t *stackPointer, uint32_t stackLength) {

   // Check tasks limit
   if (addedTasks < MY_RTOS_MAX_TASKS - 1) {
      // Initialize stack with 0
      memset(stack, 0, MY_RTOS_STACK_SIZE);

      // Point stack pointer to last unused position in stack. 8 positions used.
      *stackPointer = (uint32_t)(stack + (stackLength / 4) - 17); // 17?

      // Indicate ARM/Thumb mode in PSR registers
      stack[(stackLength / 4) - 1] = MY_RTOS_INITIAL_xPSR;

      // Program counter is the pointer to task
      stack[(stackLength / 4) - 2] = (uint32_t)task;

      // LR - Link Register. Could be use to point to a return hook. 0 for now.
      stack[(stackLength / 4) - 3] = 0;

      // R0 - First parameter passed to the task. 0 for now.
      stack[(stackLength / 4) - 8] = 0;

      stack[(stackLength / 4) - 9] = MY_RTOS_EXC_RETURN; /* lr from stack */

      // Add task to array
      tasks[addedTasks] = task;

      // Add stack pointer to array
      stackPointers[addedTasks] = *stackPointer;

      // Update amount of tasks registered
      addedTasks++;

      return true;
   }

   return false;
}


/**
 * @brief Starts the OS execution.
 * 
 */
void MyRtos_StartOS(void) {

}


uint32_t MyRtos_getNextContext(uint32_t currentSP) {
   static int32_t currentTask = MY_RTOS_ACTUAL_TASK_NONE;

   // First time return first stack pointer and initialize current tasks
   if (currentTask == MY_RTOS_ACTUAL_TASK_NONE) {
      currentTask = 0;
      return stackPointers[currentTask];
   }

   // Save stack pointer for current task
   stackPointers[currentTask] = currentSP;

   // Update currentTask with next task value (from 0 to addedTasks - 1)
   currentTask = currentTask < (addedTasks - 1) ? currentTask + 1 : 0;

   return stackPointers[currentTask];
}

void SysTick_Handler(void) {
   // Activate PendSV for context switching
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;

   // Instruction Synchronization Barrier: this way we make sure that all
   // pipelined instructions are executed
	__ISB();
	
   /* Data Synchronization Barrier: aseguramos que se
	 * completen todos los accesos a memoria
	 */
   // Data Synchronization Barrier: this way we make sure that all
   // memory accesses are completed
	__DSB();
}
