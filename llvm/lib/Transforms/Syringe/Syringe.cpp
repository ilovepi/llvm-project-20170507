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
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ExecutionEngine/Orc/IndirectionUtils.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalAlias.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/PassRegistry.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"

using namespace llvm;

static const char *const kSyringeModuleCtorName = "syringe.module_ctor";
static const char *const kSyringeInitName = "__syringe_register";

static bool doInjectionForModule(Module &M) {
  for (Function &F : M) {
    if (F.hasFnAttribute("SyringePayload") ||
        F.hasFnAttribute("SyringeInjectionSite")) {
      return true;
    }
  }
  return false;
}

PreservedAnalyses SyringePass::run(Module &M, ModuleAnalysisManager &AM) {
  if (!doInjectionForModule(M))
    return PreservedAnalyses::all();

  return PreservedAnalyses::none();
}

/// initializeSyringe - Initialize all passes in the Syringe library.
void initializeSyringeLegacyPass(PassRegistry &Registry) {
  initializeSyringeLegacyPassPass(Registry);
}

/// LLVMInitializeSyringe - C binding for initializeSyringe.
void LLVMInitializeSyringe(LLVMPassRegistryRef R) {
  initializeSyringeLegacyPassPass(*unwrap(R));
}

SyringeLegacyPass::SyringeLegacyPass() : ModulePass(ID) {}

/// Specify pass name for debug output
StringRef SyringeLegacyPass::getPassName() const {
  return "Syringe Instrumentation";
}

/// run module pass
bool SyringeLegacyPass::runOnModule(Module &M) {
  if (skipModule(M)) {
    return false;
  }
  return doBehaviorInjectionForModule(M);
}

/// create funciton stub for behavior injection
bool SyringeLegacyPass::doBehaviorInjectionForModule(Module &M) {
  // errs() << "Running Behavior Injection Pass\n";
  bool ret = false;

  Function *target = nullptr;
  Function *stub = nullptr;
  GlobalValue *ptr_syringe = nullptr;
  Function *detour = nullptr;

  for (Function &F : M) {
    ret = true;
    // if (F.hasFnAttribute(Attribute::SyringePayload)) {
    if (F.hasFnAttribute("SyringePayload")) {
      //errs() << "Found Syringe Payload\n";

      if (F.hasFnAttribute("SyringeTargetFunction")) {
         auto targetName = F.getFnAttribute("SyringeTargetFunction").getValueAsString();

      // create alias
      auto alias = GlobalAlias::create(targetName + "$detour_impl", &F);
      alias->setVisibility(GlobalValue::VisibilityTypes::DefaultVisibility);
      detour = &F;
      }

    }
  }

  for (Function &F : M) {
    // if (F.hasFnAttribute(Attribute::SyringeInjectionSite)) {
    if (F.hasFnAttribute("SyringeInjectionSite")) {
      // errs() << "Found Syringe Injection Site\n";
      ret = true;
      ValueToValueMapTy VMap;

      // clone function
      auto *cloneDecl = orc::cloneFunctionDecl(M, F, &VMap);
      auto aliasName = F.getName().str() + "$detour_impl";
      errs() << aliasName << "\n";

      auto *internalAlias = M.getNamedAlias(aliasName);

      if (internalAlias == nullptr) {
        auto aliasDecl = orc::cloneFunctionDecl(M, F, nullptr);
        aliasDecl->setName(aliasName);
        aliasDecl->removeFnAttr("SyringeInjectionSite");
        aliasDecl->setLinkage(GlobalValue::LinkageTypes::ExternalLinkage);
      }

       cloneDecl->setName(F.getName() + "$syringe_impl");
      //cloneDecl->setName("_Z18hello_syringe_implv");
      orc::moveFunctionBody(F, VMap, nullptr, cloneDecl);
      cloneDecl->removeFnAttr("SyringeInjectionSite");
      stub = cloneDecl;

      // create impl pointer
      auto SyringePtr = orc::createImplPointer(
          *F.getType(), M, F.getName() + "$syringe_ptr", cloneDecl);
      SyringePtr->setVisibility(GlobalValue::DefaultVisibility);
      target = &F;
      ptr_syringe = SyringePtr;

      // create stub body for original call
      orc::makeStub(F, *SyringePtr);
    }
  }

  // if we've made a modification, add a global ctor entry for the function
  if (ret && target && ptr_syringe) {

    // target function types
    auto funcTy = target->getType();

    // the parameter types of the registration function
    Type *ParamTypes[] = {funcTy, funcTy, funcTy, ptr_syringe->getType()};
    auto ParamTypesRef = makeArrayRef(ParamTypes, 4);

    // actual parameters
    Value *ParamArgs[] = {target, stub, detour, ptr_syringe};
    auto ParamArgsRef = makeArrayRef(ParamArgs, 4);

    // void return type
    auto retTy = Type::getVoidTy(M.getContext());

    // the type of the registration function
    auto fTy = FunctionType::get(retTy, ParamTypesRef, false);

    // create or find the registration function
    auto constF = M.getOrInsertFunction(kSyringeInitName, fTy);
    auto regFunc = M.getFunction(kSyringeInitName);

    // set its linkage
    regFunc->setLinkage(GlobalValue::LinkageTypes::ExternalLinkage);

    // create a ctor
    Function *Ctor = Function::Create(
        FunctionType::get(Type::getVoidTy(M.getContext()), false),
        GlobalValue::InternalLinkage, kSyringeModuleCtorName, &M);

    // give it a body and have it call the registration function w/ our target
    // arguments
    BasicBlock *CtorBB = BasicBlock::Create(M.getContext(), "", Ctor);
    IRBuilder<> IRB(ReturnInst::Create(M.getContext(), CtorBB));
    IRB.CreateCall(constF, ParamArgsRef);

    // Ctor->setLinkage(GlobalValue::LinkageTypes::ExternalLinkage);
    // Ctor->setComdat(M.getOrInsertComdat(kSyringeModuleCtorName));
    appendToGlobalCtors(M, Ctor, 65535);
    // errs() << M;
  }

  return ret;
}

ModulePass *llvm::createSyringe() { return new SyringeLegacyPass(); }
char SyringeLegacyPass::ID;

INITIALIZE_PASS(SyringeLegacyPass, "syringe",
                "Syringe: dynamic behavior injection.", false, false)
