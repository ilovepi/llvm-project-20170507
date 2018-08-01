#include <iostream>

extern int injected_count;
[[clang::syringe_payload("_Z5hellov")]] void injected() {
  std::cout << "I've Hijacked the World!" << std::endl;
  injected_count++;
}
