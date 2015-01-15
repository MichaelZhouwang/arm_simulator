

.data
	N: .word 10
.bss
	tab : .skip 10*4 @ N * 4

.global main
.text
@ procédure tri_insertion(tableau T, entier n)
@       pour i de 1 à n-1
@           x ← T[i]
@           j ← i
@			w ← j - 1
@			y ← T[w]
@           tant que j > 0 et y > x
@               T[j] ← y
@               j ← w
@           fin tant que
@           T[j] ← x
@      fin pour
@   fin procédure
@@@@@@@@@@@@@@@@@@@@@
@@  TRI INSERTION  @@
@@@@@@@@@@@@@@@@@@@@@
@ r1 ← i
@ r2 ← j
@ r3 ← x = T[i]
@ r4 ← w ← j - 1
@ r5 ← y = T[w]
@ r6 ← N
tri_insertion:

	stmda sp!, {r0-r5}
	
	ldr r0, ptr_n
	ldr r6, [r0]
	ldr r0, ptr_tab
	mov r1, #1							@ i ← 1

	loop_t:
	  	cmp r1, r6						@ r1 < r6 = N
		beq end_loop_t

		ldr r3, [r0, r1, LSL #2] 		@ r3 ← x = T[i]
		mov r2, r1				 		@ r2 ← j = i

		loop_t2:
			sub r4, r2, #1           	@ r4 ← w = j - 1 = r2 - 1
			cmp r2, #0
			ble end_loop_t2 			@ j > 0
			ldr r5, [r0, r4, LSL #2]	@ r5 ← T[w] = T[j - 1]
			cmp r5, r3
			ble end_loop_t2				@ T[w] > x

			str r5, [r0, r2, LSL #2]	@ T[j] ← T[w] = T[j - 1]
			mov r2, r4					@ j ← w = j - 1
			bal loop_t2
		end_loop_t2:

		str r3, [r0, r2, LSL #2]		@ T[j] ← x
		add r1, r1, #1 					@ i++


		bal loop_t
	end_loop_t:
	
	ldmib sp!, {r0-r5}
	mov pc, lr
@@@@@@@@@@@@@@@@@@@@@@@
@@ FIN TRI INSERTION @@
@@@@@@@@@@@@@@@@@@@@@@@

@@@@@@@@@@@@@@@@@@@@@@@
@@  REMPLIR TABLEAU  @@
@@@@@@@@@@@@@@@@@@@@@@@
remplir_tab:
	ldr r0, ptr_n
	ldr r1, [r0]	@ r1 = N
	ldr r0, ptr_tab	
	
	loop_r:
		cmp r1, #0
		ble end_loop_r
	
		str r1, [r0], #4
		@ r0 pointera sur la prochaine case du tableau
		sub r1, r1, #1
	
		bal loop_r

	end_loop_r:
		mov pc, lr
@@@@@@@@@@@@@@@@@@@@@@@@@
@@ FIN REMPLIR TABLEAU @@
@@@@@@@@@@@@@@@@@@@@@@@@@


remplir_tab_aleat:
@ 7 10 5 9 1 3 2 6 7 4
	ldr r0, ptr_tab
	mov r1, #7
	str r1, [r0], #4
	mov r1, #-10
	str r1, [r0], #4
	mov r1, #5
	str r1, [r0], #4
	mov r1, #9
	str r1, [r0], #4
	mov r1, #1
	str r1, [r0], #4
	mov r1, #-3
	str r1, [r0], #4
	mov r1, #2
	str r1, [r0], #4
	mov r1, #6
	str r1, [r0], #4
	mov r1, #7
	str r1, [r0], #4
	mov r1, #4
	str r1, [r0], #4
	
	mov pc, lr

@@@@@@@@@@
@  MAIN  @
@@@@@@@@@@
main:
	msr CPSR_c, #0 @ Pour être en user
	
	@bl remplir_tab
	bl remplir_tab_aleat
end_remplissage:

	bl tri_insertion
	
end_file:
	swi 0x123456
	
	
	ptr_tab: .word tab
	ptr_n:   .word N
