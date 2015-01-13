@ Utilisation de tous les cas des fonctions B et BL
@ p.112

.global main
.text
add:
  add r0, r0, #2
  cmp r0, r1
  mov pc, lr

main:
  @ NE  LT  LE  GE  EQ  GT
  mov r0, #0
  mov r1, #6
  bl add
  bne LE @ r0 == 2
LT:
  bl add
  blt GE @ r0 == 4
LE:
  cmp r0, r1
  ble LT @ r0 == 4
EQ:
  cmp r0, r1
  beq GT @ r0 == 6
GE:
  bl add
  bge EQ  @ r0 == 6
GT:
  bl add
  bgt CS @ r0 == 8

@ CS/HS  CC/LO  PL  VS  VC  HI  LS
CS:
  cmp r0, r1
  bcs CC
PL:
  cmp r0, r1
  bpl VS
CC:
  cmp r0, r1
  bcc PL
VC:
  cmp r0, r1
  bvc HI
VS:
  cmp r0, r1
  bvs VC
LS:
  cmp r0, r1
  bls end
HI:
  cmp r0, r1
  bhi LS


end:
	


	swi 0x123456
