struct buf {
  int valid;   // has data been read from disk?
  int disk;    // does disk "own" buf?
  uint dev;
  uint blockno;
  // spinlock is situable for multiple cores that not contend too much
  // or the thread holds the lock for a short time
  // if the lock contends too much or holds a long time
  // don't spin, just give up the cpu to other threads/process (although with context switch cost) 
  struct sleeplock lock; 
  uint refcnt;
  struct buf *prev; // LRU cache list
  struct buf *next;
  uchar data[BSIZE];

  uint ticks; // using ticks time_stamp instead of list
  int bucketno; // bucket number
};

