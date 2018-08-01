#include "syringe.hpp"
#include <iostream>

extern int hello_count;
[[clang::syringe_injection_site]] void hello() {
  std::cout << "Hello World!" << std::endl;
  ++hello_count;
}

