
// RUN: %clang_cc1 %s -fsyringe -std=c++11 -x c++ -emit-llvm -o - -triple x86_64-unknown-linux-gnu | FileCheck %s

[[clang::syringe_injection_site]] int __attribute((always_inline)) foo(int a) {
  // CHECK: %0 = load i32 (i32)*, i32 (i32)** @"_Z3fooi$syringe_impl_ptr"
  // CHECK-NEXT: tail call i32 %0(i32 %a) #0
  // CHECK-NEXT: ret i32 %1
  return a + 1;
}

[[clang::syringe_payload("_Z3fooi")]] int bar(int a) {
  // CHECK: define i32 @_Z3bari(i32 %a) #1 {
  return a - 1;
}

// CHECK-NOT: call i32 @_Z3foov()
// CHECK: %[[REG1:[0-9]+]] = load i32 (i32)*, i32 (i32)** @"_Z3fooi$syringe_impl_ptr"
// CHECK-NEXT: %[[REG2:[0-9]+]] = call i32 %[[REG1]](i32 1)
// CHECK-NEXT: store i32 %[[REG2]], i32*
// CHECK: ret i32
// CHECK-NOT: call i32 @_Z3foov()

int main() {
  auto j = foo(1);
  return j;
}
