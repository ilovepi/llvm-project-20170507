#include "syringe.hpp"
#include "util.hpp"

#include <syringe/syringe_rt.h>

using namespace __syringe;

int main()
{
    SyringeRT s;
    s.RegisterInjection(hello, hello_syringe_impl, injected, _Z17hello_syringe_ptr);
    init(false);
    hello();
    goodbye();

    init(true);
    hello();
    goodbye();

    return 0;
}
