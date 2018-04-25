#ifndef MY_RTOS_EVENTS_H
#define MY_RTOS_EVENTS_H

#include "stdint.h"
#include "stdbool.h"
#include "my_rtos.h"

typedef enum {
   EVENT_UNINITIALIZED = 0,
   EVENT_INITIALIZED = 1,
   EVENT_PENDING = 2,
} eventState_t;

typedef struct {
   taskID_t taskID;
   eventID_t eventID;
   eventState_t state;
} event_t;

bool MyRtos_EventInit(event_t *event);

bool MyRtos_EventWait(event_t *event);

bool MyRtos_EventSend(event_t *event);

#endif