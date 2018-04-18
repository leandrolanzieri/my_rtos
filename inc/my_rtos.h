#ifndef MY_RTOS_H
#define MY_RTOS_H

#include "stdint.h"
#include "stdbool.h"

#define MY_RTOS_LAST_TASK {0,0,0,0,0}

/***********************************************************************/
#define MY_RTOS_STACK_SIZE  512

#define MY_RTOS_INITIAL_xPSR    (1 << 24)

#define MY_RTOS_EXC_RETURN (0xFFFFFFF9)

#define MY_RTOS_TASK_NONE -1

#define MY_RTOS_IDLE_TASK -2

#define MY_RTOS_MAX_TASKS  64

#define MY_RTOS_PRIORITY_LEVELS 4

#define MY_RTOS_INIT_TASK(_entryPoint, _stack, _stackSize, _parameter, _priority)        \
      {.entryPoint = _entryPoint, .stack = _stack, .stackPointer = 0,         \
      .stackSize = _stackSize, .state = TASK_READY,                           \
      .initialParameter = _parameter, .delay = 0, .basePriority = _priority,  \
      .instantPriority = _priority},


/***********************************************************************/
typedef void(*task_t)(void *);

typedef int32_t taskID_t;

typedef uint32_t eventID_t;

typedef uint32_t taskPriority_t;

typedef enum {
   TASK_ERROR = 0,
   TASK_READY = 1,
   TASK_RUNNING = 2,
   TASK_BLOCKED = 3,
   TASK_HALTED = 4
} taskState_t;

typedef struct {
   task_t entryPoint;
   uint32_t *stack;
   uint32_t stackPointer;
   uint32_t stackSize;
   taskState_t state;
   void* initialParameter;
   uint32_t delay;
   taskPriority_t basePriority;
   taskPriority_t instantPriority;
} taskControl_t;

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

void MyRtos_StartOS(void);

void MyRtos_DelayMs(uint32_t ms);

bool MyRtos_EventInit(event_t *event);

bool MyRtos_EventWait(event_t *event);

bool MyRtos_EventSend(event_t *event);

#endif
