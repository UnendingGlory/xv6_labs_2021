#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  char buf[512];
  int n;
  for (;;) {
    // fd 0: standard input
    n = read(0, buf, sizeof(buf));
    if (n == 0) {
      break;
    }
    if (n < 0) {
      // fd 2: standard error
      fprintf(2, "read error\n");  // file printf
      exit(0);
    }
    // fd 1: standard output
    if (write(1, buf, n) != n) {
      fprintf(2, "write error\n");
      exit(0);
    }
  }
  return 0;
}
