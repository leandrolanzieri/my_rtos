#include "my_rtos.h"
#include "my_rtos_events.h"
#include "chip.h"


// Tasks list
extern taskControl_t MyRtos_TasksList[];
extern osState_t osState;
extern bool osSwitchRequired;

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

   // Check if valid calling context
   if (osState == STATE_IRQ) {
      return false;
   }

   // Disable interrupts to avoid race conditions
   os_enter_critical();

   // Check if state can be used
   if (event->state != EVENT_INITIALIZED) {
      // Enable interrupts before returning
      os_exit_critical();

      return false;
   }

   // Get current task's ID
   taskID_t currentTask = MyRtos_GetCurrentTask();

   // Save current task ID into the event to unblock it
   event->taskID = currentTask;

   // Mark event as pending
   event->state = EVENT_PENDING;

   // Mark current task as blocked
   MyRtos_TasksList[currentTask].state = TASK_BLOCKED;

   // Enable interrupts before returning
   os_exit_critical();

   // Call the scheduler for context switching
   MyRtos_SchedulerUpdate();
   
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
   // Disable interrupts to avoid race conditions
   os_enter_critical();

   if (osState == STATE_IRQ) {
      // Mark that a scheduler update is needed
      osSwitchRequired = true;
   }

   // Check if state was pending
   if (event->state != EVENT_PENDING) {
      // Enable interrupts before returning
      os_exit_critical();

      return false;
   }

   // Add the blocked task as ready
   MyRtos_AddReadyTask(event->taskID);

   // Reset the event's task id
   event->taskID = MY_RTOS_TASK_NONE;

   // Reset the event's state
   event->state = EVENT_INITIALIZED;

   // Enable interrupts before returning
   os_exit_critical();

   // Call switch context only if not calling from an IRQ
   if (osState != STATE_IRQ) {
      // Call the scheduler for context switching
      MyRtos_SchedulerUpdate();
   }

   return true;
}