.global main
.global irq_handler


.text

main:
	mov r0, #0
@	cdp P3, #2, C12, C10, C3, #4;
	mov r1, #10
	swi 0x10

loop:
	add r0, #1
	b loop


	swi 0x123456
	


undefined_instruction_handler:
	str r0 , [sp, #-4]!		
	mov r0, #1
	ldr r0, [sp, #4]!
	subs pc, lr, #4

swi_handler:
	mov r4, #2
	mov r5, lr
	ldrb r5, [r5, #-1]
	movs pc, lr

prefetch_abort_handler:
	mov r4, #3
	subs pc, lr, #4

data_abort_handler:
	mov r4, #4
	subs pc, lr, #4

irq_handler:
	mov r4, #5
	subs pc, lr, #4

fiq_handler:
	mov r4, #6
	subs pc, lr, #4

.org 0xFE0
_reset: b main  @0x1000
_undefined_instruction: b undefined_instruction_handler
_swi: b swi_handler
_prefetch_abort: b prefetch_abort_handler
_data_abort: b data_abort_handler
.org 0xFF8
_irq: b irq_handler
_fiq: b fiq_handler

