#include <string.h>
#include "chip.h"
#include "my_rtos.h"
#include "my_rtos_task.h"
#include "my_rtos_user_tasks.h"

///////////////////////////////////////////////////////////////////////////////

// Tasks list
#define MY_RTOS_INIT_TASK(_entryPoint, _stack, _stackSize, _parameter)        \
      {.entryPoint = _entryPoint, .stack = _stack, .stackPointer = 0,         \
      .stackSize = _stackSize, .state = TASK_READY,                           \
      .initialParameter = _parameter},

static taskControl_t MyRtos_TasksList[] = {
      MY_RTOS_TASKS
      MY_RTOS_LAST_TASK
};

#undef MY_RTOS_INIT_TASK


// // Amount of tasks declared
// #define MY_RTOS_INIT_TASK(_entryPoint, _stack, _stackSize, _parameter) +1
//       static uint32_t MyRtos_amountOfTasks = 0 MY_RTOS_TASKS;
// #undef MY_RTOS_INIT_TASK

///////////////////////////////////////////////////////////////////////////////

static bool MyRtos_initTask(taskControl_t *task);

static void MyRtos_idleTask(void *param);

static bool MyRtos_readyTasks(void);

///////////////////////////////////////////////////////////////////////////////

extern taskControl_t MyRtos_TasksList[];

static uint8_t idleStack[MY_RTOS_STACK_SIZE];

static taskControl_t idleTaskControl = {
   .entryPoint = MyRtos_idleTask,
   .stackSize = MY_RTOS_STACK_SIZE,
   .stack = (uint32_t *)idleStack,
   .state = TASK_READY,
   .stackPointer = 0,
   .initialParameter = 0
};

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Starts the OS execution.
 * 
 */

void MyRtos_StartOS(void) {
   memset(idleStack, 0, MY_RTOS_STACK_SIZE);

   taskControl_t * t = MyRtos_TasksList;

   while(t->entryPoint != 0) {
      MyRtos_initTask(t);
      t++;
   }

   SystemCoreClockUpdate();
   SysTick_Config(SystemCoreClock / 1000);

   // Lowest priority to pendSV interrupt
   // 100000b - 1b = 011111b 
   NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
}


uint32_t MyRtos_GetNextContext(uint32_t currentSP) {
   static int32_t currentTask = MY_RTOS_ACTUAL_TASK_NONE;

   // If no tasks were declared execute idle task
   if (MyRtos_TasksList[0].entryPoint == 0) {
      return idleTaskControl.stackPointer;
   }

   // The first time return first stack pointer and initialize current tasks
   if (currentTask == MY_RTOS_ACTUAL_TASK_NONE) {
      currentTask = 0;
      return MyRtos_TasksList[currentTask].stackPointer;
   }

   // Save current task stack pointer and change state
   MyRtos_TasksList[currentTask].stackPointer = currentSP;
   MyRtos_TasksList[currentTask].state = TASK_READY;

   // Increment the task index
   currentTask++;

   // Check if there is any 'Ready' task to be executed
   if (!MyRtos_readyTasks()) {
      return idleTaskControl.stackPointer;
   }

   // Check if 'currentTask' needs to be reset
   if (MyRtos_TasksList[currentTask].entryPoint == 0) {
      currentTask = 0;
   }

   // Mark the new current task as 'Running'
   MyRtos_TasksList[currentTask].state = TASK_RUNNING;

   // Return new stack pointer
   return MyRtos_TasksList[currentTask].stackPointer;
}

/**
 * @brief Determines if there is any task in 'Ready' state
 * 
 * @return true There is al least one ready task
 * @return false There are none ready tasks
 */

static bool MyRtos_readyTasks(void) {
   uint32_t i = 0;

   while(MyRtos_TasksList[i].entryPoint != 0) {
      if (MyRtos_TasksList[i].state == TASK_READY) {
         return true;
      }
      i++;
   }
   
   return false;
}


/**
 * @brief Register a new task to the OS scheduler.
 * 
 * @param task Pointer to the task control structure
 */

static bool MyRtos_initTask(taskControl_t *task) {
   // Initialize stack with 0
   memset(task->stack, 0, task->stackSize);

   // Point stack pointer to last unused position in stack. 17 posisions used.
   task->stackPointer = (uint32_t)(task->stack + (task->stackSize / 4) - 17);

   // Indicate ARM/Thumb mode in PSR registers
   task->stack[(task->stackSize / 4) - 1] = MY_RTOS_INITIAL_xPSR;

   // Program counter is the pointer to task
   task->stack[(task->stackSize / 4) - 2] = (uint32_t)task->entryPoint;

   // LR - Link Register. Could be use to point to a return hook. 0 for now.
   task->stack[(task->stackSize / 4) - 3] = 0;

   // R0 - First parameter passed to the task. 0 for now.
   task->stack[(task->stackSize / 4) - 8] = 0;

   task->stack[(task->stackSize / 4) - 9] = MY_RTOS_EXC_RETURN; /* lr from stack */

   return true;
}

static void MyRtos_idleTask(void *param) {
   __WFI();
}

void SysTick_Handler(void) {
   // Activate PendSV for context switching
   SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;

   // Instruction Synchronization Barrier: this way we make sure that all
   // pipelined instructions are executed
   __ISB();
      
   // Data Synchronization Barrier: this way we make sure that all
   // memory accesses are completed
   __DSB();
}
