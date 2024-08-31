# 语义分析

在之前的阶段中，我们编写了词法分析和语法分析程序，实现了源代码到抽象语法树的转换，这意味着我们就此得到了结构化的语法信息；另外我们还定义了符号表，从而能够将某一处的语义存储并关联到程序的其他部分；最后我们还定义了中间代码的数据结构，让我们能够方便地构建并输出中间代码。而在这些之后，我们需要一个阶段将这些内容全部连接起来，这就是语义分析阶段。

根据语言的不同，语义分析阶段的目标也可能很不相同。例如 C 的语义分析阶段肯定与 SQL 的语义分析差别巨大。不过总体来说，语义分析阶段的思路还是类似的，那就是遍历语法树、维护符号表并构建中间代码（或者类似的翻译操作）。

## 一、遍历语法树

树的遍历各位同学当然不陌生。但是怎么样遍历才更好却依旧是一个值得思考的问题。假设我们现在用 `Node` 类表示树的节点：

```cpp
struct Node {
    int val;
    std::vector<Node*> children;
}
```

那么我们很容易想到两种遍历的方式。第一种便是在 `Node` 中增加一个方法 `visit`。

```cpp
struct Node {
    int val;
    std::vector<Node> children;

    void visit() {
        // do something, like `std::cout << val << std::endl;`
        for (auto child : children) {
            child.visit();
        }
    }
};
```

而第二种方式则是将 `Node` 作为参数。为了避免使用全局变量，我们需要创建一个新的类 `Visitor`。

```cpp
struct Visitor {
    void visit(Node &node) {
        // do something, like `std::cout << val << std::endl;`
        for (auto child : children) {
            visit(child);
        }
    }
};
```

那么就这两种方法来说，哪种更加适合抽象语法树的遍历呢？答案是第二种使用 `Visitor` 的方法。因为语义分析的关键是跨节点的语义信息传递。采取第一种方法时，信息被隔离在了每一个节点之中，想要传播只能作为 `visit` 函数的参数和返回值，沿着节点的遍历顺序进行传递。而对于第二种方法，类中所有方法都共享一个类作用域，这意味着信息不仅能沿着遍历顺序传递，还能**跨越**语法树的分支传播到需要的地方，而不需要经过层层调用。这种功能十分重要，因为实现上下文关系的关键，符号，就是跨越语法树分支存在的。

由于上面的例子只有 `Node` 一个类型，所以并没有完全展示第二种方法。接下来让我们以 tolangc 的代码为例。在 tolangc 中，语法分析部分定义在 `visitor.h` 中。其中对于每一个语法树节点，我们都定义了对应的访问函数。例如对 `FuncDef`，我们定义了 `void _visit_func_def(const FuncDef &node)`。另外对于语法树的根节点类型 `CompUnit`，我们对外暴露了 `void visit(const CompUnit &node)` 函数，由于外部调用进行语义分析。

由于所有的节点访问函数都处在同一个作用域中，因此我们可以将符号表设定为 `Visitor` 的成员。另外我们总是保存当前作用域的符号表，因此这里字段名设为 `_cur_scope`。

```cpp
// include/visitor.h

class Visitor {
public:
    // ...
    void visit(const CompUnit &node);

private:
    void _visit_func_def(const FuncDef &node);
    void _visit_var_decl(const VarDecl &node);
    // ...
    ValuePtr _visit_ident_exp(const IdentExp &node);
    // ...
    std::shared_ptr<SymbolTable> _cur_scope;
    // ...
};
```

通过共享类作用域，符号的定义和使用都可以方便地实现。我们只需要在访问函数变量或标签定义时调用 `_cur_scope->add_symbol`，在访问表达式中符号时调用 `_cur_scope->get_symbol` 即可。而如果我们使用第一种遍历方法，则不得不将 `_cur_scope` 顺着调用链条不断传递。除了符号之外，类似的需要跨越语法树分支的信息还包括 `continue` 和 `break` 的跳转位置、函数的返回值类型等等。

定义 `Visitor` 类之后，我们考虑访问函数的调用关系。在某一节点的访问函数 `_visit_xxx` 中，我们会递归地调用所有子节点的访问函数 `_visit_yyy1`、`_visit_yyy2`...。而调用的顺序则视情况而定，其基本原则是，按照中间代码生成的顺序不断访问对应子树。这一点将在后面部分详细介绍。

> 很容易观察到 `Parser` 和 `Visitor` 的相似之处。既然构建语法树和遍历语法树都采用递归调用的方式实现，那么我们也可以想象一个建立语法树和遍历语法树在同一次调用中实现的编译器。不过这需要更加精心的设计，同时并不利于理解。所以这里我们选择将语法分析与语义分析分为两个不同的阶段。

以 `Visitor` 中的 `_visit_func_def` 为例。`FuncDef` 节点中包括 `exp` 字段，而对于函数定义来说，我们只需要获取 `exp` 解析之后的返回值，因此我们应当将 `Exp` 的语义分析过程定义在另一个访问函数 `_visit_exp` 中，并通过函数返回获取函数定义所需要的信息。

```cpp
struct FuncDef : public Node {
    std::unique_ptr<Ident> ident;
    std::vector<std::unique_ptr<Ident>> func_f_params;
    std::unique_ptr<Exp> exp;
};
```

```cpp
void Visitor::_visit_func_def(const FuncDef &node) {
    // ...
    // get return val from _visit_exp
    auto val = _visit_exp(*node.exp);
    // ...
}
```

这里还有一点值得关注。让我们查看一下 `_visit_exp` 的定义。

```cpp
ValuePtr Visitor::_visit_exp(const Exp &node) {
    return std::visit(
        overloaded{
            [this](const BinaryExp &node) { return _visit_binary_exp(node); },
            [this](const CallExp &node) { return _visit_call_exp(node); },
            [this](const UnaryExp &node) { return _visit_unary_exp(node); },
            [this](const IdentExp &node) { return _visit_ident_exp(node); },
            [this](const Number &node) { return _visit_number(node); },
        },
        node);
}
```

此处我们使用了 C++17 中的 `std::visit`。其主要作用是根据 `std::variant` 的类型，选择调用对应参数类型函数。这一功能称为模式匹配。从这里我们也可以看出，在抽象语法树中，我们将具有共性的类型抽象为统一的节点。

当然，一些语言并不具有模式匹配这样的功能（比如说 Java），那又要如何实现 `_visit_exp` 这样的函数呢？如果是面向对象语言，则你的语法树一定会通过继承实现，因此有：

```cpp
struct Exp {
    virtual ~Exp() = default;
};
struct BianryExp : public Exp {/* ... */};
struct CallExp : public Exp {/* ... */};
struct UnaryExp : public Exp {/* ... */};
// ...
```

这时可以通过父类转子类判断具体的子类类型。在这种情况下，`_visit_exp` 可以被重写为：

```cpp
ValuePtr Visitor::_visit_exp(const Exp &node) {
    if (auto n = dynamic_cast<const BinaryExp*>(&node)) { return _visit_binary_exp(*n); }
    else if (auto n = dynamic_cast<const CallExp*>(&node)) { return _visit_call_exp(*n); }
    else if (auto n = dynamic_cast<const UnaryExp*>(&node)) { return _visit_unary_exp(*n); }
    // ...
    else { throw std::runtime_error("invalid type"); }
}
```

另一方面，如果你使用的是 C 这样底层的语言的话，也存在实现的方法。那就是利用 `enum` + `union`。通过 `enum` 区分不同子类，通过 `union` 选择数据解释方式。此时 `_visit_exp` 可以被重写为：

```c
enum ExpType {
    EXP_BINARY,
    EXP_CALL,
    EXP_UNARY,
};

struct Exp {
    enum ExpType type;
    union {
        struct BianryExp biany;
        struct CallExp call;
        struct UnaryExp unary;
    } data;
};

static int visit_exp(const struct Exp *node) {
    switch (node->type) {
    case EXP_BINARY:
        return visit_biany_exp(&node->data.biany);
    case EXP_CALL:
        return visit_call_exp(&node->data.call);
    case EXP_UNARY:
        return visit_unary_exp(&node->data.biany);
    //...
    }
}
```

## 二、维护符号表

### （1）添加符号的时机

在之前的章节中我们定义了符号表。符号表主要包含如下四个操作：新增作用域、删除作用域、添加符号、查询符号。

对于 tolang 来说，几乎不存在作用域的概念。因为 tolang 不存在控制流结构，同时函数之外也不存在全局变量。不过我们还是在定义函数时为函数参数创建了作用域，以便区分不同函数的参数。

这里需要注意的是函数名与其参数名处于不同的作用域中。然而为了获取函数签名，我们却又需要在新增作用域之前访问函数参数。不过这一问题并不困难，可以选择不同的处理方式。包括：

- 先遍历参数并创建参数符号。构建函数符号并插入后，之后新增作用域并插入之前创建的参数符号。
- 先创建函数符号并插入，新增作用域，遍历参数并插入参数符号，最后再根据遍历结果将函数符号补充完整。
- 在创建函数符号时遍历一次参数，新增作用域，并在插入参数符号时再遍历一次参数。
- ...

对于 tolangc 来说，我们使用的是第一种方法。当然由于在 tolang 中只有一种类型，所以函数签名可以简化为参数的个数。

```cpp
void Visitor::_visit_func_def(const FuncDef &node) {
    // ...
    
    std::vector<std::shared_ptr<VariableSymbol>> param_symbols;
    for (auto &ident : node.func_f_params) {
        // here we don't create ir argument, just create symbol
        param_symbols.push_back(std::make_shared<VariableSymbol>(
            ident->value, nullptr, ident->lineno));
    }
    //...

    auto symbol = std::make_shared<FunctionSymbol>(
        node.ident->value, _cur_func, node.lineno, param_symbols.size());

    if (!_cur_scope->add_symbol(symbol)) {
        // ....
    }
    // ..

    for (auto &param_symbol : param_symbols) {
        if (!_cur_scope->add_symbol(param_symbol)) {
            // ...
        }
        // ...
    }
    // ...
}
```

这些方法并无绝对的对错之分，只需选择合适的处理方法即可。不过从中我们可以体会到一些编译器设计时的技巧：一些信息并非在一开始就能够得知，可以选择先创建符号（**占位**），之后再补充具体信息（**回填**）。这一点在处理分支跳转时十分重要，也将在之后进行具体说明。

### （2）符号类型的构建

虽然在 tolang 中没有类型的概念，但是编译实验中却要求各位同学的编译器支持 int 和 char 两种类型。因此在语义分析部分我们有必要讨论一下类型的构建。

类型系统本身也是程序设计语言中十分重要的组成部分。在之前的一些章节中，我们讨论过类型在语法树中的表示，以及应当用什么样的数据结构表示类型。而在语义分析中，对于类型我们同样用之前的访问函数进行处理。假设现在有一子语法树表示源代码中的一处类型的定义，那么我们便需要定义一访问函数（例如 `_visit_type`），该函数接受子语法树的根作为参数，并应当以具体的类型作为返回值。

如果是基本类型，如 `int`、`float`，则这一过程十分简单。我们只需要进行一个语法树中类型到目标类型的映射即可。更为复杂的是复合类型的处理，包括数组、指针、结构体等等。

复合类型是类型的组合，能够形成复杂的嵌套结构。但是这种结构又是容易处理的，因为类型总是递归地定义，正和语义分析的解析过程相符。例如 `int[2][3]`，其准确含义并部署一个 `2 x 3` 的二维数组，而是一个长度为 `2` 的数组，其类型为 `int[3]`。因此我们在解析的时候，应当先解析 `int[3]` 的类型，而后将其结果与 `[2]` 组合，最终构成 `int[2][3]`。另一个容易产生误区的是数组作为参数的情况，例如 `int[][3]`。我们应当将其视为 `int(*)[3]`。即一个指针，其所指向的类型是 `int[3]`。

> 可惜的是 C 的类型的设计十分复杂。如果我们能稍微修改一下类型表达式的结构，那么这些内容会变得更加容易理解。这一点类似于将中缀表达式转为前缀表达式。我们将 `int[2][3]` 转换为 `[2][3]int`，而将 `int[][3]` 转换为 `[][3]int` 或 `*[3]int`。在这种情况下，`[2][3]int` 表示为（一个长度为 2 的数组，其类型是（一个长度为 3 的数组，其类型是 int））。而 `*[3]int` 则表示为（一个指针，指向（一个长度为 3 的数组，其类型是 int））。
>
> > 接触过 Golang 的同学会发现这是 Golang 的类型表示方法。

由于 C 语言中关于数组定义的语法并不利于语义分析阶段的解析，所以我们这里再额外进行说明。为了方便考虑类型的嵌套结构，我们使用标准的 BNF 范式描述 SysY 中这部分的语法；

```text
VarDecl: BType VarDefs ';'

BType:
    'int'
    | 'char'

VarDefs:
    VarDef
    | VarDefs ',' VarDef

VarDef: Ident Dims

Dims:
    ε
    | '[' ConstExp ']' Dims
```

这样便可以看出，数组类型的定义是分为两个不同部分的。其一是标识符左侧的基本类型，如 `int`、`char`，其二则是标识符右侧的维度定义，如 `[2]`、`[2][3]`。这样就意味着为了得知标识符的类型，我们不得不先访问定义最左侧的基本类型，并在访问变量定义以及维度时将基本类型信息传递到此处。考虑 `int a, b[2][3]`。当我们得知基本类型是 `int` 后，我们需要将该信息传递到 `a` 和 `b[2][3]`。而对于 `b[2][3]`，我们需要先将 `int` 传递到维度定义的最深层，之后逐层返回。这一过程可以用如下伪代码表示：

```py
def visit_dims(node: Dims, base: Type) -> Type:
    if node is None:
        return base
    size = 根据 node.exp 获取当前维长度
    return ArrayType(size, visit_dims(node.dims, base))
```

### （3）传递中间代码信息

语义分析的最终目的是生成中间代码，而符号表也会为了这一目的服务。符号表在传递语法信息的同时，也会传递中间代码的信息。例如，一个变量符号可能存储了其在中间代码中的位置信息，一个函数符号可能保存了其在中间代码中对应的函数对象，等等。因此在创建符号时，我们还会进行中间代码的构建，并将构建后的信息附加到符号中。

在 tolangc 中，我们使用统一的 `ValuePtr value` 字段存储中间代码信息。这得益于 LLVM 对中间代码数据结构的设计，具体内容已经在上一章节中有所说明。举个例子，在定义变量时我们也会创建对应的中间代码，之后，我们将中间代码中变量所在地址信息（即 `alloca` 指令）附加到其符号当中。

```cpp
void Visitor::_visit_var_decl(const VarDecl &node) {
    // ...

    auto alloca = AllocaInst::New(context->GetFloatTy());
    auto symbol = std::make_shared<VariableSymbol>(node.ident->value, alloca,
                                                   node.ident->lineno);

    _cur_scope->add_symbol(symbol)

    // ...
}
```

而当需要在表达式中使用符号的时候，我们便查找符号表得到对应符号，据此得知变量在中间代码中的存储位置，进而生成后续的中间代码。

```cpp
ValuePtr Visitor::_visit_ident_exp(const IdentExp &node) {
    auto symbol = _cur_scope->get_symbol(node.ident->value);
    if (symbol->type != SymbolType::VAR) {
        // ...
        return nullptr;
    }

    // load value from addr
    auto var_symbol = std::static_pointer_cast<VariableSymbol>(symbol);
    auto addr = var_symbol->value;
    auto val = LoadInst::New(addr);
    _cur_block->InsertInstruction(val);

    return val;
}
```

## 三、构建中间代码

通过遍历语法树并维护符号表，我们能够获取源代码的所有信息。在这个基础上，我们实现中间代码的生成。对于实验中各位需要实现的 SysY 语言来说，你可能会感觉源代码与中间代码的差别并不甚大，这主要是由于 C 语言实在过于底层，其只包含了高级语言必备的一些基本概念（结构化控制流、函数、栈等等），在此之外则将所有细节都暴露给程序员操作。但实际上源代码可以和中间代码相去甚远，在这种情况下，语义分析所需要处理的问题将复杂的多。

> 我们的 tolang 也在语法上做了一些小小的变动，比如说顶级语句，从而与中间代码有所区别。但是这种改动也只是形式上的，因为我们只需将全部顶级语句放在 `main` 函数中即可。

### （1）函数的创建

接下来我们讲解一下 tolangc 中中间代码的构建过程。首先，对于**函数**，tolang 中只有两种函数，一是 `fn` 定义的函数，二就是顶级语句定义的 `main` 函数。因此，当我们访问 `FuncDef` 节点以及开始访问 `VarDecl` 和 `Stmt` 节点时，就需要创建对应的函数。新创建的函数 `_cur_func` 相当于当前子语法树节点的上下文环境，因此也作为 `Visitor` 类的属性。当函数定义结束后，我们将 `_cur_func` 重新置为 `nullptr`。

```cpp
void Visitor::visit(const CompUnit &node) {
    for (auto &elm : node.func_defs) {
        _visit_func_def(*elm);
    }

    // create main function
    auto context = _ir_module->Context();
    _cur_func = Function::New(context->GetInt32Ty(), "main");
    _ir_module->AddMainFunction(_cur_func);

    // ...

    _cur_func = nullptr;
}

void Visitor::_visit_func_def(const FuncDef &node) {
    // ...

    // create ir function
    auto context = _ir_module->Context();
    std::vector<ArgumentPtr> args;
    for (auto &param_symbol : param_symbols) {
        auto arg = Argument::New(context->GetFloatTy(), param_symbol->name);
        args.push_back(arg);
        param_symbol->value = arg;
    }
    _cur_func = Function::New(context->GetFloatTy(), node.ident->value, args);

    // ...

    _cur_func = nullptr;
}
```

### （2）表达式的转换

tolang 中的一系列**语句**，则可以转换为不同的中间代码指令。由于 tolang 本身十分简单，所以大多数语句到中间代码都是一对一的转换，唯一较为复杂的便是**表达式**的转换。

我们在设计抽象语法树时特地不完全按照原始的 EBNF 定义，将表达式节点设定为单目或双目运算。在构建中间代码时，这将提供很大便利。因为我们的中间代码为四元式，或称**三地址码**，如果表达式语法树的节点都为单目或双目运算，则我们只需先分别访问左子树和右子树，就可以得到三地址码中的两个地址。而第三个地址，则是当前节点中创建的新的指令。我们将这第三个地址返回，便实现了这一递归的过程。例如 tolangc 中的 `_visit_binary_exp`，只是简单的访问左子树和右子树，并返回一个新创建的指令而已。

```cpp
ValuePtr Visitor::_visit_binary_exp(const BinaryExp &node) {
    auto left_val = _visit_exp(*node.lhs);
    auto right_val = _visit_exp(*node.rhs);

    // get ir binary op type, just map from ast to ir
    BinaryOpType ir_op = // ...

    auto val = BinaryOperator::New(ir_op, left_val, right_val);
    _cur_block->InsertInstruction(val);

    return val;
}
```

### （3）控制流语句

在 tolang 中我们只有非结构化的，通过标签实现的控制流。这也就是被 Dijkstra 称为 “有害” 的 goto 语句。但是这种方法实现起来十分方便，所以我们先了解一下 tolangc 对 goto 的实现。而之后我们将在此基础上介绍结构化控制流，即 `if`、`if-else` 以及 `while` 的实现方法。

goto 反映了汇编语言中控制流语句的实现方式。goto 语句可以直接翻译为对应的汇编指令。

```tolang
# tolang
tag tag1;
    # do something
    if a0 < 1 to tag2;
    to tag1;
tag2;
```

```asm
// mips
tag1:
    // do something
    li a1, 1
    blt a0, a1, tag2
    j tag1
tag2:
```

由于 LLVM 的一个基本块对应一个标签，所以对于 tolangc，在遇到 `tag` 语句时，我们需要创建一个基本块，并将该基本块附加到标签符号的 `value` 字段中。

不过各位同学很容易可以发现，标签是可能在定义之前使用的，并且这一用法十分普遍，主要的作用是实现分支执行。比如说下面的 tolang 代码，实现了简单的 `if-else` 逻辑。

> 需要注意，`if... to` 的条件与结构化 `if` 的条件相反。因此后续在实现控制流时我们并非是当条件为真时跳转，而是条件为假时跳转。 

```tolang
if a >= b to else;
    # do if a < b is true
to fi;
tag else;
    # do if a < b is false
tag fi;
```

在这种情况下，我们同样采用占位 - 回填的思想。如果需要创建跳转指令，但跳转的目标还未确定，则可以只创建跳转指令而不设定跳转目标，并将该跳转指令保存在适当位置，当真正创建跳转目标后，再将该目标回填到之前保存的跳转指令中。对于 tolangc 来说，我们在标签符号中增加一个字段 `jump_insts` 用来保存标签定义之前引用的跳转指令，并在第一次遇到跳转指令时便创建对应的标签。这样当标签真正定义时，我们就可以从 `jump_insts` 中取出指令，将标签对应的基本块填入指令当中。

上述思想在分析结构化控制流的语义时同样十分重要。尤其是在需要进行**短路求值**的时候。接下来我们重点讲解这部分内容。

结构化控制流的关键是其中的条件判断。也就是语法树中的 `Cond` 子树。在语义分析遍历 `Cond` 子树的过程中，我们会构建一系列的跳转语句，而此时我们还不知道跳转的目标。因此我们应当保留这些跳转语句，即我们需要将这些跳转语句作为解析 `Cond` 之后的返回值。为方便解释，我们考虑由下面 BNF 定义的条件语法：

```text
Cond: LOrExp

LOrExp:
    LAndExp
    | LOrExp "||" LAndExp

LAndExp:
    CmpExp
    | LAndExp "&&" CmpExp

CmpExp:
    '(' Cond ')'
    | Exp ('==' | '!=' | '<' | '<=' | '>' | '>=') Exp
```

在这种情况下，我们可以定义如下抽象语法树：

```cpp
struct LogicExp;
struct CompareExp;
using Cond = std::variant<LogicExp, CompareExp>;

struct LogicExp {
    enum {
        AND,
        OR,
    } op;
    std::unique_ptr<Cond> lhs;
    std::unique_ptr<Cond> rhs;
};

struct CompareExp {
    enum {
        EQ, NE, LT, LE, GT, GE
    } op;
    std::unique_ptr<Exp> lhs;
    std::unique_ptr<Exp> rhs;
};
```

那么，当我们访问 `CompareExp` 时，我们会在中间代码中创建一条比较指令和一条分支指令。但是该指令的条件为真和为假时的目标基本块还未确定，因此我们应当将该指令返回，且需要将跳转到真的指令和跳转到假的指令分开。不过由于此处只有一条指令，所以返回的结果相同。其伪代码如下：

```py
def visit_compare_exp(node: CompareExp) -> tuple[list[JumpInst], list[JumpInst]]:
    left_val = visit_exp(node.lhs)
    right_val = visit_exp(node.rhs)
    cmp = 根据 left_val 和 right_val 创建比较指令
    br = 根据 cmp 创建分支指令
    jump_to_true = [br]
    jump_to_false = [br]
    return jump_to_true, jump_to_false
```

而对于 `LogicExp` 来说，情况有所不同。由于短路求值的存在，我们需要保证如下规则：

- 对于 `&&`：
    - 如果左侧的条件为真，则还需跳转到右侧判断
    - 如果左侧的条件为假，则整体为假
    - 如果右侧的条件为真，则整体为真
    - 如果右侧的条件为假，则整体为假
- 对于 `||`：
    - 如果左侧的条件为真，则整体为真
    - 如果左侧的条件为假，则还需跳转到右侧判断
    - 如果右侧的条件为真，则整体为真
    - 如果右侧的条件为假，则整体为假

这样，我们就可以给出 `LogicExp` 中 `jump_to_true` 和 `jump_to_false` 的求得规则了。其伪代码如下：

```py
def visit_logic_exp(node: LogicExp) -> tuple[list[JumpInst], list[JumpInst]]:
    left_true, left_false = visit_cond(node.lhs)
    label = 在中间代码中添加一个标签
    right_true, right_false = visit_cond(node.rhs)

    if node.op == AND:
        # 左侧或右侧为假则为假
        jump_to_false = left_false + right_false

        # 左侧为真则判断右侧
        for left_true_inst in left_true:
            left_true_inst.true_target = label

        # 右侧为真则为真
        jump_to_true = right_true
    else:
        # 左侧或右侧为真则为真
        jump_to_true = left_true + right_true

        # 左侧为假则判断右侧
        for left_false_inst in left_false:
            left_false_inst.false_target = label

        # 右侧为假则为假
        jump_to_false = right_false

    return jump_to_true, jump_to_false
```

在最后，`if` 以及 `while` 同样可以调用 `visit_cond` 获取对应的 `jump_to_true` 和 `jump_to_false`。此时只需在创建基本块后将其填入到跳转指令中即可。

最后我们给出 `if`、`if-else`、`while` 和 `for` 的 tolang 表示。同学们可以参照下面例子实现控制流语句的构建。

- **if**
    ```c
    if (a < b) {
        // do something
    }
    ```
    ```tolang
    if a >= b to if_join;
        # do something
    tag if_join;
    ```
- **if-else**
    ```c
    if (a < b) {
        // do if true
    } else {
        // do if false
    }
    ```
    ```tolang
    if a >= b to if_false;
        # do if true
    to if_join;
    tag if_false;
        # do if false
    tag if_join;
    ```
- **while**
    ```c
    while (a < b) {
        continue;
        break;
    }
    ```
    ```tolang
    tag while_cond;
    if a >= b to while_join;
        to while_cond; # continue
        to while_join; # break
    to while_cond;
    tag while_join;
    ```
- **for**
    ```c
    for (i = 0; i < n; i = i + 1) {
        continue;
        break;
    }
    ```
    ```tolang
    let i = 0;
    tag for_cond;
    if i >= n to for_join;
        to for_continue; # continue
        to for_join; # break
    tag for_continue;
        let i = i + 1;
    to for_cond;
    tag for_join
    ```

### （4）访存操作

构建中间代码的最后一部分，我们讨论一下访存操作的生成。在 tolangc 中，由于不具有复合类型，所以访存操作十分简单。对于 `let` 语句，我们需要获取等号左侧符号的 `value` 字段取值，该值即变量的地址，因此直接生成 `store` 指令即可。而对于 `IdentExp`，情况类似，我们只需直接生成 `store` 指令。但如果存在数组和指针的话，情况会复杂许多。因为这涉及到地址的计算。

这里特别要提到 “左值” 的概念。“左值” 指的是**可以在赋值符号左侧**的值。左值本质上指的是具有内存地址的值。因而其可被赋值。很明显，基本类型的变量是左值，但是数组则有所不同。下面我们给出与数组 `int a[2][3][4]` 有关的一系列表达式的值。假定数组起始地址为 a。

|表达式|值类型|值|左值地址|
|---|---|---|---|
|a          |*[3][4]int |a                  |非左值|
|a[1]       |*[4]int    |a + 3 * 4              |非左值|
|a[1][1]    |*int       |a + 3 * 4 + 4            |非左值|
|a[1][1][1] |int        |*(a + 3 * 4 + 4 + 1) |a + 3 * 4 + 4 + 1|

如果我们也考虑一下指针，会发现问题更加复杂。面我们给出与指针 `int b[][3][4]` 有关的一系列表达式的值。假定指针地址为 b，指针值为 a。

|表达式|值类型|值|左值地址|
|---|---|---|---|
|b          |*[3][4]int     |a                      |b|
|*b         |*[4]int        |a                      |非左值|
|**b        |*int           |a                      |非左值|
|***b       |int            |*(a)                   |a|
|&b         |**[3][4]int    |b                      |非左值|
|b[1]       |*[4]int        |a + 3 * 4              |非左值|
|b[1][1]    |*int           |a + 3 * 4 + 4          |非左值|
|b[1][1][1] |int            |*(a + 3 * 4 + 4 + 1)   |a + 3 * 4 + 4 + 1|

看起来十分混乱，不过幸好的是我们的实验不涉及指针本身的操作，同时数组也只有一个维度。各位只需要记住只有基本类型为左值，指针取索引时需要先取指针中地址再计算地址偏移即可。也就是说：
