.section ".text"
	.align 2
.globl __uClibc_syscall
	.type	__uClibc_syscall,@function
__uClibc_syscall:
.Lsize:
	sc
	bnslr
#ifdef __PIC__
	b	__syscall_error@plt
#else
	b	__syscall_error
#endif
	.size	__uClibc_syscall,.Lsize-__uClibc_syscall
