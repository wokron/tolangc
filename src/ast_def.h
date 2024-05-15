#include <iostream>
#include <string>
#include <vector>

class Node {
public:
    int line;
    void print(){

    }
};

class CompUnit : public Node {
public:
    std::vector<Node*> funcDefs; 
    std::vector<Node*> varDecls; 
    std::vector<Node*> stmts;       
    void print() {
        printf("CompUnit: \n");
        for (auto& funcDef : funcDefs) {
            funcDef->print();
        }
        for (auto& varDecl : varDecls) {
            varDecl->print();
        }
        for (auto& stmt : stmts) {
            stmt->print();
        }
    }
};

class FuncDef : public Node {
public:
    Node* ident;
    Node* funcFParams = nullptr; 
    Node* exp; 
    void print() {
        printf("FuncDef: \n");
        ident->print();
        if(funcFParams != nullptr){
            funcFParams->print();
        }
        exp->print();
    }
};

class FuncFParams : public Node{
public:
    std::vector<Node*> idents;
    void print(){
        printf("FuncFParams: \n");
        for(auto& ident:idents){
            ident->print();
        }
    }
};

class VarDecl : public Node{
public:
    Node* ident;
    void print(){
        printf("VarDecl: \n");
        ident->print();
    }
};

class Stmt : public Node{
public:
    void print(){
        printf("Stmt: \n");
    }
};

class GetStmt : public Stmt{
public:
    Node* ident;
    void print(){
        printf("GetStmt: \n");
        ident->print();
    }
};

class PutStmt : public Stmt{
public:
    Node* exp;
    void print(){
        printf("PutStmt: \n");
        exp->print();
    }
};

class TagStmt : public Stmt{
public:
    Node* ident;
    void print(){
        printf("TagStmt: \n");
        ident->print();
    }
};

class LetStmt: public Stmt{
public:
    Node* ident;
    Node* exp;
    void print(){
        printf("LetStmt: \n");
        ident->print();
        exp->print();
    }
};

class IfStmt : public Stmt{
public:
    Node* cond;
    Node* ident;
    void print(){
        printf("IfStmt: \n");
        cond->print();
        ident->print();
    }
};

class ToStmt : public Stmt{
public:
    Node* ident;
    void print(){
        printf("ToStmt: \n");
        ident->print();
    }
};

class Exp: public Node{
public:
    Node* addExp;
    void print(){
        printf("Exp: \n");
        addExp->print();
    }
};

class AddExp : public Node{
public:
    std::vector<Node*> mulExps;
    std::vector<std::string> ops;
    void print(){
        printf("AddExp: \n");
        for(auto& mulExp : mulExps){
            mulExp->print();
        }
        for(std::string op: ops){
            printf("%s ",op);
        }
    }
};

class MulExp: public Node{
public:
    std::vector<Node*> unaryExps;
    std::vector<std::string> ops;
    void print(){
        printf("MulExp: \n");
        for(auto& unaryExp: unaryExps){
            unaryExp->print();
        }
        for(std::string op : ops){
            printf("%s ", op);
        }
    }
};

class UnaryExp: public Node{
public:
    Node* primaryExp = nullptr;
    Node* ident = nullptr;
    std::vector<Node*> funcRParams;
    std::string op;
    Node* unaryExp = nullptr;
    void print(){
        printf("UnaryExp: \n");
        if(primaryExp != nullptr){
            primaryExp->print();
        }
        else if(ident != nullptr){
            ident->print();
            for(auto& funcRParam: funcRParams){
                funcRParam->print();
            }
        }
        else if(unaryExp != nullptr){
            unaryExp->print();
        }
    }
};

class PrimaryExp : public Node{
public:
    Node* exp = nullptr;
    Node* ident = nullptr;
    Node* number = nullptr;
    void print(){
        printf("PrimaryExp: \n");
        if (exp != nullptr){
            exp->print();
        }
        else if (ident != nullptr){
            ident->print();
        }
        else if (number != nullptr){
            number->print();
        }
    }
};

class FuncRParams: public Node{
public:
    std::vector<Node*> exps;
    void print(){
        printf("FuncRParams:\n");
        for(auto& exp: exps){
            exp->print();
        }
    }
};

class Cond : public Node{
public:
    Node* lexp;
    std::string op;
    Node* rexp;
    void print(){
        printf("Cond: \n ");
        lexp->print();
        printf("op:%s \n",op);
        rexp->print();
    }
};

class Ident : public Node{
public:
    std::string name;
    void print(){
        printf("Ident: \n ==> name: %s\n",name);
    }  
};

class Number: public Node{
public:
    std::string value;
    void print(){
        printf("Number: \n ==> value: %s\n",value);
    }
};