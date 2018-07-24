#include "llvm/syringe_rt.h"

  std::vector<InjectionData> data;
  template <typename T> bool SyringeRT::ToggleImpl(T *orig_func) {
    auto ptr = FindImplPointer(orig_func);
    if (!ptr) {
      return false;
    }

    if (ptr->impl_ptr == ptr->stub_impl) {
      ptr->impl_ptr = ptr->detour_func;
    } else {
      ptr->impl = ptr->stub_impl;
    }
    return true;
  }

  template <typename T> bool SyringRT::ChangeImpl(T *orig_func, T *new_impl) {
    auto ptr = FindImplPointer(orig_func);
    if (!ptr) {
      return false;
    }

    ptr->impl_ptr = new_impl;
    return true;
  }

  template <typename T>
  void SyringRT::RegisterInjection(T *orig_func, T *stub_impl, T *detour_func, T *impl_ptr) {
    data.emplace_back(orig_func, stub_impl, detour_func, impl_ptr);
  }

  template <typename T> InjectionData *SyringRT::FindImplPointer(T *orig_func) {
    std::find_if(data.begin(), data.end(),
                 [orig_func](InjectionData it) -> bool {
                   return it.orig_func == orig_func;
                 });
