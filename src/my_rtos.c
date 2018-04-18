#include <string.h>
#include "chip.h"
#include "my_rtos.h"

///////////////////////////////////////////////////////////////////////////////
// Internal functions declarations
///////////////////////////////////////////////////////////////////////////////

static bool MyRtos_initTask(taskControl_t *task, taskID_t ID);

static void MyRtos_idleTask(void *param);

static void MyRtos_schedulerUpdate(void);

static void MyRtos_delaysUpdate(void);

static void MyRtos_returnHook(void);

static bool MyRtos_getReadyTask(taskID_t *ID);

static void MyRtos_addReadyTask(taskID_t ID);

static void MyRtos_updateReadyList(taskID_t ID);

static void MyRtos_initStack(taskControl_t *task);

///////////////////////////////////////////////////////////////////////////////
// Internal data definitions
///////////////////////////////////////////////////////////////////////////////

static uint32_t systemTicks = 0;

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

// Tasks list
extern taskControl_t MyRtos_TasksList[];

///////////////////////////////////////////////////////////////////////////////
// External functions definitions
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Starts the OS execution.
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


void MyRtos_DelayMs(uint32_t ms) {
   if (currentTask != MY_RTOS_TASK_NONE && ms != 0 &&
       MyRtos_TasksList[currentTask].state == TASK_RUNNING)
   {
      MyRtos_TasksList[currentTask].delay = ms;
      MyRtos_TasksList[currentTask].state = TASK_BLOCKED;
      MyRtos_schedulerUpdate();
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
      currentTask = MY_RTOS_IDLE_TASK;
      return idleTaskControl.stackPointer;
   }

   // Mark the new current task as 'Running'
   MyRtos_TasksList[currentTask].state = TASK_RUNNING;

   // Return new stack pointer
   return MyRtos_TasksList[currentTask].stackPointer;
}


/**
 * @brief Initializes an event for it's future use.
 * 
 * @param event Pointer to the event to be initialized
 * @return true Event was initialized
 * @return false Problem initializing event
 */
bool MyRtos_EventInit(event_t *event) {
   static eventID_t ID = 0;

   // Initialize event structure
   event->state = EVENT_INITIALIZED;
   event->eventID = ID;
   event->taskID = MY_RTOS_TASK_NONE;

   // Increment event ID for next one
   ID++;

   return true;
}


/**
 * @brief 
 * 
 * @param event Waits for an event to be sent. The task that calls this function
 *              will be blocked until the event is sent.
 * @return true Event waited successfully
 * @return false Could not wait for event (check event state)
 */
bool MyRtos_EventWait(event_t *event) {
   
   // Check if state can be used
   if (event->state != EVENT_INITIALIZED) {
      return false;
   }

   // Save current task ID into the event to unblock it
   event->taskID = currentTask;

   // Mark event as pending
   event->state = EVENT_PENDING;

   // Mark current task as blocked
   MyRtos_TasksList[currentTask].state = TASK_BLOCKED;

   // Call the scheduler for context switching
   MyRtos_schedulerUpdate();
   
   return true;
}


/**
 * @brief Sends an event to unblock a task that was waiting for it.
 * 
 * @param event Pointer to the event to be sent
 * @return true Event was sent successfully
 * @return false Event could not be sent
 */
bool MyRtos_EventSend(event_t *event) {

   // Check if state was pending
   if (event->state != EVENT_PENDING) {
      return false;
   }

   // Add the blocked task as ready
   MyRtos_addReadyTask(event->taskID);

   // Reset the event's task id
   event->taskID = MY_RTOS_TASK_NONE;

   // Reset the event's state
   event->state = EVENT_INITIALIZED;

   // Call the scheduler for context switching
   MyRtos_schedulerUpdate();

   return true;
}
///////////////////////////////////////////////////////////////////////////////
// Internal functions definitions
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Looks for the highest priority ready task on the lists and returns
 *        it's ID.
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
 * @brief Adds a task ID to the bottom of the ready list of it's priority and
 *        changes it's state to ready.
 * 
 * @param ID The ID of the task to be added to the list
 */
static void MyRtos_addReadyTask(taskID_t ID) {
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
 * @brief Updates a ready tasks list. Pushes the current to the bottom
 *        if it was running and updates it's state.
 * 
 * @param ID ID of the task to be moved on the list (or removed)
 */
static void MyRtos_updateReadyList(taskID_t ID) {
   // Slot 0 is supposed to be used by the running task
   uint32_t slot = 1;

   // Check if it's a valid ID
   if (ID == MY_RTOS_TASK_NONE) {
      return;
   }

   taskPriority_t priority = MyRtos_TasksList[ID].basePriority;

   // Verify that the passed task is the running task on it's list
   if (readyTasks[priority][0] != ID) {
      return;
   }

   // Sweep the list pushing the tasks to the top
   while (readyTasks[priority][slot] != MY_RTOS_TASK_NONE &&
          slot < MY_RTOS_MAX_TASKS)
   {
      readyTasks[priority][slot - 1] = readyTasks[priority][slot];
      slot++;
   }

   // Check if the slot is inside boundaries
   if (slot < MY_RTOS_MAX_TASKS) {

      // If the current task was running put it at the bottom
      if (MyRtos_TasksList[ID].state == TASK_RUNNING) {
         MyRtos_TasksList[ID].state = TASK_READY;
         readyTasks[priority][slot] = ID;
      } else {
         readyTasks[priority][slot - 1] = MY_RTOS_TASK_NONE;
      }
      
   }
}

/**
 * @brief Register a new task to the OS scheduler.
 * 
 * @param task Pointer to the task control structure
 */
static bool MyRtos_initTask(taskControl_t *task, taskID_t ID) {
   MyRtos_initStack(task);

   MyRtos_addReadyTask(ID);

   return true;
}

static void MyRtos_initStack(taskControl_t *task) {
     // Initialize stack with 0
   memset(task->stack, 0, task->stackSize);

   // Point stack pointer to last unused position in stack. 17 posisions used.
   task->stackPointer = (uint32_t)(task->stack + (task->stackSize / 4) - 17);

   // Indicate ARM/Thumb mode in PSR registers
   task->stack[(task->stackSize / 4) - 1] = MY_RTOS_INITIAL_xPSR;

   // Program counter is the pointer to task
   task->stack[(task->stackSize / 4) - 2] = (uint32_t)task->entryPoint;

   // LR - Link Register. Could be use to point to a return hook. 0 for now.
   task->stack[(task->stackSize / 4) - 3] = MyRtos_returnHook;

   // R0 - First parameter passed to the task. 0 for now.
   task->stack[(task->stackSize / 4) - 8] = task->initialParameter;

   // LR from stack
   task->stack[(task->stackSize / 4) - 9] = MY_RTOS_EXC_RETURN; 
}

static void MyRtos_idleTask(void *param) {
   while(1) {
      __WFI();
   }
}

static void MyRtos_returnHook(void) {
   while(1) {
      __WFI();
   }
}

static void MyRtos_delaysUpdate(void) {
   taskID_t i = 0;

   while(MyRtos_TasksList[i].entryPoint != 0) {
      if (MyRtos_TasksList[i].delay != 0) {
         MyRtos_TasksList[i].delay--;
         if (MyRtos_TasksList[i].delay == 0) {
            MyRtos_addReadyTask(i);
         }
      }
      i++;
   }
}

static void MyRtos_schedulerUpdate(void) {
   // Activate PendSV for context switching
   SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;

   // Instruction Synchronization Barrier: this way we make sure that all
   // pipelined instructions are executed
   __ISB();
      
   // Data Synchronization Barrier: this way we make sure that all
   // memory accesses are completed
   __DSB();
}

void SysTick_Handler(void) {
   // Increment system ticks
   systemTicks++;

   MyRtos_delaysUpdate();

   MyRtos_schedulerUpdate();
}
