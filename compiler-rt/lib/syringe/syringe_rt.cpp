
#include <vector>

#include "syringe/injection_data.h"
#include "syringe/syringe_rt.h"

namespace __syringe {

std::vector<InjectionData> GlobalSyringeData;

} // end namespace __syringe

//extern "C" {

void __syringe_register(void *OrigFunc, void *StubImpl, void *DetourFunc,
                        void **ImplPtr) {
    __syringe::registerInjection(OrigFunc, StubImpl, DetourFunc, ImplPtr);
}
//}// end extern
