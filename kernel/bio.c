// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

// use hash table instead of bidirectional double linked list
#define NBFSIZE 5 // if too small, may panic in "usertests manywrites"
#define NBUCKET 13

struct {
  struct spinlock lock;
  struct buf buf[NBFSIZE];
} bcache[NBUCKET];

uint
hash(uint blockno)
{
  return blockno % NBUCKET; // simple linear hash
}

void
binit(void)
{
  struct buf *b, *bbuf;

  for (int i = 0; i < NBUCKET; ++i) {
    initlock(&bcache[i].lock, "bcache");
    bbuf = bcache[i].buf;
    for (b = bbuf; b < bbuf + NBFSIZE; ++b) { // each buf in the hash bucket
      initsleeplock(&b->lock, "buffer");
      b->ticks = ticks; // timestamp
      b->bucketno = i;  // belong to which bucket
    }
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  uint key = hash(blockno);
  struct buf *b;

  acquire(&bcache[key].lock);

  struct buf *bbuf = bcache[key].buf;
  // Is the block already cached?
  for (b = bbuf; b < bbuf + NBFSIZE; ++b){
    if (b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      b->ticks = ticks;
      release(&bcache[key].lock);
      acquiresleep(&b->lock); // acquire sleep lock to read/write
      return b; // return a locked buffer
    }
  }

  // Not cached. Least recently used by using ticks in trap.
  struct buf *found = 0;
  uint min_ticks = ~0U;
  for (b = bbuf; b < bbuf + NBFSIZE; ++b) {
    if (b->ticks < min_ticks && b->refcnt == 0) {
      found = b;
      min_ticks = b->ticks;
    }
  }

  // The hints said:
  // "When replacing a block, you might move a struct buf from one bucket to another bucket, 
  // because the new block hashes to a different bucket. 
  // You might have a tricky case: the new block might hash to the same bucket as the old block. 
  // Make sure you avoid deadlock in that case."
  // But in this implementation, it is not possible, because we search the buf inside the bucket

  if (found) { // found a buffer cache
    found->dev = dev;
    found->blockno = blockno;
    found->valid = 0;
    found->refcnt = 1;
    found->ticks = ticks;
    release(&bcache[key].lock);
    acquiresleep(&found->lock);
    return found;
  }

  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) { // read buffer from the disk
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
// need to get the corresponding bucket number
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  acquire(&bcache[b->bucketno].lock);
  b->refcnt--;
  if (b->refcnt == 0) {
    b->ticks = ticks;
  }
  release(&bcache[b->bucketno].lock);
}

// pin the buffer to the block
void
bpin(struct buf *b) {
  acquire(&bcache[b->bucketno].lock);
  b->refcnt++;
  release(&bcache[b->bucketno].lock);
}

void
bunpin(struct buf *b) {
  acquire(&bcache[b->bucketno].lock);
  b->refcnt--;
  release(&bcache[b->bucketno].lock);
}


