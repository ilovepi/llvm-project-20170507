#ifndef SYRINGE_RT_H_
#define SYRINGE_RT_H_ 1

#include <algorithm>
#include <vector>

namespace __syringe {

struct InjectionData {
  void *orig_func;
  void *stub_impl;
  void *detour_func;
  void *impl_ptr;
  template <typename T>
  InjectionData(T *orig_func, T *stub_impl, T *detour_func, T *impl_ptr)
      : orig_func(orig_func), stub_impl(stub_impl), detour_func(detour_func),
        impl_ptr(impl_ptr) {}
};

class SyringeRT {
public:
  SyringeRT() = default;
  virtual ~SyringeRT() = default;
  std::vector<InjectionData> data;
  template <typename T> bool ToggleImpl(T *orig_func);
  template <typename T> bool ChangeImpl(T *orig_func, T *new_impl);

  template <typename T>
  void RegisterInjection(T *orig_func, T *stub_impl, T *detour_func,
                         T *impl_ptr);

  template <typename T> InjectionData *FindImplPointer(T *orig_func);

private:
};

} // end namespace __syringe
#endif /* ifndef SYRINGE_RT_H_ */
