#ifndef SYRINGE_SYRINGE_RT_H
#define SYRINGE_SYRINGE_RT_H 1

#include "syringe/injection_data.h"

#include <algorithm>
#include <cstdint>
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

/// Toggle virutal function implementation
/// Currently only works for Itanium ABI
template <typename T, typename R>
bool toggleVirtualImpl(T OrigFunc, R Instance) {
  // use char * to represent vtbl address for easy indexing
  typedef char *vtblptr_t;

  // convert the OrigFunc to a member function pointer type
  mPtrTy *MemberFnPtr = (mPtrTy *)&OrigFunc;

  // get the vtbl for the passed in instance
  vtblptr_t *PtrToVtblPtr = (vtblptr_t *)Instance;
  // on itanium the vtable pointer is the at the instance's address
  auto VtblPtr = *PtrToVtblPtr;

  // TODO: do we need the adjustment?
  // calculate the offset of into the vtable for the target function
  auto FnOffset = MemberFnPtr->ptr + MemberFnPtr->adj - 1;
  auto TargetAddr =
      static_cast<void **>(static_cast<void *>(VtblPtr + FnOffset));

  auto Ptr = findImplPointer(*TargetAddr);
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
