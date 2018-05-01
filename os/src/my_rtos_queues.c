#include "stdbool.h"
#include "chip.h"
#include "my_rtos.h"
#include "my_rtos_events.h"
#include "my_rtos_queues.h"

extern osState_t osState;
extern bool osSwitchRequired;

/**
 * @brief Initializes a queue for it's use.
 * 
 * @param queue Pointer to the queue to be initialized
 * @return true Queue initialized successfully
 * @return false Could not initialize queue
 */
bool MyRtos_QueueInit(queue_t *queue) {
   queueID_t ID = 0;

   // Initialize queue structure
   queue->state = QUEUE_EMPTY;
   queue->ID = ID;

   // Initialize send & receive events
   MyRtos_EventInit(&(queue->sendEvent));
   MyRtos_EventInit(&(queue->receiveEvent));

   // Increment ID for next one
   ID++;

   return true;
}


/**
 * @brief Receives a data from a queue. If queue is empty the task will be
 *        blocked until data is available
 * 
 * @param queue Pointer to the queue to be read
 * @param data Pointer to save the queue data
 * @return true Queue read successfully
 * @return false Could not read queue
 */
bool MyRtos_QueueReceive(queue_t *queue, queueItem_t *data) {

   // Check if valid calling context
   if (osState == STATE_IRQ) {
      return false;
   }
   
   os_enter_critical();

   // Check if queue is ready to use
   if (queue->state == QUEUE_UNINITIALIZED) {
      return false;
   } else if (queue->state == QUEUE_EMPTY) {
      // If queue is empty task should wait for the event
      // only if the event is free to use
      if (queue->receiveEvent.state == EVENT_INITIALIZED) {
         MyRtos_EventWait(&(queue->receiveEvent));
      } else {

         os_exit_critical();

         return false;
      }
   }

   // Save data to return
   *data = queue->item;

   // Mark queue as empty
   queue->state = QUEUE_EMPTY;

   // Send send event to unblock any waiting task
   MyRtos_EventSend(&(queue->sendEvent));

   os_exit_critical();

   return true;
}


/**
 * @brief Sends data to a queue. If the queue is full the task should wait 
 *        until there is free space.
 * 
 * @param queue Pointer to the queue
 * @param data Data to put into que queue
 * @return true Data was sent succesfully
 * @return false Could not send data
 */
bool MyRtos_QueueSend(queue_t *queue, queueItem_t data) {

   // Disable interrupts to avoid race conditions
   os_enter_critical();
   
   if (osState == STATE_IRQ) {
      // Mark that a scheduler update is needed
      osSwitchRequired = true;
   }

   // Check if queue is ready to use
   if (queue->state == QUEUE_UNINITIALIZED) {
      // Enable interrupts before returning
         os_exit_critical();

      return false;
   } else if (queue->state == QUEUE_FULL) {
      // If queue is full task should wait for the event
      // only if the event is free to use
      if (queue->sendEvent.state == EVENT_INITIALIZED) {
         MyRtos_EventWait(&(queue->sendEvent));
      } else {
         // Enable interrupts before returning
            os_exit_critical();

         return false;
      }
   }

   // Save data to the queue
   queue->item = data;

   // Mark queue as full
   queue->state = QUEUE_FULL;

   // Send receive event to unblock any waiting task
   MyRtos_EventSend(&(queue->receiveEvent));

   os_exit_critical();

   return true;
}