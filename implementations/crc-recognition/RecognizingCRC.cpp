#include "llvm/Transforms/Utils/RecognizingCRC.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/Analysis/BasicAliasAnalysis.h"
#include "llvm/Analysis/ConstantFolding.h"
#include "llvm/Analysis/GlobalsModRef.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PatternMatch.h"
#include "llvm/Transforms/Utils/BuildLibCalls.h"
#include "llvm/Transforms/Utils/Local.h"
#include <cassert>
#include <cstring>
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IntrinsicsRISCV.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;
using namespace PatternMatch;
using namespace llvm;

// User defined option that can bi passed to opt for checking whether optimized implementation 
// of CRC algorithm is already in use
static cl::opt<bool> CheckForOptimizedCRC("check-crc-opt", cl::init(false), cl::Hidden, 
                              cl::desc("searching for the implementation of optimized CRC algorithm"));

// User defined option that can bi passed to opt for running IR level CRC optimization
static cl::opt<bool> UseNaiveCRCOptimization("crc-opt", cl::init(false), cl::Hidden, 
                              cl::desc("running IR level CRC algorithm optimization"));

// User defined option that can bi passed to opt for running CRC optimization with intrinsic functions!
static cl::opt<bool> UseIntrinsicsCRCOptimization("crc-opt-intrinsic", cl::init(false), cl::Hidden, 
                              cl::desc("running CRC algorithm optimization with intrinsic function usage"));

static bool checkForOptimizedCRCInstructions(Instruction &I){
  // TO-DO
  return false;
}

static void printGeneratedIRInstructions(Function *F){
  errs() << "-----------------------------------------\n";
  errs() << "Function name: " << F->getName() << " \n";
  
  errs() << "Function body:\n";
  for (BasicBlock &BB : *F) {
    errs() << BB.getName() << ":\n";
    for (Instruction &I : BB) {
      errs() << I << "\n";
    }
  }
  errs() << "-----------------------------------------\n";
}

static bool recognizingUnoptimizedCRCInstructions(Instruction &I){
  if(I.getFunction()->getName()=="main"){
    return false;
  }
  
  ReturnInst *RIfinal = dyn_cast<ReturnInst>(&I);
  Value *help1;
  Value *help2;
  bool flag;
  // Check for instruction: ret i16 %57
  if (!RIfinal && match(RIfinal, m_Load(m_Value(help1))))
    return false;

  LoadInst *LIfinal = dyn_cast<LoadInst>(RIfinal->getPrevNode());
  LoadInst *LI = dyn_cast<LoadInst>(RIfinal->getPrevNode());
  IRBuilder<> Builder(LIfinal);
  // Check for instruction: %57 = load i16, i16* %4, align 2
  if (!LIfinal)
    return false;

  BasicBlock *BB = dyn_cast<BasicBlock>(LI->getParent()->getPrevNode());
  BranchInst *BI = dyn_cast<BranchInst>(&BB->back());
  // Check for instruction: br label %8, !llvm.loop !6
  if (!BI && BI->isUnconditional())
    return false;

  StoreInst *SI = dyn_cast<StoreInst>(BI->getPrevNode());
  // Check for instruction: store i8 %55, i8* %5, align 1
  if (!SI && match(SI, m_Store(m_Add(m_Value(help1), m_SpecificInt(1)), m_Value(help2))))
    return false;

  Instruction *II = dyn_cast<Instruction>(SI->getPrevNode());
  if (!II)
    return false;

  // Check for instruction: %55 = add i8 %54, 1
  if (!match(II, m_Add(m_Load(m_Value(help1)), m_SpecificInt(1))))
    return false;

  LI = dyn_cast<LoadInst>(II->getPrevNode());
  // Check for instruction: %54 = load i8, i8* %5, align 1
  if (!LI)
    return false;

  BB = dyn_cast<BasicBlock>(LI->getParent()->getPrevNode());
  BI = dyn_cast<BranchInst>(&BB->back());
  // Check for instruction: br label %53
  if (!BI && BI->isUnconditional())
    return false;

  BB = dyn_cast<BasicBlock>(BI->getParent()->getPrevNode());
  BI = dyn_cast<BranchInst>(&BB->back());
  // Check for instruction: br label %52
  if (!BI && BI->isUnconditional())
    return false;

  SI = dyn_cast<StoreInst>(BI->getPrevNode());
  // Check for instruction: store i16 %51, i16* %4, align 2
  if (!SI || !match(SI, m_Store(m_Trunc(m_Value(help1)), m_Value(help2))))
    return false;

  TruncInst *TI = dyn_cast<TruncInst>(SI->getPrevNode());
  // Check for instruction: %51 = trunc i32 %50 to i16
  if (!TI || !match(TI, m_Trunc(m_And(m_Value(help1), m_SpecificInt(32767)))))
    return false;

  // For some reason we could not see trunc instruction!
  II = dyn_cast<Instruction>(TI->getPrevNode());
  // Check for instruction: %50 = and i32 %49, 32767
  if (!match(II, m_And((m_ZExt(m_Value(help1))), m_SpecificInt(32767))))
    return false;

  ZExtInst *ZI = dyn_cast<ZExtInst>(II->getPrevNode());
  // Check for instruction: %49 = zext i16 %48 to i32
  if (!ZI || !match(ZI, m_ZExt(m_Load(m_Value(help1)))))
    return false;

  LI = dyn_cast<LoadInst>(ZI->getPrevNode());
  // Check for instruction: %48 = load i16, i16* %4, align 2
  if (!LI)
    return false;

  errs() << "Here we are?!\n";
  BB = dyn_cast<BasicBlock>(LI->getParent()->getPrevNode());
  if (!BB)
    return false;

  BI = dyn_cast<BranchInst>(&BB->back());
  // Check for instruction: br label %52
  if (!BI || !BI->isUnconditional())
    return false;

  SI = dyn_cast<StoreInst>(BI->getPrevNode());
  // Check for instruction: store i16 %46, i16* %4, align 2
  if (!SI)
    return false;

  TI = dyn_cast<TruncInst>(SI->getPrevNode());
  // Check for instruction: %46 = trunc i32 %45 to i16
  if (!TI)
    return false;

  II = dyn_cast<Instruction>(TI->getPrevNode()); // Should recognize -32768 constant here!
  // Check for instruction: or i32 %44, 32768
  if (!match(II, m_Or(m_Value(help1), m_Value(help2))))
    return false;

  ZI = dyn_cast<ZExtInst>(II->getPrevNode());
  // Check for instruction: %44 = zext i16 %43 to i32
  if (!ZI)
    return false;

  LI = dyn_cast<LoadInst>(ZI->getPrevNode());
  // Check for instruction: %43 = load i16, i16* %4, align 2
  if (!LI)
    return false;

  BB = dyn_cast<BasicBlock>(LI->getParent()->getPrevNode());
  if (!BB)
    return false;

  BI = dyn_cast<BranchInst>(&BB->back());
  // Check for instruction: br i1 %41, label %42, label %47
  if (!BI || !BI->isConditional())
    return false;

  ICmpInst *ICMPI = dyn_cast<ICmpInst>(BI->getPrevNode());
  // Check for instruction: %41 = icmp ne i8 %40, 0
  if (!ICMPI)
    return false;

  LI = dyn_cast<LoadInst>(ICMPI->getPrevNode());
  // Check for instruction: %40 = load i8, i8* %7, align 1
  if (!LI)
    return false;

  SI = dyn_cast<StoreInst>(LI->getPrevNode());
  // Check for instruction: store i16 %39, i16* %4, align 2
  if (!SI)
    return false;

  TI = dyn_cast<TruncInst>(SI->getPrevNode());
  // Check for instruction: %39 = trunc i32 %38 to i16
  if (!TI)
    return false;

  II = dyn_cast<Instruction>(TI->getPrevNode());
  // Check for instruction: %38 = ashr i32 %37, 1
  if (!match(II, m_AShr(m_Value(help1), m_SpecificInt(1))))
    return false;

  ZI = dyn_cast<ZExtInst>(II->getPrevNode());
  // Check for instruction: %37 = zext i16 %36 to i32
  if (!ZI)
    return false;

  LI = dyn_cast<LoadInst>(ZI->getPrevNode());
  // Check for instruction: %36 = load i16, i16* %4, align 2
  if (!LI)
    return false;

  BB = dyn_cast<BasicBlock>(LI->getParent()->getPrevNode());
  if (!BB)
    return false;

  BI = dyn_cast<BranchInst>(&BB->back());
  // Check for instruction: br label %35
  if (!BI)
    return false;

  SI = dyn_cast<StoreInst>(BI->getPrevNode());
  // Check for instruction: store i8 0, i8* %7, align 1
  if (!SI)
    return false;

  BB = dyn_cast<BasicBlock>(SI->getParent()->getPrevNode());
  if (!BB)
    return false;

  BI = dyn_cast<BranchInst>(&BB->back());
  // Check for instruction: br label %35
  if (!BI)
    return false;

  // Here we should have two checks!
  flag = true;
  while(true){
    SI = dyn_cast<StoreInst>(BI->getPrevNode());
    if (!SI){
      flag = false;
      break;
    }
    
    SI = dyn_cast<StoreInst>(SI->getPrevNode());
    if (!SI){
      flag = false;
      break;
    }
    
    TI = dyn_cast<TruncInst>(SI->getPrevNode());
    if (!TI){
      flag = false;
      break;
    }
    
    II = dyn_cast<Instruction>(TI->getPrevNode());
    if (!match(II, m_Xor(m_Value(help1), m_SpecificInt(16386)))){
      flag = false;
      break;
    }  

    ZI = dyn_cast<ZExtInst>(II->getPrevNode());
    if (!ZI){
      flag = false;
      break;
    }
    LI = dyn_cast<LoadInst>(ZI->getPrevNode());
    if (!LI){
      flag = false;
      break;
    }

    break;
  }

  if(!flag){
    // Check for instruction: store i16 %33, i16* %4, align 2
    SI = dyn_cast<StoreInst>(BI->getPrevNode());
    if (!SI)
      return false;
    
    // Check for instruction: %33 = trunc i32 %32 to i16
    TI = dyn_cast<TruncInst>(SI->getPrevNode());
    if (!TI)
      return false;
    
    II = dyn_cast<Instruction>(TI->getPrevNode());
    // Check for instruction: %32 = xor i32 %31, 16386
    if (!match(II, m_Xor(m_Value(help1), m_SpecificInt(16386))))
      return false; 

    ZI = dyn_cast<ZExtInst>(II->getPrevNode());
    // Check for instruction: %31 = zext i16 %30 to i32
    if (!ZI)
      return false;
    
    LI = dyn_cast<LoadInst>(ZI->getPrevNode());
    // Check for instruction: %30 = load i16, i16* %4, align 2
    if (!LI)
      return false;

    SI = dyn_cast<StoreInst>(LI->getPrevNode());
    // Check for instruction: store i8 1, i8* %7, align 1
    if (!SI)
      return false;
  }

  BB = dyn_cast<BasicBlock>(LI->getParent()->getPrevNode());
  if (!BB)
    return false;

  BI = dyn_cast<BranchInst>(&BB->back());
  // Check for instruction: br i1 %28, label %29, label %34
  if (!BI)
    return false;

  ICMPI = dyn_cast<ICmpInst>(BI->getPrevNode());
  // Check for instruction: %28 = icmp eq i32 %27, 1
  if (!ICMPI)
    return false;

  ZI = dyn_cast<ZExtInst>(ICMPI->getPrevNode());
  // Check for instruction: %27 = zext i8 %26 to i32
  if (!ZI)
    return false;

  LI = dyn_cast<LoadInst>(ZI->getPrevNode());
  // Check for instruction: %26 = load i8, i8* %6, align 1
  if (!LI)
    return false;

  SI = dyn_cast<StoreInst>(LI->getPrevNode());
  // Check for instruction: store i8 %25, i8* %3, align 1
  if (!SI)
    return false;

  TI = dyn_cast<TruncInst>(SI->getPrevNode());
  // Check for instruction: %25 = trunc i32 %24 to i8
  if (!TI)
    return false;

  II = dyn_cast<Instruction>(TI->getPrevNode());
  // Check for instruction: %24 = ashr i32 %23, 1
  if (!match(II, m_AShr(m_Value(help1), m_SpecificInt(1))))
    return false;

  ZI = dyn_cast<ZExtInst>(II->getPrevNode());
  // Check for instruction: %23 = zext i8 %22 to i32
  if (!ZI)
    return false;

  LI = dyn_cast<LoadInst>(ZI->getPrevNode());
  // Check for instruction: %22 = load i8, i8* %3, align 1
  if (!LI)
    return false;

  SI = dyn_cast<StoreInst>(LI->getPrevNode());
  // Check for instruction: store i8 %21, i8* %6, align 1
  if (!SI)
    return false;

  TI = dyn_cast<TruncInst>(SI->getPrevNode());
  // Check for instruction: %21 = trunc i32 %20 to i8
  if (!TI)
    return false;

  II = dyn_cast<Instruction>(TI->getPrevNode());
  // Check for instruction: %20 = xor i32 %15, %19
  if (!match(II, m_Xor(m_Value(help1), m_Value(help2))))
    return false;

  II = dyn_cast<Instruction>(II->getPrevNode());
  // Check for instruction: %19 = and i32 %18, 1
  if (!match(II, m_And(m_Value(help1), m_SpecificInt(1))))
    return false;

  ZI = dyn_cast<ZExtInst>(II->getPrevNode());
  // Check for instruction: %18 = zext i8 %17 to i32
  if (!ZI)
    return false;

  TI = dyn_cast<TruncInst>(ZI->getPrevNode());
  // Check for instruction: %17 = trunc i16 %16 to i8
  if (!TI)
    return false;

  LI = dyn_cast<LoadInst>(TI->getPrevNode());
  // Check for instruction: %16 = load i16, i16* %4, align 2
  if (!LI)
    return false;

  II = dyn_cast<Instruction>(LI->getPrevNode());
  // Check for instruction: %15 = and i32 %14, 1
  if (!match(II, m_And(m_Value(help1), m_SpecificInt(1))))
    return false;

  ZI = dyn_cast<ZExtInst>(II->getPrevNode());
  // Check for instruction: %14 = zext i8 %13 to i32
  if (!ZI)
    return false;

  LI = dyn_cast<LoadInst>(ZI->getPrevNode());
  // Check for instruction: %13 = load i8, i8* %3, align 1
  if (!LI)
    return false;

  BB = dyn_cast<BasicBlock>(LI->getParent()->getPrevNode());
  if (!BB)
    return false;

  BI = dyn_cast<BranchInst>(&BB->back());
  // Check for instruction: br i1 %11, label %12, label %56
  if (!BI)
    return false;

  ICMPI = dyn_cast<ICmpInst>(BI->getPrevNode());
  // Check for instruction: %11 = icmp slt i32 %10, 8
  if (!ICMPI)
    return false;

  ZI = dyn_cast<ZExtInst>(ICMPI->getPrevNode());
  // Check for instruction: %10 = zext i8 %9 to i32
  if (!ZI)
    return false;

  LI = dyn_cast<LoadInst>(ZI->getPrevNode());
  // Check for instruction: %9 = load i8, i8* %5, align 1
  if (!LI)
    return false;

  BB = dyn_cast<BasicBlock>(LI->getParent()->getPrevNode());
  if (!BB)
    return false;

  BI = dyn_cast<BranchInst>(&BB->back());
  // Check for instruction: br label %8
  if (!BI)
    return false;

  // Here we have to match 6 more consecutive store instructions and 5 consecutive alloca instructions!
  SI = dyn_cast<StoreInst>(BI->getPrevNode());
  // Check for instruction: store i8 0, i8* %5, align 1
  if (!SI && SI->getPointerOperandType()!=Builder.getInt8PtrTy())
    return false;

  SI = dyn_cast<StoreInst>(SI->getPrevNode());
  // Check for instruction: store i8 0, i8* %7, align 1
  if (!SI && SI->getPointerOperandType()!=Builder.getInt8PtrTy())
    return false;

  SI = dyn_cast<StoreInst>(SI->getPrevNode());
  // Check for instruction: store i8 0, i8* %6, align 1
  if (!SI && SI->getPointerOperandType()!=Builder.getInt8PtrTy())
    return false;

  SI = dyn_cast<StoreInst>(SI->getPrevNode());
  // Check for instruction: store i8 0, i8* %5, align 1
  if (!SI && SI->getPointerOperandType()!=Builder.getInt8PtrTy())
    return false;

  SI = dyn_cast<StoreInst>(SI->getPrevNode());
  // Check for instruction: store i16 %1, i16* %4, align 2
  if (!SI)
    return false;

  SI = dyn_cast<StoreInst>(SI->getPrevNode());
  // Check for instruction: store i8 %0, i8* %3, align 1
  if (!SI && SI->getPointerOperandType()!=Builder.getInt8PtrTy())
    return false;

  // Last thing we have to match are 5 alloca instructions!
  AllocaInst *AI = dyn_cast<AllocaInst>(SI->getPrevNode());
  // Check for instruction: %7 = alloca i8, align 1
  if (!AI && AI->getAllocatedType()!=Builder.getInt8Ty())
    return false;

  AI = dyn_cast<AllocaInst>(AI->getPrevNode());
  // Check for instruction: %6 = alloca i8, align 1
  if (!AI && AI->getAllocatedType()!=Builder.getInt8Ty())
    return false;

  AI = dyn_cast<AllocaInst>(AI->getPrevNode());
  // Check for instruction: %5 = alloca i8, align 1
  if (!AI && AI->getAllocatedType()!=Builder.getInt8Ty())
    return false;

  AI = dyn_cast<AllocaInst>(AI->getPrevNode());
  // Check for instruction: %4 = alloca i16, align 2
  if (!AI && AI->getAllocatedType()!=Builder.getInt16Ty())
    return false;

  AI = dyn_cast<AllocaInst>(AI->getPrevNode());
  // Check for instruction: %3 = alloca i8, align 1
  if (!AI && AI->getAllocatedType()!=Builder.getInt8Ty())
    return false;

  return true;  
}

static bool tryToRecognizeCRC32_v2(Instruction &I) {
  bool flag = recognizingUnoptimizedCRCInstructions(I);
  if(!flag)
    return false;

  ReturnInst *RIfinal = dyn_cast<ReturnInst>(&I);
  LoadInst *LIfinal = dyn_cast<LoadInst>(RIfinal->getPrevNode());

  errs() << "Original unoptimized form of CRC32 algorithm has been recognized!\n";
  Value *argument1 = LIfinal->getFunction()->getArg(0);
  Value *argument2 = LIfinal->getFunction()->getArg(1);
  Type *ArgType1 = argument1->getType();
  Type *ArgType2 = argument2->getType();
  IRBuilder<> Builder(LIfinal);

  // Here is hidden another aproach for replacing unoptimized crc with optimized version! 
  //auto CRC8 = Builder.CreateIntrinsic(Intrinsic::crc8, {}, {argument1, argument2});
  
  auto CRC8 = Builder.CreateIntrinsic(Intrinsic::riscv_crc_petar, {}, {argument1, argument2});
  LIfinal->replaceAllUsesWith(CRC8);
  Function *F = LIfinal->getFunction();

  BasicBlock *bb_help10 = RIfinal->getParent()
                              ->getPrevNode()
                              ->getPrevNode()
                              ->getPrevNode()
                              ->getPrevNode()
                              ->getPrevNode()
                              ->getPrevNode()
                              ->getPrevNode()
                              ->getPrevNode()
                              ->getPrevNode()
                              ->getPrevNode();
  BasicBlock *bb_help9 = RIfinal->getParent()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode();
  BasicBlock *bb_help8 = RIfinal->getParent()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode();
  BasicBlock *bb_help7 = RIfinal->getParent()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode();
  BasicBlock *bb_help6 = RIfinal->getParent()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode();
  BasicBlock *bb_help5 = RIfinal->getParent()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode();
  BasicBlock *bb_help4 = RIfinal->getParent()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode();
  BasicBlock *bb_help3 = RIfinal->getParent()->getPrevNode()->getPrevNode()->getPrevNode();
  BasicBlock *bb_help2 = RIfinal->getParent()->getPrevNode()->getPrevNode();
  BasicBlock *bb_help1 = RIfinal->getParent()->getPrevNode();
  DeleteDeadBlocks({bb_help1, bb_help2, bb_help3, bb_help4, bb_help5, bb_help6, bb_help7, bb_help8, bb_help9, bb_help10});

  F->back().back().getPrevNode()->eraseFromParent();

  // Call the function to print out the generated IR instruction
  printGeneratedIRInstructions(F);

  return true;
}

static bool tryToRecognizeCRC32_v1(Instruction &I) {
  bool flag = recognizingUnoptimizedCRCInstructions(I);
  if(!flag)
    return false;

  ReturnInst *RIfinal = dyn_cast<ReturnInst>(&I);
  LoadInst *LIfinal = dyn_cast<LoadInst>(RIfinal->getPrevNode());

  errs() << "Original unoptimized form of CRC32 algorithm has been recognized!\n";
  Value *argument1 = LIfinal->getFunction()->getArg(0);
  Value *argument2 = LIfinal->getFunction()->getArg(1);
  Type *ArgType1 = argument1->getType();
  Type *ArgType2 = argument2->getType();

  BasicBlock *bb_help10 = RIfinal->getParent()
                              ->getPrevNode()
                              ->getPrevNode()
                              ->getPrevNode()
                              ->getPrevNode()
                              ->getPrevNode()
                              ->getPrevNode()
                              ->getPrevNode()
                              ->getPrevNode()
                              ->getPrevNode()
                              ->getPrevNode();
  BasicBlock *bb_help9 = RIfinal->getParent()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode();
  BasicBlock *bb_help8 = RIfinal->getParent()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode();
  BasicBlock *bb_help7 = RIfinal->getParent()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode();
  BasicBlock *bb_help6 = RIfinal->getParent()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode();
  BasicBlock *bb_help5 = RIfinal->getParent()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode();
  BasicBlock *bb_help4 = RIfinal->getParent()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode()
                             ->getPrevNode();
  BasicBlock *bb_help3 = RIfinal->getParent()->getPrevNode()->getPrevNode()->getPrevNode();
  BasicBlock *bb_help2 = RIfinal->getParent()->getPrevNode()->getPrevNode();
  BasicBlock *bb_help1 = RIfinal->getParent()->getPrevNode();
  DeleteDeadBlocks({bb_help1, bb_help2, bb_help3, bb_help4, bb_help5, bb_help6, bb_help7, bb_help8, bb_help9, bb_help10});

  // Getting current function pointer! 
  Function *F = LIfinal->getFunction();

  // Declaring the IR builder!
  IRBuilder<> Builder(LIfinal);
  LIfinal->getParent()->setName("entry");
  
  BasicBlock *ForEndBB = BasicBlock::Create(F->getContext(), "for.end", F, nullptr);
  
  // I think we can start this way!
  BasicBlock *ForCondBB = BasicBlock::Create(F->getContext(), "for.cond", F, ForEndBB);
  IRBuilder<> ForCondBuilder(ForCondBB, ForCondBB->end());
  
  BasicBlock *ForBodyBB = BasicBlock::Create(F->getContext(), "for.body", F, ForEndBB);
  IRBuilder<> ForBodyBuilder(ForBodyBB, ForBodyBB->end());
  
  BasicBlock *ForIncBB = BasicBlock::Create(F->getContext(), "for.inc", F, ForEndBB);
  IRBuilder<> ForIncBuilder(ForIncBB, ForIncBB->end());

  // Creation of "entry" basic block!

  // Alloca instructions
  AllocaInst *AI1 = Builder.CreateAlloca(ArgType1, 0, "data.addr");
  AllocaInst *AI2 = Builder.CreateAlloca(ArgType2, nullptr, "_crc.addr");
  AllocaInst *AI3 = Builder.CreateAlloca(ArgType1, nullptr, "i");
  AllocaInst *AI4 = Builder.CreateAlloca(ArgType1, nullptr, "x16");
  AllocaInst *AI5 = Builder.CreateAlloca(ArgType1, nullptr, "carry");
  AllocaInst *AI6 = Builder.CreateAlloca(Builder.getInt64Ty(), nullptr, "crc");
  
  // Store instructions
  StoreInst *SI1 = Builder.CreateStore(argument1, AI1);
  StoreInst *SI2 = Builder.CreateStore(argument2, AI2);
  StoreInst *SI3 = Builder.CreateStore(Builder.getInt8(0), AI3);
  StoreInst *SI4 = Builder.CreateStore(Builder.getInt8(0), AI4);
  StoreInst *SI5 = Builder.CreateStore(Builder.getInt8(0), AI5);
  
  LoadInst *LI1 = Builder.CreateLoad(ArgType2, AI2, "");
  Value *ZI1 = Builder.CreateZExt(LI1, Builder.getInt64Ty(), "conv");
  StoreInst *SI6 = Builder.CreateStore(ZI1, AI6);
  LoadInst *LI2 = Builder.CreateLoad(Builder.getInt8Ty(), AI1, "");
  Value *ZI2 = Builder.CreateZExt(LI2, Builder.getInt64Ty(), "conv1");
  LoadInst *LI3 = Builder.CreateLoad(Builder.getInt64Ty(), AI6, "");
  Value *XOR1 = Builder.CreateXor(LI3, ZI2, "xor");
  StoreInst *SI7 = Builder.CreateStore(XOR1, AI6);
  StoreInst *SI8 = Builder.CreateStore(Builder.getInt8(0), AI3);
  BranchInst *BI1 = Builder.CreateBr(ForCondBB);
  // End of "entry" basic block creation!

  // Moving last load instruction into last basic block!
  Instruction *I1 = LIfinal;
  Instruction *I2 = LIfinal->getNextNode();
  I1->removeFromParent();
  I2->removeFromParent();
  I1->insertInto(ForEndBB, ForEndBB->end());
  I2->insertInto(ForEndBB, ForEndBB->end());

  // Creation of "for.cond" basic block!
  LoadInst *LI4 = ForCondBuilder.CreateLoad(ForCondBuilder.getInt8Ty(), AI3);
  Value *ZI3 = ForCondBuilder.CreateZExt(LI4, ForCondBuilder.getInt32Ty(), "conv2");
  Value *ICI1 = ForCondBuilder.CreateICmp(llvm::CmpInst::ICMP_SLT, ZI3, ForCondBuilder.getInt32(8), "cmp");
  BranchInst *BI2 = ForCondBuilder.CreateCondBr(ICI1, ForBodyBB, ForEndBB);
  // End of "for.cond" basic block creation!

  // Creation of "for.body" basic block!
  LoadInst *LI5 = ForBodyBuilder.CreateLoad(ForBodyBuilder.getInt64Ty(), AI6);
  Value *TI1 = ForBodyBuilder.CreateTrunc(LI5, ForBodyBuilder.getInt8Ty(), "conv4");
  Value *ZI4 = ForBodyBuilder.CreateZExt(TI1, ForCondBuilder.getInt32Ty(), "conv5");
  Value *And1 = ForBodyBuilder.CreateAnd(ZI4, ForBodyBuilder.getInt32(1), "and");
  Value *TI2 = ForBodyBuilder.CreateTrunc(And1, ForBodyBuilder.getInt8Ty(), "conv6");
  StoreInst *SI9 = ForBodyBuilder.CreateStore(TI2, AI4);
  LoadInst *LI6 = ForBodyBuilder.CreateLoad(ForBodyBuilder.getInt8Ty(), AI1);
  Value *ZI5 = ForBodyBuilder.CreateZExt(LI6, ForCondBuilder.getInt32Ty(), "conv7");
  Value *Ashr1 = ForBodyBuilder.CreateAShr(ZI5, ForBodyBuilder.getInt32(1), "shr");
  Value *TI3 = ForBodyBuilder.CreateTrunc(Ashr1, ForBodyBuilder.getInt8Ty(), "conv8");
  StoreInst *SI10 = ForBodyBuilder.CreateStore(TI3, AI1);
  LoadInst *LI7 = ForBodyBuilder.CreateLoad(ForBodyBuilder.getInt64Ty(), AI6);
  Value *Ashr2 = ForBodyBuilder.CreateAShr(LI7, ForBodyBuilder.getInt64(1), "shr9");
  StoreInst *SI11 = ForBodyBuilder.CreateStore(Ashr2, AI6);
  LoadInst *LI8 = ForBodyBuilder.CreateLoad(ForBodyBuilder.getInt8Ty(), AI4);
  Value *ZI6 = ForBodyBuilder.CreateZExt(LI8, ForCondBuilder.getInt32Ty(), "conv10");
  Value *And2 = ForBodyBuilder.CreateAnd(ZI6, ForBodyBuilder.getInt32(1), "and11");
  Value *ICI2 = ForBodyBuilder.CreateICmp(llvm::CmpInst::ICMP_NE, And2, ForCondBuilder.getInt32(0), "tobool");
  Value *ZI7 = ForBodyBuilder.CreateZExt(ICI2, ForCondBuilder.getInt64Ty(), "");
  Value *Select1 = ForBodyBuilder.CreateSelect(ICI2, ForBodyBuilder.getInt32(40961), ForBodyBuilder.getInt32(0));
  Value *Sext1 = ForBodyBuilder.CreateSExt(Select1, ForBodyBuilder.getInt64Ty(), "conv12");
  LoadInst *LI9 = ForBodyBuilder.CreateLoad(ForBodyBuilder.getInt64Ty(), AI6);
  Value *XOR2 = ForBodyBuilder.CreateXor(LI9, Sext1, "xor13");
  StoreInst *SI12 = ForBodyBuilder.CreateStore(XOR2, AI6);
  BranchInst *BI3 = ForBodyBuilder.CreateBr(ForIncBB);
  // End of "for.body" basic block creation!

  // Creation of "for.inc" basic block!
  LoadInst *LI10 = ForIncBuilder.CreateLoad(ForBodyBuilder.getInt8Ty(), AI3);
  Value *Add1 = ForIncBuilder.CreateAdd(LI10, ForIncBuilder.getInt8(1), "inc");
  StoreInst *SI20 = ForIncBuilder.CreateStore(Add1, AI3);
  BranchInst *BI4 = ForIncBuilder.CreateBr(ForCondBB); // Need to check this branch instruction again!
  // End of "for.inc" basic block creation!

  // Last, we have to change a couple of instructions within "for.end" basic block!
  IRBuilder<> ForEndBuilder(LIfinal);
  LoadInst *LI11 = ForEndBuilder.CreateLoad(ForEndBuilder.getInt64Ty(), AI6);
  Value *TI20 = ForEndBuilder.CreateTrunc(LI11, ForEndBuilder.getInt16Ty(), "conv14");

  LIfinal->replaceAllUsesWith(TI20);

  F->back().back().getPrevNode()->eraseFromParent();
  
  // Call the function to print out the generated IR instruction
  printGeneratedIRInstructions(F);

  return true;
}

PreservedAnalyses RecognizingCRCPass::run(Function &F, FunctionAnalysisManager &AM) {
  if (UseNaiveCRCOptimization && !UseIntrinsicsCRCOptimization) {
    errs() << "The IR level CRC optimization is about to be run...\n";
    bool crc_flag = tryToRecognizeCRC32_v1(F.back().back());
    
    if (crc_flag) {
      errs() << "The IR level CRC optimization has been successfully applied!" << "\n";
    }  
  } else if (!UseNaiveCRCOptimization && UseIntrinsicsCRCOptimization) {
    errs() << "The CRC optimization with intrinsic function is about to be run...\n";
    bool crc_flag = tryToRecognizeCRC32_v2(F.back().back());
    
    if (crc_flag) {
      errs() << "The CRC optimization with intrinsic function has been successfully applied!" << "\n";
    }
  } else if (UseNaiveCRCOptimization && UseIntrinsicsCRCOptimization) {
    errs() << "Wrong usage! Choose one optimization approach only!\n";
  }

  return PreservedAnalyses::all();
}
