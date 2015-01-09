@ Utilisation des instructions ADD, ADC, SUB, SBC, RSB, RSC
.global main
.text

main:
	@@@ ADD @@@
	mov r1, #1
	mov r2, #2
@ Immediate
	add r0, r1, #1

@ Register
	add r0, r1, r2

@ Logical shift left by immediate
	add r0, r1, r1, LSL #1

@ Logical shift left by register
	add r0, r1, r1, LSL r2

mov r1, #8
@ Logical shift right by immediate
	add r0, r1, r1, LSR #1 

@ Logical shift right by register
	add r0, r1, r1, LSR r2

@ Arithmetic shift right by immediate
	add r0, r1, r1, ASR #1

@ Arithmetic shift right by register
	add r0, r1, r1, ASR r2

@ Rotate right by immediate
	add r0, r1, r1, ROR #1

@ Rotate right by register
	add r0, r1, r1, ROR r2

@ Rotate right with extend
	add r0, r1, r1, RRX


	@@@ ADC @@@
	mov r1, #1
	mov r2, #2
@ Immediate
	adc r0, r1, #1

@ Register
	adc r0, r1, r2

@ Logical shift left by immediate
	adc r0, r1, r1, LSL #1

@ Logical shift left by register
	adc r0, r1, r1, LSL r2

mov r1, #8
@ Logical shift right by immediate
	adc r0, r1, r1, LSR #1 

@ Logical shift right by register
	adc r0, r1, r1, LSR r2

@ Arithmetic shift right by immediate
	adc r0, r1, r1, ASR #1

@ Arithmetic shift right by register
	adc r0, r1, r1, ASR r2

@ Rotate right by immediate
	adc r0, r1, r1, ROR #1

@ Rotate right by register
	adc r0, r1, ROR r2

@ Rotate right with extend
	adc r0, r1, r1, RRX


	@@@ SUB @@@
	mov r1, #64
	mov r2, #4
@ Immediate
	sub r0, r1, #1

@ Register
	sub r0, r1, r2

@ Logical shift left by immediate
	sub r0, r1, r2, LSL #1

@ Logical shift left by register
	mov r3, #1 
	sub r0, r1, r2, LSL r3

@ Logical shift right by immediate
	sub r0, r1, r2, LSR #1

@ Logical shift right by register
	sub r0, r1, r2, LSR r2

@ Arithmetic shift right by immediate
	sub r0, r1, r2, ASR #1

@ Arithmetic shift right by register
	sub r0, r1, r2, ASR r2

@ Rotate right by immediate
	sub r0, r1, r2, ROR #1

@ Rotate right by register
	sub r0, r1, r2, ROR r2

@ Rotate right with extend
	sub r0, r1, r2, RRX
	
	
	@@@ SBC @@@
	mov r1, #64
	mov r2, #4
@ Immediate
	sbc r0, r1, #1

@ Register
	sbc r0, r1, r2

@ Logical shift left by immediate
	sbc r0, r1, r2, LSL #1

@ Logical shift left by register
	mov r3, #1 
	sbc r0, r1, r2, LSL r3

@ Logical shift right by immediate
	sbc r0, r1, r2, LSR #1

@ Logical shift right by register
	sbc r0, r1, r2, LSR r2

@ Arithmetic shift right by immediate
	sbc r0, r1, r2, ASR #1

@ Arithmetic shift right by register
	sbc r0, r1, r2, ASR r2

@ Rotate right by immediate
	sbc r0, r1, r2, ROR #1

@ Rotate right by register
	sbc r0, r1, r2, ROR r2

@ Rotate right with extend
	sbc r0, r1, r2, RRX
	
	
	@@@ RSB @@@
	mov r1, #64
	mov r2, #4
@ Immediate
	rsb r0, r1, #1

@ Register
	rsb r0, r1, r2

@ Logical shift left by immediate
	rsb r0, r2, r1, LSL #1

@ Logical shift left by register
	mov r3, #1 
	rsb r0, r1, r2, LSL r3

@ Logical shift right by immediate
	rsb r0, r1, r2, LSR #1

@ Logical shift right by register
	rsb r0, r1, r2, LSR r2

@ Arithmetic shift right by immediate
	rsb r0, r1, r2, ASR #1

@ Arithmetic shift right by register
	rsb r0, r1, r2, ASR r2

@ Rotate right by immediate
	rsb r0, r1, r2, ROR #1

@ Rotate right by register
	rsb r0, r1, r2, ROR r2

@ Rotate right with extend
	rsb r0, r1, r2, RRX


	@@@ RSC @@@
	mov r1, #64
	mov r2, #4
@ Immediate
	rsc r0, r1, #1

@ Register
	rsc r0, r1, r2

@ Logical shift left by immediate
	rsc r0, r2, r1, LSL #1

@ Logical shift left by register
	mov r3, #1 
	rsc r0, r1, r2, LSL r3

@ Logical shift right by immediate
	rsc r0, r1, r2, LSR #1

@ Logical shift right by register
	rsc r0, r1, r2, LSR r2

@ Arithmetic shift right by immediate
	rsc r0, r1, r2, ASR #1

@ Arithmetic shift right by register
	rsc r0, r1, r2, ASR r2

@ Rotate right by immediate
	rsc r0, r1, r2, ROR #1

@ Rotate right by register
	rsc r0, r1, r2, ROR r2

@ Rotate right with extend
	rsc r0, r1, r2, RRX
	
	
	swi 0x123456
