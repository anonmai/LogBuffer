#ifndef _HX_ASM_H
#define _HX_ASM_H

#include "types.h"
#include "desc.h"
/*
 * contain all the assembly code for use
 */

/*
 * 对static inline的一些注解：静态内联函数 。 它不使用函数调用，直接将汇编代码插入在调用该函数处
 * 你可以把它认为是一个static的函数，加上了inline的属性。这个函数大部分表现和普通的static函数一样，只不过在调用这种函数的时候，gcc会在其调用处将其汇编码展开编译而不为这个函数生成独立的汇编码。
 * 除了以下几种情况外：
 * （1）函数的地址被使用的时候。如通过函数指针对函数进行了间接调用。这种情况下就不得不为static inline函数生成独立的汇编码，否则它没有自己的地址。
 * （2）其他一些无法展开的情况，比如函数本身有递归调用自身的行为等。

    static inline函数和static函数一样，其定义的范围是local的，
    即可以在程序内有多个同名的定义（只要不位于同一个文件内即可）。
    注意
    gcc的static inline的表现行为和C99标准的static inline是一致的。所以这种定义可以放心使用而没有兼容性问题。
    要点：
 *  gcc的static inline相对于static函数来说只是在调用时建议编译器进行内联展开；
 *  gcc不会特意为static inline函数生成独立的汇编码，除非出现了必须生成不可的情况（如通过函数指针调用和递归调用）；
 *  gcc的static inline函数仅能作用于文件范围内。
 */
static inline void asm_rdmsr32 (ulong num, u32 *a, u32 *d)  //前面不加static，编译时会显示重定义,static仅在文件内使用的局部函数
{
	asm volatile ("rdmsr"                               //READ MSR specified by ECX
			: "=a" (*a), "=d"(*d)                       //into EDX:EAX
			: "c" (num));
}

static inline void asm_rdmsr64 (ulong num, u64 *value)
{
	u32 a,d;
	asm_rdmsr32(num, &a, &d);
	*value= (u64)a | ((u64)d << 32);
}

static inline void asm_rdmsr (ulong num,u64 *value)
{
#ifdef __x86_64__
	asm_rdmsr64 (num,(u64 *)value);
#else
	u32 dummy;
	asm_rdmsr32 (num,(u32 *)value, &dummy);
#endif
}

static inline void asm_wrmsr32 (ulong num, u32 a, u32 d)
{
	asm volatile ("wrmsr"
		      :
		      : "c" (num), "a" (a), "d" (d));
}

static inline void asm_wrmsr64 (ulong num, u64 value)
{
	u32 a, d;
	a = (u32)value;
	d = (u32)(value >> 32);
	asm_wrmsr32 (num, a, d);
}

static inline void asm_wrmsr (ulong num, ulong value)
{
#ifdef __x86_64__
	asm_wrmsr64 (num, (u64)value);
#else
	asm_wrmsr32 (num, (u32)value, 0);
#endif
}

static inline void asm_cpuid (u32 num, u32 numc,
                              u32 *a, u32 *b, u32 *c, u32 *d)
{
	asm volatile ("cpuid"
		      : "=a" (*a), "=b" (*b), "=c" (*c), "=d" (*d)
		      : "a" (num), "c" (numc)
		      : "memory");
}

/*******************************control register***************************/

static inline unsigned long asm_rdcr0 (void)
{
    unsigned long val;
	asm volatile ("mov %%cr0,%0"
		      : "=r" (val));
    return val;
}

static inline void asm_wrcr0 (u64 cr0)
{
	asm volatile ("mov %0,%%cr0"
		      :
		      : "r" (cr0));
}

static inline unsigned long asm_rdcr2 (void)
{
    unsigned long val;
	asm volatile ("mov %%cr2,%0"
		      : "=r" (val));
    return val;
}

static inline void asm_wrcr2 (u64 cr2)
{
	asm volatile ("mov %0,%%cr2"
		      :
		      : "r" (cr2));
}

static inline unsigned long asm_rdcr3 (void)
{
    unsigned long val;
	asm volatile ("mov %%cr3,%0"
		      : "=r" (val));
    return val;
}

static inline void asm_wrcr3 (u64 cr3)
{
	asm volatile ("mov %0,%%cr3"
		      :
		      : "r" (cr3));
}

static inline unsigned long asm_rdcr4 (void)
{
    unsigned long val;
	asm volatile ("mov %%cr4,%0"
		      : "=r" (val));
    return val;
}

static inline void asm_wrcr4 (u64 cr4)
{
	asm volatile ("mov %0,%%cr4"
		      :
		      : "r" (cr4));
}

#ifdef __x86_64__
static inline void asm_rdcr8 (u64 *cr8)
{
	asm volatile ("mov %%cr8,%0"
		      : "=r" (*cr8));
}

static inline void asm_wrcr8 (u64 cr8)
{
	asm volatile ("mov %0,%%cr8"
		      :
		      : "r" (cr8));
}
#endif

/*********************************vm instructions***********************************/

/*不知道这样写这条指令是否有误，以后再调试吧*/
static inline void asm_invept(int ext, u64 eptp, u64 rsvd)
{
    /*struct {                         //error operand type mismatch for `invept'
        u64 eptp,rsvd;
    }descriptor = {eptp, rsvd};

    asm volatile ("invept %1, %0"
            :
            : "r"(ext), "m"(&descriptor)
            : "cc", "memory");*/
}

/* 66 0f c7 33                  vmclear (%ebx )*/
static inline void asm_vmclear(void *p)
{
	asm volatile ("vmclear %0"
			:
			: "m" (*(u64 *)p)
			: "cc", "memory");
}

/*0f c7 33                       vmprtld (%ebx)*/
static inline void asm_vmptrld (void *p)
{
	asm volatile ("vmptrld %0"
			:
			: "m" (*(u64 *)p)
			: "cc", "memory");
}

/*0f 78 c2                       vmread %eax,%edx*/
static inline void asm_vmread (ulong index, ulong *val)
{
	asm volatile ("vmread %1,%0"
			: "=rm" (*val)
			: "r" (index)
			: "cc");
}

/*0f 79 c2                       vmwrite %edx, %eax */
static inline void asm_vmwrite (ulong index, ulong val)
{
	asm volatile ("vmwrite %1,%0"
			:
			: "r"(index),"rm"(val)
			: "cc");
}


/* f3 0f c7 33                   vmxon (%ebx) */
static inline void asm_vmxon (u64 * vmxon_region)
{
	asm volatile ("vmxon %0"
			:
			: "m"(*vmxon_region)
			: "cc", "memory");
}

/******************************other instructions*********************************/

static inline u16 asm_rdes(void)
{
    u16 es;
    asm volatile ("mov %%es,%0"
            : "=rm" (es));
    return es;
}

static inline u16 asm_rdcs(void)
{
    u16 cs;
    asm volatile ("mov %%cs,%0"
            : "=rm" (cs));
    return cs;
}

static inline u16 asm_rdss(void)
{
    u16 ss;
    asm volatile ("mov %%ss,%0"
            : "=rm" (ss));
    return ss;
}

static inline u16 asm_rdds(void)
{
    u16 ds;
    asm volatile ("mov %%ds,%0"
            : "=rm" (ds));
    return ds;
}

static inline u16 asm_rdgs(void)
{
    u16 gs;
    asm volatile ("mov %%gs,%0"
            : "=rm" (gs));
    return gs;
}

static inline u16 asm_rdfs(void)
{
    u16 fs;
    asm volatile ("mov %%fs,%0"
            : "=rm" (fs));
    return fs;
}

static inline u16 asm_rdtr(void)
{
    u16 tr;
    asm volatile ("str %0"
            : "=rm" (tr));
    return tr;
}

static inline u16 asm_rdldtr(void)
{
    u16 ldtr;
    asm volatile ("sldt %0"
            : "=rm" (ldtr));
    return ldtr;
}

static inline void asm_lsl (u32 sel, ulong *limit)
{
    asm volatile ("lsl %1,%0"
            : "=r" (*limit)
            : "rm" ((ulong)sel)); /* avoid assembler bug */
}

static inline void asm_lar (u32 sel, ulong *ar)
{
    asm volatile ("lar %1,%0"
            : "=r" (*ar)
            : "rm" ((ulong)sel)); /* avoid assembler bug */
}

static inline void asm_rdgdtr (struct hxvm_desc_ptr *ptr)
{               /*6 byte memory location*/
    asm volatile ("sgdt %0"
            : "=m" (*ptr));
}

static inline void asm_rdidtr (struct hxvm_desc_ptr *ptr)
{
    asm volatile ("sidt %0"
            : "=m" (*ptr));
}

static inline void asm_rddr7 (ulong *dr7)
{
    asm volatile ("mov %%dr7,%0"
            : "=r" (*dr7));
}

static inline void asm_rdrflags (ulong *rflags)
{
    asm volatile (
#ifdef __x86_64__
            "pushfq ; popq %0"
#else
            "pushfl ; popl %0"
#endif
            : "=rm" (*rflags));
}

/*static inline void asm_store_gdt(struct hxvm_desc_ptr *dtr)
{
    asm volatile("sgdt %0":"=m"(*dtr));
}*/
#endif
