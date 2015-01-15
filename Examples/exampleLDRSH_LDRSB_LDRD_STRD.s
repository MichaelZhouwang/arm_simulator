@ Utilisation des instructions LDRSH, LDRSB, LDRD, STRD

.data
	A: .hword 10
	B: .hword 11
	C: .byte 12
	D: .byte 13
	E: .dword 14
	F: .dword 15

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
	ldr r0, ptr_a
	sub r0, r0, #1
	ldrsh r1, [r0, #1]!
	
@ Resgister pre-indexed
	ldr r0, ptr_b
	sub r0, r0, #1
	ldrsh, r1, [r0, r2]!


@ Immediate post-indexed
	ldr r0, ptr_b
	ldrsh, r1, [r0], #1

@ Register post-indexed
	ldr r0, ptr_b
	ldrsh, r1, [r0], r2


  @@@ LDRSB @@@
@ Immediate offset
	ldr r0, ptr_c
	sub r0, r0, #1
	ldrsb r1, [r0, #1]
	
@ Resgister offset
	mov r2, #1
	ldr r0, ptr_d
	sub r0, r0, #1
	ldrsb r1, [r0, r2]


@ Immediate pre-indexed
	ldr r0, ptr_c
	sub r0, r0, #1
	ldrsb r1, [r0, #1]!
	
@ Resgister pre-indexed
	ldr r0, ptr_d
	sub r0, r0, #1
	ldrsb, r1, [r0, r2]!


@ Immediate post-indexed
	ldr r0, ptr_c
	ldrsb, r1, [r0], #1

@ Register post-indexed
	ldr r0, ptr_d
	ldrsb, r1, [r0], r2


  @@@ LDRD @@@
@ Immediate offset
	ldr r0, ptr_e
	sub r0, r0, #1
	ldrd r1, [r0, #1]
	
@ Resgister offset
	mov r2, #1
	ldr r0, ptr_f
	sub r0, r0, #1
	ldrd r1, [r0, r2]


@ Immediate pre-indexed
	ldr r0, ptr_e
	sub r0, r0, #1
	ldrd r1, [r0, #1]!
	
@ Resgister pre-indexed
	ldr r0, ptr_f
	sub r0, r0, #1
	ldrd, r1, [r0, r2]!


@ Immediate post-indexed
	ldr r0, ptr_e
	ldrd, r1, [r0], #1

@ Register post-indexed
	ldr r0, ptr_f
	ldrd, r1, [r0], r2
	

  @@@ STRD @@@
	mov r1, #2
@ Immediate offset
	ldr r0, ptr_e
	sub r0, r0, #1
	strd r1, [r0, #1]
	
@ Resgister offset
	mov r2, #1
	ldr r0, ptr_f
	sub r0, r0, #1
	strd r1, [r0, r2]


	mov r1, #4
@ Immediate pre-indexed
	ldr r0, ptr_e
	sub r0, r0, #1
	strd r1, [r0, #1]!
	
@ Resgister pre-indexed
	ldr r0, ptr_f
	sub r0, r0, #1
	strd, r1, [r0, r2]!


	mov r1, #6
@ Immediate post-indexed
	ldr r0, ptr_e
	strd, r1, [r0], #1

@ Register post-indexed
	ldr r0, ptr_f
	strd, r1, [r0], r2
	
	
 ptr_a: .word A
 ptr_b: .word B
 ptr_c: .word C
 ptr_d: .word D
 ptr_e: .word E
 ptr_f: .word F
