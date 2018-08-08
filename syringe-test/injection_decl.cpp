#include <iostream>

extern int injected_count;
extern int other_counter;
[[clang::syringe_payload("_Z5hellov")]] void injected() {
  ++injected_count;
}
