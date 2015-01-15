@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 
@ Fonction PGCD(a, b: entier): entier; 
@     				
@ Var
@    résultat: entier;
@     
@ Debut
@     
@    Si(a = b) Alors
@        résultat := a;
@    Sinon Si(a > b) Alors
@        résultat := PGCD(a - b, b);
@    Sinon
@        résultat := PGCD(a, b - a);
@    FinSi
@     
@    Renvoyer résultat;
@     
@ Fin
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


.bss
	res: .word
.data
	A: .word 12
	B: .word 9


.global main
.text

main:
	@ r0 = A, r1 = B, r2 = res
	
	ldr r0, =A
	ldr r0, [r0]
	ldr r1, =B
	ldr r1, [r1]
	stmdb sp!, {r0, r1}
	bl pgcd
	ldmia sp!, {r0, r1}
	ldr r2, =res
	str r2, [r2]
	
	swi 0x123456
	
pgcd:
	@ r0 = A, r1 = B, r2 = res
	ldr r0, [sp, #4]
	ldr r1, [sp]
	cmp r0, r1
		bne else1
		mov r2, r0
		b endif
	else1:
		bls else2
			sub r0, r0, r1
			str lr, [sp, #-4]!
			stmdb sp!, {r0, r1}
			bl pgcd
			ldmia sp!, {r0, r1}
			ldr lr, [sp], #4
			b endif
		else2:
			sub r1, r1, r0
			str lr, [sp, #-4]!
			stmdb sp!, {r0, r1}
			bl pgcd
			ldmia sp!, {r0, r1}
			ldr lr, [sp], #4
	endif:
	
	mov pc, lr
	
