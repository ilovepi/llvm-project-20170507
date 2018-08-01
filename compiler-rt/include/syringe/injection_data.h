#ifndef SYRINGE_INJECTION_DATA_H
#define SYRINGE_INJECTION_DATA_H 1

struct InjectionData {
  void *OrigFunc;
  void *StubImpl;
  void *DetourFunc;
  void **ImplPtr;
  template <typename T, typename R>
  InjectionData(T OrigFunction, T StubImplementation, T DetourFunction,
                R ImplPointer)
      : OrigFunc((void *)OrigFunction), StubImpl((void *)StubImplementation),
        DetourFunc((void *)DetourFunction), ImplPtr((void **)ImplPointer) {}
};

#endif // SYRINGE_INJECTION_DATA_H
