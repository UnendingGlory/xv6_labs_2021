// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

// add a reference for each pyhsical page
int pgref[PHYSTOP / PGSIZE];

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Increase the page reference
void
kpgref(void *pa)
{
  int pg_idx = (uint64)pa / PGSIZE;
  if (pg_idx >= 0 && pg_idx < (PHYSTOP/PGSIZE) && pgref[pg_idx] > 0) {
    ++pgref[pg_idx];
  } else {
    panic("kpgref page ref error\n");
  }
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // TODO: decrease the pgref
  // only free it when the ref is 0
  int pg_idx = (uint64)pa / PGSIZE;
  if (pgref[pg_idx] <= 0) {
    printf("%d\n", pgref[pg_idx]);
    panic("kfree page ref error\n");
  }
  if (pg_idx >= 0 && pg_idx < (PHYSTOP/PGSIZE) && pgref[pg_idx] > 0) {
    --pgref[pg_idx];
  } else {
    printf("%d\n", pg_idx);
    panic("kfree page ref error\n");
  }

  if (pgref[pg_idx] > 0) // if page is still available
    return;

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  r = kmem.freelist;
  if(r) { // first free page
    kmem.freelist = r->next;
    uint64 pa = (uint64)r;
    int pg_idx = pa / PGSIZE;
    if (pg_idx >= 0 && pg_idx < (PHYSTOP/PGSIZE)) {
      pgref[pg_idx] = 1;
    } else {
      panic("kalloc page ref error\n");
    }
  }

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
