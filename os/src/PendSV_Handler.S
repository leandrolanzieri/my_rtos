/*
 */
	.syntax unified

	.text

	.global PendSV_Handler

	.extern MyRtos_GetNextContext

	.thumb_func

PendSV_Handler:
   cpsid i
   /* Push FPU context if necessary */
   tst lr, 0x10 /* EXC_RETURN[4] == 1 ? */
   it eq
   vpusheq {s16-s31}

   /* Integer context */
	push {r4-r11,lr}
   
   /* Get next context  using current context as parameter (R0)*/
	mrs r0,msp
	bl MyRtos_GetNextContext
	msr msp,r0

   /* Recover integer context */
	pop {r4-r11,lr}
	
   /* Recover FPU context if necessary*/
   tst lr, 0x10
   it eq
   vpopeq {s16-s31}
   cpsie i
   bx lr
