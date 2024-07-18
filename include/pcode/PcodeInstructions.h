#pragma once

#include "./PcodeInstruction.h"
#include "./PcodeVariable.h"
#include "./PcodeFunction.h"
#include <iostream>
#include <string>

class DefineInst : public PcodeInstruction {
private:
    PcodeVarPtr & _var;

public:
    DefineInst(PcodeVarPtr &var):
        PcodeInstruction(PcodeInstruction::DEF), _var(var) {}
    
    void print(std::ostream &out) const override {
        out << "DEF\t\t" << _var->getName() << std::endl;
    }
};

class ArgumentInst : public PcodeInstruction {
private:
    int _index;

public:
    ArgumentInst(int index) : 
        PcodeInstruction(PcodeInstruction::ARG), _index(index) {}

    int getIndex() const { return _index; }

    void print(std::ostream &out) const override {
        out << "ARG\t\t" << _index << std::endl;
    }
};

class LoadImmediateInst : public PcodeInstruction {
private:
    float _imm;

public:
    LoadImmediateInst(float imm) :
        PcodeInstruction(PcodeInstruction::LI), _imm(imm) {}

    float getImm() const { return _imm; }

    void print(std::ostream &out) const override {
        out << "LI\t\t" << _imm << std::endl;
    }
};

class OperationInst : public PcodeInstruction {
public:
    typedef enum {
        ADD, SUB, MUL, DIV, /* MOD, */ NEG,
        LES, LEQ, GRE, GEQ, EQL, NEQ
    } OperationType;

private:
    OperationType _op;

public:
    OperationInst(OperationType op) :
        PcodeInstruction(PcodeInstruction::OPR), _op(op) {}

    OperationType getOp() const { return _op; }

    void print(std::ostream &out) const override { 
        out << "OPR\t\t";
        std::string op;
        switch (_op) {
            case ADD: op = "add"; break;
            case SUB: op = "sub"; break;
            case MUL: op = "mul"; break;
            case DIV: op = "div"; break;
            // case MOD: op = "mod"; break;   
            case NEG: op = "neg"; break;
            case LES: op = "les"; break;
            case LEQ: op = "leq"; break;
            case GRE: op = "gre"; break;
            case GEQ: op = "geq"; break;
            case EQL: op = "eql"; break;
            case NEQ: op = "neq"; break;
        }
        out << op << std::endl;
    }
};

class LoadInst : public PcodeInstruction {
private:
    PcodeVarPtr _var;

public:
    LoadInst(PcodeVarPtr & var) : 
        PcodeInstruction(PcodeInstruction::LOAD), _var(var) {}

    PcodeVarPtr &getVar() { return _var; }

    void print(std::ostream &out) const override {
        out << "LOAD\t" << _var->getName() << std::endl;
    }
};

class StoreInst : public PcodeInstruction {
private:
    PcodeVarPtr _var;

public:
    StoreInst(PcodeVarPtr &var):
        PcodeInstruction(PcodeInstruction::STORE), _var(var) {}

    PcodeVarPtr &getVar() { return _var; }

    void print(std::ostream &out) const override {
        out << "STORE\t" << _var->getName() << std::endl;
    }
};

class CallInst : public PcodeInstruction {
private:
    PcodeFuncPtr _fn;

public:
    CallInst(PcodeFuncPtr &fn) : 
        PcodeInstruction(PcodeInstruction::CALL), _fn(fn) {}
    
    PcodeFuncPtr &getFn() { return _fn; }

    void print(std::ostream &out) const {
        out << "CALL\t" << _fn->getName() << std::endl;
    }
};

class ReturnInst : public PcodeInstruction {
public:
    ReturnInst() : PcodeInstruction(PcodeInstruction::RET) {}

    void print(std::ostream &out) const override {
        out << "RET" << std::endl;
    }
};

class JumpIfTrueInst : public PcodeInstruction {
private:
    std::string _label;

public:
    JumpIfTrueInst(const std::string &label) :
        PcodeInstruction(PcodeInstruction::JIT), _label(label) {}

    const std::string &getLabel() const { return _label; }

    void print(std::ostream &out) const override {
        out << "JIT\t\t" << _label << std::endl;
    }
};

class JumpInst : public PcodeInstruction {
private:
    std::string _label;

public:
    JumpInst(const std::string &label) :
        PcodeInstruction(PcodeInstruction::JUMP), _label(label) {}

    const std::string &getLabel() const { return _label; }

    void print(std::ostream &out) const override {
        out << "JUMP\t" << _label << std::endl;
    }
};

class ReadInst : public PcodeInstruction {
public:
    ReadInst() : PcodeInstruction(PcodeInstruction::READ) {}

    void print(std::ostream &out) const override {
        out << "READ" << std::endl;
    }
};

class WriteInst : public PcodeInstruction {
public:
    WriteInst() : PcodeInstruction(PcodeInstruction::WRITE) {}

    void print(std::ostream &out) const override {
        out << "WRITE" << std::endl;
    }
};

class LabelInst : public PcodeInstruction {
private:
    std::string _label;

public:
    LabelInst(const std::string &label) :
        PcodeInstruction(PcodeInstruction::LABEL), _label(label) {}

    const std::string &getLabel() const { return _label; }

    void print(std::ostream &out) const override {
        out << "LABEL\t" << _label << std::endl;
    }
};

using DefInstPtr = std::shared_ptr<DefineInst>;
using ArgInstPtr = std::shared_ptr<ArgumentInst>;
using LiInstPtr = std::shared_ptr<LoadImmediateInst>;
using OprInstPtr = std::shared_ptr<OperationInst>;
using LoadInstPtr = std::shared_ptr<LoadInst>;
using StoreInstPtr = std::shared_ptr<StoreInst>;
using CallInstPtr = std::shared_ptr<CallInst>;
using RetInstPtr = std::shared_ptr<ReturnInst>;
using JitInstPtr = std::shared_ptr<JumpIfTrueInst>;
using JumpInstPtr = std::shared_ptr<JumpInst>;
using ReadInstPtr = std::shared_ptr<ReadInst>;
using WriteInstPtr = std::shared_ptr<WriteInst>;
using LabelInstPtr = std::shared_ptr<LabelInst>;
