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


.data
	A: .word 11
	B: .word 4
	res: .word 0


.global main
.text

main:	
	@ r0 = A, r1 = B, r2 = res, r3 = @res
	ldr r0, =A
	ldr r0, [r0]
	ldr r1, =B
	ldr r1, [r1]
	stmda sp!, {r0, r1}
	bl pgcd
	ldmib sp!, {r0, r1}
	ldr r3, =res
	str r2, [r3]
	
	swi 0x123456
	
	
	
pgcd:
	@ r0 = A, r1 = B, r2 = res
	str lr, [sp] , #-4
	
	cmp r0, r1
		bne else
		mov r2, r0
		b endif
	else:
			subhi r0, r0, r1
			subls r1, r1, r0
			bl pgcd
	endif:
	
	ldr lr, [sp, #4]!
	mov pc, lr
	
