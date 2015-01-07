@ Utilisation de tous les cas de la fonction STRH
@ p.473 -- A5-33

.global main
.text

main:
@ Immediate offset
  mov r1, #1 @ r1 contient 1 pour tout le programme
  ldr r0, ptr_a
  sub r0, r0, #1
  strh r1, [r0, #1] @ r1 est chargé dans A 
  
@ Register offset
  ldr r0, ptr_b
  sub r0, r0, #1
  mov r2, #1
  strh r1, [r0, r2] @ r1 est chargé dans B

  
@ Immediate pre-index
  ldr r0, ptr_a
  sub r0, r0, #1
  strh r1, [r0, #1]! @ r1 est chargé dans A
  @ r0 modifié contrairement à avant

@ Register pre-indexed
  ldr r0, ptr_b
  sub r0, r0, #1
  strh r1, [r0, r2]! @ r1 est chargé dans B
  @ r0 modifié

  
@ Immediate post-indexed
  ldr r0, ptr_a
  strh r1, [r0], #1 @ r1 est chargé dans A
  @ r0 est modifié
  sub r0, r0, #1 @ r0 à la valeur de ptr_a

@ Register post-indexed
  ldr r0, ptr_b
  strh r1, [r0], r2 @ r1 est chargé dans B
  @ r0 modifié
  add r0, r0, #1 @ r0 à la valeur de ptr_b

  swi 0x123456
  

  A: .hword 10
  B: .hword 11
  C: .hword 12
  
  ptr_a: .word A
  ptr_b: .word B
  ptr_c: .word C
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
