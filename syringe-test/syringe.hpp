#ifndef _SYRINGE_HPP_
#define _SYRINGE_HPP_

[[clang::syringe_injection_site]] void hello();

[[clang::syringe_payload("_Z5hellov")]] void injected();

class SyringeBase {
public:
  SyringeBase() = default;
  virtual ~SyringeBase() = default;

  int counter;
  int other_counter;
  [[clang::syringe_injection_site]] void virtual increment();
  [[clang::syringe_injection_site]] int getCounter();
  void foo();
};

class SyringeDerived : public SyringeBase {
public:
  SyringeDerived() = default;
  virtual ~SyringeDerived() = default;
  void increment();
  int getCounter() ;
};

#endif
