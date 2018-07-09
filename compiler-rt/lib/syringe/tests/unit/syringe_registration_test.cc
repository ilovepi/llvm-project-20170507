//===-- syringe_registration_test.cc
//-----------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is a part of Syringe, a behavior injection system.
//
//===----------------------------------------------------------------------===//
#include "gtest/gtest.h"

#include "syringe/syringe_rt.h"
#include "syringe/syringe_rt_cxx.h"
namespace {
[[clang::syringe_injeciton_site]] int foo() { return 1; }
[[clang::syringe_payload("_Z3fooi")]] int bar() { return 0; }
} // namespace

namespace __syringe {

TEST(RegistrationTest, Simple) {
  __syringe_register(nullptr, nullptr);
  auto data_ptr = findImplPtr(nullptr);
  ASSERT_NE(nullptr, data_ptr);
  ASSERT_EQ(nullptr, data_ptr->ImplPtr);
  ASSERT_GT(GlobalSyringeData.size(), 1);
}

TEST(RegistrationTest, NoDoubleInsert) {
  __syringe_register(nullptr, nullptr);
  ASSERT_DEATH(__syringe_register(nullptr, nullptr), "Cannot register two payloads for the same injection site!");
}

} // namespace __syringe
