@ Utilisation des instructions MRS et MSR

.global main
.text

main:
	mrs r0, CPSR
	mrs r1, SPSR


	msr CPSR_c, #5
	mov r1, #0
	msr CPSR_x, r1
	msr CPSR_s, #5
	msr CPSR_f, #0xA0000000


	swi 0x123456

