   .syntax unified
   .data
state: .word 0 // Context state

   .text // This is code
   .global SysTick_Handler // SysTick_Handler must be seen from other files
   .extern sp1, sp2 // sp1 & sp2 are defined elsewhere
   .thumb_func // These are thumb instructions
SysTick_Handler:

   ldr r0, =state // r0 = &state

   ldr r1, [r0] // r1 = *r0

   tbb [pc, r1]

// Switch case...
tabla:
   .byte (case0 - tabla) / 2
   .byte (case1 - tabla) / 2
   .byte (case2 - tabla) / 2
   .byte (case_end - tabla) / 2

case0:
   // state = 1
   mov r1, 1
   str r1, [r0]

   // SP = SP1
   ldr r0, =sp1
   ldr r1, [r0]
   msr msp, r1

   // break
   b case_end

case1:
   // state = 2
   mov r1, 2
   str r1, [r0]

   // SP1 = SP
   ldr r0, =sp1
   mrs r1, msp
   str r1, [r0]

   // SP = SP2
   ldr r0, =sp2
   ldr r1, [r0]
   msr msp, r1

   // break
   b case_end

case2:
   // state = 1
   mov r1, 1
   str r1, [r0]

   // SP2 = SP
   ldr r0, =sp2
   mrs r1, msp
   str r1, [r0]

   // SP = SP1
   ldr r0, =sp1
   ldr r1, [r0]
   msr msp, r1

   // break
   b case_end

case_end:
   bx lr
