// RUN: echo "- filename: %s"    > %t.syringe-imbue.yml
// RUN: echo "  targets:"                       >> %t.syringe-imbue.yml
// RUN: echo "  - name: _Z3foov"                >> %t.syringe-imbue.yml
// RUN: echo "    enable: ALWAYS"               >> %t.syringe-imbue.yml
// RUN: echo "    disable: NEVER"               >> %t.syringe-imbue.yml
// RUN: echo "  payloads:"                      >> %t.syringe-imbue.yml
// RUN: echo "  - name: _Z3barv"                >> %t.syringe-imbue.yml
// RUN: echo "    target: _Z3foov"              >> %t.syringe-imbue.yml
// RUN: %clang -fsyringe -fsyringe-config-file=%t.syringe-imbue.yml \
// RUN:     -x c++ -std=c++11 -emit-llvm -S  %s  -o - | FileCheck %s

void foo(){}

void bar(){}

// CHECK: define dso_local void @_Z3foov() #[[FOO:[0-9]+]] {
// CHECK: define dso_local void @_Z3barv() #[[BAR:[0-9]+]] {

// CHECK: attributes #[[FOO]] = {{.*}}"syringe-injection-site"{{.*}}
// CHECK: attributes #[[BAR]] = {{.*}} "syringe-payload" {{.*}}"syringe-target-function"="_Z3foov"
