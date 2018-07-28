
#include <vector>

#include "syringe/injection_data.h"
#include "syringe/syringe_rt.h"

namespace __syringe {

std::vector<InjectionData> global_syringe_data;

} // end namespace __syringe

//extern "C" {

void __syringe_register(void *orig_func, void *stub_impl, void *detour_func,
                        void **impl_ptr) {
    __syringe::RegisterInjection(orig_func, stub_impl, detour_func, impl_ptr);
}
//}// end extern
