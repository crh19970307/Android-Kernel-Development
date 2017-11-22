/*
	
	This program implements  system call:  expose_page_table
	Syscall expose_page_table{
		ID: 356
		function: Map a target process's page table into user space with a range of virtual memory
	}
*/
#include <linux/mm.h>
#include <linux/rwsem.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/unistd.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/rwsem.h>


MODULE_LICENSE("Dual BSD/GPL");

static inline unsigned long pmd_index(unsigned long address)
{
    return (address >> PMD_SHIFT) & (PTRS_PER_PMD - 1);
}

#define __NR_expose_page_table 356
//The number of new syscall

struct pagetable_layout_info {
uint32_t pgdir_shift;
uint32_t pmd_shift;
uint32_t page_shift;
};

static int (*oldcall)(void);
int expose_page_table(pid_t pid, unsigned long fake_pgd,
                        unsigned long addr,unsigned long begin_vaddr,unsigned long end_vaddr)
{
    struct vm_area_struct *pgd_vma, *pt_vma;
    struct task_struct *p;
    unsigned long va, pa, pfn, zero;
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    int res = 0;
    int i, j, k;

    zero = 0;

    if (pid < -1)
        return -EINVAL;

    if (pid == -1) {
        pid = current->pid;
        p = current;
    } else {
        p=pid_task(find_vpid(pid), PIDTYPE_PID);
               if (!p)
            return -EINVAL;
    }
if (begin_vaddr>end_vaddr)
{
	printk("Wrong virtual address range!!!\n ");
	return -EINVAL;
}
if(end_vaddr>0xfffff000)
{
	printk("Wrong end vaddr!!!\n");
	return -EINVAL;
}
    /* We do not process kernel threads, which have NULL mm */
    if (!p->mm)
        return -EINVAL;

    down_read(&p->mm->mmap_sem);//This is a semaphore

    pgd_vma = find_vma(current->mm, fake_pgd);
    pt_vma = find_vma(current->mm, addr);

    if (!pgd_vma || !pt_vma) {
        res = -EINVAL;
        goto out;
    }

    if (pgd_vma->vm_end - fake_pgd < PTRS_PER_PGD * sizeof(unsigned long))
        res = -EINVAL;

    if (pt_vma->vm_end - addr < PTRS_PER_PGD * PAGE_SIZE)
        res = -EINVAL;

    if (pgd_vma->vm_flags & VM_EXEC)
        res = -EINVAL;

    if (pt_vma->vm_flags & VM_WRITE || pt_vma->vm_flags & VM_EXEC)
        res = -EINVAL;

    if (res)
        goto out;

    pgd_vma->vm_flags |= VM_RESERVED;
    pt_vma->vm_flags |= VM_RESERVED;

    for (i = pgd_index(begin_vaddr); i <= pgd_index(end_vaddr); i++) {
        pgd = p->mm->pgd + i;

        /* Stop if reached end of userspace memory */
        if (i >= pgd_index(TASK_SIZE))
            break;

        if (pgd_none(*pgd) || pgd_bad(*pgd))
            continue;
        pud = pud_offset(pgd, zero);
        for (j = 0; j < PTRS_PER_PUD; j++) {
            if (pud_none(*pud) || pud_bad(*pud))
                continue;
            pmd = pmd_offset(pud, zero);
            for (k = 0; k < PTRS_PER_PMD; k++) {
                if (pmd_none(*pmd) || pmd_bad(*pmd))
                    continue;
                pa = pmd_val(*pmd);
                if (pa && (pa & (1 << 0))) {
                    pfn = pa >> PAGE_SHIFT;
                    va = addr;
                    remap_pfn_range(pt_vma, va, pfn,
                        PAGE_SIZE, PAGE_READONLY);
                    //remap the page
                } else {
                    va = 0;
                }

                /* Put the va in fake_pgd */
                if (copy_to_user((void *)
                    (fake_pgd + i * sizeof(va)),
                    &va, sizeof(va))) {
                    res = -EFAULT;
                    goto out;
                }

                addr += PAGE_SIZE;

                pmd++;
            }

            pud++;
        }
    }

out:
    up_read(&p->mm->mmap_sem);
    return res;
}
static int addsyscall_init(void){
    long*syscall=(long*)0xc000d8c4;
    oldcall=(int(*)(void))(syscall[__NR_expose_page_table]);
    syscall[__NR_expose_page_table]=(unsigned long)expose_page_table;
    printk(KERN_INFO"module load!\n");
    return 0;
}


static void addsyscall_exit(void){
    long*syscall=(long*)0xc000d8c4;
    syscall[__NR_expose_page_table]=(unsigned long)oldcall;
    printk(KERN_INFO"module exit!\n");
}

module_init(addsyscall_init);//Load modulze
module_exit(addsyscall_exit);//Exit module
