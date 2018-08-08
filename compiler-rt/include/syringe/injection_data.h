#ifndef SYRINGE_INJECTION_DATA_H
#define SYRINGE_INJECTION_DATA_H 1

#include <stddef.h>
namespace __syringe {

typedef void (*fptr_t)(void);

struct InjectionData ;
struct mPtrTy {
  ptrdiff_t ptr;
  ptrdiff_t adj;
};

} // namespace __syringe
#endif // SYRINGE_INJECTION_DATA_H
