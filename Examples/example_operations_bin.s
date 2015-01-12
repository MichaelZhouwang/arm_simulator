@ Utilisation des instructions AND BIC EOR et ORR



.text
.global main

mov r1, #2
mov r2, #1

	@@@ AND @@@
@ Immediate
and r0, r1, #2

@ Register
and r0, r1, r2

@ Logical shift left by immediate
and r0, r1, r2, LSL #1

@ Logical shift left by register
and r0, r1, r2, LSL r2

@ Logical shift right by immediate
and r0, r2, r1, LSR #1

@ Logical shift right by register
and r0, r2, r1, LSR r2

@ Arithmetic shift right by immediate
and r0, r1, r2, ARS #1

@ Arithmetic shift right by register
and r0, r1, r2, ARS r2

@ Rotate right by immediate
and r0, r1, r2, ROR #1

@ Rotate right by register
and r0, r1, r2, ROR r2

@ Rotate right with extend
and r0, r1, r2, RRX


	@@@ BIC @@@
@ Immediate
bic r0, r1, #1

@ Register
bic r0, r1, r2

@ Logical shift left by immediate
bic r0, r1, r2, LSL #1

@ Logical shift left by register
bic r0, r1, r2, LSL r2

@ Logical shift right by immediate
bic r0, r1, r2, LSR #1

@ Logical shift right by register
bic r0, r1, r2, LSR r2

@ Arithmetic shift right by immediate
bic r0, r1, r2, ARS #1

@ Arithmetic shift right by register
bic r0, r1, r2, ASR r2

@ Rotate right by immediate
bic r0, r1, r2, ROR #1

@ Rotate right by register
bic r0, r1, r2, ROR r2

@ Rotate right with extend
bic r0, r1, r2, RRX
	
	
	@@@ EOR @@@
@ Immediate
eor r0, r1, #2

@ Register
eor r0, r1, r2

@ Logical shift left by immediate
eor r0, r1, r2, LSL #1

@ Logical shift left by register
eor r0, r1, r2, LSL r2

@ Logical shift right by immediate
eor r0, r1, r2, LSR #1

@ Logical shift right by register
eor r0, r1, r2, LSR r2

@ Arithmetic shift right by immediate
eor r0, r1, r2, ASR #1

@ Arithmetic shift right by register
eor r0, r1, r2, ASR r2

@ Rotate right by immediate
eor r0, r1, r2, ROR #1

@ Rotate right by register
eor r0, r1, r2, ROR r2

@ Rotate right with extend
eor r0, r1, r2, RRX
	
	
	@@@ ORR @@@
@ Immediate
orr r0, r1, #2

@ Register
orr r0, r1, r2

@ Logical shift left by immediate
orr r0, r1, r2, LSL #1

@ Logical shift left by register
orr r0, r1, r2, LSL r2

@ Logical shift right by immediate
orr r0, r1, r2, LSR #1

@ Logical shift right by register
orr r0, r1, r2, LSR r2

@ Arithmetic shift right by immediate
orr r0, r1, r2, ASR #1

@ Arithmetic shift right by register
orr r0, r1, r2, ASR r2

@ Rotate right by immediate
orr r0, r1, r2, ROR #1

@ Rotate right by register
orr r0, r1, r2, ROR r2

@ Rotate right with extend
orr r0, r1, r2, RRX
