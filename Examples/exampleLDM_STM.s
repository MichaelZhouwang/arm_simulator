@ Utilisation de tous les cas de la fonction LDM et STM
@ p.186 + p.481

.global main
.text

main:
mov r0, #1
mov r1, #2
mov r2, #3
@ Increment After LDMIA + STMIA
LDMIA r3!, {r0, r1, r2}
STMIA r3!, {r0, r1, r2}

@ Increment Before LMDIB


@ Decrement After LDMDA


@ Decrement Before LDMDB



	swi 0x123456
