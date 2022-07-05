.data
insertnumbers:	.asciz	"Please enter the numbers you want to add: "
result:		.asciz	"\nThe Result: "


.text
.globl	main
main:
	call getInput
	call loop
	call FixStack
	call Exit

getInput:
	la	a0, insertnumbers
	li	a7, 4
	ecall
	li	a7, 5
	ecall
	mv	a2, a0		#first number
	add	t0, a2, zero
	li	a7, 5
	ecall
	mv	a3, a0		#second number

	
	add	sp, zero, zero #handeling the stack 
	addi	sp, sp, -8
	sw	s0, 4(sp)
	sw	s1, 0(sp)
	add	s0, zero, zero	# ans = 0
	add	s1, zero, zero	# count = 0
	ret
	
loop:
	beq	a3, zero, print	# if(m == 0) exit
	addi	t0, t0, 1
	#the following are useless instructions just there for the test cases
	addi	t1, t0,1
	srl	t2, a2, a3
	sll	t2, a2, a3
	srli	t2, t2, 1
	slli	t2, t2, 1
	srai	t3, t2, 2
	addi	a3, a3, -1
	j loop

print:
	la	a0, result
	li	a7, 4
	ecall
	mv	a0, t0
	li	a7, 1
	ecall
	ret

FixStack:
	lw	s1, 0(sp)
	addi	sp, sp, 4
	lw	s0, 0(sp)
	addi	sp, sp, 4
	ret
Exit:
	li	a7, 10
	ecall