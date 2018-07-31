#include "syringe.hpp"
#include "util.hpp"

#include <syringe/syringe_rt.h>
#include <cassert>

using namespace __syringe;

int main()
{
    assert(!__syringe::global_syringe_data.empty());
    //RegisterInjection(hello, hello_syringe_impl, injected, &_Z17hello_syringe_ptr);
    //__syringe_register((void*)hello,(void*) hello_syringe_impl,(void*) injected,(void**) &_Z17hello_syringe_ptr);
    //init(false);
    hello();
    //goodbye();

    //init(true);
    ToggleImpl(hello);
    hello();
    //goodbye();
    ToggleImpl(hello);
    hello();

    return 0;
}
