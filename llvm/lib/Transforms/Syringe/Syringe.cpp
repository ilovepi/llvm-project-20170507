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
#include "llvm/ADT/SmallVector.h"
#include "llvm/ExecutionEngine/Orc/IndirectionUtils.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalAlias.h"
#include "llvm/IR/GlobalIFunc.h"
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

static const char *const SyringeModuleCtorName = "syringe.module_ctor";
static const char *const SyringeInitName = "__syringe_register";
static const char *const SyringeStubImplSuffix = "$syringe_impl";
static const char *const SyringeDetourImplSuffix = "$detour_impl";
static const char *const SyringeImplPtrSuffix = "$syringe_impl_ptr";

namespace {

struct SyringeInitData {
  Function *Target;
  Function *Stub;
  Function *Detour;
  GlobalValue *SyringePtr;
};

} // namespace

// create a single ctor function for the module, whose body should consist of a
// call to each of the registered syringe functions.
// We create the function, add a basic block to it, and then insert calls to the
// registration functions
//
// example ctor function:
//
// void ctor_func_name(){
//      register(original_func, stub_impl, detour_impl, impl_ptr);
//      register(original_func2, stub_impl2, detour_impl2, impl_ptr2);
//      ....
// }
//
static void createCtorInit(Module &M, SmallVector<SyringeInitData, 8> &InitData) {

  // create a ctor
  Function *Ctor = Function::Create(
      FunctionType::get(Type::getVoidTy(M.getContext()), false),
      GlobalValue::InternalLinkage, SyringeModuleCtorName + M.getName() , &M);
  BasicBlock *CtorBB = BasicBlock::Create(M.getContext(), "", Ctor);
  IRBuilder<> IRB(ReturnInst::Create(M.getContext(), CtorBB));

  for (auto SID : InitData) {
    auto Target = SID.Target;
    auto Stub = SID.Stub;
    auto Detour = SID.Detour;
    auto SyringePtr = SID.SyringePtr;

    // target function types
    auto FuncTy = Target->getType();

    // the parameter types of the registration function
    Type *ParamTypes[] = {FuncTy, FuncTy, FuncTy, SyringePtr->getType()};
    auto ParamTypesRef = makeArrayRef(ParamTypes, 4);

    // actual parameters
    Value *ParamArgs[] = {Target, Stub, Detour, SyringePtr};
    auto ParamArgsRef = makeArrayRef(ParamArgs, 4);

    // void return type
    auto RetTy = Type::getVoidTy(M.getContext());

    // the type of the registration function
    auto RegFnTy = FunctionType::get(RetTy, ParamTypesRef, false);

    // create or find the registration function
    auto ConstFn = M.getOrInsertFunction(SyringeInitName, RegFnTy);
    auto RegFunc = M.getFunction(SyringeInitName);

    // set its linkage
    RegFunc->setLinkage(GlobalValue::LinkageTypes::ExternalLinkage);
    // give it a body and have it call the registration function w/ our target
    // arguments
    IRB.CreateCall(ConstFn, ParamArgsRef);
  }

  // Ctor->setLinkage(GlobalValue::LinkageTypes::ExternalLinkage);
  // Ctor->setComdat(M.getOrInsertComdat(kSyringeModuleCtorName));
  appendToGlobalCtors(M, Ctor, 65535);
  // errs() << M;
}

static std::string createStubNameFromBase(StringRef BaseName) {
  return (BaseName + SyringeStubImplSuffix).str();
}

static std::string createImplPtrNameFromBase(StringRef BaseName) {
  return (BaseName + SyringeImplPtrSuffix).str();
}

static std::string createAliasNameFromBase(StringRef BaseName) {
  return (BaseName + SyringeDetourImplSuffix).str();
}

// check if the module needs the Syringe Pass
static bool doInjectionForModule(Module &M) {
  for (Function &F : M) {
    if (F.hasFnAttribute("SyringePayload") ||
        F.hasFnAttribute("SyringeInjectionSite")) {
      return true;
    }
  }
  return false;
}

// run the syringe pass
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
  bool ChangedPayload = false;
  bool ChangedInjection = false;
  SmallVector<SyringeInitData, 8> InitData;

  for (Function &F : M) {
    ChangedPayload = true;
    if (F.hasFnAttribute("SyringePayload")) {
      // errs() << "Found Syringe Payload\n";

      if (F.hasFnAttribute("SyringeTargetFunction")) {
        auto TargetName =
            F.getFnAttribute("SyringeTargetFunction").getValueAsString();

        // create alias
        auto NewAlias =
            GlobalAlias::create(createAliasNameFromBase(TargetName), &F);
        NewAlias->setVisibility(
            GlobalValue::VisibilityTypes::DefaultVisibility);
      }
    }
  }

  for (Function &F : M) {
    if (F.hasFnAttribute("SyringeInjectionSite")) {
      // errs() << "Found Syringe Injection Site\n";
      ChangedPayload = true;
      ValueToValueMapTy VMap;

      // clone function
      auto *CloneDecl = orc::cloneFunctionDecl(M, F, &VMap);
      auto AliasName = createAliasNameFromBase(F.getName());
      // errs() << aliasName << "\n";

      Function *DetourFunction;
      auto *InternalAlias = M.getNamedAlias(AliasName);

      if (InternalAlias == nullptr) {
        auto AliasDecl = orc::cloneFunctionDecl(M, F, nullptr);
        AliasDecl->setName(AliasName);
        AliasDecl->removeFnAttr("SyringeInjectionSite");
        AliasDecl->setLinkage(GlobalValue::LinkageTypes::ExternalLinkage);
        DetourFunction = AliasDecl;
      } else {
        DetourFunction = dyn_cast<Function>(InternalAlias->getAliasee());
      }

      CloneDecl->setName(createStubNameFromBase(F.getName()));
      orc::moveFunctionBody(F, VMap, nullptr, CloneDecl);
      CloneDecl->removeFnAttr("SyringeInjectionSite");
      auto Stub = CloneDecl;

      // create impl pointer
      auto SyringePtr = orc::createImplPointer(
          *F.getType(), M, createImplPtrNameFromBase(F.getName()), CloneDecl);
      SyringePtr->setVisibility(GlobalValue::DefaultVisibility);
      auto Target = &F;

      // create stub body for original call
      orc::makeStub(F, *SyringePtr);
      InitData.push_back({Target, Stub, DetourFunction, SyringePtr});
    }
  }

  // if we've made a modification, add a global ctor entry for the function
  if (ChangedPayload) {
    createCtorInit(M, InitData);
  }

  return ChangedPayload || ChangedInjection;
}

ModulePass *llvm::createSyringe() { return new SyringeLegacyPass(); }
char SyringeLegacyPass::ID;

INITIALIZE_PASS(SyringeLegacyPass, "syringe",
                "Syringe: dynamic behavior injection.", false, false)
