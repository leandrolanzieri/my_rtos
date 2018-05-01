#include <string.h>
#include "chip.h"
#include "my_rtos.h"

///////////////////////////////////////////////////////////////////////////////
// Internal functions declarations
///////////////////////////////////////////////////////////////////////////////

static bool MyRtos_initTask(taskControl_t *task, taskID_t ID);

static void MyRtos_idleTask(void *param);

static void MyRtos_delaysUpdate(void);

static void MyRtos_returnHook(void);

static bool MyRtos_getReadyTask(taskID_t *ID);

static void MyRtos_updateReadyList(taskID_t ID);

static void MyRtos_initStack(taskControl_t *task);

///////////////////////////////////////////////////////////////////////////////
// Internal data definitions
///////////////////////////////////////////////////////////////////////////////

static osTicks_t systemTicks = 0;

static uint8_t idleStack[MY_RTOS_STACK_SIZE];

static taskControl_t idleTaskControl = {
   .entryPoint = MyRtos_idleTask,
   .stackSize = MY_RTOS_STACK_SIZE,
   .stack = (uint32_t *)idleStack,
   .state = TASK_READY,
   .stackPointer = (uint32_t)idleStack,
   .initialParameter = 0,
   .basePriority = MY_RTOS_PRIORITY_LEVELS - 1,
   .instantPriority = MY_RTOS_PRIORITY_LEVELS - 1,
};

static taskID_t currentTask = MY_RTOS_TASK_NONE;

static taskID_t readyTasks[MY_RTOS_PRIORITY_LEVELS][MY_RTOS_MAX_TASKS];

///////////////////////////////////////////////////////////////////////////////
// External data definitions
///////////////////////////////////////////////////////////////////////////////

osState_t osState = STATE_TASK;

bool osSwitchRequired;

// Tasks list
extern taskControl_t MyRtos_TasksList[];

///////////////////////////////////////////////////////////////////////////////
// External functions definitions
///////////////////////////////////////////////////////////////////////////////

int32_t critical_counter = 0;

void os_enter_critical(void) {
   critical_counter++;
   __disable_irq();
}

void os_exit_critical(void) {
   critical_counter--;
   if (critical_counter <= 0) {
      critical_counter = 0;
      __enable_irq();
   }
}

/**
 * @brief Starts the OS execution
 * 
 */
void MyRtos_StartOS(void) {
   memset(idleStack, 0, MY_RTOS_STACK_SIZE);

   memset(readyTasks, MY_RTOS_TASK_NONE,
          MY_RTOS_MAX_TASKS * MY_RTOS_PRIORITY_LEVELS * sizeof(taskControl_t*));

   taskControl_t * t = MyRtos_TasksList;
   taskID_t ID = 0;

   while(t->entryPoint != 0) {
      MyRtos_initTask(t, ID);
      t++;
      ID++;
   }

   // Initialize idle task
   MyRtos_initStack(&idleTaskControl);

   SystemCoreClockUpdate();
   SysTick_Config(SystemCoreClock / 1000);

   // Lowest priority to pendSV interrupt
   // 100000b - 1b = 011111b 
   NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
}


/**
 * @brief Returns the current task's ID
 * 
 * @return taskID_t Task's ID
 */
taskID_t MyRtos_GetCurrentTask(void) {
   return currentTask;
}


/**
 * @brief Calls the scheduler for a context switching
 * 
 */
void MyRtos_SchedulerUpdate(void) {
   // Activate PendSV for context switching
   SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;

   // Instruction Synchronization Barrier: this way we make sure that all
   // pipelined instructions are executed
   __ISB();
      
   // Data Synchronization Barrier: this way we make sure that all
   // memory accesses are completed
   __DSB();
}


/**
 * @brief Delays the current task for a given amount of milliseconds
 * 
 * @param ms Amount of ms to delay the task
 */
void MyRtos_DelayMs(osTicks_t ms) {
   
   // Check if valid calling context
   if (osState == STATE_IRQ) {
      return;
   }

   os_enter_critical();
   if (currentTask != MY_RTOS_TASK_NONE && ms != 0 &&
       MyRtos_TasksList[currentTask].state == TASK_RUNNING)
   {
      MyRtos_TasksList[currentTask].delay = ms;
      MyRtos_TasksList[currentTask].state = TASK_BLOCKED;
      os_exit_critical();
      MyRtos_SchedulerUpdate();
   } else {
      os_exit_critical();
   }
}


/**
 * @brief Returns the context of the next task to be executed
 * 
 * @param currentSP Stack pointer of the current context
 * @return uint32_t Stack pointer of the new context
 */
uint32_t MyRtos_GetNextContext(uint32_t currentSP) {

   taskID_t nextTask = MY_RTOS_TASK_NONE;

   
   // If no tasks were declared execute idle task
   if (MyRtos_TasksList[0].entryPoint == 0) {      
      return idleTaskControl.stackPointer;
   }

   if (currentTask == MY_RTOS_IDLE_TASK) {
      // If idle task is running, save its stack pointer
      idleTaskControl.stackPointer = currentSP;
   } else if (currentTask != MY_RTOS_TASK_NONE) {
      // Save current task stack pointer
      MyRtos_TasksList[currentTask].stackPointer = currentSP;

      // Update the current task's list
      MyRtos_updateReadyList(currentTask);
   }

   // Get the next higher priority ready task, switch to idle if none is found
   if (MyRtos_getReadyTask(&nextTask)) {
      currentTask = nextTask;
   } else {
      
      // Mark Idle task as current
      currentTask = MY_RTOS_IDLE_TASK;

      return idleTaskControl.stackPointer;
   }

   // Mark the new current task as 'Running'
   MyRtos_TasksList[currentTask].state = TASK_RUNNING;
   

   // Return new stack pointer
   return MyRtos_TasksList[currentTask].stackPointer;
}

/**
 * @brief Adds a task ID to the bottom of the ready list of its priority and
 *        changes it's state to ready
 * 
 * @param ID The ID of the task to be added to the list
 */
void MyRtos_AddReadyTask(taskID_t ID) {
   uint32_t slot = 0;
   taskPriority_t priority = MyRtos_TasksList[ID].basePriority;

   // Change task's state to ready
   MyRtos_TasksList[ID].state = TASK_READY;
   
   // Sweeps the list of the given task priority and searches for an empty slot
   while (readyTasks[priority][slot] != MY_RTOS_TASK_NONE &&
          slot < MY_RTOS_MAX_TASKS)
   {
      slot++;
   }

   // If the slot was found the task ID is saved there
   if (slot < MY_RTOS_MAX_TASKS) {
      readyTasks[priority][slot] = ID;
   }
}


/**
 * @brief Returns the current value of the system ticks.
 * 
 * @return uint64_t Current system ticks
 */
osTicks_t MyRtos_GetSystemTicks() {
   return systemTicks;
}
///////////////////////////////////////////////////////////////////////////////
// Internal functions definitions
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Looks for the highest priority ready task on the lists and returns
 *        its ID
 * 
 * @param ID Pointer to save the found ID
 * @return true A task was found
 * @return false No task was found
 */
static bool MyRtos_getReadyTask(taskID_t *ID) {
   // Sweep every priority looking for the first task to be executed
   for (uint32_t priority = 0; priority < MY_RTOS_PRIORITY_LEVELS; priority++) {
      
      // If slot is not empty return this ID
      if (readyTasks[priority][0] != MY_RTOS_TASK_NONE) {
         *ID = readyTasks[priority][0];
         return true;
      }

   }

   // No ready task was found
   return false;
}


/**
 * @brief Updates a ready tasks list. Pushes the current to the bottom
 *        if it was running and updates its state
 * 
 * @param ID ID of the task to be moved on the list (or removed)
 */
static void MyRtos_updateReadyList(taskID_t ID) {

   uint32_t slot = 0;

   // Check if it's a valid ID
   if (ID == MY_RTOS_TASK_NONE) {
      return;
   }

   taskPriority_t priority = MyRtos_TasksList[ID].basePriority;

   // Verify that the passed task is the running task on its list
   if (readyTasks[priority][0] != ID) {
      return;
   }

   // Push ready tasks down the list
   do {
      slot++;
      readyTasks[priority][slot - 1] = readyTasks[priority][slot];
   } while (readyTasks[priority][slot] != MY_RTOS_TASK_NONE &&
            slot < MY_RTOS_MAX_TASKS);

   // If the current task was still running mark it as ready and add it to the 
   // list
   if (MyRtos_TasksList[ID].state == TASK_RUNNING) {
      MyRtos_TasksList[ID].state = TASK_READY;
      readyTasks[priority][slot-1] = ID;
   }
}

/**
 * @brief Register a new task to the OS scheduler.
 * 
 * @param task Pointer to the task control structure
 */
static bool MyRtos_initTask(taskControl_t *task, taskID_t ID) {
   MyRtos_initStack(task);

   MyRtos_AddReadyTask(ID);

   return true;
}

static void MyRtos_initStack(taskControl_t *task) {
     // Initialize stack with 0
   memset(task->stack, 0, task->stackSize);

   // Point stack pointer to last unused position in stack. 17 positions used
   task->stackPointer = (uint32_t)(task->stack + (task->stackSize / 4) - 17);

   // Indicate ARM/Thumb mode in PSR registers
   task->stack[(task->stackSize / 4) - 1] = MY_RTOS_INITIAL_xPSR;

   // Program counter is the pointer to task
   task->stack[(task->stackSize / 4) - 2] = (uint32_t)task->entryPoint;

   // LR - Link Register. Could be use to point to a return hook. 0 for now
   task->stack[(task->stackSize / 4) - 3] = MyRtos_returnHook;

   // R0 - First parameter passed to the task. 0 for now
   task->stack[(task->stackSize / 4) - 8] = task->initialParameter;

   // LR from stack
   task->stack[(task->stackSize / 4) - 9] = MY_RTOS_EXC_RETURN; 
}


/**
 * @brief Idle task. This task will be running when there is no other ready task
 * 
 * @param param None
 */
static void MyRtos_idleTask(void *param) {
   while(1) {
      __WFI();
   }
}


/**
 * @brief Hook to catch any return from a running task
 * 
 */
static void MyRtos_returnHook(void) {
   while(1) {
      __WFI();
   }
}


/**
 * @brief Updates the delay counter of every task and sets it to ready if time
 *        is up
 * 
 */
static void MyRtos_delaysUpdate(void) {
   taskID_t i = 0;

   systemTicks++;

   while(MyRtos_TasksList[i].entryPoint != 0) {
      if (MyRtos_TasksList[i].delay != 0) {
         MyRtos_TasksList[i].delay--;
         if (MyRtos_TasksList[i].delay == 0) {
            MyRtos_AddReadyTask(i);
         }
      }
      i++;
   }
}


/**
 * @brief Handler for the systick peripheral. It updates delays and calls the
 *        scheduler
 * 
 */
void SysTick_Handler(void) {
   MyRtos_delaysUpdate();

   MyRtos_SchedulerUpdate();
}
