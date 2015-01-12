@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@ Exceptions
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.global	_undefined_instruction_vect
_undefined_instruction_vect:	.text
    mov r0, #2
    movs pc, r14

.global	_software_interrupt_vect
_software_interrupt_vect:	.text
    mov r0, #3
    movs pc, r14

.global	_prefetch_abort_vect
_prefetch_abort_vect:	.text
    mov r0, #4
    subs pc, r14, #4

.global	_data_abort_vect
_data_abort_vect:	.text
    mov r0, #5
    subs pc, r14, #8

.global	_data_abort_vect
_data_abort_vect:	.text
    mov r0, #5
    subs pc, r14, #8

.global	_irq_vect
_irq_vect:	.text
    mov r0, #6
    subs pc, r14, #8

.global	_fiq_vect
_fiq_vect:	.text
    mov r0, #7
    subs pc, r14, #8

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@ Main
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.global main
.text

decr:
    subs r0, r0, #1
    mov pc, lr

main:
    mov r0, #10
loop:
    bl decr
    bne loop
end:
    swi 0x123456
.data

