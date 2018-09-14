
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

// CHECK: define i32 @main()
// CHECK: entry:
// CHECK:   %retval = alloca i32, align 4
// CHECK:   %j = alloca i32, align 4
// CHECK:   store i32 0, i32* %retval, align 4
// CHECK:   %savedstack = call i8* @llvm.stacksave()
// CHECK:   %"_Z3fooi$syringe_bool.i" = load i8, i8* @"_Z3fooi$syringe_bool"
// CHECK:   %0 = trunc i8 %"_Z3fooi$syringe_bool.i" to i1
// CHECK:   br i1 %0, label %syringe_inject.i, label %entry.i
// CHECK: entry.i:
// CHECK:   %a.addr.i = alloca i32, align 4
// CHECK:   store i32 1, i32* %a.addr.i, align 4
// CHECK:   %1 = load i32, i32* %a.addr.i, align 4
// CHECK:   %add.i = add nsw i32 %1, 1
// CHECK:   call void @llvm.stackrestore(i8* %savedstack)
// CHECK:   br label %_Z3fooi.exit
// CHECK: syringe_inject.i:
// CHECK:   %2 = call i32 @_Z3bari(i32 1) #3
// CHECK:   call void @llvm.stackrestore(i8* %savedstack)
// CHECK:   br label %_Z3fooi.exit
// CHECK: _Z3fooi.exit:
// CHECK:   %call1 = phi i32 [ %add.i, %entry.i ], [ %2, %syringe_inject.i ]
// CHECK:   store i32 %call1, i32* %j, align 4
// CHECK:   %3 = load i32, i32* %j, align 4
// CHECK:   ret i32 %3



int main() {
  auto j = foo(1);
  return j;
}
///usr/local/google/home/paulkirth/workspace/llvm-dev/build/bin/clang -cc1 -internal-isystem /usr/local/google/home/paulkirth/workspace/llvm-dev/build/lib/clang/8.0.0/include -nostdsysteminc /usr/local/google/home/paulkirth/workspace/llvm-dev/clang/test/CodeGen/syringe-inline-function.cpp -fsyringe -std=c++11 -x c++ -emit-llvm -o - -triple x86_64-unknown-linux-gnu
