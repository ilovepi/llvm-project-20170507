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

#include "syringe/injection_data.h"
#include "syringe/syringe_rt.h"
#include "syringe/syringe_rt_cxx.h"

namespace __syringe {

std::vector<SimpleInjectionData> GlobalSyringeData;

SimpleInjectionData *findImplPointerImpl(fptr_t target) {
  auto It = std::find_if(GlobalSyringeData.begin(), GlobalSyringeData.end(),
                         [target](SimpleInjectionData It) -> bool {
                           return (void *)It.OrigFunc == (void *)target;
                         });

  if (It == GlobalSyringeData.end()) {
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
              << ", Injection Enabled Addr: " << (void *)item.InjectionEnabled
              << ", Injection Enabled Value: "
              << (*item.InjectionEnabled ? "true" : "false") << std::endl;
  }

  std::cout << std::endl;
}

bool toggleImpl(fptr_t OrigFunc) { return __syringe::toggleImplPtr(OrigFunc); }

void __syringe_register(void *OrigFunc, bool *InjectionEnabled) {
  __syringe::registerInjection((fptr_t)OrigFunc, InjectionEnabled);
}
