//===-- SizeOpts.cpp - code size optimization related code ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains some shared code size optimization related code.
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Utils/SizeOpts.h"

using namespace llvm;

cl::opt<bool> EnablePGSO(
    "pgso", cl::Hidden, cl::init(true),
    cl::desc("Enable the profile guided size optimizations. "));

cl::opt<bool> PGSOLargeWorkingSetSizeOnly(
    "pgso-lwss-only", cl::Hidden, cl::init(true),
    cl::desc("Apply the profile guided size optimizations only "
             "if the working set size is large (except for cold code.)"));

cl::opt<bool> PGSOColdCodeOnly(
    "pgso-cold-code-only", cl::Hidden, cl::init(true),
    cl::desc("Apply the profile guided size optimizations only "
             "to cold code."));

cl::opt<bool> PGSOIRPassOrTestOnly(
    "pgso-ir-pass-or-test-only", cl::Hidden, cl::init(false),
    cl::desc("Apply the profile guided size optimizations only"
             "to the IR passes or tests."));

cl::opt<bool> ForcePGSO(
    "force-pgso", cl::Hidden, cl::init(false),
    cl::desc("Force the (profiled-guided) size optimizations. "));

cl::opt<int> PgsoCutoffInstrProf(
    "pgso-cutoff-instr-prof", cl::Hidden, cl::init(250000), cl::ZeroOrMore,
    cl::desc("The profile guided size optimization profile summary cutoff "
             "for instrumentation profile."));

cl::opt<int> PgsoCutoffSampleProf(
    "pgso-cutoff-sample-prof", cl::Hidden, cl::init(800000), cl::ZeroOrMore,
    cl::desc("The profile guided size optimization profile summary cutoff "
             "for sample profile."));

namespace {
struct BasicBlockBFIAdapter {
  static bool isFunctionColdInCallGraph(const Function *F,
                                        ProfileSummaryInfo *PSI,
                                        BlockFrequencyInfo &BFI) {
    return PSI->isFunctionColdInCallGraph(F, BFI);
  }
  static bool isFunctionHotInCallGraphNthPercentile(int CutOff,
                                                    const Function *F,
                                                    ProfileSummaryInfo *PSI,
                                                    BlockFrequencyInfo &BFI) {
    return PSI->isFunctionHotInCallGraphNthPercentile(CutOff, F, BFI);
  }
  static bool isColdBlock(const BasicBlock *BB,
                          ProfileSummaryInfo *PSI,
                          BlockFrequencyInfo *BFI) {
    return PSI->isColdBlock(BB, BFI);
  }
  static bool isHotBlockNthPercentile(int CutOff,
                                      const BasicBlock *BB,
                                      ProfileSummaryInfo *PSI,
                                      BlockFrequencyInfo *BFI) {
    return PSI->isHotBlockNthPercentile(CutOff, BB, BFI);
  }
};
} // end anonymous namespace

bool llvm::shouldOptimizeForSize(const Function *F, ProfileSummaryInfo *PSI,
                                 BlockFrequencyInfo *BFI,
                                 PGSOQueryType QueryType) {
  return shouldFuncOptimizeForSizeImpl<BasicBlockBFIAdapter>(F, PSI, BFI,
                                                             QueryType);
}

bool llvm::shouldOptimizeForSize(const BasicBlock *BB, ProfileSummaryInfo *PSI,
                                 BlockFrequencyInfo *BFI,
                                 PGSOQueryType QueryType) {
  return shouldOptimizeForSizeImpl<BasicBlockBFIAdapter>(BB, PSI, BFI,
                                                         QueryType);
}
