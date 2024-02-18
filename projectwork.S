  # projectwork.S
.data
.align 2
.text
.global

.global enable_interrupt
enable_interrupt:

	ei 
	jr $ra
