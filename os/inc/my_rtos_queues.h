#ifndef MY_RTOS_QUEUES_H
#define MY_RTOS_QUEUES_H

#include "stdbool.h"

#include "my_rtos.h"
#include "my_rtos_events.h"
#include "my_rtos_config.h"

typedef uint32_t queueID_t;

typedef MY_RTOS_QUEUE_TYPE queueItem_t;

typedef enum {
   QUEUE_UNINITIALIZED = 0,
   QUEUE_EMPTY = 1,
   QUEUE_FULL = 2
} queueState_t;

typedef struct {
   event_t sendEvent;
   event_t receiveEvent;
   queueID_t ID;
   queueItem_t item;
   queueState_t state;
} queue_t;

bool MyRtos_QueueInit(queue_t *queue);

bool MyRtos_QueueReceive(queue_t *queue, queueItem_t *data);

bool MyRtos_QueueSend(queue_t *queue, queueItem_t data);

#endif