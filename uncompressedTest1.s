.globl _boot
.text

_boot:      
#I
addi t0, x0, 5
add s1, s1, x0
sub s2, t0, s1
sll s3, t0, s1
slt t1, t0, s1
sltu t1, t0, s1
xor t2, t0, s1
srl t3, t0, s1
sra s4, s1, t0
or t5, s1, s1
and t6, s1, t0
#B
beq t0, s1, Label1
Label1: 
bne t0, s1, Label2
Label2:
blt s1, t0, Label3 
Label3:
bge s1, t0, Label4 
Label4:
bltu s1, x0, Label5
Label5: 
bgeu s1, x0, U

 U:
 lui t1, 10000
 auipc s1, 10000