#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  // 执行到这里，fork将父进程内容拷贝形成子进程
  // 返回给子进程的pid为0，返回给父进程子进程的pid
  int pid = fork();
  if (pid > 0) {
    printf("parent: child=%d\n", pid);
    pid = wait(NULL);
    printf("child %d is done\n", pid);
  } else if (pid == 0) {
    printf("child: exiting\n");
    exit(0);
  } else {
    printf("fork error\n");
  }
  return 0;
}

// ouput: (理论上第一行和第二行顺可能交换)
// parent: child=45287
// child: exiting
// child 45287 is done