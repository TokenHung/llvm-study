# HW1 Loop Dependence

## Code Analysis

First, we split `test1.ll` into 5 sections. They are:

- [entry](###entry)
- [for.cond](###for.cond)
- [for.body](###for.body)
- [for.inc](###for.inc)
- [for.end](###for.end)

We can use `BB->getName().find()` to find the block title. For example, if we want to find `.entry`, we can use:
```c
if(!BB->getName().find("entry", 0)) //matched
```

### entry

> Block `entry` initializes the variables which are not put inside the for-loop.

The instruction `store i32 4, i32* %i, align 4` acts to store integer 4 to register %1. We can iterate the instructions and filter `store` with the method `getOpcode()`. For example:
```c
switch(itrIns->getOpcode()) {
  case Instruction::Store:
  {
  // more code
  break;
  }
}
```

Once we fetch `Instruction::Store`, we can use `getOperand()` to get the left/right side of intructions.
```c
Value *tmp1 = itrIns->getOperand(0); // tmp1 = 
Value *tmp2 = itrIns->getOperand(1); // tmp2 = 
```

`tmp1` stores the integer value, we can use `getZExtValue()` to get the value.
```c
if(ConstantInt* Integer = dyn_cast<ConstantInt>(tmp1)){
  value = Integer->getZExtValue();
  //errs() << "value=" << value << "\n";
}
```

`tmp2` stores the instructions `???`, we can use `getName()` to get which variable it stores in.
```c
if(Instruction *I = dyn_cast<Instruction>(tmp2)){
  name = I->getName();
  //errs() << "name=" << name << "\n";
}
```

#### test1.ll entry block

```c=
// int i, A[20], C[20], D[20];
// i = 4;
entry:
  %retval = alloca i32, align 4
  %i = alloca i32, align 4
  %A = alloca [20 x i32], align 16
  %C = alloca [20 x i32], align 16
  %D = alloca [20 x i32], align 16
  store i32 0, i32* %retval, align 4
  store i32 4, i32* %i, align 4
  br label %for.cond
```

### for.cond

> Block `for.cond` initialize the for-loop condition block.

we fetch the instruction `icmp` to get the for-loop uppder bound. Then uses `getOperand(1)` and `getZExtValue()` to get the max index of the for-loop.
```c
if(!strcmp("icmp", itrIns->getOpcodeName())) {
  if(ConstantInt* Integer = dyn_cast<ConstantInt>(itrIns->getOperand(1))) {
      maxIndex = Integer->getZExtValue();
      //errs() << "maxIndex=" << maxIndex << "\n";
  }
}
```

#### test1.ll block for.cond

```c=
// %0 = i (i=4)
// %cmp = i < 20
// if(%cmp < 20)   jump to %for.body
// else            jump to %for.end
for.cond:           ; preds = %for.inc, %entry
  %0 = load i32, i32* %i, align 4
  %cmp = icmp slt i32 %0, 20
  br i1 %cmp, label %for.body, label %for.end
```

### for.body

> Block `for.body` executes the core logic of the for-loop.

There are several instructions we might encounter.
- load
- store
- add
- sub
- mul

#### Instructions::Load

There are two conditions of `load` we face:

1. `load i32, i32* %i, align 4`         
2. `load i32, i32* %arrayidx, align 4`

> Condition 1: `load %i` means we are going to initialize an array.
```c
if(!tmp1->getName().find("i", 0)) {
  node.add = 0;
  node.mul = 1;
  node.arrayName = "Z";
  node.arrayX = "i";
}
```

> Condition 2: load %arrayidx means we finally complete all the calculation of array index.
```c
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
```

#### Instruction::Store

#### Instruction::Add

#### Instruction::Mul

#### Instruction::Sub






```c=
for.body:                          ; preds = %for.cond

  // Calculate C[i]
  // %1 = %i
  // %idxprom = %1
  // %arrayidx = &C[%idxprom]
  // %2 = C[%arrayidx]
  %1 = load i32, i32* %i, align 4
  %idxprom = sext i32 %1 to i64
  %arrayidx = getelementptr inbounds [20 x i32], [20 x i32]* %C, i64 0, i64 %idxprom
  %2 = load i32, i32* %arrayidx, align 4
  
  // Calculate A[i]
  // %3 = %i
  // %idxprom = %3
  // %arrayidx = &A[%idxprom]
  // %4 = A[%arrayidx2] 
  %3 = load i32, i32* %i, align 4
  %idxprom1 = sext i32 %3 to i64
  %arrayidx2 = getelementptr inbounds [20 x i32], [20 x i32]* %A, i64 0, i64 %idxprom1
  store i32 %2, i32* %arrayidx2, align 4
  
  // Calculate A[i-4]
  // %4 = %i
  // %sub = %4 - 4
  // %idxprom3 = %sub
  // %arrayidx4 = &A[%idxprom3]
  // %5 = A[%arrayidx4]
  %4 = load i32, i32* %i, align 4
  %sub = sub nsw i32 %4, 4
  %idxprom3 = sext i32 %sub to i64
  %arrayidx4 = getelementptr inbounds [20 x i32], [20 x i32]* %A, i64 0, i64 %idxprom3
  %5 = load i32, i32* %arrayidx4, align 4
  
  // Calculate D[i]
  // %6 = %i
  // %idxprom5 = %6
  // %arrayidx6 = &D[%idxprom5]
  // %5 = A[%arrayidx6] 
  %6 = load i32, i32* %i, align 4
  %idxprom5 = sext i32 %6 to i64
  %arrayidx6 = getelementptr inbounds [20 x i32], [20 x i32]* %D, i64 0, i64 %idxprom5
  store i32 %5, i32* %arrayidx6, align 4
  br label %for.inc
```

### for.inc

```c=
for.inc:                                          ; preds = %for.body
  // %7 = %i
  // %inc = %7 + 1
  // %i = %inc
  // jump to %for.cond
  %7 = load i32, i32* %i, align 4
  %inc = add nsw i32 %7, 1
  store i32 %inc, i32* %i, align 4
  br label %for.cond
```

```c=
int main(void) {
    int i, A[20], C[20], D[20];

    for(i = 4; i < 20; i++) {
        A[i] = C[i];
        D[i] = A[i-4];
    }

    return 0;
}
```
