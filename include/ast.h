#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <vector>

struct Node {
    int line;

    Node() = default;
    Node(int line) : line(line) {}
    virtual void print(std::ostream &out) = 0;
};

struct Ident;

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
    Ident(int line, const std::string &value) : Node(line), value(value) {}
};

struct CompUnit : public Node {
    std::vector<std::shared_ptr<FuncDef>> funcDefs;
    std::vector<std::shared_ptr<VarDecl>> varDecls;
    std::vector<std::shared_ptr<Stmt>> stmts;

    void print(std::ostream &out) override;
};

struct FuncFParams;

struct FuncDef : public Node {
    std::shared_ptr<Ident> ident;
    std::shared_ptr<FuncFParams> funcFParams;
    std::shared_ptr<Exp> exp;

    void print(std::ostream &out) override;
};

struct FuncFParams : public Node {
    std::vector<std::shared_ptr<Ident>> idents;

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
    std::shared_ptr<Exp> lexp;
    enum BinaryOp {
        PLUS,
        MINU,
        MULT,
        DIV,
        MOD,
    } op;
    std::shared_ptr<Exp> rexp;

    BinaryExp() = default;

    BinaryExp(BinaryOp op, std::shared_ptr<Exp> lexp, std::shared_ptr<Exp> rexp)
        : lexp(lexp), op(op), rexp(rexp) {}

    void print(std::ostream &out) override;
};

struct FuncRParams;

struct CallExp : public Node {
    std::shared_ptr<Ident> ident;
    std::shared_ptr<FuncRParams> funcRParams;

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
    std::shared_ptr<Exp> left;
    enum {
        LT,
        GT,
        LE,
        GE,
        EQ,
        NE,
    } op;
    std::shared_ptr<Exp> right;

    void print(std::ostream &out) override;
};

struct Number : public Node {
    float value;

    void print(std::ostream &out) override;
};
