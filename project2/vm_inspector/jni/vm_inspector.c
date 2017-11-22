/*
	vm_inspector.c
	Type "./vm_inspector #Pid begin_vaddr end_vaddr" command on the shell 
	show the page map from virtual memory to physical memory in the range  [begin_vaddr, end_vaddr]
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>



#define PAGE_SHIFT		12
#define PAGE__SIZE		4096
#define	PGDIR_SHIFT		21
#define PTRS_PER_PGD		2048
#define PTRS_PER_PTE            512
#define pgd_index(addr)         ((addr) >> PGDIR_SHIFT)
#define pte_index(addr)         (((addr) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))
#define pte_pfn(pte)            (pte >> PAGE_SHIFT)

int main(int argc, char **argv)
{
	//freopen("out.txt", "w", stdout);

	pid_t pid;
	char *end;
	unsigned long begin_vaddr,end_vaddr,va, pte_base, pte_va, pte, index_pgd, index_pte;

	if (argc !=4) {
		printf("Usage: vm_inspector  <pid> begin_addr end_addr\n");
		return 0;
	}

	
	pid = strtol(argv[1], &end, 10);
	if (*end) {
		printf("error: could not parse pid");
		exit(1);
	}
	begin_vaddr=strtoul(argv[2],&end,16);
	if (*end) {
		printf("error: could not parse begin_vaddr");
		exit(1);
	}

	end_vaddr=strtoul(argv[3],&end,16);
	if (*end) {
		printf("error: could not parse end_vaddr");
		exit(1);
	}

	void *fake_pgd = mmap(NULL, PTRS_PER_PGD * sizeof(unsigned long),
				PROT_READ | PROT_WRITE,
				MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if (fake_pgd == MAP_FAILED) {
		printf("error: create fake_pgd failed\n");
		exit(1);
	}

	void *addr = mmap(NULL, PTRS_PER_PGD * PAGE__SIZE,
				PROT_READ,
				MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if (addr == MAP_FAILED) {
		printf("error: create addr failed\n");
		exit(1);
	}

	if (syscall(356 , pid,
		(unsigned long) fake_pgd,
		(unsigned long) addr,begin_vaddr,end_vaddr) < 0) {
		printf("error: expose_page_table failed!\n");
		exit(1);
	}

	for (va = begin_vaddr; va < end_vaddr; va += PAGE__SIZE) {
		index_pgd = pgd_index(va);
		pte_base = *((unsigned long *) (fake_pgd +
				(index_pgd * sizeof(unsigned long))));

		if (pte_base) {
			index_pte = pte_index(va);
			pte_va = pte_base + (index_pte * sizeof(unsigned long));
			pte = *((unsigned long *) pte_va);
			if (pte) {
				printf("0x%lx 0x%lx 0x%lx\n",index_pgd,va,pte_pfn(pte));
			} 
		
		}
	}

	if (munmap(fake_pgd, PTRS_PER_PGD * sizeof(unsigned long)) == -1) {
		printf("error: release fake_pgd failed\n");
		exit(1);
	}

	if (munmap(addr, PTRS_PER_PGD * PAGE__SIZE) == -1) {
		printf("error: release addr failed\n");
		exit(1);
	}

	return 0;
}
