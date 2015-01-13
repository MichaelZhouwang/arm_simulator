@ Utilisation des instructions TST, TEQ, CMP
.global main
.text

main:	
	mov r0, #3
	mov r1, #5
	
		@@@ TST @@@	&

@ Immediate
	tst r0, #4 @ NZCV = 01xx

@ Register
	tst r0, r1 @ NZCV = 00xx


		@@@ TEQ @@@ ^

@ Immediate
	teq r0, #4 @ NZCV = 00xx

@ Register
	teq r0, r1 @ NZCV = 00xx
	
	
		@@@ CMP @@@ -

@ Immediate
	cmp r0, #4 @ NZCV = 100

@ Register
	cmp r0, r1 @ NZCV =  
	
	

		@@@ CMN @@@ +

@ Immediate
	cmn r0, #4 @ NZCV =  

@ Register
	cmn r0, r1 @ NZCV =  


 swi 0x123456

