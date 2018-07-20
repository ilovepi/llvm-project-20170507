//===-- Syringe.cpp - Syringe Infrastructure ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the common initialization infrastructure for the
// Syringe library.
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Syringe.h"
#include "llvm-c/Initialization.h"
#include "llvm/InitializePasses.h"
#include "llvm/PassRegistry.h"


#include "llvm/ExecutionEngine/Orc/IndirectionUtils.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"


using namespace llvm;

/// initializeSyringe - Initialize all passes in the Syringe
/// library.
void initializeSyringe(PassRegistry &Registry) {
  initializeSyringePass(Registry);
}

/// LLVMInitializeSyringe - C binding for
/// initializeSyringe.
void LLVMInitializeSyringe(LLVMPassRegistryRef R) {
  initializeSyringePass(*unwrap(R));
}


Syringe::Syringe() : ModulePass(ID) {}

/// Specify pass name for debug output
StringRef Syringe::getPassName() const { return "Syringe Instrumentation"; }

/// run module pass
bool Syringe::runOnModule(Module &M) {
  if (skipModule(M)) {
    return false;
  }
  return doBehaviorInjectionForModule(M);
}

/// create funciton stub for behavior injection
bool Syringe::doBehaviorInjectionForModule(Module &M) {
  errs() << "Running Behavior Injection Pass\n";
  bool ret = false;
  for (Function &F : M) {
    if (F.hasFnAttribute(Attribute::SyringeInjectionSite)) {
      ret = true;

      // clone function
      // TODO: Decide if this nullptr should be a VMap
      auto cloneDecl = orc::cloneFunctionDecl(M, F, nullptr);
      cloneDecl->setName(F.getName() + "_syringe_impl");

      // create impl pointer
      // orc::moveFunctionBody(F, cloneDecl, );
      auto SyringePtr = orc::createImplPointer(
          *F.getType(), M, F.getName() + "_syringe_ptr", nullptr);

      // create stub body for original call
      orc::makeStub(F, *SyringePtr);

      // replace original body w/ indirect call
    }
  }

  return ret;
}

ModulePass *createSyringe() { return new Syringe(); }
char Syringe::ID;

INITIALIZE_PASS(Syringe, "syringe",
                "Syringe: dynamic behavior injection.", false, false)
