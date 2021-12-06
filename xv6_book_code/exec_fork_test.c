#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
  int pid = fork();
  if (pid > 0) {
    pid = wait(NULL);
  } else if (pid == 0) {
    // 关闭文件描述符0，由于open会挑选最小的闲置文件描述符
    // 所以用0作为新打开的exec_fork_test.txt的文件描述符
    // 而文件描述符0代表标准输入
    close(0);
    char *args[] = {"cat", NULL};
    open("exec_fork_test.txt", O_RDONLY);  // 使用exec_fork_test.txt作为标准输入
    execvp(args[0], args);  // 通常忽略第一个参数
    exit(0);
  } else {
    printf("fork error\n");
  }
  return 0;
}

// ouput: (即input.txt的内容)
// exec cat test