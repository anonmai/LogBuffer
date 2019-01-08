#ifndef __HX_HXVM_HYPERCALL_H
#define __HX_HXVM_HYPERCALL_H

/*return values for hypercalls*/
#define HXVM_ENOSYS 1000
#define HXVM_HCOK   0

/*hypercall number*/
#define HYPERCALL_TEST          0
#define HYPERCALL_REGISTER      1
#define HYPERCALL_UNREGISTER    2
#define HYPERCALL_SWITCH_EPTP   3

/*
 * This instruction is vmcall. On non-VT architectures, it will generate a
 * trap that we will then rewrite to the appropriate instruction.
 */

#define HXVM_HYPERCALL ".byte 0x0f,0x01,0xc1"

static inline long hxvm_hypercall0(unsigned int nr)
{
    long ret;
    asm volatile(HXVM_HYPERCALL
             : "=a"(ret)
             : "a"(nr)
             : "memory");
    return ret;
}

static inline long hxvm_hypercall1(unsigned int nr, unsigned long *p1)
{
	long ret;
	asm volatile(HXVM_HYPERCALL
		     : "=a"(ret)
		     : "a"(nr), "b"(*p1)
		     : "memory");
	return ret;
}

static inline long hxvm_hypercall2(unsigned int nr, unsigned long *p1,
				  unsigned long *p2)
{
	long ret;
	asm volatile(HXVM_HYPERCALL
		     : "=a"(ret)
		     : "a"(nr), "b"(*p1), "c"(*p2)
		     : "memory");
	return ret;
}

static inline long hxvm_hypercall3(unsigned int nr, unsigned long *p1,
				  unsigned long *p2, unsigned long *p3)
{
	long ret;
	asm volatile(HXVM_HYPERCALL
		     : "=a"(ret), "=b"(*p1), "=c"(*p2), "=d"(*p3)
		     : "a"(nr), "b"(*p1), "c"(*p2), "d"(*p3)
		     : "memory");
	return ret;
}

static inline long hxvm_hypercall4(unsigned int nr, unsigned long *p1,
				  unsigned long *p2, unsigned long *p3,
				  unsigned long *p4)
{
	long ret;
	asm volatile(HXVM_HYPERCALL
		     : "=a"(ret)
		     : "a"(nr), "b"(*p1), "c"(*p2), "d"(*p3), "S"(*p4)
		     : "memory");
	return ret;
}

#endif
