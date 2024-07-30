# 语法分析

### 一、语法分析概述

在上一节中，我们通过词法分析将输入的源程序解析成一个个单词。在语法分析部分，我们将对这些单词进行进一步地分析，将其建立成结构化的语法树，以便于后续的编译。

#### （一）语法树

在此，我们先回顾一下什么是语法树。语法树是按照给定的文法，将一个句子转化为有结构层次的树结构。**树中的每个节点都代表一个语法成分，而其子节点则代表组成该语法成分的其他语法成分，根节点为开始符号**。以下面的几条文法为例，假设Exp为开始符号（可以理解成最高级的语法成分）：

```c
左值表达式 LVal → Ident {'[' Exp ']'}
数值 Number → IntConst
基本表达式 PrimaryExp → '(' Exp ')' | LVal | Number
一元表达式 UnaryExp → PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp
单目运算符 UnaryOp → '+' | '−' | '!'
乘除模表达式 MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
加减表达式 AddExp → MulExp | AddExp ('+' | '−') MulExp
表达式 Exp → AddExp
```

那么对于下面这个简单的句子（其中a和b为Ident）

```c
a + (3 + b) * 2
```

我们则可以建立如下的语法树：

<img src="imgs\chapter03\parser_1.png" alt="parser_1" style="zoom:40%;" />

语法树展现了**从上到下的推理过程**，即我们是如何从一个语法成分推理出我们的句子（在上述例子中，从Exp推理出`a+(3+b)*2`），也就是语法树从根开始往下生长的过程；另一方面，语法树还展现了**从下到上的归约过程**，即我们是如何从一个句子不断地“合成”，最终合并成一个语法成分的（在上述例子中，从`a+(3+b)*2`最终合成Exp语法成分），也就是语法树从叶子结点向上不断合并的过程。

除此之外，不难发现，语法树中的叶子结点都是终结符（因为到终结符就无法继续拆分了），而且叶子结点从左到右刚好组成了我们输入的句子。

#### （二）语法分析的作用

上面我们介绍了给定文法时，一个句子的语法树是什么样的，以及语法树的一些特点。那么，**为什么我们要将一个简单的句子转化成如此复杂的语法树呢？**原因就在于，一个简单的句子是线性结构，不包含相应的语法结构信息，如果直接对线性的句子进行处理，是非常困难的。而语法树的层次化结构可以很好地表现语法结构，以及语法成分之间的关系；而树结构又可以很方便地利用递归等方法来进行处理，因此，将输入的源程序转化成包含语法结构的语法树，将为后续的处理带来极大的便利。

例如，对于上面例子中的表达式

```c
a + (3 + b) * 2
```

对于人来说，可以很容易地确定表达式的运算顺序。但是，从计算机角度而言，要编写一个程序仅仅根据这个线性字符串来确定运算的顺序是比较困难的，尤其是当表达式更为复杂的时候。如果我们建立了语法树，则可以很轻松地利用递归等方法来进行计算。

<img src="imgs\chapter03\parser_2.png" alt="parser_2" style="zoom:40%;" />

例如，在上面的例子中，我们要求表达式Exp的值（图中序号1），那么我们只需要求出AddExp（序号2）和MulExp（序号3）的值并将它们相加即可。要求MulExp（序号3）的值，我们只需要求出MulExp（序号4）和UnaryExp（序号5）的值并将它们相乘即可。同样地，要求MulExp（序号4）的值，我们只需要求出AddExp（序号6）和MulExp（序号7）的值并将它们相乘即可。对于每种语法成分的处理，如AddExp和MulExp的计算求值，我们都可以采用递归的方法很方便地实现，而且**语法树的层次结构自然保证了计算顺序的正确性**。

事实上，就像上面介绍的表达式计算，后续的编译过程也是基于语法树，利用递归等方法来进行的。因此，**语法分析的作用就是根据词法分析器解析出来的一个个单词，生成这样一棵具有层次化结构的语法树**，方便后续的编译。



### 二、语法分析器实现思路

接下来介绍实现语法分析器（Parser）的常用方法——递归下降。

#### （一）递归下降

由于树结构可以很方便的用递归来处理（例如DFS），因此，我们同样可以采用递归的方法来生成语法树。

递归下降的主要思路，就是**为每个语法成分都编写一个子程序，该子程序会调用其他的子程序来解析组成该语法成分的其他语法成分，每个子程序返回的结果都是一棵对应语法成分的子树**。具体来说，例如有以下文法：

```
FuncDef: 'fn' Ident '(' [FuncFParams] ')' '=>' Exp ';'
```

那么，就可以为语法成分FuncDef编写一个这样的子程序（可以暂时忽略词法分析器lexer的调用，只关注其他子程序的调用顺序）：

``` C++
std::unique_ptr<FuncDef> Parser::_parse_func_def() {
    auto func_def = std::make_unique<FuncDef>();
    func_def->lineno = _token.lineno;

    _match(_token, Token::TK_FN); 

    func_def->ident = _parse_ident(); // 解析Ident
 
    _match(_token, Token::TK_LPARENT); 

    if (_token.type != Token::TK_RPARENT) {
        _parse_func_f_params(func_def->func_f_params); // 解析函数形参FuncFParams
    }

    _match(_token, Token::TK_RPARENT);

    _match(_token, Token::TK_RARROW);

    func_def->exp = _parse_exp(); // 解析Exp

    _match(_token, Token::TK_SEMINCN);

    return func_def;
}
```

解析FuncDef的子程序分别调用解析Ident和FuncFParams和Exp的子程序，得到了三棵子树，再将三棵子树和终结符'fn','(',')'等合并为语法成分FuncDef的语法树。而**在解析FuncDef的子程序中，我们并不关心解析Ident和FuncFParams和Exp的子程序究竟做了什么，只关注其返回的结果。**

<img src="imgs\chapter03\parser_3.png" alt="parser_3" style="zoom:60%;" />

当然，解析FuncDef的子程序也可能被其他子程序调用，它们也不关心解析FuncDef的子程序究竟做了什么。


**总而言之，我们只需要为文法中的每个语法成分都编写一个子程序并确保它们之间的正确调用，最后调用开始符号的解析子程序，就可以生成整棵语法树。**

#### （二）语法分析与词法分析的配合

前面我们强调了语法分析利用词法分析解析出的一个个单词来构建语法树，因此，在编写递归下降程序的过程中，需要特别注意语法分析和词法分析的配合。如果任何一个地方错误调用了词法分析，就可能对整个语法树的构建造成影响。

为了确保语法分析和词法分析的配合，我们做出如下规定供参考：

- 一个子程序在调用其他子程序前，需要调用词法分析器来预读一个单词
- 一个子程序在退出时，需要调用词法分析器来预读一个单词

有了上述规定，就可以确保：

- 刚进入一个子程序时，词法分析器已经预读好了一个单词
- 从一个子程序返回时，词法分析器已经预读好了一个单词

即``_parser._token``时刻指向待解析的第一个单词.
与此同时，我们在语法分析时的同时进行**语法错误处理**。当我们在当前位置需要匹配一个语法成分时，parser“试探性”地去解析该语法成分，如果解析失败，则报错；并“假装”这个语法成分解析正确，继续进行语法分析（这就是我们经常提到的错误处理的局部化，它可以使得编译器能够发现尽可能多的语法错误，而不至于每次编译只抛出一个错误编译器就罢工了）。

于是，我们设计了一个``_match``方法，当parser成功匹配语法成分时则读入下一个单词，否则报错，从而这个函数兼具错误处理和语法分析的作用。同学们在编写自己的编译器时也可以使用这样的方法来处理语法错误。
```cpp
void Parser::_match(const Token &token, Token::TokenType expected) {
    if (token.type != expected) {
        ErrorReporter::error(_token.lineno,
                             "expect '" + token_type_to_string(expected) + "'");
    } else {
        _next_token();
    }
}
```


因此，对于前面例子中的解析FuncDef的子程序，词法分析器的调用顺序如下：


``` C++
std::unique_ptr<FuncDef> Parser::_parse_func_def() {
    auto func_def = std::make_unique<FuncDef>();
    func_def->lineno = _token.lineno;

    _match(_token, Token::TK_FN); // 匹配fn关键字

    func_def->ident = _parse_ident(); 
 
    _match(_token, Token::TK_LPARENT); // 匹配左括号

    if (_token.type != Token::TK_RPARENT) {
        _parse_func_f_params(func_def->func_f_params); 
    }

    _match(_token, Token::TK_RPARENT); // 匹配右括号

    _match(_token, Token::TK_RARROW); // 匹配右箭头

    func_def->exp = _parse_exp(); // 

    _match(_token, Token::TK_SEMINCN); // 匹配分号

    return func_def;
}
```

这样我们就可以确保语法分析和词法分析的协调配合。

#### （三）多产生式

如果一个语法成分只有一条产生式，那么其解析方法就是唯一确定的。但是文法中可能存在某个语法成分有多条产生式，例如

```c
Stmt:
    'get' Ident ';'
    | 'put' Exp ';'
    | 'tag' Ident ';'
    | 'let' Ident '=' Exp ';'
    | 'if' Cond 'to' Ident ';'
    | 'to' Ident ';'
```

那么，**在解析Stmt的子程序中，就要考虑选择哪一条产生式进行后续的解析。**这里，我们可以考虑产生式右边的FIRST集。在理论课中，同学们会知道一个产生式的FIRST集就是头符号集合。我们的tolangc文法非常简单，对于Stmt语法成分，我们一眼就能看出右产生式的FIRST集。当当前单词是``'get'``时，则一定使用规则``Stmt:'get' Ident ';'``解析；若是``'put'``则用``Stmt:'put' Exp ';'``解析。

从而对于具有多产生式的语法成分的递归子程序``_parse_stmt``可以这样编写：

```cpp
std::unique_ptr<Stmt> Parser::_parse_stmt() {
    switch (_token.type) {
    case Token::TK_GET: {
        GetStmt get_stmt;
        // parse get_stmt
    }
    case Token::TK_PUT: {
        PutStmt put_stmt;
        // parse put_stmt
    }
    // ...
	}
}
```

当然，实验课的文法会更加复杂：FIRST集需要计算得出，而且存在我们无法仅凭预读一个单词就能判断该用哪个产生式的情况，在此不做赘述。


#### （四）左递归文法

文法中存在左递归文法，如果直接据此编写递归下降子程序，将会导致无限递归，最终栈溢出程序崩溃。

```c
AddExp: AddExp ('+' | '-') MulExp

MulExp: MulExp ('*' | '/') UnaryExp
```

我们需要通过改写文法来解决左递归问题。一般来说，有两种改写方法：

第一种是将左递归改为右递归，这也是tolangc的实现方法。我们可以将上述两条文法改写为：

   ```c
   AddExp → MulExp | MulExp ('+' | '−') AddExp
   MulExp → UnaryExp | UnaryExp ('*' | '/') MulExp
   ```

   以解析AddExp为例，先调用MulExp的解析子程序，然后判断后面的单词是否是'+'或'-'，如果是，再递归调用解析AddExp的子程序。
   在此，我们提供一种非递归的写法来解析AddExp：
       1. 先解析一个MulExp，作为预备返回的exp；
       2. 若当前符号为'+'或者'-'，则构造一个binary_exp，其左子表达式为已经解析的exp，右子表达式为新解析的MulExp，并把这个binary_exp作为预备返回的exp，循环执行该步骤
   
```cpp
std::unique_ptr<Exp> Parser::_parse_add_exp() {
    auto lineno = _token.lineno;

    auto exp = _parse_mul_exp();
    while (_token.type == Token::TK_PLUS || _token.type == Token::TK_MINU) {
        BinaryExp binary_exp;
        binary_exp.lineno = lineno;
        binary_exp.lhs = std::move(exp);
        binary_exp.op =
            _token.type == Token::TK_PLUS ? BinaryExp::PLUS : BinaryExp::MINU;

        _next_token();
        auto rhs = _parse_mul_exp();
        binary_exp.rhs = std::move(rhs);
        exp = std::make_unique<Exp>(std::move(binary_exp));
    }
    return exp;
}
```

相关的语法树数据结构如下，关键点在于使用了``std::variant``和``std::unique_ptr``

```cpp
using Exp = std::variant<BinaryExp, CallExp, UnaryExp, IdentExp, Number>;

struct BinaryExp : public Node {
    std::unique_ptr<Exp> lhs;
    enum BinaryOp {
        PLUS,
        MINU,
        MULT,
        DIV,
        MOD,
    } op;
    std::unique_ptr<Exp> rhs;
};
```

对于MulExp的解析程序类似。

第二种是改写为BNF范式。通过分析结构，不难发现AddExp本质上是由若干个MulExp组成，MulExp本质上是由若干个UnaryExp组成，因此可以将上述两条文法改写为

   ```c
   AddExp → MulExp {('+' | '−') MulExp}
   MulExp → UnaryExp {('*' | '/') UnaryExp}
   ```

   其中，{}表示其中的语法成分可以出现0到若干次。

   以解析AddExp为例，先调用MulExp的解析子程序，然后判断后面的单词是否是'+'或'-'，如果是，则再次调用MulExp的解析子程序，直至解析完MulExp后的单词不是'+'也不是'-'。



