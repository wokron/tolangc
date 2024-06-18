## tolangc 中间代码 LLVM 实现设计说明

### 文法涉及的 llvm 语法结构

1. 全局变量 `@i1 = dso_local global`

2. 函数块 `define dso_local i32 @add(i32 %0, i32 %1) {}`

3. 指令集 Instrunction：

| 指令类型                      | 包含的llvm指令形式                                           |
| ----------------------------- | ------------------------------------------------------------ |
| UnaryInstruction：1个操作数   | **+ - ! number**<br />**load** i32, i32* %1                  |
| BinaryInstruction：两个操作数 | **a + - *  / % b**,<br />**a >  <  >= <= == != b**<br />**store** i32 %0, i32* %1 |
| Instrunction: 其他指令        | **alloca** i32<br />**ret** i32 %7<br />**call** i32 @add(i32 @i1, i32 @i2)<br />call i32 @**get()**<br />call void @**put**(i32 %5) |

4. 局部变量/字面量

### 架构设计

根据涉及的语法结构，我们可以把语法成分粒度从高到低划分为：

1. 整个模块 Module
2. 函数 Function 
3. 基本代码块 BasicBlock 
4. 指令 Instruction 
5. 变量/常量 && 符号
6. 变量：参数、全局变量、局部变量；常量：全局常量，字面量

对于模块中不同粒度的所有语法成分，我们借鉴LLVM官方库的实现方法，将其全部统一成 ValueRegTy 结构。User类是Value 的子类，用于根据层次粒度关系，双向索引语法结构的上下级关系。

<img src="figure\llvm-0-3.png" alt="llvm-0-3"  />



```llvm
A: %add = add nsw i32 %a, %b
B: %add1 = add nsw i32 %a, %b
C: %add2 = add nsw i32 %add, %add1
```

比如，以上这段实例代码中，A 这一条语句被看作是一个Instruction，这个 Instruction（继承自User）是一个 User（继承自Value，在这条Instruction中这个Value就是 %add），他 Use 了 a 和 b 这两个 ValueRegTy，与此同时他又被 C 这条 Instruction 所Use。

Value中主要的属性值设计：

`````c++
class ValueRegTy {
protected:
    TypePtr _type;
    std::string _name;

    UseList _useList;
    UseList _userList;

  private:
    ValueType _valueType;
};
`````

可以看到，基本Value类中的几个重要属性成员分别为：语法类型 ValueType，值类型指针 TypePtr，

接下来我们围绕 value 成员讲解总体架构设计。

#### 语法类型 ValueType

**`ValueType _valueType;`**中，ValueType 是 ValueRegTy 的类型枚举。几乎所有语法结构都是Value的子类，不同子类通过修改 **_valueType** 值区分：

```c++
enum class ValueType {
    // ValueRegTy
    ArgumentTy, //参数
    BasicBlockTy, //基本块

    // ValueRegTy -> Constant
    ConstantTy, //常量标识符
    ConstantDataTy, //字面量

    // ValueRegTy -> Constant -> GlobalValue
    FunctionTy,
    GlobalVariableTy,

    // ValueRegTy -> User -> Instruction
    BinaryOperatorTy,
    CompareInstTy,
    BranchInstTy,
    ReturnInstTy,
    StoreInstTy,
    CallInstTy,
    InputInstTy,
    OutputInstTy,
    AllocaInstTy,
    LoadInstTy,
    UnaryOperatorTy,
};
```

#### 值类型 Type

**`TypePtr _type;`** 记录的是返回值类型，与 valueType 不属于一个系统：

- valueType是我们自行实现的 llvm 架构中的数据类型表示

- type 记录的是llvm语法结构中的数据类型

>比如对于指令对象 `%add = add i32 %a, %b`，返回值类型为 %add 的类型 i32，值类型为二元操作数指令类型 BinaryOperatorTy

type记录的数据类型包括：

````c++
enum TypeID {
    // Primitive types
    VoidTyID,
    LabelTyID,

    // Derived types
    IntegerTyID,
    FunctionTyID,
    PointerTyID
};
````

#### 索引 Use & User

通过 User 和 ValueRegTy 的配对，索引Value间的使用和被使用关系。

目前Use关系只用于记录指令和操作数的关系。每条指令建立时，将操作数作为 Usee，指令作为User，创建Use对象，将Use对象同时保存在全局记录和 User中的 uselist 属性中。指令的每个操作数都是一个Use关系，每个User的Use关系数量不定（一条指令中的操作数数量不定，比如参数调用中参数个数不限）。

我们使用 LlvmContext 语法结构记录全局的 Use 关系和 语法成分 Type。每个编译单元Module中只有一个 LlvmContext：

```
class LlvmContext {
    std::vector<FunctionTypePtr> _functionTypes;
    std::vector<PointerTypePtr> _pointerTypes;

    std::vector<ValuePtr> _values;
    std::vector<UsePtr> _uses;
};
```

LlvmContext 只存储是对象的指针（引用），不关心对象的具体名称。

记录对象之间的索引关系为之后的代码优化提供便利，让我们能查找到某个变量的所有使用地点，更方便的进行变量替换。设计的代码优化方法有活跃变量分析、公共子表达式删除等。

#### 名称记录

**`std::string _name;`** 中记录语法结构的名称。名称记录只在**全局变量**（GlobalVariable, Function）中有意义。

**局部变量**的名称在LLVM中没有意义，因为在翻译过后需要用数字对虚拟寄存器重新命名，按出现顺序为寄存器寄存器编号。翻译过程中，通过Use保存域内各个语法结构的指针，记录成员，语法对象的保存和索引与语法对象的名称无关。因此，分配虚拟寄存器编号可以推迟到代码打印的部分。

在我们的实现中，SlotTracker 类作为一个工具类，在中间代码打印阶段为一个函数域记录和分配虚拟寄存器。

```
class SlotTracker final {
	std::unordered_map<ValuePtr, int> _slot;
};
```

在打印前使用 SlotTracker 类中的 SlotTracker::Trace方法，按出现顺序建立 ValuePtr 和编号值之间的索引；打印时通过 SlotTracker::Slot 方法获取对应寄存器的编号。

### LLVM文件结构说明

include/llvm/ir:

```
+---asm
|       AsmWriter.h     //存储中间代码字符串
|       AsmPrinter.h    //打印 writer 中存储的结构化字符串
\---ir
    |   IrForward.h     // 各个语法结构前向声明（防止交叉引用报错）：定义指针类型
    |   Llvm.h          // 罗列LLVM翻译过程中给所需的头文件集合
    |   Module.h        // LLVM的一个编译单元，按文法定义存储：函数列表、main函数、上下文语义记录LlvmContext
    |   LlvmContext.h   // LLVM 编译单元Module的语义记录，保存类型字典、Use关系（一个Module对应一个LlvmContext）
    |   SlotTracker.h   // 为虚拟寄存器编号和输出的工具类
    |   Type.h          //定义LLVM的对象类型和类型子类，类型包括：Void, Label, Integer, Function, Pointer
    \---value           //LLVM 翻译中自行定义的语法单元
        |   ValueRegTy.h         //所有语法单元的父类，主要记录语法单元类型和Use关系
        |   Use.h           // 一个Use对象记录一个User和Value对
        |   User.h          // Value的子类
        |   ArgRegTy.h      // 参数 ValueRegTy
        |   BasicBlock.h    // 代码块 ValueRegTy
        |   Constant.h      // 常量 ValueRegTy
        |   ConstantData.h  // 字面量 ValueRegTy -> Constant
        |   GlobalValue.h   // 全局变量 ValueRegTy -> Constant
        |   Function.h      // 函数 ValueRegTy -> Constant -> GlobalValue
        \---inst            // 指令 ValueRegTy -> User
                Instruction.h           // 指令基本类
                InstructionTypes.h      // 定义存在类型区分的指令类：UnaryInstruction -> UnaryOperator,BinaryInstruction -> BinaryOperator & CompareInstruction
                Instructions.h          // 定义LLVM固有指令：AllocaInst, LoadInst, StoreInst, BranchInst, JumpInst, ReturnInst, CallInst
                ExtendedInstructions.h  // 定义外部库指令：InputInst, OutputInst
                
```