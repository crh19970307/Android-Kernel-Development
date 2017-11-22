/*
	This program implements  system calls: get_pagetable_layout 
	Syscall get_page_table{
		ID: 356
		function: Investigate the page table layout in current operating system
	}
	
*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

#include <asm/pgtable.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/syscalls.h>
#include <asm/page.h>
#include <asm/memory.h>
#include <linux/uaccess.h>
MODULE_LICENSE("Dual BSD/GPL");

#define __NR_get_pagetable_layout 356
//The number of new syscall

struct pagetable_layout_info {
uint32_t pgdir_shift;
uint32_t pmd_shift;
uint32_t page_shift;
};


static int (*oldcall)(void);
//function pointer

int get_pagetable_layout( struct pagetable_layout_info * pgtbl_info, int size)
{
    struct pagetable_layout_info layout_info;
    if (size < sizeof(struct pagetable_layout_info))
        return -1;
    layout_info.pgdir_shift = PGDIR_SHIFT;
    layout_info.pmd_shift = PMD_SHIFT;
    layout_info.page_shift = PAGE_SHIFT;
    if (copy_to_user(pgtbl_info, &layout_info, size))
        return -2;

    return 0;

}

static int addsyscall_init(void){
    long*syscall=(long*)0xc000d8c4;
    oldcall=(int(*)(void))(syscall[__NR_get_pagetable_layout]);
    syscall[__NR_get_pagetable_layout]=(unsigned long)get_pagetable_layout;
    printk(KERN_INFO"module load!\n");
    return 0;
}


static void addsyscall_exit(void){
    long*syscall=(long*)0xc000d8c4;
    syscall[__NR_get_pagetable_layout]=(unsigned long)oldcall;
    printk(KERN_INFO"module exit!\n");
}

module_init(addsyscall_init);//Load modulze
module_exit(addsyscall_exit);//Exit module


