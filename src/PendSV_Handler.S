/*
 */
	.syntax unified

	.text

	.global PendSV_Handler

	.extern MyRtos_GetNextContext

	.thumb_func

PendSV_Handler:
	push {r4-r11,lr}
	mrs r0,msp
	bl MyRtos_GetNextContext
	msr msp,r0
	pop {r4-r11,lr}
	bx lr
