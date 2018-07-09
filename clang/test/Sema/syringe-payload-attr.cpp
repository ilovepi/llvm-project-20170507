// RUN: %clang_cc1 %s -verify -fsyntax-only -std=c++11 -x c++
void foo [[clang::syringe_payload]] ();  // expected-error {{'syringe_payload' attribute takes one argument}}

struct [[clang::syringe_payload]] a { int x; }; // expected-warning {{'syringe_payload' attribute only applies to functions and Objective-C methods}}

class b {
 void c [[clang::syringe_payload("_Z3foov")]] ();
};

void baz [[clang::syringe_payload("foo")]] (); 
