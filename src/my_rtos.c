#include <string.h>
#include "chip.h"
#include "my_rtos.h"

// TODO: Remove
extern uint32_t sp1, sp2;
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
}


/**
 * @brief Starts the OS execution.
 * 
 */
void MyRtos_StartOS(void) {

}


uint32_t MyRtos_getNextContext(uint32_t currentSP) {
   static int actualTask = MY_RTOS_ACTUAL_TASK_NONE;
   uint32_t nextSP;

   switch(actualTask) {
      case 1:
         sp1 = currentSP;
         nextSP = sp2;
         actualTask = 2;
      break;

      case 2:
         sp2 = currentSP;
         nextSP = sp1;
         actualTask = 1;
      break;

      default:
         nextSP = sp1;
         actualTask = 1;
      break;
   }

   return nextSP;
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