
.global main
.global irq_handler


.text

main:
	mov r0, #0
	mov r1, #10
	cdp P3, #2, C12, C10, C3, #4;
	mov r1, #10


	swi 0x123456
	

undef_instruction:
	mov r4, #0xFF

	subs pc, lr, #4

.org 0xFE0
_reset: b main  @0x1000
_undefined_instruction: b undef_instruction

