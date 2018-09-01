#include "syringe.hpp"
#include <iostream>

extern int hello_count;

//__attribute__((syringe_injection_site)) void hello() { ++hello_count; }

SyringeBase::SyringeBase() : counter(0), other_counter(0) {}

SyringeDerived::SyringeDerived() {}

[[clang::syringe_injection_site]] int SyringeBase::getCounter() {
  return counter;
}

//[[clang::syringe_payload(SyringeBase::getCounter)]]
__attribute__((syringe_class_payload(SyringeBase))) 
int SyringeDerived::getCounter() {
  return other_counter;
}

[[clang::syringe_injection_site]] void SyringeBase::increment() { ++counter; };

[[clang::syringe_payload("_ZN11SyringeBase9incrementEv")]] void
SyringeDerived::increment() {
  ++other_counter;
}

void SyringeBase::foo() {}
