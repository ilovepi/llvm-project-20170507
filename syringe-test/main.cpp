#include "syringe.hpp"
#include "util.hpp"

#include <syringe/syringe_rt.h>

using namespace __syringe;

int main()
{
    //RegisterInjection(hello, hello_syringe_impl, injected, &_Z17hello_syringe_ptr);
    //__syringe_register((void*)hello,(void*) hello_syringe_impl,(void*) injected,(void**) &_Z17hello_syringe_ptr);
    init(false);
    hello();
    goodbye();

    init(true);
    hello();
    goodbye();
    ToggleImpl(hello);
    hello();

    return 0;
}
