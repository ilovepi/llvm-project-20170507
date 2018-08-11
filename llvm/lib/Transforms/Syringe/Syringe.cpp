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
#include "llvm/SyringeRecord.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"

using namespace llvm;

// Strings for Syringe names and Suffix
static const char *const SyringeModuleCtorName = "syringe.module_ctor";
static const char *const SyringeInitName = "__syringe_register";
static const char *const SyringeStubImplSuffix = "$syringe_impl";
static const char *const SyringeDetourImplSuffix = "$detour_impl";
static const char *const SyringeImplPtrSuffix = "$syringe_impl_ptr";

namespace {

// contains data used for registratin function
struct SyringeInitData {
  // the address of the original function, serves as key for runtime lookups
  Function *Target;
  // the address of the new stub impl (the moved body of the origianl function)
  Function *Stub;
  // the address of the function whose behavior we wish to inject
  Function *Detour;
  // the global function pointer used in the stub for indirect calls
  GlobalValue *SyringePtr;
};

} // namespace

// create a single ctor function for the module, whose body should consist of a
// series of calls to the registration function. One call per injection site.
// We create the function, add a basic block to it, and then insert calls to the
// registration function for each injection site in the module.
//
// example ctor function:
//
// void ctor_func_name(){
//      register(original_func, stub_impl, detour_impl, impl_ptr);
//      register(original_func2, stub_impl2, detour_impl2, impl_ptr2);
//      ....
// }
//
static void createCtorInit(Module &M,
                           SmallVector<SyringeInitData, 8> &InitData) {

  // create a ctor to register all injection sites
  Function *Ctor = Function::Create(
      FunctionType::get(Type::getVoidTy(M.getContext()), false),
      GlobalValue::InternalLinkage, SyringeModuleCtorName + M.getName(), &M);
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

    // assert that these are the same size incase we are ever change the
    // implementation
    assert(ParamArgsRef.size() == ParamTypesRef.size());

    // void return type
    auto RetTy = Type::getVoidTy(M.getContext());

    // the type of the registration function
    auto RegFnTy = FunctionType::get(RetTy, ParamTypesRef, false);

    // create or find the registration function
    auto ConstFn = M.getOrInsertFunction(SyringeInitName, RegFnTy);
    auto RegFunc = M.getFunction(SyringeInitName);

    // set its linkage
    RegFunc->setLinkage(GlobalValue::LinkageTypes::ExternalLinkage);

    // add a call to the registration function w/ our target arguments
    IRB.CreateCall(ConstFn, ParamArgsRef);
  }

  // FIXME: the magic constant is in parts of clang as well, should we move to a
  // const int? follow Clang's example for setting priority for global ctors
  appendToGlobalCtors(M, Ctor, 65535);
}

// Naming APIs

static std::string createSuffixedName(StringRef BaseName, StringRef Suffix) {
  return (BaseName + Suffix).str();
}

static std::string createStubNameFromBase(StringRef BaseName) {
  return createSuffixedName(BaseName, SyringeStubImplSuffix);
}

static std::string createImplPtrNameFromBase(StringRef BaseName) {
  return createSuffixedName(BaseName, SyringeImplPtrSuffix);
}

static std::string createAliasNameFromBase(StringRef BaseName) {
  return createSuffixedName(BaseName, SyringeDetourImplSuffix);
}

bool SyringeLegacyPass::parse(std::unique_ptr<MemoryBuffer> &MapFile) {
  bool status = false;
  yaml::Input yin(MapFile->getBuffer());

  yin >> Metadata;
  if (yin.error())
    return false;

  if (!Metadata.empty())
    status = true;

  for (syringe::YAMLSyringeRecord Rec : Metadata) {
    Metadata.push_back(Rec);
  }
  return status;
}

bool SyringeLegacyPass::parse(const std::string &MapFile) {
  ErrorOr<std::unique_ptr<MemoryBuffer>> Mapping =
      MemoryBuffer::getFile(MapFile);

  if (!Mapping)
    report_fatal_error("unable to read rewrite map '" + MapFile +
                       "': " + Mapping.getError().message());

  if (!parse(*Mapping))
    report_fatal_error("unable to parse rewrite map '" + MapFile + "'");

  return true;
}

// Module APIs

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
  for(auto& rec : Metadata)
  {
    if(M.getName() == rec.Filename)
    {
      errs() << "Found a matachin file\n";
    }

  }
  return doBehaviorInjectionForModule(M);
}

/// create funciton stub for behavior injection
// Algorithm:
// examine the functions in the module.
// First process any payloads
// -- makes processing injection sites in the same translation unit easier
// -- also create any aliases required
// Next process the injection sites
// -- create any declarations needed for the payload/alias
// -- copy the function body of the injeciton site into a new function
// -- replace the target functions's body with a stub that makes an indirect
// tail call through an implementation pointer
// -- record the data required for registering the initialization function
// -- if any changes to the injection site were made, register the init data
// in a ctor
bool SyringeLegacyPass::doBehaviorInjectionForModule(Module &M) {
  bool ChangedPayload = false;
  bool ChangedInjection = false;
  SmallVector<SyringeInitData, 8> InitData;

  for (Function &F : M) {
    ChangedPayload = true;
    if (F.hasFnAttribute("SyringePayload")) {

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
      ChangedInjection = true;
      ValueToValueMapTy VMap;

      // clone function
      auto *CloneDecl = orc::cloneFunctionDecl(M, F, &VMap);
      auto AliasName = createAliasNameFromBase(F.getName());

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

  // if we've made a modification to the injection site, add a global ctor
  // entry for the function
  if (ChangedInjection) {
    createCtorInit(M, InitData);
  }

  return ChangedPayload || ChangedInjection;
}

ModulePass *llvm::createSyringe() { return new SyringeLegacyPass(); }
char SyringeLegacyPass::ID;

INITIALIZE_PASS(SyringeLegacyPass, "syringe",
                "Syringe: dynamic behavior injection.", false, false)
