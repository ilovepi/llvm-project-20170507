#include "syringe.hpp"

#include <iostream>
#include <syringe/syringe_rt.h>
#include <cassert>

using namespace __syringe;

int injected_count = 0;
int hello_count = 0;

int main()
{
    // ensure that Syringe metadata is initialized
    assert(!__syringe::GlobalSyringeData.empty());
    //assert(__syringe::GlobalSyringeData.size() >1);

    hello();
    assert( hello_count == 1 && "Hello Count incorrect");
    toggleImpl(hello);
    hello();
    assert( injected_count == 1 && "Hello Count incorrect");
    toggleImpl(hello);
    hello();
    assert( hello_count == 2 && "Hello Count incorrect");

    //std::cout <<"hello addr:" << (void*)hello <<std::endl;
    //printSyringeData();

    SyringeBase b;
    std::cout << b.counter <<std::endl;
    //assert(b.counter == 0);
    assert(b.getCounter() == b.counter);
    b.increment();

    assert(b.counter == 1);
    assert(b.getCounter() == b.counter);

    toggleImpl(&SyringeBase::increment);
    b.increment();
    b.increment();

    assert(b.counter == 1);
    assert(b.other_counter == 2);
    assert(b.getCounter() == b.counter);
    assert(b.getCounter() != b.other_counter);


    return 0;
}
