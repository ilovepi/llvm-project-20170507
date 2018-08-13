//===--- SyrigneLists.h - Syrigne automatic attribution ---------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// User-provided filters for always/never Syrigne instrumenting certain functions.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_CLANG_BASIC_SYRIGNELISTS_H
#define LLVM_CLANG_BASIC_SYRIGNELISTS_H

#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SpecialCaseList.h"
#include <memory>

namespace clang {

class SyringeFunctionFilter {
  std::unique_ptr<llvm::SpecialCaseList> InjectionSite;
  std::unique_ptr<llvm::SpecialCaseList> Payload;
  std::unique_ptr<llvm::SpecialCaseList> AttrList;
  SourceManager &SM;

public:
  SyringeFunctionFilter(ArrayRef<std::string> InjectionSitePaths,
                     ArrayRef<std::string> PayloadPaths,
                     ArrayRef<std::string> AttrListPaths, SourceManager &SM);

  enum class ImbueAttribute {
    NONE,
    INJECTION_SITE,
    PAYLOAD,
  };

  ImbueAttribute shouldImbueFunction(StringRef FunctionName) const;

  ImbueAttribute
  shouldImbueFunctionsInFile(StringRef Filename,
                             StringRef Category = StringRef()) const;

  ImbueAttribute shouldImbueLocation(SourceLocation Loc,
                                     StringRef Category = StringRef()) const;
};

} // namespace clang

#endif
