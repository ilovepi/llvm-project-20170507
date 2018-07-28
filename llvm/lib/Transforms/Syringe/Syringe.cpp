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

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ExecutionEngine/Orc/IndirectionUtils.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalAlias.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"

using namespace llvm;
static const char *const kSyringeModuleCtorName = "syringe.module_ctor";
static const char *const kSyringeInitName = "__syringe_register";

/// initializeSyringe - Initialize all passes in the Syringe library.
void initializeSyringe(PassRegistry &Registry) {
  initializeSyringePass(Registry);
}

/// LLVMInitializeSyringe - C binding for initializeSyringe.
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

    Function *target;
    Function* stub;
    GlobalValue *ptr_syringe;
    Function *detour;

  for (Function &F : M) {
    ret = true;
    if (F.hasFnAttribute(Attribute::SyringePayload)) {
      errs() << "Found Syringe Payload\n";
      // create alias
      auto alias = GlobalAlias::create("_Z18hello_detour_implv", &F);
      alias->setVisibility(GlobalValue::VisibilityTypes::DefaultVisibility);
      detour = &F;
    }
  }

  for (Function &F : M) {
    if (F.hasFnAttribute(Attribute::SyringeInjectionSite)) {
      errs() << "Found Syringe Injection Site\n";
      ret = true;
      ValueToValueMapTy VMap;

      // clone function
      auto *cloneDecl = orc::cloneFunctionDecl(M, F, &VMap);
      //auto mangledFuncName = F.getName();
      //errs() << "Mangled Name: " << mangledFuncName << "\n";

      auto *internalAlias = M.getNamedAlias("_Z18hello_detour_implv");

      if (internalAlias == nullptr) {
        auto aliasDecl = orc::cloneFunctionDecl(M, F, nullptr);
        aliasDecl->setName("_Z18hello_detour_implv");
        aliasDecl->removeFnAttr(Attribute::AttrKind::SyringeInjectionSite);
        aliasDecl->setLinkage(GlobalValue::LinkageTypes::ExternalLinkage);
      }

      // cloneDecl->setName(F.getName() + "_syringe_impl");
      cloneDecl->setName("_Z18hello_syringe_implv");
      orc::moveFunctionBody(F, VMap, nullptr, cloneDecl);
      cloneDecl->removeFnAttr(Attribute::AttrKind::SyringeInjectionSite);
      stub = cloneDecl;

      // auto *cloneFunc = CloneFunction(&F, VMap);
      // cloneFunc->removeFnAttr(Attribute::AttrKind::SyringeInjectionSite);
      // cloneFunc->setName(F.getName() + "_syringe_impl");

      // create impl pointer
      auto SyringePtr = orc::createImplPointer(
          *F.getType(), M, "_Z17hello_syringe_ptr", cloneDecl);
      //*F.getType(), M, F.getName() + "$stub_ptr", cloneDecl);
      SyringePtr->setVisibility(GlobalValue::DefaultVisibility);
      target = &F;
      ptr_syringe = SyringePtr;

      // create stub body for original call
      orc::makeStub(F, *SyringePtr);
    }
  }

  // if we've made a modification, add a global ctor entry for the function
  if (ret && target && ptr_syringe) {
      auto regFuncName = "__syringe_register";
      auto funcTy = target->getType();
    Type *ParamTypes[] = {funcTy, funcTy, funcTy, ptr_syringe->getType() };
    auto ParamTypesRef = makeArrayRef(ParamTypes,4);
    auto retTy = Type::getVoidTy(M.getContext());
    auto fTy = FunctionType::get(retTy, ParamTypesRef , false);
    auto constF = M.getOrInsertFunction(regFuncName, fTy);
    auto regFunc = M.getFunction(regFuncName);
    regFunc->setLinkage(GlobalValue::LinkageTypes::ExternalLinkage);

    Value* ParamArgs[] = {target, stub, detour, ptr_syringe};
    auto ParamArgsRef = makeArrayRef(ParamArgs,4);

    //errs() << "Param Args\n";
    //for(auto a: ParamArgsRef)
    //{
        //errs() << "Item: " << *a << ", ";
    //}
    //errs() << "\n";

    errs() << "Param Types\n";
for(auto a: ParamTypesRef)
    {
        errs()<<  "Item: " << *a << ", ";
    }
    errs() << "\n";


    Function *Ctor = Function::Create(
      fTy,
      GlobalValue::InternalLinkage,kSyringeModuleCtorName , &M);
  BasicBlock *CtorBB = BasicBlock::Create(M.getContext(), "", Ctor);
  IRBuilder<> IRB(ReturnInst::Create(M.getContext(), CtorBB));
  IRB.CreateCall(constF, ParamArgsRef);

  Ctor->setLinkage(GlobalValue::LinkageTypes::ExternalLinkage);

    //appendToGlobalCtors(M, Ctor, 1, Ctor);
  }

  return ret;
}

ModulePass *createSyringe() { return new Syringe(); }
char Syringe::ID;

INITIALIZE_PASS(Syringe, "syringe", "Syringe: dynamic behavior injection.",
                false, false)
