//===- SyringeRecord.h - Syringe Trace Record -----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file replicates the record definition for Syringe instrumentation.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_SYRINGE_SYRINGE_RECORD_H
#define LLVM_SYRINGE_SYRINGE_RECORD_H

#include <cstdint>
#include <vector>
#include <string>

namespace llvm {
namespace syringe {


enum class SyringeTrigger {NONE, NEVER, ALWAYS, COUNT};


struct SyringeTarget
{
  std::string Name;
  SyringeTrigger EnableTrigger;
  SyringeTrigger DisableTrigger;
  uint32_t Count=0;
};

struct SyringePayload{
  std::string Name;
  std::string Target;
};


struct SyringeRecord {
  std::string Filename;
  std::vector<SyringeTarget> Targets;
  std::vector<SyringePayload> Payloads;
};

} // namespace syringe
} // namespace llvm

#endif // LLVM_SYRINGE_SYRINGE_RECORD_H
