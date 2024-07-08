//
#pragma once
#include "asm/AsmWriter.h"
#include "mips/mips_forward.h"
#include "utils.h"
#include <optional>
#include <string>
#include <utility>

class MipsData;
class MipsCode;

class MipsData {
protected:
    virtual void PrintData(AsmWriterPtr out);
    MipsData(std::string &name) : name(name){};
    virtual void printName(AsmWriterPtr out);
    virtual void printValue(AsmWriterPtr out);

protected:
    std::string name;
    int w_value;
    float f_value;
    std::string str;
public:
    std::string GetName() { return name; };
    int GetValue{};
};

// .word
class WordData : public MipsData {
public:
    WordData(std::string name, int value) : MipsData(name) {
        w_value = value;
        f_value = 0;
        str = nullptr;
        ;
    }
    void PrintData(AsmWriterPtr out) override;

private:
    int *array;
    void printName(AsmWriterPtr out) override;
    void printValue(AsmWriterPtr out) override;
};

// .float
class FloatData : public MipsData {
public:
    FloatData(std::string name, float value) : MipsData(name) {
        w_value = 0;
        f_value = value;
        str = nullptr;
    }
    void PrintData(AsmWriterPtr out) override;

private:
    void printName(AsmWriterPtr out) override;
    void printValue(AsmWriterPtr out) override;
};

// .asciiz
class AsciizData : public MipsData {
public:
    AsciizData(std::string name, std::string &value) : MipsData(name) {
        w_value = 0;
        f_value = 0;
        str = value;
    }
    void PrintData(AsmWriterPtr out) override;

private:
    void printName(AsmWriterPtr out) override;
    void printValue(AsmWriterPtr out) override;
};

enum MipsCodeType {
    //error
    Error,
    // ICode
    BC1F,
    BC1T,
    LA,
    Bnez,
    LW,
    SW,
    LS,
    SS,
    Addiu,
    Subiu,
    // ICode or RCode
    Div,
    Mul,
    Rem,
    // RCode
    Addu,
    Subu,
    AddS,
    SubS,
    MulS,
    DivS,
    Slt,
    Sle,
    Sgt,
    Sge,
    Seq,
    Sne,
    Sll,
    CEqS,
    CLeS,
    CLtS,
    CvtSW,
    CvtWS,
    TruncWS,
    Jr,
    Nop,
    Syscall,
    // JCode
    J,
    Jal,
    // Label
    Label
};

class MipsCode {
protected:
    MipsRegPtr rs, rt, rd;
    int intermediate;
    std::string label;
    MipsCodeType op;
    MipsCode(MipsCodeType op, MipsRegPtr rd, MipsRegPtr rs, MipsRegPtr rt,
             int inter)
        : op(op), rs(rs), rt(rt), rd(rd), intermediate(inter){};

public:
    virtual void PrintCode(AsmWriterPtr out);
};

class RCode : public MipsCode {
public:
    //  Div, Mul, Rem,
    //  Addu, Subu,
    //  AddS, SubS, MulS, DivS,
    RCode(MipsCodeType op, MipsRegPtr rd, MipsRegPtr rs, MipsRegPtr rt)
        : MipsCode(op, rd, rs, rt, 0) {
        if (op < Div || op > Sne) {
            TOLANG_DIE("MipsCode not supported.");
        }
    };

    // CEqS, CLeS, CLtS, CvtSW, CvtWS, TruncWS,
    RCode(MipsCodeType op, MipsRegPtr rs, MipsRegPtr rt)
        : MipsCode(op, nullptr, rs, rt, 0) {
        if (op > TruncWS || op < CEqS) {
            TOLANG_DIE("MipsCode not supported.");
        }
    };

    // sll $t, $t, 2
    RCode(MipsCodeType op, MipsRegPtr rd, MipsRegPtr rt, int sa)
        : MipsCode(op, rd, nullptr, rt, sa) {
        if (op != Sll) {
            TOLANG_DIE("MipsCode not supported.");
        }
    };

    // jr
    RCode(MipsCodeType op, MipsRegPtr rs)
        : MipsCode(op, nullptr, rs, nullptr, 0) {
        if (op != Jr) {
            TOLANG_DIE("MipsCode not supported.");
        }
    }

    // syscall, nop
    RCode(MipsCodeType op) : MipsCode(op, nullptr, nullptr, nullptr, 0) {
        if (op != Syscall || op != Nop) {
            TOLANG_DIE("MipsCode not supported.");
        }
    };

    void PrintCode(AsmWriterPtr out) override;
};

class ICode : public MipsCode {
public:
    // LW, SW, LS, SS
    // Addiu, Subiu, Div, Mul, Rem
    ICode(MipsCodeType op, MipsRegPtr rt, MipsRegPtr rs, int inter)
        : MipsCode(op, nullptr, rs, rt, inter) {
        if (op > Rem || op < LW) {
            TOLANG_DIE("MipsCode not supported.");
        }
    }

    // Bnez, Lw, Sw, Ls, Ss,(全局变量), La
    ICode(MipsCodeType op, MipsRegPtr rs, std::string label)
        : MipsCode(op, nullptr, rs, nullptr, 0) {
        this->label = std::move(label);
        if (op > SS || op < LA) {
            TOLANG_DIE("MipsCode not supported.");
        }
    };

    // BC1F, BC1T
    ICode(MipsCodeType op, std::string label)
        : MipsCode(op, nullptr, nullptr, nullptr, 0) {
        this->label = std::move(label);
        if (op > BC1T || op < BC1F) {
            TOLANG_DIE("MipsCode not supported.");
        }
    };

    void PrintCode(AsmWriterPtr out) override;
};

class JCode : public MipsCode {
public:
    JCode(MipsCodeType op, std::string label)
        : MipsCode(op, nullptr, nullptr, nullptr, 0) {
        this->label = std::move(label);
    };
    void PrintCode(AsmWriterPtr out) override;
};

class MipsLabel : public MipsCode {
public:
    explicit MipsLabel(std::string label)
        : MipsCode(MipsCodeType::Label, nullptr, nullptr, nullptr, 0) {
        this->label = std::move(label);
    };

    std::string GetName(){return label;}
    void PrintCode(AsmWriterPtr out) override;
};