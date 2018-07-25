#ifndef INJECTION_DATA_H_
#define INJECTION_DATA_H_ 1

struct InjectionData {
  void *orig_func;
  void *stub_impl;
  void *detour_func;
  void **impl_ptr;
  template <typename T, typename R>
  InjectionData(T orig_func, T stub_impl, T detour_func, R impl_ptr)
      : orig_func((void *)orig_func), stub_impl((void *)stub_impl),
        detour_func((void *)detour_func), impl_ptr((void **)impl_ptr) {}
};

#endif
