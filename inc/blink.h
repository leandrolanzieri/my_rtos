#include "my_rtos.h"
#include "sapi.h"

extern uint8_t stack2[MY_RTOS_STACK_SIZE];

typedef struct {
   uint32_t delay;
   gpioMap_t led;
} blinkTaskData_t;

void blink(void *config);