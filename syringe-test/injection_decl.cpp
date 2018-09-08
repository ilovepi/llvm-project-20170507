#include <iostream>

extern int injected_count;
extern int other_counter;
extern void hello();

//[[clang::syringe_payload("_Z5hellov")]] void injected() {
[[clang::syringe_payload(hello)]] void injected() {
  ++injected_count;
}
