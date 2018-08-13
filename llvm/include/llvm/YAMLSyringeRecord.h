//===- YAMLXRayRecord.h - XRay Record YAML Support Definitions ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Types and traits specialisations for YAML I/O of XRay log entries.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_SYRINGE_YAML_SYRINGE_RECORD_H
#define LLVM_SYRINGE_YAML_SYRINGE_RECORD_H

#include <type_traits>

#include "llvm/Support/YAMLTraits.h"
#include "llvm/SyringeRecord.h"

namespace llvm {
namespace syringe {

struct YAMLSyringeTarget {
  std::string Name;
  SyringeTrigger EnableTrigger;
  SyringeTrigger DisableTrigger;
  uint32_t Count = 0;
};

struct YAMLSyringePayload {
  std::string Name;
  std::string Target;
};

struct YAMLSyringeRecord {
  std::string Filename;
  std::vector<YAMLSyringeTarget> Targets;
  std::vector<YAMLSyringePayload> Payloads;
};

} // namespace syringe

namespace yaml {

template <typename T> struct MappingTraits;

// YAML Traits
// -----------
template <> struct ScalarEnumerationTraits<syringe::SyringeTrigger> {
  static void enumeration(IO &IO, syringe::SyringeTrigger &Type) {
    IO.enumCase(Type, "NONE", syringe::SyringeTrigger::NONE);
    IO.enumCase(Type, "NEVER", syringe::SyringeTrigger::NEVER);
    IO.enumCase(Type, "ALWAYS", syringe::SyringeTrigger::ALWAYS);
    IO.enumCase(Type, "COUNT", syringe::SyringeTrigger::COUNT);
    IO.enumCase(Type, "ONCE", syringe::SyringeTrigger::ONCE);
  }
};

template <> struct MappingTraits<syringe::YAMLSyringeTarget> {
  static void mapping(IO &IO, syringe::YAMLSyringeTarget &Target) {
    IO.mapRequired("name", Target.Name);
    IO.mapRequired("enable", Target.EnableTrigger);
    IO.mapRequired("disable", Target.DisableTrigger);
    IO.mapOptional("count", Target.Count);
  }
};

template <> struct MappingTraits<syringe::YAMLSyringePayload> {
  static void mapping(IO &IO, syringe::YAMLSyringePayload &Payload) {
    IO.mapRequired("name", Payload.Name);
    IO.mapRequired("target", Payload.Target);
  }
};

template <> struct MappingTraits<syringe::YAMLSyringeRecord> {
  static void mapping(IO &IO, syringe::YAMLSyringeRecord &Record) {
    IO.mapRequired("filename", Record.Filename);
    IO.mapOptional("targets", Record.Targets);
    IO.mapOptional("payloads", Record.Payloads);
  }
};

} // namespace yaml
} // namespace llvm

LLVM_YAML_IS_SEQUENCE_VECTOR(syringe::YAMLSyringeTarget)
LLVM_YAML_IS_SEQUENCE_VECTOR(syringe::YAMLSyringePayload)
LLVM_YAML_IS_SEQUENCE_VECTOR(syringe::YAMLSyringeRecord)

#endif // LLVM_syringe_YAML_syringe_RECORD_H
