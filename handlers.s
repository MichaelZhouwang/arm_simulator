	.global	_irq_handler

_irq_handler:	.text
	stmfd   sp!,{r0-r12,lr}
	bl	traiter_requete
	ldmfd   sp!,{r0-r12,pc}^

