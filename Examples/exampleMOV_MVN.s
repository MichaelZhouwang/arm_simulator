@ Utilisation des instructions MVN et MOV

.global main
.text

main:
	@@@ MOV @@
@ Immediate
	mov r1, #2
	mov r3, #0x80000000
	
@ Register
	mov r2, r1 @  2

@ Logical shift left by immediate
	mov r2, r1, LSL #1 @  2 << 1  => 4
	mov r2, r3, LSL #1 @  2^31 << 1  => 2^32 OVERFLOW  =>> 0

@ Logical shift left by register
	mov r2, r1, LSL r1 @  2 << 2  => 8
	mov r2, r3, LSL r1 @  2^31 << 2  => 2^33 OVERFLOW  =>> 0

@ Logical shift right by immediate
	mov r2, r1, LSR #1 @  2 >> 1  => 1
	mov r2, r3, LSR #1 @  2^31 >> 1  => 2^30  => 0x4000 0000 

@ Logical shift right by register
	mov r2, r1, LSR r1 @  2 >> 2  => 0
	mov r2, r3, LSR r1 @  2^31 >> 2  => 2^29  => 0x2000 0000 

@ Arithmetic shift right by immediate
	mov r2, r1, ASR #1 @  2 asr 1  => 1 
	mov r2, r3, ASR #1 @  2^31 asr 1  => 2^31 + 2^30  => 0xc000 0000 

@ Arithmetic shift right by register
	mov r2, r1, ASR r1 @  2 asr 2  => 0
	mov r2, r3, ASR r1 @  2^31 asr 2  => 2^31 + 2^30 + 2^29  => 0xe000 0000 

@ Rotate right by immediate
	mov r2, r1, ROR #1 @  2 ror 1  => 1
	mov r2, r3, ROR #1 @  2^31 ror 1  => 2^30  => 0x4000 0000 

@ Rotate right by register
	mov r2, r1, ROR r1 @  2 ror 2  => 0x8000 0000
	mov r2, r3, ROR r1 @  2^31 ror 2  => 2^29  => 0x2000 0000 

@ Rotate right with extend
	adds r0, r1, r1 @  C  = 0
	mov r2, r1, RRX @  2 rrx  => 1
	mov r2, r3, RRX @  2^31 rrx  => 2^30  => 0x4000 0000 
	adds r0, r3, r3 @  C  => 1
	mov r2, r1, RRX @  2 rrx  => 2^31 + 1  => 0x8000 0001
	mov r2, r3, RRX @  2^31 rrx  => 2^31 + 2^30  => 0xc000 0000 

	@@@ MVN @@@
@ Immediate
	mvn r1, #2 @ 0xFFFF FFFD

@ Register
	mvn r2, r1 @ 2

@ Logical shift left by immediate 
	mvn r2, r1, LSL #1 @ ~(0xFFFF FFFD << 1) => ~0xFFFF FFFA => 4

@ Logical shift left by register
	mvn r2, r1, LSL r1 @ ~(0xFFFF FFFD << 0xFFFF FFFD) => ~0 => 0xFFFF FFFF

@ Logical shift right by immediate
	mvn r2, r1, LSR #1 @ ~(0xFFFF FFFD >> 1) => ~0x7FFF FFFE => 0x8000 0001 

@ Logical shift right by register
	mvn r2, r1, LSR r1 @ ~(0xFFFF FFFD >> 0xFFFF FFFD) => ~0 => 0xFFFF FFFF

@ Arithmetic shift right by immediate
	mvn r2, r1, ASR #1 @ ~(0xFFFF FFFD >> 1) => ~0xFFFF FFFE => 1 

@ Arithmetic shift right by register
	mvn r2, r1, ASR r1 @ ~(0xFFFF FFFD >> 0xFFFF FFFD) => ~0xFFFF FFFF => 0

@ Rotate right by immediate
	mvn r2, r1, ROR #1 @ ~(0xFFFF FFFD ror 1) => ~0xFFFF FFFE => 1

@ Rotate right by register 0x80000001 7FFFFFFE
	mvn r3, r1 @ 2
	mvn r2, r3, ROR r3 @ ~(2 ror 2) => ~0x8000 0000 => 0x7FFF FFFF

@ Rotate right with extend
	adds r0, r3, r3 @ C = 0
	mvn r2, r1, RRX @ <=> mvn r2, r1, LSR #1 => 0x8000 0001
	adds r0, r1, #0xF @ C = 1
	mvn r2, r1, RRX @ ~(0x80000000 + 0xFFFFFFFD >> 1) => ~0xFFFFFFFE => 1
	
	
	swi 0x123456
