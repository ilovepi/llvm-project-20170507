#ifndef LLVM_TRANSFORMS_IPO_H
#define LLVM_TRANSFORMS_IPO_H

#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"

namespace llvm {
class ModulePass;

void initializeSyringe(PassRegistry &Registry);

class SyringePass : public ModulePass {
public:
  /// pass identification
  static char ID;

  SyringePass();
  virtual ~SyringePass() = default;

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
