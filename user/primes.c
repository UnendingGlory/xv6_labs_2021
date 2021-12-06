#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
prime_process(int prime, int wr_fd)
{
  int p[2];
  pipe(p);

  int pid = fork();

  if (pid < 0) {
    fprintf(2, "fork error, please try agian!\n");
    exit(1);
  } else if (pid == 0) {
    int num;
    while(read(wr_fd, &num, sizeof(int))) {
      if (num % prime) {
        write(p[1], &num, sizeof(int));
      }
    }
    // child copies the parent memory
    // so child and parent both need to close
    close(wr_fd);
    close(p[1]);
    exit(0);  // child exits here
  } else {
    close(wr_fd);
    close(p[1]);
    wait(0);
  }
  
  return p[0];
}

int
main(int argc, char *argv[])
{
  int p[2];
  pipe(p);
  
  int i;
  const int max_num = 35;
  for (i = 2; i <= max_num; ++i) {
    write(p[1], &i, sizeof(int));
  }

  close(p[1]);
  
  int rd_fd = p[0], first_prime;
  // break the loop if is empty pipe
  while (read(rd_fd, &first_prime, sizeof(int))) {
    fprintf(1, "prime %d\n", first_prime);
    // rd_fd is the next wr_fd
    rd_fd = prime_process(first_prime, rd_fd);
  }

  exit(0);
}
