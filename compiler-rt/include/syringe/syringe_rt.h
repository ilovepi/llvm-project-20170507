#ifndef SYRINGE_RT_H_
#define SYRINGE_RT_H_ 1

#include "syringe/injection_data.h"

#include <algorithm>
#include <vector>

namespace __syringe {

extern std::vector<InjectionData> global_syringe_data;

template <typename T> InjectionData *FindImplPointer(T orig_func) {
  return &*std::find_if(global_syringe_data.begin(), global_syringe_data.end(),
                        [orig_func](InjectionData it) -> bool {
                          return it.orig_func == orig_func;
                        });
}

template <typename T, typename R>
void RegisterInjection(T orig_func, T stub_impl, T detour_func, R impl_ptr) {
  auto it = FindImplPointer(orig_func);
  if (it == nullptr) {
    global_syringe_data.emplace_back(orig_func, stub_impl, detour_func,
                                     impl_ptr);
  }
}

template <typename T> bool ToggleImpl(T orig_func) {
  auto ptr = FindImplPointer(orig_func);
  if (!ptr) {
    return false;
  }

  if (*(ptr->impl_ptr) == ptr->stub_impl) {
    *(ptr->impl_ptr) = ptr->detour_func;
  } else {
    *(ptr->impl_ptr) = ptr->stub_impl;
  }
  return true;
}

template <typename T> bool ChangeImpl(T orig_func, T new_impl) {
  auto ptr = FindImplPointer(orig_func);
  if (!ptr) {
    return false;
  }

  *(ptr->impl_ptr) = new_impl;
  return true;
}

} // end namespace __syringe

//extern "C" {

//void __syringe_register(void *orig_func, void *stub_impl, void *detour_func,
                        //void **impl_ptr);
//}

#endif /* ifndef SYRINGE_RT_H_ */
