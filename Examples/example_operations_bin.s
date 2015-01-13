@ Utilisation des instructions AND BIC EOR et ORR



.text
.global main

mov r1, #3
mov r2, #1

	@@@ AND @@@
@ Immediate
and r0, r1, #2 @ 2

@ Register
and r0, r1, r2 @ 1


	@@@ BIC @@@
@ Immediate
bic r0, r1, #1

@ Register
bic r0, r1, r2


	@@@ EOR @@@
@ Immediate
eor r0, r1, #2

@ Register
eor r0, r1, r2
	
	
	@@@ ORR @@@
@ Immediate
orr r0, r1, #2

@ Register
orr r0, r1, r2
