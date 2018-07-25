#include "util.hpp"

// void (*_Z17hello_syringe_ptr)() = nullptr;
// extern void (*_Z17hello_syringe_ptr)();

void init(bool shouldInject) {
  if (shouldInject) {
    _Z17hello_syringe_ptr = injected;
  } else {
    _Z17hello_syringe_ptr = hello_syringe_impl;
  }
}

void goodbye() { _Z17hello_syringe_ptr(); }
