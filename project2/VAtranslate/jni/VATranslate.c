/*
	VATranslate.c
	Type "./VATranslate #Pid Virtual_Address" command on the shell to translate \
	the virtual address to physical address in the target process
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

struct pagetable_layout_info {
uint32_t pgdir_shift;
uint32_t pmd_shift;
uint32_t page_shift;
};


#define PAGE_SHIFT		12
//#define PAGE_SIZE		4096
#define	PGDIR_SHIFT		21
#define PTRS_PER_PGD		2048
#define PTRS_PER_PTE            512
#define pgd_index(addr)         ((addr) >> PGDIR_SHIFT)
#define pte_index(addr)         (((addr) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))
#define pte_pfn(pte)            (pte >> PAGE_SHIFT)






int main(int argc, char **argv)
{
	
	pid_t pid;
	char *end;
	unsigned long va, pte_base, pte_va, pte, index_pgd, index_pte;

	if (argc !=3) {
		printf("Usage: VATranslate #pid VA \n");
		return 0;
	}

	if (argc == 3) {
		pid = strtol(argv[1], &end, 10);

	}

	if (*end) {
		printf("error: could not parse pid");
		exit(1);
	}
	va=strtoul(argv[2],&end,16);
	if (*end) {
		printf("error: could not parse VA ");
		exit(1);
	}
	void *fake_pgd = mmap(NULL, PTRS_PER_PGD * sizeof(unsigned long),
				PROT_READ | PROT_WRITE,
				MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if (fake_pgd == MAP_FAILED) {
		printf("error: create fake_pgd failed\n");
		exit(1);
	}

	void *addr = mmap(NULL, PTRS_PER_PGD * PAGE_SIZE,
				PROT_READ,
				MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if (addr == MAP_FAILED) {
		printf("error: create addr failed\n");
		exit(1);
	}

	if (syscall(356, pid,
		(unsigned long) fake_pgd,
		(unsigned long) addr,(unsigned long)0x00000000,(unsigned long)0xfffff000) < 0) {
		printf("error: expose_page_table failed!\n");
		exit(1);
	}

	
		index_pgd = pgd_index(va);
		pte_base = *((unsigned long *) (fake_pgd +
				(index_pgd * sizeof(unsigned long))));

		if (pte_base) {
			index_pte = pte_index(va);
			pte_va = pte_base + (index_pte * sizeof(unsigned long));
			pte = *((unsigned long *) pte_va);
			if (pte) {
				printf("0x%lx \n",pte_pfn(pte));
			} else  {
				printf("0x%lx \n",(unsigned long) 0);
			}
		}
	

	if (munmap(fake_pgd, PTRS_PER_PGD * sizeof(unsigned long)) == -1) {
		printf("error: release fake_pgd failed\n");
		exit(1);
	}

	if (munmap(addr, PTRS_PER_PGD * PAGE_SIZE) == -1) {
		printf("error: release addr failed\n");
		exit(1);
	}

	return 0;
}
