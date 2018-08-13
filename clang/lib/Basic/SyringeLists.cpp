//===--- SyringeFunctionFilter.cpp - Syringe automatic-attribution --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// User-provided filters for always/never Syringe instrumenting certain functions.
//
//===----------------------------------------------------------------------===//
#include "clang/Basic/SyringeLists.h"

using namespace clang;

SyringeFunctionFilter::SyringeFunctionFilter(
    ArrayRef<std::string> InjectionSitePaths,
    ArrayRef<std::string> PayloadPaths,
    ArrayRef<std::string> AttrListPaths, SourceManager &SM)
    : InjectionSite(
          llvm::SpecialCaseList::createOrDie(InjectionSitePaths)),
      Payload(llvm::SpecialCaseList::createOrDie(PayloadPaths)),
      AttrList(llvm::SpecialCaseList::createOrDie(AttrListPaths)), SM(SM) {}

SyringeFunctionFilter::ImbueAttribute
SyringeFunctionFilter::shouldImbueFunction(StringRef FunctionName) const {
  // First apply the always instrument list, than if it isn't an "always" see
  // whether it's treated as a "never" instrument function.
  // TODO: Remove these as they're deprecated; use the AttrList exclusively.
  if (InjectionSite->inSection("syringe_injection_site", "fun", FunctionName) ||
      AttrList->inSection("syringe_target", "fun", FunctionName))
    return ImbueAttribute::INJECTION_SITE;

  if (Payload->inSection("syringe_payload", "fun", FunctionName) ||
      AttrList->inSection("syringe_payload", "fun", FunctionName))
    return ImbueAttribute::PAYLOAD;

  return ImbueAttribute::NONE;
}

SyringeFunctionFilter::ImbueAttribute
SyringeFunctionFilter::shouldImbueFunctionsInFile(StringRef Filename,
                                                  StringRef Category) const {
  if (InjectionSite->inSection("syringe_injection_site", "src", Filename,
                               Category) ||
      AttrList->inSection("syringe_target", "src", Filename, Category))
    return ImbueAttribute::INJECTION_SITE;
  if (Payload->inSection("syringe_payload", "src", Filename, Category) ||
      AttrList->inSection("syringe_payload", "src", Filename, Category))
    return ImbueAttribute::PAYLOAD;
  return ImbueAttribute::NONE;
}

SyringeFunctionFilter::ImbueAttribute
SyringeFunctionFilter::shouldImbueLocation(SourceLocation Loc,
                                        StringRef Category) const {
  if (!Loc.isValid())
    return ImbueAttribute::NONE;
  return this->shouldImbueFunctionsInFile(SM.getFilename(SM.getFileLoc(Loc)),
                                          Category);
}
