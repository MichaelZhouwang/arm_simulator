@ Utilisation des instructions TST, TEQ, CMP

		@@@ TST @@@
mov r0, #42
mov r1, #4

@ Immediate
tst r0, #4

@ Register
tst r0, r1

@ Logical shift left by immediate
tst r0, r1 LSL #1

@ Logical shift left by register
mov r2, #2
tst r0, r1 LSL r2

mov r1, #16

@ Logical shift right by immediate
tst r0, r1, LSR #1

@ Logical shift right by register
tst r0, r1, LSR r2

@ Arithmetic shift right by immediate
tst r0, r1, ASR #1

@ Arithmetic shift right by register
tst r0, r1, ASR r2

@ Rotate right by immediate
tst r0, r1, ROR #1

@ Rotate right by register
tst r0, r1, ROR r2

@ Rotate right with extend
tst r0, r1, RXX


	@@@ TEQ @@@
mov r1, #4

@ Immediate
teq r0, #4

@ Register
teq r0, r1

@ Logical shift left by immediate
teq r0, r1 LSL #1

@ Logical shift left by register
mov r2, #2
teq r0, r1 LSL r2

mov r1, #16

@ Logical shift right by immediate
teq r0, r1, LSR #1

@ Logical shift right by register
teq r0, r1, LSR r2

@ Arithmetic shift right by immediate
teq r0, r1, ASR #1

@ Arithmetic shift right by register
teq r0, r1, ASR r2

@ Rotate right by immediate
teq r0, r1, ROR #1

@ Rotate right by register
teq r0, r1, ROR r2

@ Rotate right with extend
teq r0, r1, RXX	


	@@@ CMP @@@
mov r1, #4

@ Immediate
cmp r0, #4

@ Register
cmp r0, r1

@ Logical shift left by immediate
cmp r0, r1 LSL #1

@ Logical shift left by register
mov r2, #2
cmp r0, r1 LSL r2

mov r1, #16

@ Logical shift right by immediate
cmp r0, r1, LSR #1

@ Logical shift right by register
cmp r0, r1, LSR r2

@ Arithmetic shift right by immediate
cmp r0, r1, ASR #1

@ Arithmetic shift right by register
cmp r0, r1, ASR r2

@ Rotate right by immediate
cmp r0, r1, ROR #1

@ Rotate right by register
cmp r0, r1, ROR r2

@ Rotate right with extend
cmp r0, r1, RXX


	@@@ CMN @@@
mov r1, #4

@ Immediate
cmn r0, #4

@ Register
cmn r0, r1

@ Logical shift left by immediate
cmn r0, r1 LSL #1

@ Logical shift left by register
mov r2, #2
cmn r0, r1 LSL r2

mov r1, #16

@ Logical shift right by immediate
cmn r0, r1, LSR #1

@ Logical shift right by register
cmn r0, r1, LSR r2

@ Arithmetic shift right by immediate
cmn r0, r1, ASR #1

@ Arithmetic shift right by register
cmn r0, r1, ASR r2

@ Rotate right by immediate
cmn r0, r1, ROR #1

@ Rotate right by register
cmn r0, r1, ROR r2

@ Rotate right with extend
cmn r0, r1, RXX

