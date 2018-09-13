
// RUN: %clang_cc1 %s -fsyringe -std=c++11 -x c++ -emit-llvm -o - -triple x86_64-unknown-linux-gnu | FileCheck %s

[[clang::syringe_injection_site]] int __attribute((always_inline)) foo(int a) {
  // CHECK: %"_Z3fooi$syringe_bool" = load i8, i8* @"_Z3fooi$syringe_bool"
  // CHECK_NEXT: %0 = trunc i8 %"_Z3fooi$syringe_bool" to i1
  // CHECK_NEXT: br i1 %0, label %syringe_inject, label %entry
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
///usr/local/google/home/paulkirth/workspace/llvm-dev/build/bin/clang -cc1 -internal-isystem /usr/local/google/home/paulkirth/workspace/llvm-dev/build/lib/clang/8.0.0/include -nostdsysteminc /usr/local/google/home/paulkirth/workspace/llvm-dev/clang/test/CodeGen/syringe-inline-function.cpp -fsyringe -std=c++11 -x c++ -emit-llvm -o - -triple x86_64-unknown-linux-gnu
