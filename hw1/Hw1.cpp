#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/ADT/Statistic.h"

using namespace llvm;

namespace {

  std::map<std::string, std::string> variableMap;
  std::map<std::string, int> valueMap;

  int maxIndex;
  int minIndex;

  struct Hw1 : public ModulePass {
    static char ID; // Pass identification, replacement for typeid
    Hw1() : ModulePass(ID) {}
    
    void getLoadDef(Value *inv){    //recursive traverse the IR
      if(Instruction *I = dyn_cast<Instruction>(inv)){   //make sure *inv is an Instruction

        //when you call getName for llvm temp variable such as %1 %2 ... is empty char
        //so when getName is empty char means you must search deeper inside the IR
        if(I->getOpcode() == Instruction::GetElementPtr) {
            Value *tmp = I->getOperand(0);
            //%ArrayName = alloca [20 x i32], align 16
            std::string name = tmp->getName();
            //variableMap[I->getName()] = name;
            errs() << I->getName() << " " << name << "\n";
        }
        else{
          //recursive to the upper define-use chain
          for(User::op_iterator OI = (*I).op_begin(),e = (*I).op_end();OI!=e;++OI){
            Value *v=*OI;
            //errs() <<*v<<"\n";
            getLoadDef(v);
          }
        }
      }
    }

    virtual bool runOnModule(Module &M) {
      errs() << "I'm going to find Loop Dependence." << "\n";
      
      //whole *.bc is a module,module's iterator F is class of a function
      for (Module::iterator F = M.begin(); F != M.end(); F++){
        
        // function's iterator BB is class of a basicblock
        //int functionCount = 0;
        //errs() << "Function name : " << F->getName() << "\n";

        for (Function::iterator BB = (*F).begin(); BB != (*F).end(); BB++)
        {
          // entry block initialize the variables
          if(!BB->getName().find("entry", 0)) {
            errs() << "entry" << "\n";
            for (BasicBlock::iterator itrIns = (*BB).begin(); itrIns != (*BB).end(); itrIns++) {
              switch(itrIns->getOpcode()) {
                case Instruction::Store:
                  Value *tmp1 = itrIns->getOperand(0);
                  Value *tmp2 = itrIns->getOperand(1);
                  //errs() << "=======================================\n";
                  //errs() << "store" << "\ntmp1=" << *tmp1 << "\ntmp2=" << *tmp2 << "\n";
                  //errs() << "=======================================\n";
                  int value;
                  std::string name;
                  if(ConstantInt* Integer = dyn_cast<ConstantInt>(tmp1)){
                    value = Integer->getZExtValue();
                    //errs() << "value=" << value << "\n";
                  }
                  //errs() << "tmp2=" << *tmp2 << "\n";
                  if(Instruction *I = dyn_cast<Instruction>(tmp2)){
                    name = I->getName();
                    //errs() << "name=" << name << "\n";
                  }
                  //errs() << name << "=" << value << "\n";
                  valueMap[name] = value;

                  break;

              }
            }
          }
          // for.cond is the start of the for block
          if(!BB->getName().find("for.cond", 0)) {
            errs() << "for.cond" << "\n";
            minIndex = valueMap["i"];
            for (BasicBlock::iterator itrIns = (*BB).begin(); itrIns != (*BB).end(); itrIns++) {
              if(!strcmp("icmp", itrIns->getOpcodeName())) {
                if(ConstantInt* Integer = dyn_cast<ConstantInt>(itrIns->getOperand(1))) {
                    maxIndex = Integer->getZExtValue();
                    //errs() << "maxIndex=" << maxIndex << "\n";
                }
                
              }
            }
          }
            
          // for.body is the start of the for block
          if(!BB->getName().find("for.body", 0)) {
            errs() << "for.body" << "\n";
            for (BasicBlock::iterator itrIns = (*BB).begin(); itrIns != (*BB).end(); itrIns++){
              std::string arrayName;
              int value;
              std::string name;
              switch(itrIns->getOpcode())
              {
                
                
                case Instruction::Sub:
                {
                  //
                  Value *tmp1 = itrIns->getOperand(0);
                  Value *tmp2 = itrIns->getOperand(1);
                  errs() << "=======================================\n";
                  //errs() << "Sub\n" << "tmp1=" << *tmp1 << "\ntmp2=" << *tmp2 << "\n";
                  
                  
                  if(ConstantInt* Integer = dyn_cast<ConstantInt>(tmp2)){
                    value = Integer->getZExtValue();
                    //errs() << "value=" << value << "\n";
                  }

                  if(Instruction *I = dyn_cast<Instruction>(tmp1)) {
                    Value *tmp3 = I->getOperand(0);
                    name = tmp3->getName();
                  }
                  
                  errs() << name <<" = " << name << " - " << value << "\n";

                  errs() << "=======================================\n";
                  break;
                }
                case Instruction::Store:
                {
                  //tmp1 = tmp2 in c code.
                  Value *tmp1 = itrIns->getOperand(0);
                  Value *tmp2 = itrIns->getOperand(1);
                  //errs() << "=======================================\n";
                  //errs() << "store\n" << "tmp1=" << *tmp1 << "\ntmp2=" << *tmp2 << "\n";
                  //errs() << "=======================================\n";

                  if(!tmp1->hasName()) {
                    getLoadDef(tmp1);
                  }

                  if(Instruction *I = dyn_cast<Instruction>(tmp2)) {
                      if(I->getOpcode() == Instruction::GetElementPtr) {
                          tmp1 = I->getOperand(0);
                          std::string name = tmp1->getName();
                          //variableMap[I->getName()] = name;
                          errs() << "name=" << name << "\n";
                      }
                  }
                  break;
                }
              }
              
              
            }

          }
          // for.end is the start of the for block
          if(!BB->getName().find("for.end", 0)) {
            errs() << "for.end" << "\n";
          }

          for(int i = minIndex; i < maxIndex; i++) {

          }
        }
      }
      return false;
    }

  };
}

//initialize identifier
char Hw1::ID = 0;
//"Hw1" is the name of pass
//"Hw1 pass written by yenchunli" is the explaination of your pass
static RegisterPass<Hw1> GS("Hw1", "Hw1 pass written by yenchunli");
