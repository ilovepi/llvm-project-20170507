#ifndef _SYRINGE_HPP_
#define _SYRINGE_HPP_

[[clang::syringe_injection_site]] void hello();

[[clang::syringe_payload("_Z5hellov")]] void injected();

#endif
