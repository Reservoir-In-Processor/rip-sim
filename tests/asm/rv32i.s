addi x16, x0, 5
addi x17, x0, 3
slti x17, x16, -2
sltiu x17, x16, 5 
xori x17, x16, 6 
ori x17, x16, 6
andi x17, x16, 7
jalr x18, x0, 44
lb x18, 4(x0)
lh x18, 4(x0)
lbu x18, 4(x0)
lw x18, 4(x0)
slli x17, x16, 3
srli x17, x16, 2
srai x17, x16, 2
add x2, x3, x4
sub x2, x3, x4
sll x18, x16, x17
slt x18, x16, x17
sltu x18, x17, x16
xor x18, x16, x17
srl x18, x16, x17
sra x18, x16, x17
or x18, x16, x17
and x18, x16, x17
lui x16, 2
auipc x16, 2
jal x18, 12
jal x18, 699050
sb x16, 4(x0)
sh x16, 4(x0)
sw x16, 4(x0)
sw x16, -1336(x0)
beq x16, x17, 12
bne x16, x17, 12
blt x16, x17, -8
bge x16, x17, 12
bltu x16, x17, 12
bgeu x16, x17, 12
bgeu x16, x17, -1366