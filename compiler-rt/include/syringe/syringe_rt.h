//===- syringe_rt.h -----------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is a part of Syringe, a dynamic behavior injection system.
//
// APIs for using the Syringe Runtime.
//===----------------------------------------------------------------------===//

#ifndef SYRINGE_SYRINGE_RT_H
#define SYRINGE_SYRINGE_RT_H 1

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
// typedef for generic function pointer type to use when casting
// or manipulating funciton pointers in the runtime.
typedef void (*fptr_t)(void);

// toggles the current state of injection
bool toggleImpl(fptr_t OrigFunc);

// prints the addresses of syringe metadata, and their values
void printSyringeData();

// registers metadata for initialization in the syringe runtime
void __syringe_register(void *OrigFunc, bool *InjecitonEnabled);

#ifdef __cplusplus
}
#endif

#endif // SYRINGE_SYRINGE_RT_H
