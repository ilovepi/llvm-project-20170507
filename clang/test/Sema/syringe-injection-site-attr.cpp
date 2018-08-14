// RUN: %clang_cc1 %s -verify -fsyntax-only -std=c++11 -x c++
void foo [[clang::syringe_injection_site]] ();

struct [[clang::syringe_injection_site]] a { int x; }; // expected-warning {{'syringe_injection_site' attribute only applies to functions and Objective-C methods}}

class b {
 void c [[clang::syringe_injection_site]] ();
};

void baz [[clang::syringe_injection_site("not-supported")]] (); // expected-error {{'syringe_injection_site' attribute takes no arguments}}
