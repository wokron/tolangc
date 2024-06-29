#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <front/lexer/token.h>

struct Node {
    int line;

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
struct Number;
using Exp = std::variant<BinaryExp, CallExp, UnaryExp, Ident, Number>;
struct FuncDef;
struct VarDecl;


struct Ident : public Node {
    std::string ident;

    void print(std::ostream &out) override;
};

struct CompUnit : public Node {
    std::vector<std::shared_ptr<FuncDef>> funcDefs;
    std::vector<std::shared_ptr<VarDecl>> varDecls;
    std::vector<std::shared_ptr<Stmt>> stmts;

    void print(std::ostream &out) override;
};

struct FuncFParams;

struct FuncDef : public Node {
    Ident ident;
    std::shared_ptr<FuncFParams> funcFParams;
    std::shared_ptr<Exp> exp;

    void print(std::ostream &out) override;
};

struct FuncFParams : public Node {
    std::vector<Ident> idents;

    void print(std::ostream &out) override;
};

struct VarDecl : public Node {
    Ident ident;

    void print(std::ostream &out) override;
};

struct GetStmt : public Node {
    Ident ident;

    void print(std::ostream &out) override;
};

struct PutStmt : public Node {
    std::shared_ptr<Exp> exp;

    void print(std::ostream &out) override;
};

struct TagStmt : public Node {
    Ident ident;

    void print(std::ostream &out) override;
};

struct LetStmt : public Node {
    Ident ident;
    std::shared_ptr<Exp> exp;

    void print(std::ostream &out) override;
};

struct Cond;

struct IfStmt : public Node {
    std::shared_ptr<Cond> cond;
    Ident ident;

    void print(std::ostream &out) override;
};

struct ToStmt : public Node {
    Ident ident;

    void print(std::ostream &out) override;
};

struct BinaryExp : public Node {
    std::shared_ptr<Exp> lexp;
    enum {
        PLUS,
        MINU,
        MULT,
        DIV,
        MOD,
    } op;
    std::shared_ptr<Exp> rexp;

    void print(std::ostream &out) override;
};

struct FuncRParams;

struct CallExp : public Node {
    Ident ident;
    std::shared_ptr<FuncRParams> funcRParams;

    void print(std::ostream &out) override;
};

struct UnaryExp : public Node {
    enum {
        PLUS,
        MINU,
    } op;
    std::shared_ptr<Exp> exp;
    bool hasOp;
    void print(std::ostream &out) override;
};

struct FuncRParams : public Node {
    std::vector<std::shared_ptr<Exp>> exps;

    void print(std::ostream &out) override;
};

struct Cond : public Node {
    std::shared_ptr<Exp> left;
    enum {
        LSS,
        GRE,
        LEQ,
        GEQ,
        EQL,
        NEQ,
    } op;
    std::shared_ptr<Exp> right;

    void print(std::ostream &out) override;
};

struct Number : public Node {
    float value;

    void print(std::ostream &out) override;
};
