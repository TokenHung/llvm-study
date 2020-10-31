#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/ADT/Statistic.h"

using namespace llvm;

namespace {

  struct Hw1 : public ModulePass {
    static char ID; // Pass identification, replacement for typeid
    Hw1() : ModulePass(ID) {}
    
    virtual bool runOnModule(Module &M) {
      errs() << "Hello, I'm Hw1!" << "\n";

      return false;
    }

  };
}

//initialize identifier
char Hw1::ID = 0;
//"Hw1" is the name of pass
//"Hw1 pass written by yenchunli" is the explaination of your pass
static RegisterPass<Hw1> GS("HW1", "Hw1 pass written by yenchunli");
