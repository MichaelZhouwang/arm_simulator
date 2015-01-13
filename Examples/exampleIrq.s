
.global main
.global irq_handler


.text

main:
	mov r0, #0
	mov r1, #10
	mov r1, #10
	mov r1, #10

loop:
	cmp r0, #10
	beq end_loop

	add r0, #1
	
	b loop
end_loop:
	mov r2, r1

	swi 0x123456
	

irq_handler:

	subs pc, lr, #4

.org 0xFE0
_reset: b main  @0x1000
_undefined_instruction: subs pc, lr, #4
_swi: subs pc, lr, #4
.org 0xFF8
_irq: b irq_handler @0x1018
@_fiq:  fiq_handler

