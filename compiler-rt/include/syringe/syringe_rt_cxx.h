//===- syringe_rt_cxx.h -----------------------------------------*- C++ -*-===//
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

#ifndef SYRINGE_SYRINGE_RT_CXX_H
#define SYRINGE_SYRINGE_RT_CXX_H 1

#include "syringe/injection_data.h"

#include <algorithm>
#include <cstdint>
#include <type_traits>
#include <vector>

namespace __syringe {

/// A container for the Syringe runtime data
extern std::vector<SimpleInjectionData> GlobalSyringeData;

/// Use a untion to effectivly cast between member function pointers
/// and normal funciton pointers.
/// On Itanium a non-virutal member function pointer is the first 8 bytes
/// of the member function pointer. We can (ab)use that to get its address.
template <typename T> fptr_t convertMemberPtr(T foo) {
  union {
    T mfunc;
    fptr_t addr;
  };
  mfunc = foo;
  return addr;
}

/// Look up the metadata associated with target
/// @target pointer to the target injection site
SimpleInjectionData *findImplPointerImpl(fptr_t target);

/// look up the memtadata for OrigFunc
/// @OrigFunc pointer to the target injection site
/// We use a dirty hack with a union to enable casting between
/// pointer types (like class method pointers) that are normally not allowed.
template <typename T> SimpleInjectionData *findImplPointer(T OrigFunc) {
  fptr_t target;
  target = convertMemberPtr(OrigFunc);

  return findImplPointerImpl(target);
}

/// register the metadata about OrigFunc in our global metadata
/// Creates a tuple binding the address of OrigFunc to its injeciton flag
/// @OrigFunc pointer to the target function
/// @InjectionEnabled address the OrigFunc's syringe boolean
template <typename T>
void registerInjection(T OrigFunc, bool *InjectionEnabled) {
  assert(findImplPointer(OrigFunc) == nullptr &&
         "Cannot register two payloads for the same injection site!");
  GlobalSyringeData.emplace_back(OrigFunc, InjectionEnabled);
}

/// Changes if the injected behavior is active or inactive
/// @OrigFunc pointer to the Syringe Site
/// @return true if state changed, false when pointer not found
template <typename T> bool toggleImplPtr(T OrigFunc) {
  auto Ptr = findImplPointer(OrigFunc);
  if (!Ptr) {
    return false;
  }

  *Ptr->InjectionEnabled = !(*Ptr->InjectionEnabled);
  return true;
}

/// Toggle virutal function implementation
/// Currently only works for Itanium ABI
/// Relies on horrible ABI hacks to get the address of virtual methods
/// @OrigFunc pointer to the member function
/// @Instance address of an instance of the class in question
template <typename T, typename R>
bool toggleVirtualImpl(T OrigFunc, R Instance) {
  // use char * to represent vtbl address for easy indexing
  typedef char *vtblptr_t;

  // convert the OrigFunc to a pointer to a member function pointer type
  mPtrTy *MemberFnPtrPtr = (mPtrTy *)&OrigFunc;

  // get a pointer to the vtbl pointer for the passed in instance
  vtblptr_t *PtrToVtblPtr = (vtblptr_t *)Instance;

  // on itanium the vtable pointer is the at the instance's address
  auto VtblPtr = *PtrToVtblPtr;

  // TODO: do we need the adjustment?
  // calculate the offset of into the vtable for the target function
  auto FnOffset = MemberFnPtrPtr->ptr + MemberFnPtrPtr->adj - 1;
  auto TargetAddr =
      static_cast<void **>(static_cast<void *>(VtblPtr + FnOffset));

  return toggleImplPtr(*TargetAddr);
}

/// Gets the address of a member function
template <typename T> fptr_t lookupMemberFunctionAddr(T FPtr) {
  assert(FPtr != nullptr &&
         "Syringe Member Function Lookup was passed a nullptr!");
  mPtrTy *j = (mPtrTy *)FPtr;
  return (fptr_t)(j->ptr);
}

} // end namespace __syringe

#endif // SYRINGE_SYRINGE_RT_H
