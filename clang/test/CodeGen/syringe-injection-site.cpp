// RUN: %clang_cc1 %s -fsyringe -std=c++11 -x c++ -emit-llvm -o - -triple x86_64-unknown-linux-gnu  | FileCheck %s

// CHECK: @"_Z3fooi$syringe_bool" = global i8 0
// CHECK: @llvm.global_ctors = appending global [1 x { i32, void ()*, i8* }]

// CHECK: @"_Z3fooi$detour_impl" = alias void (i32), void (i32)* @_Z3bari

// Make sure that the LLVM attribute for Syringe-annotated functions do show up.
[[clang::syringe_injection_site]] void foo(int a) {
// CHECK: define void @_Z3fooi(i32 %a) #0
// CHECK-DAG: %"_Z3fooi$syringe_bool" = load i8, i8* @"_Z3fooi$syringe_bool"
// CHECK-DAG: [[REG2:%[0-9]+]] = trunc i8 %"_Z3fooi$syringe_bool" to i1
// CHECK: br i1 [[REG2]], label %syringe_inject, label %entry
// CHECK:  tail call void @_Z3bari(i32 %a)
// CHECK-NEXT: ret void
};

[[clang::syringe_payload("_Z3fooi")]] void bar(int a) {
// CHECK: define void @_Z3bari(i32 %a) #1 {
};


// CHECK: define internal void @"syringe.module_ctor{{.*}}syringe-injection-site.cpp"()
// CHECK-NEXT: call void @__syringe_register(void (i32)* @_Z3fooi, i8* @"_Z3fooi$syringe_bool")
// CHECK-NEXT: ret void

// CHECK: declare void @__syringe_register(void (i32)*, i8*)


// CHECK: #0 = {{.*}}"syringe-injection-site"{{.*}}
// CHECK: #1 = {{.*}}"syringe-payload"{{.*}}"syringe-target-function"="_Z3fooi"
