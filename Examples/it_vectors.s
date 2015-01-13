.global main



.text


main:
	mov r1, #5
	mov r2, #9
	mov r3, #110
	mov r3, #2
	mov r3, #4
	mov r3, #5
	mov r3, #6
	mov r3, #7
	mov r3, #2


_irq:
	mov r3, #27
	mov r3, #10
	mov r3, #20
	mov r3, #30
	subs pc, lr, #4

.org 0xFE0
reset: b main
undef: b main
swi: b main
prefetch: b main
data: b main
reserved: b main
irq: b _irq
fiq:

