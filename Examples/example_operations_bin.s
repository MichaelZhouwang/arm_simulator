@ Utilisation des instructions AND BIC EOR et ORR



.text
.global main

main :
	mov r1, #3
	mov r2, #5

		@@@ AND @@@
	@ Immediate
	and r0, r1, #2 @ 2
	@ Register
	and r0, r1, r2 @ 1


		@@@ BIC @@@
	@ Immediate
	bic r0, r1, #2 @ 1 
	@ Register
	bic r0, r1, r2 @ 2


		@@@ EOR @@@
	@ Immediate
	eor r0, r1, #2 @ 1
	@ Register
	eor r0, r1, r2 @ 6
	
	
		@@@ ORR @@@
	@ Immediate
	orr r0, r1, #2 @ 3
	@ Register
	orr r0, r1, r2 @ 7
	
	
    swi 0x123456
