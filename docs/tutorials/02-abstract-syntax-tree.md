# 抽象语法树

在词法分析阶段，我们将字符序列解析为单词序列，其中每一个单词都是一个最小的语法单元。词法分析的分析结果是**非结构化**的序列，无法表达程序的复杂语法结构。因此我们还需进行语义分析，用给定的上下文无关文法识别单词序列。而语义分析的最终结果，就是**抽象语法树**。

## 一、语法树的表示方法

文法反映了句子中不同结构之间的组成关系。例如下面的文法：

```text
VarDecl: BType Ident Dims [ '=' InitVals ]
```

便反映了变量定义由类型、标识符、维度以及可选的初始化值**组成**。

而对于**组成**这一概念，十分常见的建模方法便是将整体定义为类，而将各子部分定义为其属性。因此对于上面的文法，可以将其建模为如下形式：

```cpp
struct VarDecl {
    BType btype;
    Ident ident;
    Dims dims;
    InitVals *init_vals;
};
```

同理，我们也可以进一步定义 `BType`、`Ident`、`Dims` 以及 `InitVals`。如此继续下去，我们便可以对文法中的任意符号定义对应的类型，从而实现对源程序的结构化表示。我们所定义的这一系列类型，实际上也就是文法所对应的语法树中的可能节点。

在定义节点的时候，需要注意某一非终结符对应**多条规则**的情况。例如下面的规则：

```text
Stmt:
    'if' '(' Cond ')' Stmt [ 'else' Stmt ]
    | 'while' '(' Cond ')' Stmt
```

这种情况下我们会发现，if 语句和 while 语句的结构并不相同，然而不管是 if 语句还是 while 语句，它们都**是**语句。

对**是**（is-a）关系的一种很容易想到的建模方法便是**继承**。因此，我们可以用如下方法表示 Stmt 符号对应的两条规则：

```cpp
struct Stmt {};
struct IfStmt : public Stmt {};
struct WhileStmt : public Stmt {};
```

然而，继承不仅意味着**变量多态**，还意味着**函数多态**，可是在语法树的表示中，我们实际上只是希望用同一个名称指代不同的数据类型，而并不需要同一个函数拥有不同行为。因此，继承在此处似乎太 “重” 了。

我们只需要一种方法，能够为多个数据类型设定同一个名字。这一概念即**联合类型**。Java 当然并不支持联合类型，但 C++ 却提供了这一功能。首先便是继承自 C 的 `union`。

```cpp
union Stmt {
    IfStmt if_stmt;
    WhileStmt while_stmt;
};
```

然而，仅有 `union` 我们并不能得知 `Stmt` 的具体类型，因此 `union` 常常与 `enum` 一同使用。通过添加一个枚举类型作为标签，指示 `union` 中实际存储的类型。

```cpp
struct Stmt {
    enum {
        IF_STMT,
        WHILE_STMT,
    } type;
    union {
        IfStmt if_stmt;
        WhileStmt while_stmt;
    } data;
};
```

不过，这种方法相当原始。C++17 标准则提出了另一种更加 “现代” 的处理方法。那就是使用 `std::variant`。`variant` 提供了一种定义联合类型的方法。我们直接用例子展示其用法：

```cpp
using Stmt = std::variant<IfStmt, WhileStmt>;
```

这一条语句实现了与 `enum + union` 基本相同的功能。如果希望获取其中存储的具体类型，可以使用 `std::get` 强制获取某一类型取值，或使用 `std::get_if` 在类型匹配时返回取值。

```cpp
Stmt stmt = IfStmt();
if (auto if_stmt = std::get_if<IfStmt>(&stmt)) {
    // ...
}
```

> 其他一些语言也提供了对联合类型的支持。例如 Python 类型注解中的 `Union` 或 `|`、Rust 中的 `enum` 等等。

在 tolangc 中，我们使用的便是 `std::variant`。当然，这并不意味着各位同学的实验编译器也一定要使用 C++ 和使用 `variant`。同学们可以自由选择不同的方法。

> 在以上三种方法之外，当然存在着其他不同的语法树表示方法。比如说，完全可以用一棵树表示语法树（这个说法很是奇怪），用统一的节点类表示所有语法树节点。更多的方法也请各位同学自行探索。

对于 tolangc 来说，我们定义了语法树节点基类 `Node`，并在其中定义了行号等各节点共同具有的属性。

```cpp
struct Node {
    int lineno;

    Node() = default;
    Node(int lineno) : lineno(lineno) {}
};
```

为了方便语法分析部分的处理，对于各节点，我们使用指针指向其子节点。由于语法树一旦建立，其所有权便不会发生变化，所以这里我们使用 `std::unique_ptr`。

```cpp
struct CompUnit : public Node {
    std::vector<std::unique_ptr<FuncDef>> func_defs;
    std::vector<std::unique_ptr<VarDecl>> var_decls;
    std::vector<std::unique_ptr<Stmt>> stmts;
};
```

对于具有不同规则的非终结符，我们则使用 `std::variant` 进行定义。

```cpp
struct GetStmt;
struct PutStmt;
struct TagStmt;
struct LetStmt;
struct IfStmt;
struct ToStmt;
using Stmt = std::variant<GetStmt, PutStmt, TagStmt, LetStmt, IfStmt, ToStmt>;
```

按照这一规则，我们便定义了 tolangc 的语法树结构。

## 二、抽象语法树

上一小节中所说的，根据文法定义语法树节点的方法十分方便和机械。然而我们考虑下面的文法：

```text
PrimaryExp:
    '(' Exp ')'
    | Number
```

在这种情况下，我们是否需要为 `'(' Exp ')'` 和 `Number` 分别定义子类呢？由于这两条规则中都只有一个具有语法信息的字段，且去掉一层额外的访问并不会对语法树的含义造成影响（表达式的结构不会发生变化）。所以我们实际上可以选择不去定义 `PrimayExp` 及其子类。

进一步的，我们可以再添加一些文法规则：

```text
Exp: AddExp

AddExp:
    MulExp
    | AddExp '+' MulExp

MulExp:
    PrimaryExp
    | MulExp '*' PrimaryExp
```

这时我们会发现，上述和表达式相关的规则中都存在只有单一字段的规则。因此我们有这样的关系：Exp 是 PrimaryExp、PrimaryExp 是 MulExp、MulExp 是 AddExp、AddExp 是 Exp。因此，实际上所有的 *Exp 都是 Exp。这时我们可以抛开文法规则，以一种更加简洁的方式定义表达式相关的语法树节点：

```cpp
struct AddExp;
struct MulExp;
using Exp = std::variant<AddExp, MulExp, Number>;

struct AddExp {
    Exp *lhs, *rhs;
};
struct MulExp {
    Exp *lhs, *rhs;
};
```

> 实际上，我们还可以将 `AddExp` 和 `MulExp` 合并为 `BinaryExp`，并使用一枚举字段表示运算符的类型。

再举一例，对于下面的文法：

```text
FuncDef: FuncType Ident '(' [ FuncFParams ] ')' Block

FuncFParams: FuncFParam { ',' FuncFParam }
```

我们是否需要定义一个 `FuncFParams` 类型呢？其实并不需要。这里将 FuncFParams 设为单独的非终结符的原因其实是，我们没有办法在一条规则中同时表示函数没有参数或有一到多个参数这两种情况。因此，如果之前的 FuncDef 和 FuncFParams 节点被定义为如下类型：

```cpp
struct FuncDef {
    // ...
    FuncFParams *func_f_params;
    // ...
};
struct FuncFParams {
    std::vector<FuncFParam> func_f_params;
};
```

那我们便可以将其化简为如下形式：

```cpp
struct FuncDef {
    // ...
    std::vector<FuncFParam> func_f_params;
    // ...
};
```

参照上述原理，我们便可以对原本语法树进行化简。化简过程去掉冗余结构的同时，依然记录了完整的程序结构信息。经过化简的语法树不再与文法所生成的语法树保持一致，我们称其为**抽象语法树**。在设计良好的情况下，抽象语法树能够更加方便地被程序创建和处理。

在 tolangc 中，我们也对语法树进行了化简。例如我们将所有表达式统一为 `Exp`。

```cpp
struct BinaryExp;
struct CallExp;
struct UnaryExp;
struct IdentExp;
struct Number;
using Exp = std::variant<BinaryExp, CallExp, UnaryExp, IdentExp, Number>;
```

## 三、节点的设计

将语法树化简为抽象语法树，意味着脱离文法对语法树节点进行改造。这一过程并没有明确的规则。然而，不同的节点设计却可能导致不同的后果，或简洁明了、易于处理；或晦涩复杂、反而成了编译器后续阶段的障碍。这里给出一种抽象语法树定义的建议，那就是在设计语法树时，应当尽可能保证：**语法树子树表示程序模块，语法树深度反映求值顺序**。

我们还举上一小节中的文法片段作为例子。对于 FuncDef 部分的文法，我们并不应当定义额外的 `FuncFParams` 类型，因为 `FuncFParams` **并不足以表示函数形参**这一程序模块。该类型只能表示函数形参数量大于等于 1 的情况，而函数形参数量为 0 的情况则需要根据 `FuncDef` 中的对应字段是否为空来判断。

```text
FuncDef: FuncType Ident '(' [ FuncFParams ] ')' Block

FuncFParams: FuncFParam { ',' FuncFParam }
```

而对于 Exp 部分的文法，实验文法中应当会使用 EBNF 表示为下面的形式：

```text
Exp: AddExp

AddExp: MulExp { '+' MulExp }

MulExp: PrimaryExp { '*' PrimaryExp }
```

然而在上一小节中我们并没有选择将 `AddExp` 和 `MulExp` 定义为如下的形式：

```cpp
struct AddExp {
    std::vector<Exp*> operands;
};
struct MulExp {
    std::vector<Exp*> operands;
};
```

这是因为，抽象语法树应当保持程序在运行时的求值顺序。当我们使用上一小节中的定义的时候，我们只需要使用二叉树的后序遍历便可完成整个表达式的求值。当然，本小节中的定义也可以通过循环遍历实现同样的效果，但这必然以代码的清晰性为代价。

> 此处可以引出一个值得关注的地方。为什么加减乘除运算的文法规则一定是**左递归**的？因为我们总是**从左向右**进行运算。因此，**左侧子表达式的求值一定先于右侧**。

就此，我们学习了抽象语法树的定义方法。各位同学在实验中设计语法树时，也请参考本章节的内容。设计良好的语法树将为编译器后续阶段的实现提供巨大的帮助。在下一章节中，我们将实现编译器的语法分析部分，在进行语法分析时，我们将同步进行抽象语法树的构建。
