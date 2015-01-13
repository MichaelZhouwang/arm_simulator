		.section .init

		.global _reset
		.global	_start
		.global	_interrupt_vectors
		.global	_undef_vec
		.global	_swi_vec
		.global	_prefetch_abort_vect
		.global	_data_abort_vect
		.global	_reserved
		.global	_irq_vec
		.global	_fiq_vec
		.global	_main
		.global _irq_handler

@######################################################################
@#interrupt vectors table
@######################################################################

		.org 0x0
		@#do not insert anything here
		@#vectors must be mapped at absolute address 0

_start:
_reset:
_interrupt_vectors: 	b main		@#00000000
_undef_vec:		b _undef_handler	@#00000004
_swi_vec:		b _swi_handler		@#00000008
_prefetch_abort_vect:	b _instr_abort_handler	@#0000000c
_data_abort_vect:	b _data_abort_handler	@#00000010
_reserved:		b _reserved		@#00000014
_irq_vec:		b _irq_handler		@#00000018
_fiq_vec:		b _fiq_handler		@#0000001c


main:
	mov r1, #5
	mov r2, #9
	mov r3, #110
	mov r3, #2
	mov r3, #4
	mov r3, #5
	mov r3, #6
	mov r3, #7
	mov r3, #2


_undef_handler:
_swi_handler:
_instr_abort_handler:
_data_abort_handler:
_irq_handler:
	mov r3, #27

_fiq_handler:

