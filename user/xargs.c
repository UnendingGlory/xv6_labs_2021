#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int 
main(int argc, char *argv[])
{
  // do not care about optimization first
  // optimization args: -n 1

  if (argc < 2) {
    fprintf(2, "xargs too few arguments!\n");
    exit(1);
  }

  char c, *params[MAXARG];  // remember to malloc for pointers
  int i;
  
  for (i = 0; i < argc - 1; ++i) {  // ignore xrags
    params[i] = argv[i+1];
  }

  int j = argc - 1;
  i = 0;
  char tmp_arg[128];

  // | means first command output as the second command std input
  // read from the std input
  while (read(0, &c, sizeof(c))) {
    if (c == '\n') {
      memset(tmp_arg+i, '\0', sizeof(char));
      params[j] = malloc(sizeof(char) * (strlen(tmp_arg)+1));
      strcpy(params[j], tmp_arg);
      // a newline

      if (fork() == 0) {
        exec(params[0], params);
        exit(0);
      } else {
        wait(0);
      }

      j = argc - 1;
      i = 0;
    } else if (c == ' ') {
      memset(tmp_arg+i, '\0', sizeof(char));
      params[j] = malloc(sizeof(char) * (strlen(tmp_arg)+1));
      strcpy(params[j], tmp_arg);
      
      // next param
      ++j;
      i = 0;
    } else {
      memset(tmp_arg+i, c, sizeof(c));
      ++i;
    }
  }
  
  exit(0);
}
