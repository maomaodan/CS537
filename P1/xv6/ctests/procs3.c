#include "types.h"
#include "user.h"
#include "param.h"


#define assert(x) if (x) { /* pass */ } else { \
   printf(1, "assert failed %s %s %d\n", #x , __FILE__, __LINE__); \
   exit(); \
   }


// Test3: 2 + 61 sleeping, 1 running
int
main(int argc, char *argv[])
{
  int i = 0;
  for(i = 3; i < NPROC; i++) { // 0: init, 1: sh, 2: procs3
    if (fork() != 0) {
      wait();
      exit();
    }
  }

  // printf(1, "%d processes are currently running.\n", getprocs());
  assert(getprocs() == NPROC);
  printf(1, "TEST PASSED\n");

  exit();
}
