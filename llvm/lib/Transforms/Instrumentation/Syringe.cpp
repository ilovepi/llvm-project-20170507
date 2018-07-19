#include "llvm/IR/Attributes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

class SyringePass : public ModulePass {
public:
  /// pass identification
  static char ID;

  SyringePass() : ModulePass(ID) {}
  virtual ~SyringePass();

  /// Specify pass name for debug output
  StringRef getPassName() const override { return "Syringe Instrumentation"; }
  bool runOnModule(Module &M) override {
    if (skipModule(M)) {
      return false;
    }
    return doBehaviorInjectionForModule(M);
  }

  bool doBehaviorInjectionForModule(Module &M) {
    bool ret = false;
    for (Function &F : M) {
      if (F.hasFnAttribute(Attribute::SyringeInjectionSite)) {
        ret = true;
        // create global function pointer
        auto globals = M.getGlobalList();
        auto SyringeName = F.getFunctionName() + "_syringe_ptr";
        auto GV =
            GlobalVariable(M, F.getFunctionType(),
                           /*isConstant*/ false, GlobalValue::ExternalLinkage,
                           /*init*/ nullptr, namek,
                           /*insertbefore*/ nullptr, GV.getThreadLocalMode(),
                           GV.getType()->getAddressSpace());

        GlobalValue gv(type, valuety, use, numops, linkage, name, addrsapce);
        globals.emplace_back()
        // clone function
        // replace original body w/ indirect call
      }
    }

    return ret;
  }

private:
  /* data */
};
