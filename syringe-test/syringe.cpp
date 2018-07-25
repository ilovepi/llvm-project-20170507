#include "syringe.hpp"
#include "util.hpp"
#include <iostream>

[[clang::syringe_injection_site]] void hello() {
  std::cout << "Hello World!" << std::endl;
}

[[clang::syringe_payload]] void injected() {
  std::cout << "I've Hijacked the World!" << std::endl;
}
