# 前言
## 本教程的背景

编译技术实验是计算机科学与技术专业核心专业必修课编译技术的实践环节，旨在帮助学生深入理解编译原理和技术，并掌握实际编译器的设计和实现方法。在本实验中，各位同学需要自行设计、实现一个完整的编译器，该编译器能够实现 SysY 语言（C 语言子集）到中间代码 LLVM、Pcode 或目标代码 MIPS 的编译过程。

亲自实现一个编译器对各位同学理解编译技术有着极大的帮助。

在以往的教学实践中我们发现，理论课上的内容并不能很好的指导同学们进行编译器的实际设计。

这并不意味着理论课的内容毫无必要，而是理论与实际之间本就有着难以轻易逾越的鸿沟。将理论与实践融会贯通，本就需要付出极大的努力。

为了能稍稍帮助同学们跨越这道鸿沟，我们教程组决定做出一点自己的贡献。以往的编译实验教程主要关注于讲述实验中可能会用到的相关知识，尽管也有一些辅助理解的例子，但也常常局限于单一章节的内容，不同章节间内容互不相通。这一定程度上妨碍了同学们从中学习编译器的设计方法，并容易造成了理解上的混乱。

因此，我们决定修改原有的编译实验教程，通过示例驱动的方式给同学们提供更有参考价值的教程内容。具体来说，我们会设计一个十分简单的编程语言：tolang。在本教程中，我们将以 tolang 编译器 tolangc 的编写为主线，介绍基本的编译器设计和实现方法，从而为同学们提供一个相对较好的实践样例。

## tolang 介绍
tolang 这个名字很奇怪，最初起名时的含义是 toy lang，但是也有 too long 的谐音。不过这个语言确实是一个玩具语言，根本没有复杂冗长的语法语义。

### 文法定义
我们的 tolang 可以由如下的 EBNF 范式确定：

```text
CompUnit: {FuncDef} {VarDecl} {Stmt}

FuncDef: 'fn' Ident '(' [FuncFParams] ')' '=>' Exp ';'

FuncFParams: Ident { ',' Ident }

VarDecl: 'var' Ident ';'

Stmt:
    'get' Ident ';'
    | 'put' Exp ';'
    | 'tag' Ident ';'
    | 'let' Ident '=' Exp ';'
    | 'if' Cond 'to' Ident ';'
    | 'to' Ident ';'

Exp: AddExp

AddExp:
    MulExp
    | AddExp ('+' | '-') MulExp

MulExp:
    UnaryExp
    | MulExp ('*' | '/' | '%') UnaryExp

UnaryExp:
    PrimaryExp
    | Ident '(' [FuncRParams] ')'
    | ('+' | '-') UnaryExp

PrimaryExp:
    '(' Exp ')'
    | Ident
    | Number

FuncRParams: Exp { ',' Exp }

Cond:
    Exp ('<' | '>' | '<=' | '>=' | '==' | '!=') Exp
```

其中标识符（Ident）和数字（Number）可以由如下正则表达式确定：

```text
Ident: [a-zA-Z_][0-9a-zA-Z_]*
Number: (0|[1-9][0-9]*)(\.[0-9]+)?
```

当然，只是列出文法不太负责。接下来我们以一般教程的方式介绍该语言：

1. tolang 的内容只能写在单个文件中。

2. tolang 的代码包含三个部分：函数定义、变量定义和语句。三个部分的顺序不能颠倒。

3. tolang 的函数定义如下。函数中只能包含单一表达式。
    ```text
    fn func_name(a1, a2, a3) => a1 + a2 + a3 + 1.23;
    ```

4. tolang 的变量定义如下所示。变量只有（单精度）浮点数类型
    ```text
    var a; var b;
    var c;
    ```

5. tolang 的输入输出如下所示。
    ```text
    get a; # input
    put a; # output
    ```

6. tolang 的计算和赋值如下所示。
    ```text
    let a = a + b - c * d / 1;
    ```

7. tolang 不具有结构化控制流，而是使用标签 `tag`、无条件跳转 `to` 和有条件跳转 `if... to` 实现控制流。`if` 的条件只能是两个表达式值比较。
    ```text
    # if-else
    if a + 1 < b to else;
        put a;
        to fi;
    tag else;
        put b;
    tag fi;

    # while
    let i = 1;
    tag while; if i >= 10 to done;
        put a;
        let i = i + 1;
    to while; tag done;
    ```

8. tolang 中，从 `#` 开始直到本行末尾为注释内容。

## tolangc 介绍

tolangc 是 tolang 语言的编译器，目前代码托管在[此处](https://github.com/wokron/tolangc)。tolangc 的使用方法如下：

```text
Usage: ./build/tolangc [options] [file]
Options:
  -h, --help: Show this help message
  --emit-ast: Emit AST
  --emit-ir: Emit IR
  -S, --emit-asm: Emit assembly
  -o, --output: Output file
```

tolangc 以 LLVM 作为中间代码，以 MIPS 作为目标代码，通过命令行选项的不同可以选择输出不同的代码。

<!-- 编译技术实验是计算机科学与技术专业核心专业必修课编译技术的实践环节，旨在帮助学生深入理解编译原理和技术，并掌握实际编译器的设计和实现方法。本实验的主要任务是将一个类C语言的子集——sysY语言的源代码，通过编译器的各个阶段，转换成不同的目标代码形式，包括Pcode、LLVM和MIPS代码。

编译器是一种将高级程序语言转换成底层计算机硬件能够执行的机器码的工具。编译器通常包括词法分析、语法分析、语义分析、中间代码生成、优化以及目标代码生成等多个阶段。在本实验中，学生将通过逐步实现这些阶段，深入了解编译器的工作原理和实现细节。

sysY语言是一个简化版的C语言，包括了基本的语法和语义规则，例如控制结构、数据类型、函数定义和调用等。学生需要在本实验中理解sysY语言的语法和语义，实现相应的编译器功能，将sysY源代码转换成目标代码形式。

本实验涉及到三种目标代码形式，包括Pcode、LLVM和MIPS代码。Pcode是一种虚拟机代码，通常用于编译器前端和后端之间的中间代码表示。LLVM是一种低级虚拟指令集，用于编译器优化和代码生成。MIPS是一种常见的处理器架构，相比前两种，难度要更大一些。学生需要了解这些不同的目标代码形式，掌握其语法和语义，实现相应的目标代码生成功能。课程组为大家推荐的架构为前端-中端-后端。其中前端包含了词法分析，语法分析，中端主要为语义分析以及一些可能的优化，将源语言翻译为中间代码，后端将中间代码再次翻译为目标语言。

通过本实验，你们将能够深入理解编译器的工作原理和实现细节，掌握编译器的设计和实现方法，提高自己的编程能力和软件工程能力。同时，通过实际的编译器实现，你们也会加深对C语言和计算机硬件的理解，并培养自己的问题解决能力和团队合作能力。 -->
