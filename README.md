# tolangc: tolang compiler

tolang 指 toy lang，是一款用于教学目的的简单编程语言。tolangc 即 tolang compiler，是 tolang 语言的样例编译器，可以为编译技术的学习者提供对编译器架构的初步认识。

## tolangc 介绍

- 使用 C++ 编写
- 以 LLVM 作为中间代码形式

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

其中

```text
Ident: [a-zA-Z_][0-9a-zA-Z_]*
Number: [1-9][0-9]*(\.[0-9]+)?
```

## 项目维护

见 [CONTRIBUTING.md](CONTRIBUTING.md)。

## Todo
- [ ] 编译器前端
  - [ ] 词法分析
  - [ ] 语法树数据结构
  - [ ] 语法分析
  - [ ] 符号表
  - [ ] 中间代码数据结构
  - [ ] 中间代码构建接口
  - [ ] 语义分析
  - [ ] 中间代码输出

- [ ] 编译器后端
    - [ ] 寄存器分配
    - [ ] 目标代码生成
