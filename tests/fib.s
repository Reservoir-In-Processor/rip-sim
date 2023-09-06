	.text
	.attribute	4, 16
	.attribute	5, "rv32i2p0_m2p0_a2p0_c2p0"
	.file	"fib.c"
	.globl	fib
	.p2align	1
	.type	fib,@function
fib:
	li	a2, 2
	li	a1, 1
	blt	a0, a2, .LBB0_4
	addi	sp, sp, -16
	sw	ra, 12(sp)
	sw	s0, 8(sp)
	sw	s1, 4(sp)
	sw	s2, 0(sp)
	li	s0, 0
	addi	s1, a0, 2
	li	s2, 3
.LBB0_2:
	addi	a0, s1, -3
	call	fib
	addi	s1, s1, -2
	add	s0, s0, a0
	bltu	s2, s1, .LBB0_2
	addi	a1, s0, 1
	lw	ra, 12(sp)
	lw	s0, 8(sp)
	lw	s1, 4(sp)
	lw	s2, 0(sp)
	addi	sp, sp, 16
.LBB0_4:
	mv	a0, a1
	ret
.Lfunc_end0:
	.size	fib, .Lfunc_end0-fib

	.globl	main
	.p2align	1
	.type	main,@function
main:
	addi	sp, sp, -16
	sw	ra, 12(sp)
	li	a0, 10
	call	fib
.LBB1_1:
	j	.LBB1_1
.Lfunc_end1:
	.size	main, .Lfunc_end1-main

	.ident	"Homebrew clang version 16.0.6"
	.section	".note.GNU-stack","",@progbits
	.addrsig
