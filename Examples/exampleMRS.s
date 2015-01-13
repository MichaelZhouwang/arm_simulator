.global main
.text

main:
    mrs r0, CPSR
    @ interruption svp
    mrs r1, SPSR

    swi 0x123456
.data
