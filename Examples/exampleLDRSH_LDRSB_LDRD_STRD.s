@ Utilisation des instructions LDRSH, LDRSB, LDRD, STRD
.data
	A: .hword 10
	B: .hword 11
	C: .hword 12
.global main
.text

	@@@ LDRSH @@@
@ Immediate offset
	ldr r0, ptr_a
	sub r0, r0, #1
	ldrsh r1, [r0, #1]
	
@ Resgister offset
	mov r2, #1
	ldr r0, ptr_b
	sub r0, r0, #1
	ldrsh r1, [r0, r2]

@ Immediate pre-indexed
	ldr r0, ptr_c
	sub r0, r0, #1
	ldrsh r1, [r0, #1]!
	add r0, r0, #1
@ Resgister pre-indexed


@ Immediate post-indexed


@ Register post-indexed





 ptr_a: .word A
