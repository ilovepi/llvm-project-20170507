//===--- TypePromotionInMathFnCheck.h - clang-tidy---------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_PERFORMANCE_TYPE_PROMOTION_IN_MATH_FN_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_PERFORMANCE_TYPE_PROMOTION_IN_MATH_FN_H

#include "../ClangTidy.h"
#include "../utils/IncludeInserter.h"

namespace clang {
namespace tidy {
namespace performance {

/// Finds calls to C math library functions with implicit float to double
/// promotions.
///
/// For example, warns on ::sin(0.f), because this funciton's parameter is a
/// double.  You probably meant to call std::sin(0.f) (in C++), or sinf(0.f) (in
/// C).
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/performance-type-promotion-in-math-fn.html
class TypePromotionInMathFnCheck : public ClangTidyCheck {
public:
  TypePromotionInMathFnCheck(StringRef Name, ClangTidyContext *Context);

  void registerPPCallbacks(CompilerInstance &Compiler) override;
  void storeOptions(ClangTidyOptions::OptionMap &Opts) override;
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  std::unique_ptr<utils::IncludeInserter> IncludeInserter;
  const utils::IncludeSorter::IncludeStyle IncludeStyle;
};

} // namespace performance
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_PERFORMANCE_TYPE_PROMOTION_IN_MATH_FN_H
