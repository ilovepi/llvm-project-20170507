//===- injection_data.h -----------------------------------------*- C++ -*-===//
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
// Data structures used by the syringe runtime.
//===----------------------------------------------------------------------===//


#ifndef SYRINGE_INJECTION_DATA_H
#define SYRINGE_INJECTION_DATA_H 1

#include <stddef.h>

// Outside of namespace for C compatibility
typedef void (*fptr_t)(void);

namespace __syringe {

struct InjectionData {
  fptr_t OrigFunc;
  fptr_t StubImpl;
  fptr_t DetourFunc;
  fptr_t *ImplPtr;
  template <typename T, typename R>
  InjectionData(T OrigFunction, T StubImplementation, T DetourFunction,
                R ImplPointer)
      : OrigFunc((fptr_t)OrigFunction), StubImpl((fptr_t)StubImplementation),
        DetourFunc((fptr_t)DetourFunction), ImplPtr((fptr_t *)ImplPointer) {}
};

struct mPtrTy {
  ptrdiff_t ptr;
  ptrdiff_t adj;
};

} // namespace __syringe
#endif // SYRINGE_INJECTION_DATA_H
