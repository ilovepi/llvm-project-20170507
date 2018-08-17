//===--- SyringeArgs.cpp - Arguments for Syringe --------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "clang/Driver/SyringeArgs.h"
#include "ToolChains/CommonArgs.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/DriverDiagnostic.h"
#include "clang/Driver/Options.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"

using namespace clang;
using namespace clang::driver;
using namespace llvm::opt;

namespace {
constexpr char SyringeInstrumentOption[] = "-fsyringe";
constexpr char SyringeConfigOption[] = "-fsyringe-config-file=";
} // namespace

SyringeArgs::SyringeArgs(const ToolChain &TC, const ArgList &Args) {
  const Driver &D = TC.getDriver();
  const llvm::Triple &Triple = TC.getTriple();
  if (Args.hasFlag(options::OPT_fsyringe, options::OPT_fnosyringe, false)) {
    if (Triple.getOS() == llvm::Triple::Linux ||
        Triple.getOS() == llvm::Triple::Fuchsia) {
      switch (Triple.getArch()) {
      case llvm::Triple::x86_64:
      case llvm::Triple::arm:
      case llvm::Triple::aarch64:
      case llvm::Triple::ppc64le:
      case llvm::Triple::mips:
      case llvm::Triple::mipsel:
      case llvm::Triple::mips64:
      case llvm::Triple::mips64el:
        break;
      default:
        D.Diag(diag::err_drv_clang_unsupported)
            << (std::string(SyringeInstrumentOption) + " on " + Triple.str());
      }
    } else if (Triple.getOS() == llvm::Triple::FreeBSD ||
               Triple.getOS() == llvm::Triple::OpenBSD ||
               Triple.getOS() == llvm::Triple::Darwin ||
               Triple.getOS() == llvm::Triple::NetBSD) {
      if (Triple.getArch() != llvm::Triple::x86_64) {
        D.Diag(diag::err_drv_clang_unsupported)
            << (std::string(SyringeInstrumentOption) + " on " + Triple.str());
      }
    } else {
      D.Diag(diag::err_drv_clang_unsupported)
          << (std::string(SyringeInstrumentOption) + " on " + Triple.str());
    }

    SyringeInject = true;

    if (!Args.hasFlag(options::OPT_fsyringe_link_deps,
                      options::OPT_fnosyringe_link_deps, true))
      SyringeRT = false;

    for (const auto &Filename :
         Args.getAllArgValues(options::OPT_fsyringe_config_file)) {
      if (llvm::sys::fs::exists(Filename)) {
        ConfigFiles.push_back(Filename);
        // ExtraDeps.push_back(Filename);
      } else
        D.Diag(clang::diag::err_drv_no_such_file) << Filename;
    }
  }
}

void SyringeArgs::addArgs(const ToolChain &TC, const ArgList &Args,
                          ArgStringList &CmdArgs, types::ID InputType) const {
  if (!SyringeInject)
    return;

  CmdArgs.push_back(SyringeInstrumentOption);
  for (const auto &Config : ConfigFiles) {
    SmallString<64> ConfigFileOpt(SyringeConfigOption);
    ConfigFileOpt += Config;
    CmdArgs.push_back(Args.MakeArgString(ConfigFileOpt));
  }
}
