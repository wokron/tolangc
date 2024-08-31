# tolangc: tolang compiler

tolang 指 “toy lang”，是一款用于教学目的的简单编程语言。tolangc 即 tolang compiler，是 tolang 语言的样例编译器，可以为编译技术的学习者提供对编译器架构的初步认识。

> 本编译器为北京航空航天大学本科三年级《编译技术》课程实验教程的配套示例，实验教程同样位于本仓库中。

## tolangc 介绍

- 使用 C++ 编写
- 以 LLVM / Pcode 作为中间代码形式
- 支持输出 MIPS 汇编码 / Pcode 解释执行

## 文法定义
tolang 语言（v1）的文法由下面的 EBNF 范式确定：

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
    | MulExp ('*' | '/') UnaryExp

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

其中

```text
Ident: [a-zA-Z_][0-9a-zA-Z_]*
Number: (0|[1-9][0-9]*)(\.[0-9]+)?
```

## 项目维护

见 [CONTRIBUTING.md](CONTRIBUTING.md)。

## 编译构建

- 经过测试的编译环境：ubuntu-22.04、macos-14。
- 前置条件：已安装 cmake、make、clang。
- 构建方式：运行 `./script/build.sh` 脚本，并指定对应后端，如  `bash ./script/build.sh llvm`。

## Todo
- [x] 编译器前端
  - [x] 词法分析
  - [x] 语法树数据结构
  - [x] 语法分析
  - [x] 符号表
  - [x] 中间代码数据结构
  - [x] 中间代码构建接口
  - [x] 语义分析
  - [x] 中间代码输出

- [x] 编译器后端
    - [x] 寄存器分配
    - [x] 目标代码生成
