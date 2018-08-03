#include "syringe.hpp"

#include <cassert>
#include <iostream>
#include <syringe/syringe_rt.h>

using namespace __syringe;

int injected_count = 0;
int hello_count = 0;

int main() {
  // ensure that Syringe metadata is initialized
  assert(!__syringe::GlobalSyringeData.empty());
  // assert(__syringe::GlobalSyringeData.size() >1);


  hello(); // normal call to hello()
  assert(hello_count == 1 && "Hello Count incorrect");

  // switch implementation
  assert(toggleImpl(hello) && "hello() could not be toggled!");
  hello();// should be a call to injected()
  assert(injected_count == 1 && "Hello Count incorrect");
  //switch implementation again
  assert(toggleImpl(hello) && "hello() could not be toggled!");
  hello();// another call to hello
  assert(hello_count == 2 && "Hello Count incorrect");


 // check behavior in classes with virtual methods

  // create a base class
  SyringeBase b;

  // verify its initialization state
  assert(b.counter == 0);
  assert(b.other_counter == 0);
  assert(b.getCounter() == b.counter);


  b.increment();// increment b.counter
  assert(b.counter == 1);
  assert(b.getCounter() == b.counter);

  // get the member pointer to the target baseclass function
  mPtrTy *myP = (mPtrTy *)convertMemberPtr(&SyringeBase::increment);

  // switch its implementation
  assert(toggleVirtualImpl(myP, &b) &&
         "SyringeBase::increment() could not be toggled!");
  b.increment();// increment other_counter
  b.increment();// increment other_counter

  assert(b.counter == 1);
  assert(b.other_counter == 2);
  assert(b.getCounter() != b.other_counter);
  assert(b.getCounter() == b.counter);

  std::cout << "\n\033[1;32mAll checks have passed!\033[0m" <<std::endl;
  return 0;
}
