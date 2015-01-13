
.org 0xFE0
_reset: b main 	@0x1000
_undefined_instruction: subs pc, lr, #4
_swi: subs pc, lr, #4
.org 0xFF8
_irq: b irq_handler @0x1018
@_fiq:  fiq_handler

