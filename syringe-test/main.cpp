#include "syringe.hpp"

#include <syringe/syringe_rt.h>
#include <cassert>

using namespace __syringe;

int injected_count = 0;
int hello_count = 0;

int main()
{
    // ensure that Syringe metadata is initialized
    assert(!__syringe::GlobalSyringeData.empty());

    hello();
    assert( hello_count == 1 && "Hello Count incorrect");
    toggleImpl(hello);
    hello();
    assert( injected_count == 1 && "Hello Count incorrect");
    toggleImpl(hello);
    hello();
    assert( hello_count == 2 && "Hello Count incorrect");

    return 0;
}
