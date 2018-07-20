#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"

namespace llvm {
class ModulePass;

void initializeSyringe(PassRegistry &Registry);

class Syringe : public ModulePass {
public:
  /// pass identification
  static char ID;

  Syringe();
  virtual ~Syringe() = default;

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
