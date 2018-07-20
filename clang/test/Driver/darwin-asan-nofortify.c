// Make sure AddressSanitizer disables _FORTIFY_SOURCE on Darwin.

// RUN: %clang -fsanitize=address  %s -E -dM -target x86_64-darwin -resource-dir %S/Inputs/resource_dir | FileCheck %s

// CHECK: #define _FORTIFY_SOURCE 0
