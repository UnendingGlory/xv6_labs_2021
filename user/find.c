#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
search(char *path, char *dist)
{

  struct stat st;
  int fd;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "fstate: cannot stat %s\n", path);
    close(fd);
    return;
  }

  struct dirent de;
  char p[512];

  switch (st.type)
  {
    case T_FILE:
      if (strcmp(path, dist) == 0) {
        fprintf(1, "%s/%s\n", path, dist);
      }
      break;
    
    case T_DIR:
      while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0) { continue; }  // inode 0 is used to indicate null inode
        
        // skip . and ..
        if (strcmp(de.name, ".") == 0) { continue; }
        if (strcmp(de.name, "..") == 0) { continue; }

        if (strcmp(de.name, dist) == 0) {
          fprintf(1, "%s/%s\n", path, de.name);
        }
        
        int len = strlen(path);
        strcpy(p, path);
        memset(p+len, '/', 1);
        memmove(p+len+1, de.name, sizeof(de.name));
        // printf("%s\n", p);

        search(p, dist);
      }
      break;
  }

  close(fd);  // remeber to close
}


int
main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(2, "Usage: find path filename\n");
    exit(1);
  }

  if (argc == 2) {  // default find in the current dir
    search(".", argv[1]);
  } else if (argc == 3) {
    search(argv[1], argv[2]);
  } else {
    fprintf(2, "find: too many arguments!\n");
    exit(1);
  }

  exit(0);
}
