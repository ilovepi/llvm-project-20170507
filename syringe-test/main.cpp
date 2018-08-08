#include "syringe.hpp"
#include "template.hpp"

#include <cassert>
#include <iostream>
#include <syringe/syringe_rt.h>

using namespace __syringe;

int injected_count = 0;
int hello_count = 0;

template int bad_foo<int>(int a, int b);
template class BadAdder<int>;

int main() {
  // ensure that Syringe metadata is initialized
  assert(!__syringe::GlobalSyringeData.empty());

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

  // check behavior in classes with virtual methods

  // create a base class
  SyringeBase b;

  // verify its initialization state
  assert(b.counter == 0);
  assert(b.other_counter == 0);
  assert(b.getCounter() == b.counter);

  b.increment(); // b.counter = 1
  assert(b.counter == 1);
  assert(b.getCounter() == b.counter);

  // get the member pointer to the target baseclass function
  mPtrTy *myP = (mPtrTy *)convertMemberPtr(&SyringeBase::increment);

  // switch its implementation
  assert(toggleVirtualImpl(myP, &b) &&
         "SyringeBase::increment() could not be toggled!");
  b.increment(); // other_counter = 1
  b.increment(); // other_counter = 2

  assert(b.counter == 1);
  assert(b.other_counter == 2);
  assert(b.getCounter() != b.other_counter);
  assert(b.getCounter() == b.counter);

  // test template function
  assert(foo(1, 1) == 2 && "Problem with original template funciton foo!");
  toggleImpl(foo<int>);
  assert(foo(1, 1) == 0 && "injection failed for function foo!");

  Adder<int> a(1);
  assert(a.data == 1);
  assert(a.add(1) == 2 && "Problem with original class template Adder::add()!");

  assert(toggleImpl(&Adder<int>::add) &&
         "SyringeBase::increment() could not be toggled!");

  assert(a.add(1) == 0 && "Injection failed for class template Adder::add()!");

  std::cout << "\n\033[1;32mAll checks have passed!\033[0m" << std::endl;
  return 0;
}
