@ Utilisation des instructions ADD, ADC, SUB, SBC, RSB, RSC
.global main
.text

main:

	mov r1, #1
	mov r2, #2
	mov r3, #-1

	@@@ ADD @@@
@ Immediate
	add r0, r1, #1 @ 2
@ Register
	add r0, r1, r2 @ 3
	add r0, r1, r3 @ 0



	@@@ ADC @@@
@ C = 0
	adds r0, r1, #0 
@ Immediate
	adc r0, r1, #1 @ 2
@ Register
	adc r0, r1, r2 @ 3
	adc r0, r1, r3 @ 0

@ C = 1
	adds r0, r1, r3
@ Immediate
	adc r0, r1, #1 @ 3
@ Register
	adc r0, r1, r2 @ 4
	adc r0, r1, r3 @ 1


	@@@ SUB @@@
@ Immediate
	sub r0, r1, #1 @ 0
@ Register
	sub r0, r1, r2 @ -1 => 0xFFFF FFFF
	sub r0, r1, r3 @ 2
	
	
	@@@ SBC @@@
@ C = 0
	adds r0, r1, #0 
@ Immediate
	sbc r0, r1, #1 @ -1 => 0xFFFF FFFF
@ Register
	sbc r0, r1, r2 @ -2 => 0xFFFF FFFE
	sbc r0, r1, r3 @ 1
	
@ C = 1
	adds r0, r1, r3
@ Immediate
	sbc r0, r1, #1 @ 0
@ Register
	sbc r0, r1, r2 @ -1 => 0xFFFF FFFF
	sbc r0, r1, r3 @ 2
	
	@@@ RSB @@@
@ Immediate
	rsb r0, r1, #1 @ 0
@ Register
	rsb r0, r1, r2 @ 1
	rsb r0, r1, r3 @ -2 => 0xFFFF FFFE


	@@@ RSC @@@
@ C = 0
	adds r0, r1, #0  
@ Immediate
	rsc r0, r1, #1 @ -1 => 0xFFFF FFFF
@ Register
	rsc r0, r1, r2 @ 0
	rsc r0, r1, r3 @ -3 => 0xFFFF FFFD
	
@ C = 1
	adds r0, r1, r3
@ Immediate
	rsc r0, r1, #1 @ 0
@ Register
	rsc r0, r1, r2 @ 1
	rsc r0, r1, r3 @ -2 => 0xFFFF FFFE
	
	
	swi 0x123456
