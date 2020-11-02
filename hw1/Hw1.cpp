#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/ADT/Statistic.h"

#include <vector>

using namespace llvm;

namespace {

  std::map<std::string, std::string> variableMap;
  std::map<std::string, int> valueMap;

  int maxIndex;
  int minIndex;
  std::string arrayName;
  std::string arrayIdx;

  struct arrayData {
    int mul;
    int add;
    std::string arrayName;
    std::string arrayX;
  };

  struct finalData {
        std::string arrayName;
        int value;
        int order;
        int isLeft;
      };

  std::vector<finalData> vecFinal;

  std::vector<arrayData> v1;
  std::map<std::string, arrayData> idxMap;

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
            //errs() << I->getName() << " " << name << "\n";
            //tmp->dump();
            arrayName = name;
            arrayIdx = I->getName();
            
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
          
          arrayData node;

          //int addValue = 0;
          //int mulValue = 1;

          // for.body is the start of the for block
          if(!BB->getName().find("for.body", 0)) {
            errs() << "for.body" << "\n";
            for (BasicBlock::iterator itrIns = (*BB).begin(); itrIns != (*BB).end(); itrIns++){
              
              int value;
              std::string name;
              switch(itrIns->getOpcode())
              {
                
                case Instruction::Load:
                {
                  Value *tmp1 = itrIns->getOperand(0);
                  //Value *tmp2 = itrIns->getOperand(1);

                  errs() << "Load\n" << "tmp1=" << *tmp1 << "\n";

                  errs() << "getName=" << tmp1->getName() << "\n";
                  
                  if(!tmp1->getName().find("i", 0)) {
                    node.add = 0;
                    node.mul = 1;
                    node.arrayName = "Z";
                    node.arrayX = "i";
                  }
                  else if(Instruction *I = dyn_cast<Instruction>(tmp1)) {
                      if(I->getOpcode() == Instruction::GetElementPtr) {
                          Value *tmp3 = I->getOperand(0);
                          arrayName = tmp3->getName();
                          //variableMap[I->getName()] = name;
                          //errs() << "arrayName=" << arrayName << "\n";
                          node.arrayName = arrayName;
                          errs() << "node=" << node.add << node.mul << node.arrayName << node.arrayX << "\n";
                          idxMap[tmp1->getName()] = node;
                          
                      }
                  }
                  
                  break;
                }
                case Instruction::Sub:
                {
                  //
                  Value *tmp1 = itrIns->getOperand(0);
                  Value *tmp2 = itrIns->getOperand(1);
                  errs() << "=======================================\n";
                  errs() << "Sub\n" << "tmp1=" << *tmp1 << "\ntmp2=" << *tmp2 << "\n";
                  
                  
                  if(ConstantInt* Integer = dyn_cast<ConstantInt>(tmp2)){
                    value = Integer->getZExtValue();
                    //errs() << "value=" << value << "\n";
                  }

                  if(Instruction *I = dyn_cast<Instruction>(tmp1)) {
                    Value *tmp3 = I->getOperand(0);
                    name = tmp3->getName();
                  }
                  
                  errs() << name <<" = " << name << " - " << value << "\n";
                  node.add -= value;
                  errs() << "=======================================\n";
                  break;
                }
                case Instruction::Store:
                {
                  errs() << "============================\nStore\n";

                  //tmp1 = tmp2 in c code.
                  Value *tmp1 = itrIns->getOperand(0);
                  Value *tmp2 = itrIns->getOperand(1);
                  errs() << "=======================================\n";
                  errs() << "store\n" << "tmp1=" << *tmp1 << "\ntmp2=" << *tmp2 << "\n";
                  errs() << "=======================================\n";

                  if(!tmp1->hasName()) {
                    getLoadDef(tmp1);
                    //errs() << "arrayName=" << arrayName << "\n";
                    //left.arrayName = arrayName;
                    
                    //errs() << "arrayIdx=" << arrayIdx << "\n";
                    //errs() << "tmp1=" << *tmp1 << "\n";
                    errs() << "left " << idxMap[arrayIdx].arrayName << "[" << idxMap[arrayIdx].mul << "*" << idxMap[arrayIdx].arrayX << "+" << idxMap[arrayIdx].add << "]\n";
                    v1.push_back(idxMap[arrayIdx]);
                  }

                  if(Instruction *I = dyn_cast<Instruction>(tmp2)) {
                    if(I->getOpcode() == Instruction::GetElementPtr) {
                        Value *tmp3 = I->getOperand(0);
                        arrayName = tmp3->getName();
                        //variableMap[I->getName()] = name;
                        //errs() << "arrayName=" << arrayName << "\n";
                        node.arrayName = arrayName;
                        errs() << "node=" << node.add << node.mul << node.arrayName << node.arrayX << "\n";
                        idxMap[tmp2->getName()] = node;
                        
                    }
                  }
                  
                  getLoadDef(tmp2);
                  errs() << "right " << idxMap[arrayIdx].arrayName << "[" << idxMap[arrayIdx].mul << "*" << idxMap[arrayIdx].arrayX << "+" << idxMap[arrayIdx].add << "]\n";
                  v1.push_back(idxMap[arrayIdx]);
                  //errs() << "arrayName=" << arrayName << "\n";
                  //right.arrayName = arrayName;
                  
                  //errs() << "arrayIdx=" << arrayIdx << "\n";
                  //errs() << "tmp1=" << *tmp1 << "\n";
                  
                  /*
                  if(Instruction *I = dyn_cast<Instruction>(tmp2)) {
                      if(I->getOpcode() == Instruction::GetElementPtr) {
                          tmp1 = I->getOperand(0);
                          arrayName = tmp1->getName();
                          //variableMap[I->getName()] = name;
                          errs() << "arrayName=" << arrayName << "\n";
                          right.arrayName = arrayName;
                      }
                  }
                  */

                  //v1.push_back(left);
                  //v1.push_back(right);
                  
                  errs() << "======================\n";
                  
                  break;
                }
              }
              
              
            }

          }
          // for.end is the start of the for block
          if(!BB->getName().find("for.end", 0)) {
            errs() << "for.end" << "\n";
          } 
        }
      }

      
      

      for(int i=minIndex; i < maxIndex; i++){
        for(std::vector<arrayData>::iterator it = v1.begin(); it != v1.end(); it++) {
          errs() << it->arrayName << "[" << it->mul*i + it->add << "] ";
          
          finalData d;
          d.arrayName = it->arrayName;
          d.value = it->mul*i + it->add;
          d.order = i;
          d.isLeft = 1;
          vecFinal.push_back(d);
          it++;
          
          errs() << it->arrayName << "[" << it->mul*i + it->add << "]\n";
          
          finalData d2;
          d2.arrayName = it->arrayName;
          d2.value = it->mul*i + it->add;
          d2.order = i;
          d2.isLeft = 0;
          vecFinal.push_back(d2);
          it++;
          
        }
      }
      
      // check flow dependency
      
      for(auto &v1 : vecFinal) {
        for(auto &v2 : vecFinal) {
          if(v1.arrayName == v2.arrayName && v1.value == v2.value && v1.order < v2.order)
            errs() << "[flow   dependency] i=" << v1.value << " --> " << v2.value << "\n";
            
        }
      }
      
      
      for(auto &v1: vecFinal) {
        errs() << v1.arrayName << v1.value << v1.order << v1.isLeft << "\n";
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
