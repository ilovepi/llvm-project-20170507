// RUN: %clang_cc1 %s -fsyringe -std=c++11 -x c++ -emit-llvm -o - -triple x86_64-unknown-linux-gnu | FileCheck %s

// CHECK: @"_Z3fooi$detour_impl" = alias void (i32), void (i32)* @_Z3bari

// Make sure that the LLVM attribute for XRay-annotated functions do show up.
[[clang::syringe_injection_site]] void foo(int a) {
// CHECK: define void @_Z3fooi(i32 %a) #0
// CHECK: %0 = load void (i32)*, void (i32)** @"_Z3fooi$syringe_impl_ptr"
// CHECK-NEXT: tail call void %0(i32 %a) #0
// CHECK-NEXT: ret void
};

[[clang::syringe_payload("_Z3fooi")]] void bar(int a) {
// CHECK: define void @_Z3bari(i32 %a) #1 {
};

// CHECK: define void @"_Z3fooi$syringe_impl"(i32) #2

// CHECK: #0 = {{.*}}"syringe-injection-site"{{.*}}
// CHECK: #1 = {{.*}}"syringe-payload"{{.*}}"syringe-target-function"="_Z3fooi"
