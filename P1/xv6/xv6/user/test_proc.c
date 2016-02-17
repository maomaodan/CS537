#include "types.h"
#include "stat.h"
#include "user.h"
#include "syscall.h"
#include "../kernel/sysfunc.h"

int main(void)
{
  int c = getprocs();
  printf(1,"%d\n",c);
  exit();

}
