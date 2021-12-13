#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// for local coding
#ifndef LAB_PGTBL
#define LAB_PGTBL
#endif

#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // first user page virtual address and bitmask beginning address
  uint64 usrpgva, bitmask;
  if (argaddr(0, &usrpgva) < 0 || argaddr(2, &bitmask) < 0)
    return -1;

  // number of pages to check
  // max number of user pages: MAXVA / PGSIZE
  int n;
  if (argint(1, &n) < 0 || n > (MAXVA/PGSIZE)) {  // 2^26
    return -1;
  }

  // get the current process
  struct proc *p = myproc();
  if (p->pagetable == 0) {  // empty pgtbl
    return -1;
  }
  
  // vmprint(p->pagetable, 0);
  // printf("Kernel: %p %d %p\n", usrpgva, n, bitmask);

  pte_t *pte;
  uint32 result = 0;

  for (int i = 0; i < n; i++) {
    pte = walk(p->pagetable, usrpgva, 0);
    if ((*pte & PTE_V) && (*pte & PTE_A)) {  // if accessed

        // printf("%d Found PTE: %p\n", i, *pte);

        result |= (1 << i);  // set the correspoding bit
        *pte ^= PTE_A;  // use XOR to clear PTE_A
    }

    usrpgva += PGSIZE;  // next pg
  }

  // store the result into the address starting from bitmask
  // needs to copy from the kernel into the user space
  if (copyout(p->pagetable, bitmask, (char *)&result, sizeof(result)) < 0) {
    return -1;
  }

  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
