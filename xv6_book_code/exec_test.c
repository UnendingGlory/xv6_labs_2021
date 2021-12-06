#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main() {
  // 最后一个参数为终止符
  char *args[] = {"/bin/echo", "echo", "hello", NULL};
  // exec会替换调用它的进程的内存但是会保留文件描述符表
  execvp(args[0], args);
  return 0;
}

// ouput:
// echo hello
