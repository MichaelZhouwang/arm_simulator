.global main
.global irq_handler


.text

main:
	msr cpsr_c, #0		@ mode USR
	mov r0, #0			@ r0 = 0
	mov r1, #1			@ r1 = 1
	mov r2, #2			@ r2 = 2

loop1:					
	cmp r0, #5			@ while (r0 < 5)
	beq end_loop1		@ {	
	add r0, #1				@ r0 ++
	b loop1				

end_loop1:				@ }

	swi 0x10

loop:					@ while (1)
	add r0, #1				@ r0 ++
	b loop


	swi 0x123456		@ return
	


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
	stmda sp!, {r0-r2}		@ sauvergarde r0, r1, r2
	
	mov r0, #0xFF			@ r0 = 0xFF
	mrs r1, CPSR			@ r1 = CPSR
	mrs r2, SPSR			@ r2 = SPSR

	mov r0, #0xEE
	mov r0, #0xBB

	ldmib sp!, {r0-r2}

	subs pc, lr, #4

fiq_handler:
	mov r4, #0xAA
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

