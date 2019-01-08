#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<asm/page.h>
#include<asm/io.h>
#include <linux/slab.h>
#include <linux/gfp.h>
#include <linux/delay.h>
#include "hxvm_hypercall.h"
#include "constants.h"
#include "msr-extra.h"
#include "asm.h"
#include "types.h"
#include "desc.h"

#define SET_LBRBUF 100
#define UNSET_LBRBUF 101
#define START_LBRPMU 102
#define STOP_LBRPMU 103
#define LBRBUF_ORD 2

ulong lbrbuf;
ulong bufsize;

static void 
gen_lapic_nmi(void)
{

    /* LBR branch filtering */
    asm_wrmsr(MSR_LBR_SELECT, LBR_KERNEL | LBR_JCC | LBR_REL_CALL | LBR_REL_JMP | LBR_IND_CALL | LBR_IND_JMP); 

    /* start PMU0 */
    asm_wrmsr(MSR_CORE_PERF_GLOBAL_OVF_CTRL, 0x1);
    asm_wrmsr(MSR_P6_EVNTSEL0, 0x5220cc);
    asm_wrmsr(MSR_ARCH_PERFMON_PERFCTR0, 0xfffffffffffffff0);

    asm_wrmsr(MSR_IA32_DEBUGCTLMSR, DEBUGCTLMSR_LBR|DEBUGCTLMSR_BTS_OFF_USR);
    asm_wrmsr(MSR_CORE_PERF_GLOBAL_CTRL, 1);
}

static int __init hello_init(void)
{
    printk(KERN_EMERG "Init start!!! \n") ;
    ulong addr;

    //lbrbuf = (u8*)kmalloc(GFP_KERNEL, bufsize);
    lbrbuf = __get_free_pages(GFP_KERNEL, LBRBUF_ORD);
    bufsize = 4096 * (1 << LBRBUF_ORD);
    addr = virt_to_phys(lbrbuf) ;
    printk(KERN_EMERG "virt %llx to phys %llx, size: %llx\n", (u64)lbrbuf, (u64)addr, (u64)bufsize);

    //gen_lapic_nmi();

    hxvm_hypercall2(SET_LBRBUF, &addr, &bufsize);
    asm("":::"memory");
    hxvm_hypercall0(START_LBRPMU) ;

    //mdelay(50000) ;

    return 0;
}

static void __exit hello_cleanup(void)
{
    printk("<1>See you,Kernel!\n");

    asm_wrmsr(MSR_IA32_DEBUGCTLMSR, 0);
    asm_wrmsr(MSR_CORE_PERF_GLOBAL_CTRL, 0);

    hxvm_hypercall0(STOP_LBRPMU) ;
    asm("":::"memory");
    hxvm_hypercall0(UNSET_LBRBUF);


    u64 *plog = (u64*)lbrbuf;
    int i = 0;
    for (i = 0; i < 32 /*bufsize/16*/; i++) {

       // printk(KERN_EMERG "Flag: %llx, To_Addr:  %llx\n", plog[i*2], plog[2*i+1]);
        if(plog[i*2] == 0)
        {
            printk(KERN_EMERG "Wrong Addr: %llx \n", plog[2*i+1]) ;
        }
        else if(plog[i*2] == 1)
        {
            printk(KERN_EMERG "Right addr: %llx \n", plog[2*i+1]) ;
        }
    }
    //kfree(lbrbuf);
   free_pages(lbrbuf, LBRBUF_ORD);
}

module_init(hello_init);        /*进入内核的时候，调用hello_init        */
module_exit(hello_cleanup);     /*离开内核的时候，调用hello_cleanup     */
MODULE_LICENSE("GPL");          /*协议*/
