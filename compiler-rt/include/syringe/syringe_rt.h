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

typedef void (*fptr_t)(void);

bool toggleImpl(fptr_t OrigFunc);

bool changeImpl(fptr_t OrigFunc, fptr_t NewImpl);

void printSyringeData();

void __syringe_register(void *OrigFunc, void *StubImpl, void *DetourFunc,
                        void **ImplPtr);

#ifdef __cplusplus
}
#endif

#endif // SYRINGE_SYRINGE_RT_H
