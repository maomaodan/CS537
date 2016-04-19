import sys, os, inspect

import toolspath
from testing import Xv6Test, Xv6Build

curdir = os.path.realpath(os.path.dirname(inspect.getfile(inspect.currentframe())))
def get_description(name):
  cfile = os.path.join(curdir, 'tests', name+'.c')
  with open(cfile, 'r') as f:
    desc = f.readline()
  desc = desc.strip()
  desc = desc[2:]
  if desc[-2:] == '*/':
    desc = desc[:-2]
  return desc.strip()

test_values = {
  'clone': 9,
  'clone2': 6,
  'clone3': 3,
  'badclone': 6,
  'stack': 8,
  'join': 9,
  'join2': 6,
  'join3': 3,
  'join4': 3,
  'thread': 10,
  'thread2': 7,
  'multi': 8,
  'locks': 8,
  'noexit': 7,
  'size' : 7,
}

all_tests = []
build_test = Xv6Build
for testname in test_values.keys():
  members = {
      'name': testname,
      'tester': 'tests/' + testname + '.c',
      'description': get_description(testname),
      'timeout': 10,
      'point_value' : test_values[testname]
      }
  newclass = type(testname, (Xv6Test,), members)
  all_tests.append(newclass)
  setattr(sys.modules[__name__], testname, newclass)

class usertests(Xv6Test):
  name = 'usertests'
  tester = 'tests/usertests.c'
  description = get_description('usertests')
  timeout = 240

#all_tests.append(usertests)

from testing.runtests import main
main(build_test, all_tests)
