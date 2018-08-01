#ifndef SYRINGE_SYRINGE_RT_H
#define SYRINGE_SYRINGE_RT_H 1

#include "syringe/injection_data.h"

#include <algorithm>
#include <vector>

namespace __syringe {

extern std::vector<InjectionData> GlobalSyringeData;

template <typename T> InjectionData *findImplPointer(T OrigFunc) {
  return &*std::find_if(
      GlobalSyringeData.begin(), GlobalSyringeData.end(),
      [OrigFunc](InjectionData It) -> bool { return It.OrigFunc == OrigFunc; });
}

template <typename T, typename R>
void registerInjection(T OrigFunc, T StubImpl, T DetourFunc, R ImplPtr) {
  auto It = findImplPointer(OrigFunc);
  if (It == nullptr) {
    GlobalSyringeData.emplace_back(OrigFunc, StubImpl, DetourFunc, ImplPtr);
  }
}

template <typename T> bool toggleImpl(T OrigFunc) {
  auto Ptr = findImplPointer(OrigFunc);
  if (!Ptr) {
    return false;
  }

  if (*(Ptr->ImplPtr) == Ptr->StubImpl) {
    *(Ptr->ImplPtr) = Ptr->DetourFunc;
  } else {
    *(Ptr->ImplPtr) = Ptr->StubImpl;
  }
  return true;
}

template <typename T> bool changeImpl(T OrigFunc, T NewImpl) {
  auto Ptr = findImplPointer(OrigFunc);
  if (!Ptr) {
    return false;
  }

  *(Ptr->ImplPtr) = NewImpl;
  return true;
}

} // end namespace __syringe

extern "C" {

void __syringe_register(void *OrigFunc, void *StubImpl, void *DetourFunc,
                        void **ImplPtr);
}

#endif // SYRINGE_SYRINGE_RT_H
