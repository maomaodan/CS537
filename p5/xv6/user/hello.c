#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int 
main(int argc, char *argv[])
{
  int fd = open("foo.txt",O_CREATE|O_SMALLFILE);
  printf(1,"file %d\n",fd);
  close(fd);
  exit();
}
