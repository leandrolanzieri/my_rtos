#ifndef MY_RTOS_TASKS_H
#define MY_RTOS_TASKS_H

#include "my_rtos_task.h"

#include "task1.h"
#include "task2.h"
#include "task_button.h"

#define MY_RTOS_TASKS                                                                \
       MY_RTOS_INIT_TASK(task1, (uint32_t *)stack1, MY_RTOS_STACK_SIZE, 0)           \
       MY_RTOS_INIT_TASK(task2, (uint32_t *)stack2, MY_RTOS_STACK_SIZE, 0)           \
       MY_RTOS_INIT_TASK(taskButton, (uint32_t *)stackButton, MY_RTOS_STACK_SIZE, 0)

#endif
