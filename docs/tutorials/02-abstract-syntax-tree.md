# 抽象语法树

## 一. 抽象语法树概述

### 1. 抽象语法树
抽象语法树（Abstract Syntax Tree，简称AST）是编译器在解析源代码时生成的一种数据结构。它代表了源代码的语法结构，但去掉了一些不必要的信息，比如括号和分号等。AST是一种树状结构，其中每个节点代表源代码中的一个语法结构，比如表达式、语句、函数等。

### 2. 抽象语法树的作用
- **语法分析**：编译器通过解析源代码生成AST，这个过程称为语法分析。AST反映了源代码的语法结构，是后续编译步骤的基础。
- **错误检测**：在生成AST的过程中，编译器可以检测到语法错误，并提供相应的错误信息。
## 二. 抽象语法树实现思路

在实现抽象语法树时，我们通常定义一系列的节点类来表示不同的语法结构。以下是使用C++和面向对象的方法来构建AST的步骤：

### 定义节点基类
首先，我们定义一个节点的基类`Node`，它包含所有节点共有的属性和方法。例如，行号`lineno`和打印方法`print`。

```cpp
struct Node {
    int lineno;

    Node() = default;
    Node(int lineno) : lineno(lineno) {}
    virtual void print(std::ostream &out) = 0; // 虚函数，要求派生类实现打印功能
};
```

### 定义语法结构的派生类
针对程序中不同的语法结构，我们定义不同的派生类。使用`std::variant`来表示可能的语句或表达式类型。

```cpp
struct GetStmt;
struct PutStmt;
// ... 其他语句结构定义

using Stmt = std::variant<GetStmt, PutStmt, /* ... 其他语句类型 */>;
```

### 实现具体语法结构
对于每种语法结构，我们创建相应的结构体，并继承自`Node`。例如，一个函数定义`FuncDef`可能包含函数名、参数列表和表达式。

```cpp
struct FuncDef : public Node {
    std::unique_ptr<Ident> ident;
    std::vector<std::unique_ptr<Ident>> func_f_params;
    std::unique_ptr<Exp> exp;

    void print(std::ostream &out) override {
        // 实现打印逻辑
    }
};
```

### 使用智能指针管理内存
在AST中，我们使用`std::unique_ptr`来管理动态分配的内存，确保资源的正确释放。

```cpp
std::unique_ptr<Ident> ident;
std::vector<std::unique_ptr<Ident>> func_f_params;
```

### 实现打印功能
为了能够可视化AST，我们需要为每个节点实现`print`方法。这通常涉及到递归地打印子节点。

```cpp
void FuncDef::print(std::ostream &out) {
    out << "Function: " << ident->value << "(";
    for (size_t i = 0; i < func_f_params.size(); ++i) {
        if (i > 0) out << ", ";
        func_f_params[i]->print(out);
    }
    out << ") { ... }";
}
```

### 构建AST
在解析源代码时，根据语法分析器的输出构建AST。这通常涉及到创建节点对象，并将它们连接成树状结构。详见语法分析部分。
