#include "types.h"
#include "user.h"
#include "x86.h"

#define assert(x) if (x) { /* pass */ } else { \
   printf(1, "assert failed %s %s %d\n", #x , __FILE__, __LINE__); \
   exit(); \
   }


// Test1: 2 sleeping, 1 running
int
main(int argc, char *argv[])
{
  // printf(1, "%d processes are currently running.\n", getprocs());
  assert(getprocs() == 3);
  printf(1, "TEST PASSED\n");
  exit();
}
