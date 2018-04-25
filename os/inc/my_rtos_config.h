#ifndef MY_RTOS_CONFIG_H
#define MY_RTOS_CONFIG_H

#include "stdint.h"

// Maximum stack size for tasks in bytes
#define MY_RTOS_STACK_SIZE  512

// Maximum number of tasks
#define MY_RTOS_MAX_TASKS  64

// Amount of priority levels
#define MY_RTOS_PRIORITY_LEVELS 4

// Type of queue item
#define MY_RTOS_QUEUE_TYPE uint32_t

#endif