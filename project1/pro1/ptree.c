/*
	This file defined a new system call ptree. More details in the internal comment.
*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <asm/uaccess.h>
MODULE_LICENSE("Dual BSD/GPL");

#define __NR_ptree 356
//The number of new syscall

struct prinfo {
    pid_t parent_pid;/* process id of parent, set 0 if it has no parent*/
    pid_t pid;/* process id */
    pid_t first_child_pid;/* pid of youngest child, set 0 if it has no child */
    pid_t next_sibling_pid; /* pid of older sibling, set 0 if it has no sibling*/
    long state;/* current state of process */
    long uid;/* user id of process owner */
    char comm[64];/* name of program executed */
};


static int (*oldcall)(void);
//function pointer

int ptree(struct prinfo* buf, int *nr){
/*
   The argument buf points to a buffer for the process data, and nr points to the size of this buffer (number of entries).
*/
    struct task_struct *p, *k;
    int cnt=0,tnr;
    if(copy_from_user(&tnr,nr,sizeof(int))!=0){
        printk(KERN_INFO"copy_from_user error!\n");
        return -1;
    }
    rcu_read_lock();//While holding the lock, your code may not perform any operations that may result in a sleep, such as memory allocation, copying of data into and out from the kernel etc.
    for_each_process(p){
/*
	This part changes the task_struct to struct prinfo and copy it to the buffer in user space
*/
        if(cnt<tnr){
            struct prinfo t;
            t.parent_pid=p->parent->pid;
            t.pid=p->pid;
            if(p->children.next==&p->children)
                t.first_child_pid=0;
            else
                t.first_child_pid=list_entry(p->children.next,struct task_struct,sibling)->pid;
            if(p->sibling.next==p->parent->children.next)
                t.next_sibling_pid=0;
            else
                t.next_sibling_pid=list_entry(p->sibling.next,struct task_struct,sibling)->pid;
            if(t.next_sibling_pid==1)
                t.next_sibling_pid=0;
            t.state=p->state;
            t.uid=p->cred->uid;
            strcpy(t.comm,p->comm);
            if(copy_to_user(buf+cnt,&t,sizeof(t))!=0){
                printk(KERN_INFO"copy_to_user error!\n");
                return -1;
            }
                
        }
        ++cnt;
    }
    if(cnt<tnr)
        if(copy_to_user(nr,&cnt,sizeof(int))!=0){
            printk(KERN_INFO"copy_to_user error!\n");
            return -1;
        }
    rcu_read_unlock();
    return cnt;
}

static int addsyscall_init(void){
    long*syscall=(long*)0xc000d8c4;
    oldcall=(int(*)(void))(syscall[__NR_ptree]);
    syscall[__NR_ptree]=(unsigned long)ptree;
    printk(KERN_INFO"module load!\n");
    return 0;
}


static void addsyscall_exit(void){
    long*syscall=(long*)0xc000d8c4;
    syscall[__NR_ptree]=(unsigned long)oldcall;
    printk(KERN_INFO"module exit!\n");
}

module_init(addsyscall_init);//Load module
module_exit(addsyscall_exit);//Exit module


