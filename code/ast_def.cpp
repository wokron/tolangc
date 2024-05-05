#include <iostream>
#include <string>
#include <vector>
#include <variant>

// 基础节点类型
struct Node {
    virtual ~Node() = default;
};

// 所有的语法树节点都继承自 Node
struct CompUnit;
struct FuncDef;
struct FuncFParams;
struct VarDecl;
struct Stmt;
struct Exp;
struct AddExp;
struct MulExp;
struct UnaryExp;
struct PrimaryExp;
struct FuncRParams;
struct Cond;

// 定义节点指针
using NodePtr = std::shared_ptr<Node>;

// 各种语法结构的具体实现
struct CompUnit : Node {
    std::vector<NodePtr> func_defs;
    std::vector<NodePtr> var_decls;
    std::vector<NodePtr> stmts;
};

struct FuncDef : Node {
    NodePtr ident;
    NodePtr func_f_params;
    NodePtr exp;
};

struct FuncFParams : Node {
    std::vector<NodePtr> idents;
};

struct VarDecl : Node {
    NodePtr ident;
};

struct Stmt : Node {
    
    struct Get : Node {
        NodePtr ident;
    };
    struct Put : Node {
        NodePtr exp;
    };
    struct Tag : Node {
        NodePtr ident;
    };
    struct Let : Node {
        NodePtr ident;
        NodePtr exp;
    };
    struct If : Node {
        NodePtr cond;
        NodePtr ident;
    };
    struct To : Node {
        NodePtr ident;
    };

    std::variant<
        Get, Put, Tag, Let, If, To
    > variant;
};

// 表达式节点
struct Exp : Node {
    NodePtr add_exp;
};

// 加法表达式节点
struct AddExp : Node {
    NodePtr mul_exp; 
    std::vector<std::pair<std::string, NodePtr>> ops_exps; // 存储操作符和右侧的MulExp
};

// 乘法表达式节点
struct MulExp : Node {
    NodePtr unary_exp; 
    std::vector<std::pair<std::string, NodePtr>> ops_exps; // 存储操作符和右侧的AddExp
};

// 一元表达式节点
struct UnaryExp : Node {
    std::variant<
        PrimaryExp,  
        std::pair<std::string, UnaryExp>, 
        std::pair<Ident, FuncRParams> // Ident 函数调用 (包括参数)
    > variant;
};

// 基本表达式节点
struct PrimaryExp : Node {
    std::variant<
        Ident,    
        Number,     
        Exp  
    > variant;
};

struct FuncRParams : Node {
    std::vector<NodePtr> exps;
};

// 条件表达式节点
struct Cond : Node {
    NodePtr lhs; // 左侧表达式
    std::string op; // 比较操作符，如 '<', '>', '=='
    NodePtr rhs; // 右侧表达式
};

struct Ident : Node {
    std::string name;
};

struct Number : Node {
    std::string value;
};

// 主要入口点，用于构建整个语法树
std::shared_ptr<CompUnit> Parse(const std::string& source);

// 辅助函数，用于创建节点
NodePtr MakeIdent(const std::string& name);