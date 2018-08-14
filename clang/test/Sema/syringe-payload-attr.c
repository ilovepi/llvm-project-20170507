// RUN: %clang_cc1 %s -verify -fsyntax-only -std=c11
void foo() __attribute__((syringe_payload)) ; // expected-error {{'syringe_payload' attribute takes one argument}}

struct __attribute__((syringe_payload)) a { int x; }; // expected-warning {{'syringe_payload' attribute only applies to functions and Objective-C methods}}

void bar() __attribute__((syringe_payload("foo"))); 
