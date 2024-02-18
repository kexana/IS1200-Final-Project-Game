  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #

.data
.align 2
.text
.global

hexasc:
	li 	$t0, 10
	andi 	$t1, $a0 ,15
	slt 	$t3,$t0,$t1
	bnez	$t3,notNums
	nop
	addi 	$t0, $t1, 0x30
	move 	$v0,$t0	
	jr $ra
	nop
	notNums: 
	addi 	$t0, $t1, 0x37
	move 	$v0,$t0	
	jr $ra
	nop

.global delay 
delay:
	PUSH $ra
	li	$t0, 0
delayloop:
	slt 	$t1, $t0,$a0
	beqz	$t1, endDelay
	nop
	
	addi	$t0,$t0, 1
	
	PUSH $t0
	jal	milisec
	nop
	POP $t0
endDelay:
	POP $ra 
	jr	$ra
	nop
milisec:
	li $t0, 0
milisecLoop:
	addi $t0,$t0, 1
	li $t1, 6524400
	bne $t0, $t1,milisecLoop
	nop
	jr $ra
	nop

.global time2string
time2string:
 	PUSH $s1
 	PUSH $a0
 	PUSH $ra
	
	move	$s1, $a0
	andi	$a0, $a1, 0xf000 
	srl 	$a0,$a0, 12
	jal 	hexasc
	nop
	sb	$v0,0($s1)
	
	andi	$a0, $a1, 0x0f00 
	srl 	$a0,$a0, 8
	jal 	hexasc
	nop
	sb	$v0,1($s1)
	
	li $a0,0x3a
	sb $a0,2($s1)
	
	andi	$a0, $a1, 0x00f0 
	srl 	$a0,$a0, 4
	jal 	hexasc
	nop
	
	seq 	$t7, $0, $a0
	
	sb	$v0,2($s1)
	
	andi	$a0, $a1, 0x000f 
	jal 	hexasc
	nop
	
	seq 	$t6, $0, $a0
	
	sb	$v0,3($s1)
	
	sb 	$zero, 4($s1)
	
	seq	$t7, $t6, $t7
	bne	$t7, $1, dontPrintx
	nop
	addi	$t7, $0, 0x58
	sb	$t7, 6($s1)
	
dontPrintx:
	
	POP $ra
	POP $a0
	POP $s1
	
 	jr $ra
 	nop

.global enable_interrupt
enable_interrupt:

	ei 
	jr $ra
