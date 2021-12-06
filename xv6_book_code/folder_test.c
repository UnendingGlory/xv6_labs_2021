#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main() {
  // https://pubs.opengroup.org/onlinepubs/009695299/functions/mkdir.html
  // with read/write/search permissions for owner and group, and with read/search permissions for others
  int ret = mkdir("folder_a", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  printf("%d\n", ret);
  ret = mkdir("folder_a/folder_b", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  printf("%d\n", ret);
  ret = mkdir("folder_a/folder_b/folder_c", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  printf("%d\n", ret);
  return 0;
}

// create three folders
