#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc != 1) {
    fprintf(2, "pingpong: too many arguments\n");
    exit(1);
  }
  
  // pipe is better unidirectional, 0 for read, 1 for write
  // two-way communication needs two pipes
  int p[2][2];
  pipe(p[0]);  // parent -> child
  pipe(p[1]);  // child -> parent 

  int pid = fork();
  uint8 send_byte = 0, received_byte;  // a byte

  if (pid < 0) {
    fprintf(2, "fork error, please try agian!\n");
  } else if (pid == 0) {
    // for child process
    close(p[0][1]);
    read(p[0][0], &received_byte, sizeof(uint8));
    fprintf(1, "%d: received ping\n", getpid());

    close(p[1][0]);
    write(p[1][1], &received_byte, sizeof(uint8));

  } else {
    // for parent process
    close(p[0][0]);
    write(p[0][1], &send_byte, sizeof(uint8));
    
    close(p[1][1]);
    read(p[1][0], &send_byte, sizeof(uint8));
    fprintf(1, "%d: received pong\n", getpid());
  }
  
  exit(0);
}
