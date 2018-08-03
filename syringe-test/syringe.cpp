#include "syringe.hpp"
#include <iostream>

extern int hello_count;

[[clang::syringe_injection_site]] void hello() {
  std::cout << "Hello World!" << std::endl;
  ++hello_count;
}

[[clang::syringe_injection_site]] int SyringeBase::getCounter() {
  return counter;
}

[[clang::syringe_payload("_ZN11SyringeBase9incrementEv")]] int badGetCounter() {
  return 5;
}

[[clang::syringe_payload("_ZN11SyringeBase10getCounterEv")]] int
SyringeDerived::getCounter() {
  return other_counter;
}

[[clang::syringe_injection_site]] void SyringeBase::increment() {
  ++counter;
};

void SyringeBase::foo() {}
