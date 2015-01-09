@ Utilisation de tous les cas de la fonction LDM** et STM**
@ p.186 + p.481

.global main
.text

main:
	mov r0, #1
	mov r1, #2
	mov r2, #3

	@ Increment After LDMIA + STMIB
	STMIB sp!, {r0, r1, r2}
	LDMIA sp!, {r0, r1, r2}

	@ Increment Before LMDIB + STMIA
	STMIA sp!, {r0, r1, r2}
	LDMIB sp!, {r0, r1, r2}

	@ Decrement After LDMDA + STMDB
	STMDA sp!, {r0, r1, r2}
	LDMDB sp!, {r0, r1, r2}

	@ Decrement Before LDMDB + STMIA
	STMDB sp!, {r0, r1, r2}
	LDMDA sp!, {r0, r1, r2}


	swi 0x123456
