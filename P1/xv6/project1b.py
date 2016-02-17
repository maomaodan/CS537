import toolspath
from testing import Xv6Build, Xv6Test

class Procs1(Xv6Test):
   name = "procs1"
   description = "call getprocs() from a user program"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 20

class Procs2(Xv6Test):
   name = "procs2"
   description = "call fork() before getprocs()"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 20

class Procs3(Xv6Test):
   name = "procs3"
   description = "call getprocs() with a full process table"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 20

class Procs4(Xv6Test):
   name = "procs4"
   description = "call getprocs() with a process table filled with zombies"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 20

class Procs5(Xv6Test):
   name = "procs5"
   description = "call getprocs() after some entries turn back to UNUSED"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 20

import toolspath
from testing.runtests import main
main(Xv6Build, [Procs1, Procs2, Procs3, Procs4, Procs5])
