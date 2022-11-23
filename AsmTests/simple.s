
.text
.org 0
.align 256

UnitTestOK 			.set    0x55aa
UnitTestFAIL 		.set 	0xaa55
UnitTestCanary 		.set 	0x80000100

.global __start

__start:
	li 		r2, UnitTestOK
	lis 	r3, UnitTestCanary@h
	stw 	r2, UnitTestCanary@l (r3)

	sc
