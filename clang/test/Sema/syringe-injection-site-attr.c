// RUN: %clang_cc1 %s -verify -fsyntax-only -std=c11
void foo() __attribute__((syringe_injection_site)) ;

struct __attribute__((syringe_injection_site)) a { int x; }; // expected-warning {{'syringe_injection_site' attribute only applies to functions and Objective-C methods}}

void bar() __attribute__((syringe_injection_site("not-supported"))); // expected-error {{'syringe_injection_site' attribute takes no arguments}}
