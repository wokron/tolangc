#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <vector>

struct Node {
    int lineno;

    Node() = default;
    Node(int lineno) : lineno(lineno) {}
    virtual void print(std::ostream &out) = 0;
};

struct GetStmt;
struct PutStmt;
struct TagStmt;
struct LetStmt;
struct IfStmt;
struct ToStmt;
using Stmt = std::variant<GetStmt, PutStmt, TagStmt, LetStmt, IfStmt, ToStmt>;

struct BinaryExp;
struct CallExp;
struct UnaryExp;
struct IdentExp;
struct Number;
using Exp = std::variant<BinaryExp, CallExp, UnaryExp, IdentExp, Number>;

struct FuncDef;
struct VarDecl;

struct Ident : public Node {
    std::string value;

    void print(std::ostream &out) override;

    Ident() = default;
    Ident(int lineno, const std::string &value) : Node(lineno), value(value) {}
};

struct CompUnit : public Node {
    std::vector<std::shared_ptr<FuncDef>> func_defs;
    std::vector<std::shared_ptr<VarDecl>> var_decls;
    std::vector<std::shared_ptr<Stmt>> stmts;

    void print(std::ostream &out) override;
};

struct FuncDef : public Node {
    std::shared_ptr<Ident> ident;
    std::vector<std::shared_ptr<Ident>> func_f_params;
    std::shared_ptr<Exp> exp;

    void print(std::ostream &out) override;
};

struct VarDecl : public Node {
    std::shared_ptr<Ident> ident;

    void print(std::ostream &out) override;
};

struct GetStmt : public Node {
    std::shared_ptr<Ident> ident;

    void print(std::ostream &out) override;
};

struct PutStmt : public Node {
    std::shared_ptr<Exp> exp;

    void print(std::ostream &out) override;
};

struct TagStmt : public Node {
    std::shared_ptr<Ident> ident;

    void print(std::ostream &out) override;
};

struct LetStmt : public Node {
    std::shared_ptr<Ident> ident;
    std::shared_ptr<Exp> exp;

    void print(std::ostream &out) override;
};

struct Cond;

struct IfStmt : public Node {
    std::shared_ptr<Cond> cond;
    std::shared_ptr<Ident> ident;

    void print(std::ostream &out) override;
};

struct ToStmt : public Node {
    std::shared_ptr<Ident> ident;

    void print(std::ostream &out) override;
};

struct BinaryExp : public Node {
    std::shared_ptr<Exp> lhs;
    enum BinaryOp {
        PLUS,
        MINU,
        MULT,
        DIV,
        MOD,
    } op;
    std::shared_ptr<Exp> rhs;

    BinaryExp() = default;

    BinaryExp(BinaryOp op, std::shared_ptr<Exp> lhs, std::shared_ptr<Exp> rhs)
        : lhs(lhs), op(op), rhs(rhs) {}

    void print(std::ostream &out) override;
};

struct FuncRParams;

struct CallExp : public Node {
    std::shared_ptr<Ident> ident;
    std::shared_ptr<FuncRParams> func_r_params;

    void print(std::ostream &out) override;
};

struct UnaryExp : public Node {
    enum {
        PLUS,
        MINU,
    } op;
    std::shared_ptr<Exp> exp;
    void print(std::ostream &out) override;
};

struct IdentExp : public Node {
    std::shared_ptr<Ident> ident;

    void print(std::ostream &out) override;
};

struct FuncRParams : public Node {
    std::vector<std::shared_ptr<Exp>> exps;

    void print(std::ostream &out) override;
};

struct Cond : public Node {
    std::shared_ptr<Exp> lhs;
    enum {
        LT,
        GT,
        LE,
        GE,
        EQ,
        NE,
    } op;
    std::shared_ptr<Exp> rhs;

    void print(std::ostream &out) override;
};

struct Number : public Node {
    float value;

    void print(std::ostream &out) override;
};
