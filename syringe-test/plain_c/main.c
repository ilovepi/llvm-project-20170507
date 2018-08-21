#include "syringe.h"

#include <assert.h>
#include <stdio.h>
#include <syringe/syringe_rt.h>


int injected_count = 0;
int hello_count = 0;


int main() {
  // ensure that Syringe metadata is initialized

  hello(); // normal call to hello()
  assert(hello_count == 1 && "Hello Count incorrect");

  // switch implementation
  assert(toggleImpl(hello) && "hello() could not be toggled!");
  hello(); // should be a call to injected()
  assert(injected_count == 1 && "Hello Count incorrect");

  // switch implementation again
  assert(toggleImpl(hello) && "hello() could not be toggled!");
  hello(); // another call to hello
  assert(hello_count == 2 && "Hello Count incorrect");

  printf( "\n\033[1;32mAll checks have passed!\033[0m\n");
  return 0;
}
