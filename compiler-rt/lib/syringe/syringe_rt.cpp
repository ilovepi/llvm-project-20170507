//===-- syringe_rt.cpp ------------------------------------------*- C++ -*-===//
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

#include <iostream>
#include <vector>
#include <stdio.h>

#include "syringe/injection_data.h"
#include "syringe/syringe_rt.h"
#include "syringe/syringe_rt_cxx.h"

namespace __syringe {

std::vector<InjectionData> GlobalSyringeData;

InjectionData *findImplPointerImpl(fptr_t target) {
  auto It = std::find_if(GlobalSyringeData.begin(), GlobalSyringeData.end(),
                         [target](InjectionData It) -> bool {
                           return (void *)It.OrigFunc == (void *)target;
                         });

  if (It == GlobalSyringeData.end()) {
    //fprintf(stderr, "Target for Syringe injection(%p) not found!\n", target);
    //exit(1);
    return nullptr;
  } else {
    return &*It;
  }
}

} // end namespace __syringe

void printSyringeData() {
  std::cout << "Syringe Global Data" << std::endl;
  for (auto &item : __syringe::GlobalSyringeData) {
    std::cout << "Orig Func: " << (void *)item.OrigFunc
              << ", StubImpl: " << (void *)item.StubImpl
              << ", DetourFunc: " << (void *)item.DetourFunc
              << ", ImplPtr: " << (void *)item.ImplPtr << std::endl;
  }

  std::cout << std::endl;
}

bool toggleImpl(fptr_t OrigFunc) { return __syringe::toggleImplPtr(OrigFunc); }

bool changeImpl(fptr_t OrigFunc, fptr_t NewImpl) {
  return __syringe::changeImpl(OrigFunc, NewImpl);
};

void __syringe_register(void *OrigFunc, void *StubImpl, void *DetourFunc,
                        void **ImplPtr) {
  __syringe::registerInjection((fptr_t)OrigFunc, (fptr_t)StubImpl,
                               (fptr_t)DetourFunc, (fptr_t *)ImplPtr);
}
