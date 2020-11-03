# HW1 Loop Dependence

106062209 Yen-Chun Li

## Environment

I write a dockerfile to set up the llvm environment. You can run the following commands to build docker and enter it.
```bash
docker build -t llvm9 .
docker run -t llvm9
```

## Code Analysis

### Global variables

```c
std::map<std::string, int> valueMap;  // used in for.cond

int maxIndex;  // for loop max index
int minIndex;  // for loop min index
std::string arrayName;  // store the array name loaded by getLoadDef()
std::string arrayIdx;   // store the array index string loaded by getLoadDef()

// arrayData stores the array details
struct arrayData {
  int mul;                        // number to multiply
  int add;                        // number to add/sub
  std::string arrayName;          // array name
  std::string arrayX;             // iterator name (ex: i)
};

std::vector<arrayData> v1;        // store the arrayData, which will be analyzed
std::map<std::string, arrayData> idxMap; // store the arrayidx mapping data, so we can find the arrayidx number by checking this map
```


### Stuctures

For example, we can split `test1.ll` into 5 blocks. They are:

- [entry](####entry)
- [for.cond](###for.cond)
- [for.body](###for.body)
- [for.inc](###for.inc)
- [for.end](###for.end)

We can use `BB->getName().find()` to find the block title. For example, if we want to find `.entry`, we can use:
```c
if(!BB->getName().find("entry", 0)) //matched
```

#### entry

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
Value *tmp1 = itrIns->getOperand(0); // i32 4
Value *tmp2 = itrIns->getOperand(1); // %i = alloca i32, align 4
```

`tmp1` stores the integer value, we can use `getZExtValue()` to get the value.
```c
if(ConstantInt* Integer = dyn_cast<ConstantInt>(tmp1)){
  value = Integer->getZExtValue(); // 4
}
```

`tmp2` stores the instructions `%i = alloca i32, align 4`, we can use `getName()` to get which variable it stores in.
```c
if(Instruction *I = dyn_cast<Instruction>(tmp2)){
  name = I->getName(); // i
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

| var or func          | value                           |
|-|-|
|itrIns                | %cmp = icmp slt i32 %0, 20      |
|itrIns->getOperand(0) | %0 = load i32, i32* %i, align 4 |
|itrIns->getOperand(1) | i32 20                          |

```c
if(!strcmp("icmp", itrIns->getOpcodeName())) {
  if(ConstantInt* Integer = dyn_cast<ConstantInt>(itrIns->getOperand(1))) {
    maxIndex = Integer->getZExtValue(); // 20
      
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

There are two conditions of `load`:

1. `load i32, i32* %i, align 4`         
2. `load i32, i32* %arrayidx, align 4`

And the llvm language also different

Here we first use `getOperand(0)` and store the value into `Value *tmp1`.
```c
Value *tmp1 = itrIns->getOperand(0);
```

We can find the two kind of tmp1:
1. `tmp1=  %i = alloca i32, align 4`
2. `tmp1=  %arrayidx = getelementptr inbounds [20 x i32], [20 x i32]* %C, i64 0, i64 %idxprom`

> **Condition 1**: `load %i` means we are going to initialize an array.
```c
if(!tmp1->getName().find("i", 0)) {
  node.add = 0;
  node.mul = 1;
  node.arrayName = "Z";
  node.arrayX = "i";
}
```

> **Condition 2**: load %arrayidx means we finally complete all the calculation of array index.
```c
if(Instruction *I = dyn_cast<Instruction>(tmp1)) {
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

We use `getOperand()` to get left/right side of `store` instruction.
```c
Value *tmp1 = itrIns->getOperand(0); // %5 = load i32, i32* %arrayidx4, align 4
Value *tmp2 = itrIns->getOperand(1); // %arrayidx6 = getelementptr inbounds [20 x i32], [20 x i32]* %D, i64 0, i64 %idxprom5
```

Check the tmp1 first, if `tmp1` `hasName()`, function `getLoadDef` help us find the original variable name recursively.
In `getLoadDef`, it will change two global variables, `arrayName` and `arrayIdx`, which means the variable mapping stored in `std::map idxMap`.

```c
if(!tmp1->hasName()) {
  getLoadDef(tmp1);
  v1.push_back(idxMap[arrayIdx]);
}
```

After checking `tmp1`, we now check `tmp2`. If `tmp2` is a instruction and its Opcode is `Instruction::GetElementPtr`, then we use `getOperand(0)` and `getName()` to get the variable name. We store the result into the `node.arrayName` and put into the idxMap which used `tmp2->getName()` as its key.
```c
if(Instruction *I = dyn_cast<Instruction>(tmp2)) {
  if(I->getOpcode() == Instruction::GetElementPtr) {
      Value *tmp3 = I->getOperand(0);
      arrayName = tmp3->getName();
      node.arrayName = arrayName;
      idxMap[tmp2->getName()] = node;
  }
}
```
    
We use `getLoadDef` to fetch the `tmp2` variable recursively and push into `std::vector v1`;
```c
getLoadDef(tmp2);
v1.push_back(idxMap[arrayIdx]);
```

#### Instruction::Add

First we use `getOperand()` to get left/right side of `add` instruction.
```c
Value *tmp1 = itrIns->getOperand(0);
Value *tmp2 = itrIns->getOperand(1);
```

For the right side, we use `getZExtValue()` to get the integer.
```c
if(ConstantInt* Integer = dyn_cast<ConstantInt>(tmp2)){
  value = Integer->getZExtValue();
}
```

For the left side, we first use `dyn_cast<Instruction>` to fetch. Then use `getOperand(0)` and `getName()` to get the variable.
```c
if(Instruction *I = dyn_cast<Instruction>(tmp1)) {
  Value *tmp3 = I->getOperand(0);
  name = tmp3->getName();
}
```

Finally, we calculate the value and store it back to the current node and update `node.add`.
```c
node.add += value;
```

#### Instruction::Sub

Almost same as `Instruction:Add`. The only difference is store the negative value back to the current node.
```c
node.add -= value;
```

#### Instruction::Mul

Some difference when we use `getOperand()`.
```c
Value *tmp1 = itrIns->getOperand(0);
Value *tmp2 = itrIns->getOperand(1);
```

The left/right side is different from `Instruction::Add` and `Instruction::Sub`.

Finally we store the value back to the current node and update `node.mul`.
```c
node.mul *= value;
```








```c
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

In general, we need to process `for.inc` to get the iterator. But this time we can find that `test1.ll`, `test2.ll`, `test3.ll` are all increase by 1. We can skip this step.

```c
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

## Dependency Analysis

We define a struct `finalData` to store the proccessed array data.
We declare a vector `vecFinal` to store all the generated array data.

| struct var | Usage |
|-|-|
| arrayName | Array Name |
| value     | actual value of a*i+b |
| order     | for-loop index  | 
| isLeft    | on the left side of equation mark |

```c
struct finalData {
  std::string arrayName;
  int value;
  int order;
  int isLeft;
};

std::vector<finalData> vecFinal;
```

Now we start to generate corresponding array data and push into `vecFinal`.
```c
for(int i=minIndex; i < maxIndex; i++){
  for(std::vector<arrayData>::iterator it = v1.begin(); it != v1.end(); it++) {
    finalData d;
    d.arrayName = it->arrayName;
    d.value = it->mul*i + it->add;
    d.order = i;
    d.isLeft = 0;
    vecFinal.push_back(d);
    it++;
    
    finalData d2;
    d2.arrayName = it->arrayName;
    d2.value = it->mul*i + it->add;
    d2.order = i;
    d2.isLeft = 1;
    vecFinal.push_back(d2);
  }
}
```

We check the three dependency here. Take a example.

```c
A[4] = C[4]    // order = 4
D[4] = A[0]    // order = 5
C[4] = A[1]    // order = 6
D[4] = A[4]    // order = 7
```

The `finalData` of all array is:

| order  | Array  | isLeft |
|-|-|-|
|4|A[4]|1|
|4|C[4]|0|
|5|D[4]|1|
|5|A[0]|0|
|6|C[4]|1|
|6|A[1]|0|
|7|D[4]|1|
|7|A[4]|0|


```c
for(auto &v1 : vecFinal) {
  for(auto &v2 : vecFinal) {
    
    if(v1.isLeft == 1 && v2.isLeft == 0 && v1.arrayName == v2.arrayName && v1.value == v2.value && v1.order <= v2.order){
      errs() << "[flow   dependency] i=" << v1.order << " --> " << v2.order << "\n";
    }
    
    if(v1.isLeft == 1 && v2.isLeft == 1 && v1.arrayName == v2.arrayName && v1.value == v2.value && v1.order < v2.order){
      errs() << "[output dependency] i=" << v1.order << " --> " << v2.order << "\n";
    }
    if(v1.isLeft == 0 && v2.isLeft == 1 && v1.arrayName == v2.arrayName && v1.value == v2.value && v1.order < v2.order){
      errs() << "[anti   dependency] i=" << v1.order << " --> " << v2.order << "\n";
    }
    
  }
}
```

## Output

```bash
[flow   dependency] A[4] i=4 --> 8
[flow   dependency] A[5] i=5 --> 9
[flow   dependency] A[6] i=6 --> 10
[flow   dependency] A[7] i=7 --> 11
[flow   dependency] A[8] i=8 --> 12
[flow   dependency] A[9] i=9 --> 13
[flow   dependency] A[10] i=10 --> 14
[flow   dependency] A[11] i=11 --> 15
[flow   dependency] A[12] i=12 --> 16
[flow   dependency] A[13] i=13 --> 17
[flow   dependency] A[14] i=14 --> 18
[flow   dependency] A[15] i=15 --> 19
===test2.ll===
[anti   dependency] A[13] i=2 --> 6
[anti   dependency] A[19] i=3 --> 8
[anti   dependency] A[25] i=4 --> 10
[anti   dependency] A[31] i=5 --> 12
[anti   dependency] A[37] i=6 --> 14
[anti   dependency] A[43] i=7 --> 16
[anti   dependency] A[49] i=8 --> 18
[anti   dependency] A[55] i=9 --> 20
[anti   dependency] A[61] i=10 --> 22
[anti   dependency] A[67] i=11 --> 24
[anti   dependency] A[73] i=12 --> 26
[anti   dependency] A[79] i=13 --> 28
[anti   dependency] A[85] i=14 --> 30
[anti   dependency] A[91] i=15 --> 32
[anti   dependency] A[97] i=16 --> 34
[anti   dependency] A[103] i=17 --> 36
[anti   dependency] A[109] i=18 --> 38
[anti   dependency] A[115] i=19 --> 40
[anti   dependency] A[121] i=20 --> 42
[anti   dependency] A[127] i=21 --> 44
[anti   dependency] A[133] i=22 --> 46
[anti   dependency] A[139] i=23 --> 48
===test3.ll===
[output dependency] D[0] i=0 --> 1
[output dependency] D[1] i=1 --> 2
[flow   dependency] A[2] i=2 --> 2
[output dependency] D[2] i=2 --> 3
[anti   dependency] A[5] i=3 --> 5
[output dependency] D[3] i=3 --> 4
[anti   dependency] A[8] i=4 --> 8
[output dependency] D[4] i=4 --> 5
[anti   dependency] A[11] i=5 --> 11
[output dependency] D[5] i=5 --> 6
[anti   dependency] A[14] i=6 --> 14
[output dependency] D[6] i=6 --> 7
[anti   dependency] A[17] i=7 --> 17
[output dependency] D[7] i=7 --> 8
[output dependency] D[8] i=8 --> 9
[output dependency] D[9] i=9 --> 10
[output dependency] D[10] i=10 --> 11
[output dependency] D[11] i=11 --> 12
[output dependency] D[12] i=12 --> 13
[output dependency] D[13] i=13 --> 14
[output dependency] D[14] i=14 --> 15
[output dependency] D[15] i=15 --> 16
[output dependency] D[16] i=16 --> 17
[output dependency] D[17] i=17 --> 18
[output dependency] D[18] i=18 --> 19
```
