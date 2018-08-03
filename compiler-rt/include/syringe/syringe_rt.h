#ifndef SYRINGE_SYRINGE_RT_H
#define SYRINGE_SYRINGE_RT_H 1

#include "syringe/injection_data.h"

#include <algorithm>
#include <type_traits>
#include <vector>

namespace __syringe {

extern std::vector<InjectionData> GlobalSyringeData;

template <typename T> fptr_t convertMemberPtr(T foo) {
  union {
    T mfunc;
    fptr_t addr;
  };
  mfunc = foo;
  return addr;
}

InjectionData *findImplPointerImpl(fptr_t target);

template <typename T> InjectionData *findImplPointer(T OrigFunc) {
  fptr_t target;
  target = convertMemberPtr(OrigFunc);

  return findImplPointerImpl(target);
}

template <typename T, typename R>
void registerInjection(T OrigFunc, T StubImpl, T DetourFunc, R ImplPtr) {
  assert(findImplPointer(OrigFunc) == nullptr &&
         "Cannot register two payloads for the same injection site!");
  GlobalSyringeData.emplace_back(OrigFunc, StubImpl, DetourFunc, ImplPtr);
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

template <typename T, typename R> bool toggleVirtualImpl(T OrigFunc, R instance) {
//fptr_t target;
  //target = convertMemberPtr(OrigFunc);


  //auto ptrd = (ptrdiff_t)target;
  mPtrTy* myPtr = (mPtrTy*)&OrigFunc;
  char** crazy = (char**)instance;
  auto vtbl = *crazy;

  printf("%p\n", vtbl);

  auto Ptr = findImplPointer(*(void**)(vtbl + myPtr->ptr + myPtr->adj -1));
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

template <typename T> fptr_t lookupMemberFunctionAddr(T FPtr) {
  // fptr_t ret = nullptr;
  // if(std::is_member_function_pointer<T>::value)
  //{
  // check if function is virtual
  mPtrTy *j = (mPtrTy *)FPtr;
  return (fptr_t)(j->ptr);

  // handle non virtual funcitons

  //}

  // return ret;
}

void printSyringeData();

} // end namespace __syringe

extern "C" {

void __syringe_register(void *OrigFunc, void *StubImpl, void *DetourFunc,
                        void **ImplPtr);
}

#endif // SYRINGE_SYRINGE_RT_H
