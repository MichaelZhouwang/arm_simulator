@ Utilisation des instructions TST, TEQ, CMP
.global main
.text

main:	

	
	mov r1, #1
	mov r2, #2
	mov r3, #3
	mov r0, r1, ROR #1	@ 1000...
	rsb r4, r1, #0		@ -1
	mov r5, r0, LSR #1	@ 0100...
	mvn r6, r5			@ 1011... 
	add r7, r5, #1		@ 0100...1
	mov r8, #0
	add r9, r0, r5		@ 1100...
	
	
TST: @ &
	@ V is unaffected
	tst r0, r0			@ 1000 = 0x8
	tst r3, r1			@ NZCV = 0000 = 0x0
	@ C is affected by the shifting of the 2nd operand
	tst r0, r2			@ 0100 = 0x4
	tst r0, r1, LSR #1	@ op2 = r2 but NZCV = 0110 = 0x6
	

TEQ: @ ^
	@ V is unaffected
	teq r1, r3			@ 0010 = 0x2
	teq r3, r3			@ 0110 = 0x6
	teq r0, r1			@ 1010 = 0xA
	
CMP: @ -	
	cmp r2, r1			@ 0010 = 0x2
	cmp r0, r7			@ 0011 = 0x3
	cmp r4, r4			@ 0110 = 0x6
	cmp r2, r3			@ 1000 = 0x8
	cmp r5, r6			@ 1001 = 0x9
	cmp r4, r1			@ 1010 = 0xA
	
CMN: @ +
	rsb r3, r3, #0
	rsb r6, r6, #0
	rsb r7, r7, #0
	
	cmn r1, r1			@ 0000 = 0x0
	cmn r2, r4			@ 0010 = 0x2
	cmn r0, r7			@ 0011 = 0x3
	cmn r8, r8			@ 0100 = 0x4
	cmn r1, r4			@ 0110 = 0x6
	cmn r0, r0			@ 0111 = 0x7
	cmn r2, r3			@ 1000 = 0x8
	cmn r5, r6			@ 1001 = 0x9
	cmn r9, r9			@ 1010 = 0xA

 swi 0x123456
 @ trouver un moyen de coder les bonnes valeurs pour les reg
 
