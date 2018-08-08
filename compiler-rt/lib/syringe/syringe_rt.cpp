
#include <iostream>
#include <vector>

#include "syringe/injection_data.h"
#include "syringe/syringe_rt.h"

namespace __syringe {

std::vector<InjectionData> GlobalSyringeData;

InjectionData *findImplPointerImpl(fptr_t target) {
  auto It = std::find_if(GlobalSyringeData.begin(), GlobalSyringeData.end(),
                         [target](InjectionData It) -> bool {
                           return (void *)It.OrigFunc == (void *)target;
                         });

  if (It == GlobalSyringeData.end()) {
    return nullptr;
  } else {
    return &*It;
  }
}

void printSyringeData() {
  std::cout << "Syringe Global Data" << std::endl;
  for (auto &item : GlobalSyringeData) {
    std::cout << "Orig Func: " << (void *)item.OrigFunc
              << ", StubImpl: " << (void *)item.StubImpl
              << ", DetourFunc: " << (void *)item.DetourFunc
              << ", ImplPtr: " << (void *)item.ImplPtr << std::endl;
  }

  std::cout << std::endl;
}
} // end namespace __syringe

// extern "C" {

void __syringe_register(void *OrigFunc, void *StubImpl, void *DetourFunc,
                        void **ImplPtr) {
  __syringe::registerInjection((fptr_t)OrigFunc, (fptr_t)StubImpl,
                               (fptr_t)DetourFunc, (fptr_t *)ImplPtr);
}
//}// end extern
