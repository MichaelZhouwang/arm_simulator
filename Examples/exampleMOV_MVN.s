@ Utilisation des instructions MVN et MOV

.global main
.text

main:
	@@@ MOV @@
@ Immediate
	mov r1, #2

@ Register
	mov r2, r1

@ Logical shift left by immediate
	mov r2, r1, LSL #1

@ Logical shift left by register
	mov r2, r1, LSL r1

@ Logical shift right by immediate
	mov r2, r1, LSR #1

@ Logical shift right by register
	mov r2, r1, LSR r1

@ Arithmetic shift right by immediate
	mov r2, r1, ASR #1

@ Arithmetic shift right by register
	mov r2, r1, ASR r1

@ Rotate right by immediate
	mov r2, r1, ROR #1

@ Rotate right by register
	mov r2, r1, ROR r1

@ Rotate right with extend
	mov r2, r1, RRX

	@@@ MVN @@@
@ Immediate
	mvn r1, #2

@ Register
	mvn r2, r1

@ Logical shift left by immediate
	mvn r2, r1, LSL #1

@ Logical shift left by register
	mvn r2, r1, LSL r1

@ Logical shift right by immediate
	mvn r2, r1, LSR #1

@ Logical shift right by register
	mvn r2, r1, LSR r1

@ Arithmetic shift right by immediate
	mvn r2, r1, ASR #1

@ Arithmetic shift right by register
	mvn r2, r1, ASR r1

@ Rotate right by immediate
	mvn r2, r1, ROR #1

@ Rotate right by register
	mvn r2, r1, ROR r1

@ Rotate right with extend
	mvn r2, r1, RRX
	
	
	swi 0x123456
