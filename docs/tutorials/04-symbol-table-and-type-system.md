# 符号表与类型系统

## 符号表

在介绍中间代码之前，我们先学习符号表的生成和管理。符号表是编译过程中的一个重要结构，主要用于记录各个符号的标识以及相应的信息，包括名称、作用域、类型、大小、维度、存储地址、行数等各项信息，目的是在编译过程中遇到对应符号时即可快速查询到相关信息。要注意，符号表的设计与使用将贯穿后续的全部实验流程，请同学们三思而后行！

简单来说，该模块我们需要考虑以下三个功能：
1. 符号表的生成
2. 符号的插入
3. 符号的查找

### 符号表的生成

在编译过程中，遇到变量声明定义的语句时，我们就可以把对应的信息填入符号表。对于一张符号表来说，通常具有如图的结构：一个指向外层作用域符号表的指针pre，表主体（符号名与对应信息），若干指向内层作用域符号表的指针next。此外，在编译的过程中，有一个指向当前作用域符号表的指针cur。

<img src="imgs/chapter04/symbol_table.png" alt="parser_3" style="zoom:40%;" />

符号表的生成主要有以下几个操作：

1. 初始时，创建一个全局变量符号表，也是最外层作用域符号表，cur指向该符号表。
2. 编译时，遇到变量声明语句，解析出需要的信息（一般包括类型、维度、大小等），填入cur指向的符号表。
3. 编译时，进入新的作用域（block），生成新的符号表，设置cur指向的符号表的next指针指向新符号表，新符号表的pre设置为cur，然后将cur指向新符号表，后续会在新符号表上填入信息。
4. 编译时，离开作用域（block），通过cur指向的符号表的pre指针回溯至外层符号表，并对应修改cur指针。

不难发现，这样生成的符号表，具有树结构。当然，也可以按栈式结构来组织，即进入新的作用域，就压入新的符号表，离开作用域时，弹出当前符号表，但是这种方法不能保存下使用过的符号表的信息。

### 符号的插入
*标识符和符号通过其共同的名称直接关联*

**符号的插入**也就是将我们声明的标识符转化为符号单元，添加到**当前**的符号表中。尽管标识符类型多样（常量与否，全局变量与否），然而允许声明标识符的位置显而易见，在这个阶段我们无需关注其语义，仅需关注**标识符名称**即可，
我们都知道，同一个作用域下是不允许出现两个同名标识符的，出现这种情况时，应按照我们的要求进行错误处理。

因此，我们的符号表应当具备：

1. 在标识符声明时，判断该标识符名称在当前作用域下是否重复
2. 在标识符声明时，若满足操作1，则将其转化为符号并插入当前符号表；若不满足，说明该标识符重复声明，报标识符重复声明错误

### 符号的查找

在编译的过程中，如果遇到一个标识符的引用，就需要确定该标识符是否定义过，以及其相关信息，此时我们需要在符号表内查找该符号。
请联想平时的编程语言，比如我们可以在函数内部使用全局变量等，因此，若当前作用域无待查询符号，应当沿符号表树向上递归查找最近的符号，直至最外层的符号表。

具体而言，我们的符号表应当具备：

1. 在使用标识符时，在当前作用域符号表内查询是否有符号记录，有则返回其信息，无则继续操作2
2. 利用当前作用域符号表的pre指针，访问外层作用域符号表。如果当前作用域符号表已为最外层符号表（pre指针为空），说明该标识符未定义，报标识符未定义错误；否则，进入外层作用域符号表，进行操作1

### 符号表与符号的设计

这部分的设计好坏很大程度会影响后续的实验体验，然而大家尚未接触中间代码生成，对符号表应该存什么内容想必会一头雾水（不用怀疑自己，作者当初也是）。那么作者给大家的建议是，先按照题目的输出要求实现，并留好充足的可扩展接口，写后续的实验时刻保持思考，考虑符号表的设计可以如何优化调整。

最后给出一些值得参考的思路，符号表中存储的信息需要尽可能**详细但不冗余**，以方便后续的查找使用，例如对于数组的信息，可以考虑将每个维度的大小都记录下来，同时由于数组维度一定是常数表达式，可以直接计算出维数大小，方便后续访问数组元素时获取维度信息；对于函数名，可以记录函数所需参数个数，各个参数类型，返回值类型等；同时注意区分单值/数组/指针，常量/变量，全局/局部，以及整数/字符类型等等。

## 类型系统

在今年的实验课程文法中，除代代相传的int类型外，我们又额外加入了char类型（与此同时为减小难度，删除了二维数组），当面临多种数据类型时，有必要聊一下我们的**类型系统**。

类型系统定义了如何将程序中的值和表达式分类为不同的类型，以及如何操作这些类型，它主要目标是确保程序的类型安全性，即在运行时不会发生类型错误，此时便需要类型系统提供完善的隐式/强制类型转换与异常处理机制。

我们的编译器类型系统较为简单，仅涉及char到int与int到char的隐式转换，也并不涉及异常处理。 需要明确的是，这种”转换“一定发生在赋值时，如文法文档所述，我们规定，每当使用char类型时，必须将其由8位有符号扩展为32位，每当赋值char类型时，必须将待赋值的32位截断至后8位。至于char类型如何存储，请各位同学八仙过海各显神通，如此简单，你学废了么？

更加复杂的类型系统设计鼓励同学们自行探索！

## 示例编译器的符号表设计

在这一部分，我们对示例编译器的符号表部分进行简要解析。

```c++
struct Symbol {
    SymbolType type;
    std::string name;
    int lineno;
    ValuePtr value;

    virtual ~Symbol() = default;

    Symbol(SymbolType type, std::string name, ValuePtr value, int lineno)
        : type(type), name(name), value(value), lineno(lineno) {}
};
```

这里定义了所有符号的父类，内部存放类型，名称，行号，LLVM用到的Value等必须用到的信息。

```c++
struct VariableSymbol : public Symbol {
    VariableSymbol(std::string name, ValuePtr value, int lineno)
        : Symbol(SymbolType::VAR, name, value, lineno) {}
};

struct TagSymbol : public Symbol {
    BasicBlockPtr target;
    std::vector<InstructionPtr> jump_insts;

    TagSymbol(std::string name, ValuePtr value, int lineno)
        : Symbol(SymbolType::TAG, name, value, lineno) {}
};

struct FunctionSymbol : public Symbol {
    int params_count = 0;

    FunctionSymbol(std::string name, ValuePtr value, int lineno,
                   int params_count)
        : Symbol(SymbolType::FUNC, name, value, lineno),
          params_count(params_count) {}
};
```

这里分别定义了符号的三个子类，根据子类符号的不同特性会存储其特有的信息，例如在Tag符号内，我们存储了该Tag对应的基本块的指针target(用于后续LLVM生成)，
在Func符号内，我们存储了函数参数数目等信息。

```c++
class SymbolTable : public std::enable_shared_from_this<SymbolTable> {
public:
    SymbolTable() : _father(nullptr) {} 
    
    bool exist_in_scope(const std::string &name) {}  // 判断名称是否已处于当前作用域符号表
    
    bool add_symbol(std::shared_ptr<Symbol> symbol) {}  // 将符号添加至当前作用域符号表
    
    std::shared_ptr<Symbol> get_symbol(const std::string &name) {}  // 根据名称获取符号
    
    std::shared_ptr<SymbolTable> push_scope() {}  // 创建并返回一个新的作用域符号表
    
    std::shared_ptr<SymbolTable> pop_scope() {}  // 退出当前作用域符号表并返回上一级作用域符号表

private:
    std::unordered_map<std::string, std::shared_ptr<Symbol>> _symbols;  // 存储符号信息
    
    std::shared_ptr<SymbolTable> _father;  // 记录父级作用域符号表
};
```

这里定义了符号表类，我们使用栈式结构组织，以"推入"+"弹出"的方式实时维护当前解析位置的符号表，实现了教程所述的全部功能，详情请自行查看源码。