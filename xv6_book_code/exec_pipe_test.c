#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


int main() {
  int p[2];
  char *args[] = {"cat", NULL};
  pipe(p);
  // 子进程负责读
  if (fork() == 0) {
    close(0);
    // 由于关闭了0的文件描述符，复制p[0]后得到的文件描述符自动分配fd=0
    // 并和p[0]共享偏移
    dup(p[0]);
    close(p[0]);
    close(p[1]);
    execvp(args[0], args);
  } else {
    write(p[1], "hello world\n", 12);
    close(p[0]);
    close(p[1]);
  }
  return 0;
}

// output
// hellow world
