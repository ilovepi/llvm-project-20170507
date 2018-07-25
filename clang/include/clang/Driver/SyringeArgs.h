//===--- SyringeArgs.h - Arguments for Syringe ------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_CLANG_DRIVER_SYRINGEARGS_H
#define LLVM_CLANG_DRIVER_SYRINGEARGS_H

#include "clang/Driver/Types.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"

namespace clang {
namespace driver {

class ToolChain;

class SyringeArgs {
  bool SyringeInject = false;
  bool SyringeRT = true;

public:
  /// Parses the Syringe arguments from an argument list.
  SyringeArgs(const ToolChain &TC, const llvm::opt::ArgList &Args);
  void addArgs(const ToolChain &TC, const llvm::opt::ArgList &Args,
               llvm::opt::ArgStringList &CmdArgs, types::ID InputType) const;

  bool needsSyringeRt() const { return SyringeInject && SyringeRT; }
};

} // namespace driver
} // namespace clang

#endif // LLVM_CLANG_DRIVER_SYRINGEARGS_H
