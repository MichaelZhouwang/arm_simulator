.global main
.text

main:
    mrs r0, CPSR
    @ interruption svp
    mrs r1, SPSR

    swi 0x123456

irq_handler:
	mov r2, #5
    mrs r1, SPSR
	add r2, #2
	subs pc, lr, #4

.org 0xFF8
_irq: b irq_handler
