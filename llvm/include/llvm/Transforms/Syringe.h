#ifndef LLVM_TRANSFORMS_SYRINGE_H
#define LLVM_TRANSFORMS_SYRINGE_H

#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

namespace llvm {
class ModulePass;

/// Pass to insert Syringe Injection sites.
class SyringePass : public PassInfoMixin<SyringePass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);
};

ModulePass *createSyringe();

void initializeSyringeLegacyPass(PassRegistry &Registry);

class SyringeLegacyPass : public ModulePass {
public:
  /// pass identification
  static char ID;

  SyringeLegacyPass();
  virtual ~SyringeLegacyPass() = default;

  /// Specify pass name for debug output
  StringRef getPassName() const override;

  /// run module pass
  bool runOnModule(Module &M) override;

  /// create funciton stub for behavior injection
  bool doBehaviorInjectionForModule(Module &M);

private:
  /* data */
};
} // namespace llvm

#endif // Include Guard
