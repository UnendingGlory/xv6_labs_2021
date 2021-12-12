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

// # define LAB_PGTBL
#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  // first user page virtual address and bitmask beginning address
  uint64 usrpgva, bitmask;
  if (argaddr(0, &usrpgva) < 0 || argaddr(1, &bitmask) < 0)
    return -1;

  // number of pages to check
  // max number of user pages: MAXVA / PGSIZE
  int n;
  if (argint(0, &n) < 0 || n > (MAXVA/PGSIZE)) {
    return -1;
  }

  // get the current process
  struct proc *p = myproc();
  if (p->pagetable == 0) {  // empty pgtbl
    return -1;
  }
  
  pte_t *pte;
  for (int i = 0; i < n; i++) {
    pte = walk(p->pagetable, usrpgva, 0);

    if (*pte & PTE_V) {
      
    }

    usrpgva += PGSIZE;  // next pg
  }

  // printf("%p %d %p\n", usrpgva, n, bitmask);

  // store the result into the address starting from bitmask
  // needs to copy from the kernel into the user space

  // if (copyout()) {
  //   first_vn
  // }

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
