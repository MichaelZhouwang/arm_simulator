@ Utilisation des instructions MRS et MSR

.global main
.text

main:
    mrs r0, CPSR
    mrs r1, SPSR


	msr CPSR_c, #5
	@ movs r1, #0
	msr CPSR_x, r1
	msr SPSR_s, #5
	msr SPSR_f, r1
	

    swi 0x123456

