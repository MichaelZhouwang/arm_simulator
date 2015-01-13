@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@ Exceptions table
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.section .vectors

resetVector:         b resetHandler
undefVector:         b UndefinedException
swiVector:           b SwiException
prefetchAbortVector: b PrefetchAbortException
dataAbortVector:     b DataAbortException
reservedVector:      b reservedVector
irqVector:           b irqHandler
fiqVector:           b fiqVector

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@ Exceptions handlers
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

resetHandler:
UndefinedException:
SwiException:
PrefetchAbortException:
DataAbortException:
reservedVector:
irqHandler:
fiqVector:
    mov r1, #99
    

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@ Main
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.global main
.text

decr:
    subs r0, r0, #1
    mov pc, lr

main:
    mov r0, #10
loop:
    bl decr
    bne loop
end:
    swi 0x123456
.data

