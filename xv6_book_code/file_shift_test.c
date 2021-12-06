#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  // 执行到这里，fork将父进程内容拷贝形成子进程
  // 返回给子进程的pid为0，返回给父进程子进程的pid
  int pid = fork();
  if (pid == 0) {
    write(1, "hello ", 6);
    exit(0);
  } else {
    wait(NULL);
    write(1, "world", 6);
  }
  return 0;
}

// 另一种使用dup的写法
// int main() {
//   // dup复制一个已有的文件描述符，返回指向同一个输入输出对象的新描述符
//   // 这两个描述符共享一个文件偏移
//   fd = dup(1);
//   write(1, "hello", 6);
//   write(fd, "world\n", 6);
//   return 0;
// }

// ouput:
// hello world