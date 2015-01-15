

.data

.bss
	tab : .skip 32

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
@ r3 ← x
@ r4 ← w ← j - 1
@ r5 ← y = T[w]
tri_insertion:
	stmdb sp!, {r0,r1,r2,r3,r4,r5}
	ldr r0, ptr_tab
	mov r1, #1						@ i ← 1

  loop_t:
  	cmp r1, #7
	beq end_loop_t

	ldr r3, [r0, r1, LSL #2] 		@ x ← T[i]
	mov r2, r1				 		@ j ← i
	sub r4, r2, #1           		@ r4 ← w = j - 1 = r2 - 1

	loop_t2:
		cmp r2, #0
		ble end_loop_t2 			@ j > 0
		ldr r5, [r0, r4, LSL #2]	@ r5 ← T[w] = T[j - 1]
		cmp r5, r3 
		ble end_loop_t2				@ T[w] > x

		str r5, [r0, r2, LSL #2]	@ T[j] ← T[w] = T[j - 1]
		mov r2, r4					@ j ← w = j - 1
	end_loop_t2:

	str r3, [r0, r2, LSL #2]		@ T[j] ← x
	add r1, r1, #1 					@ i++

	bal loop_t
  end_loop_t:
	ldmia sp!, {r0,r1,r2,r3,r4,r5}
	mov pc, lr
@@@@@@@@@@@@@@@@@@@@@@@
@@ FIN TRI INSERTION @@
@@@@@@@@@@@@@@@@@@@@@@@

@@@@@@@@@@@@@@@@@@@@@@@
@@  REMPLIR TABLEAU  @@
@@@@@@@@@@@@@@@@@@@@@@@
remplir_tab:
	ldr r0, ptr_tab
	mov r1, #8
  loop_r:
	cmp r1, #0
	beq end_loop_r
	
	str r1, [r0], #4
	@ r0 pointera sur la prochaine case du tableau
	sub r1, r1, #1
	
	bal loop_r

  end_loop_r:
	mov pc, lr
@@@@@@@@@@@@@@@@@@@@@@@@@
@@ FIN REMPLIR TABLEAU @@
@@@@@@@@@@@@@@@@@@@@@@@@@

@@@@@@@@@@
@  MAIN  @
@@@@@@@@@@
main:
	bl remplir_tab

	bl tri_insertion

	ptr_tab: .word tab
