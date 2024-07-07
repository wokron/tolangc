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
    std::vector<std::unique_ptr<FuncDef>> func_defs;
    std::vector<std::unique_ptr<VarDecl>> var_decls;
    std::vector<std::unique_ptr<Stmt>> stmts;

    void print(std::ostream &out) override;
};

struct FuncDef : public Node {
    std::unique_ptr<Ident> ident;
    std::vector<std::unique_ptr<Ident>> func_f_params;
    std::unique_ptr<Exp> exp;

    void print(std::ostream &out) override;
};

struct VarDecl : public Node {
    std::unique_ptr<Ident> ident;

    void print(std::ostream &out) override;
};

struct GetStmt : public Node {
    std::unique_ptr<Ident> ident;

    void print(std::ostream &out) override;
};

struct PutStmt : public Node {
    std::unique_ptr<Exp> exp;

    void print(std::ostream &out) override;
};

struct TagStmt : public Node {
    std::unique_ptr<Ident> ident;

    void print(std::ostream &out) override;
};

struct LetStmt : public Node {
    std::unique_ptr<Ident> ident;
    std::unique_ptr<Exp> exp;

    void print(std::ostream &out) override;
};

struct Cond;

struct IfStmt : public Node {
    std::unique_ptr<Cond> cond;
    std::unique_ptr<Ident> ident;

    void print(std::ostream &out) override;
};

struct ToStmt : public Node {
    std::unique_ptr<Ident> ident;

    void print(std::ostream &out) override;
};

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

    BinaryExp() = default;

    BinaryExp(BinaryOp op, std::unique_ptr<Exp> lhs, std::unique_ptr<Exp> rhs)
        : lhs(std::move(lhs)), op(op), rhs(std::move(rhs)) {}

    void print(std::ostream &out) override;
};

struct CallExp : public Node {
    std::unique_ptr<Ident> ident;
    std::vector<std::unique_ptr<Exp>> func_r_params;

    void print(std::ostream &out) override;
};

struct UnaryExp : public Node {
    enum {
        PLUS,
        MINU,
    } op;
    std::unique_ptr<Exp> exp;
    void print(std::ostream &out) override;
};

struct IdentExp : public Node {
    std::unique_ptr<Ident> ident;

    void print(std::ostream &out) override;
};

struct Cond : public Node {
    std::unique_ptr<Exp> lhs;
    enum {
        LT,
        GT,
        LE,
        GE,
        EQ,
        NE,
    } op;
    std::unique_ptr<Exp> rhs;

    void print(std::ostream &out) override;
};

struct Number : public Node {
    float value;

    void print(std::ostream &out) override;
};
