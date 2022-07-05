.data
.text
.globl	main
main:
	add	sp, zero, zero #handeling the stack 
	addi	sp, sp, -8
	li	t4, 0
	li	t5, 1
	li	t6, 0
	li	t2, 0
	slti	s0, t1, 5
	sltiu	t0, s0, 5
	li	t0, 10
	li	t1, 5
	sb	t4, 1(sp)
	sb	t6, 0(sp)
	sh	t5, 4(sp)
	lb	t5, 0(t4)
	lh	t5, 0(t2)
	lbu	t5, 0(t4)
	lhu	t5,0(t2)
	add	s0, zero, zero	# ans = 0
	add	s1, zero, zero	# count = 0
	j loop1
	
loop1:
	bne	t0, t1, FixStack	# if(m == 0) exit
	addi	t0, t0,-1
	xori	t2, t2, 1
	ori	t2, t2, 1
	j loop1

FixStack:
	lw	s1, 0(sp)
	addi	sp, sp, 4
	lw	s0, 0(sp)
	addi	sp, sp, 4
	j Exit
Exit:
	li	a7, 10
	ecall