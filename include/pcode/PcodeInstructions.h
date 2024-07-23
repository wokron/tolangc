#pragma once

#include "pcode/PcodeInstruction.h"
#include "pcode/PcodeVariable.h"
#include "pcode/PcodeFunction.h"
#include <iostream>
#include <string>

class PcodeDefineInst : public PcodeInstruction {
private:
    PcodeVarPtr & _var;

public:
    PcodeDefineInst(PcodeVarPtr &var):
        PcodeInstruction(PcodeInstruction::DEF), _var(var) {}
    
    void print(std::ostream &out) const override {
        out << "DEF\t\t" << _var->getName() << std::endl;
    }
};

class PcodeArgumentInst : public PcodeInstruction {
private:
    int _index;

public:
    PcodeArgumentInst(int index) : 
        PcodeInstruction(PcodeInstruction::ARG), _index(index) {}

    int getIndex() const { return _index; }

    void print(std::ostream &out) const override {
        out << "ARG\t\t" << _index << std::endl;
    }
};

class PcodeLoadImmediateInst : public PcodeInstruction {
private:
    float _imm;

public:
    PcodeLoadImmediateInst(float imm) :
        PcodeInstruction(PcodeInstruction::LI), _imm(imm) {}

    float getImm() const { return _imm; }

    void print(std::ostream &out) const override {
        out << "LI\t\t" << _imm << std::endl;
    }
};

class PcodeOperationInst : public PcodeInstruction {
public:
    typedef enum {
        ADD, SUB, MUL, DIV, /* MOD, */ NEG,
        LES, LEQ, GRE, GEQ, EQL, NEQ
    } OperationType;

private:
    OperationType _op;

public:
    PcodeOperationInst(OperationType op) :
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

class PcodeLoadInst : public PcodeInstruction {
private:
    PcodeVarPtr _var;

public:
    PcodeLoadInst(PcodeVarPtr & var) : 
        PcodeInstruction(PcodeInstruction::LOAD), _var(var) {}

    PcodeVarPtr &getVar() { return _var; }

    void print(std::ostream &out) const override {
        out << "LOAD\t" << _var->getName() << std::endl;
    }
};

class PcodeStoreInst : public PcodeInstruction {
private:
    PcodeVarPtr _var;

public:
    PcodeStoreInst(PcodeVarPtr &var):
        PcodeInstruction(PcodeInstruction::STORE), _var(var) {}

    PcodeVarPtr &getVar() { return _var; }

    void print(std::ostream &out) const override {
        out << "STORE\t" << _var->getName() << std::endl;
    }
};

class PcodeCallInst : public PcodeInstruction {
private:
    PcodeFuncPtr _fn;

public:
    PcodeCallInst(PcodeFuncPtr &fn) : 
        PcodeInstruction(PcodeInstruction::CALL), _fn(fn) {}
    
    PcodeFuncPtr &getFn() { return _fn; }

    void print(std::ostream &out) const {
        out << "CALL\t" << _fn->getName() << std::endl;
    }
};

class PcodeReturnInst : public PcodeInstruction {
public:
    PcodeReturnInst() : PcodeInstruction(PcodeInstruction::RET) {}

    void print(std::ostream &out) const override {
        out << "RET" << std::endl;
    }
};

class PcodeJumpIfTrueInst : public PcodeInstruction {
private:
    std::string _label;

public:
    PcodeJumpIfTrueInst(const std::string &label) :
        PcodeInstruction(PcodeInstruction::JIT), _label(label) {}

    const std::string &getLabel() const { return _label; }

    void print(std::ostream &out) const override {
        out << "JIT\t\t" << _label << std::endl;
    }
};

class PcodeJumpInst : public PcodeInstruction {
private:
    std::string _label;

public:
    PcodeJumpInst(const std::string &label) :
        PcodeInstruction(PcodeInstruction::JUMP), _label(label) {}

    const std::string &getLabel() const { return _label; }

    void print(std::ostream &out) const override {
        out << "JUMP\t" << _label << std::endl;
    }
};

class PcodeReadInst : public PcodeInstruction {
public:
    PcodeReadInst() : PcodeInstruction(PcodeInstruction::READ) {}

    void print(std::ostream &out) const override {
        out << "READ" << std::endl;
    }
};

class PcodeWriteInst : public PcodeInstruction {
public:
    PcodeWriteInst() : PcodeInstruction(PcodeInstruction::WRITE) {}

    void print(std::ostream &out) const override {
        out << "WRITE" << std::endl;
    }
};

class PcodeLabelInst : public PcodeInstruction {
private:
    std::string _label;

public:
    PcodeLabelInst(const std::string &label) :
        PcodeInstruction(PcodeInstruction::LABEL), _label(label) {}

    const std::string &getLabel() const { return _label; }

    void print(std::ostream &out) const override {
        out << "LABEL\t" << _label << std::endl;
    }
};

// using DefInstPtr = std::shared_ptr<PcodeDefineInst>;
// using ArgInstPtr = std::shared_ptr<PcodeArgumentInst>;
// using LiInstPtr = std::shared_ptr<PcodeLoadImmediateInst>;
// using OprInstPtr = std::shared_ptr<PcodeOperationInst>;
// using LoadInstPtr = std::shared_ptr<PcodeLoadInst>;
// using StoreInstPtr = std::shared_ptr<PcodeStoreInst>;
// using CallInstPtr = std::shared_ptr<PcodeCallInst>;
// using RetInstPtr = std::shared_ptr<ReturnInst>;
// using JitInstPtr = std::shared_ptr<PcodeJumpIfTrueInst>;
// using JumpInstPtr = std::shared_ptr<PcodeJumpInst>;
// using ReadInstPtr = std::shared_ptr<PcodeReadInst>;
// using WriteInstPtr = std::shared_ptr<PcodeWriteInst>;
// using LabelInstPtr = std::shared_ptr<PcodeLabelInst>;
