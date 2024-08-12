#pragma once

#include "llvm/utils.h"

enum MipsRegType {
    ZeroRegTy,    //$zero
    ArgRegTy,     //$a0-$a3
    ValueRegTy,   //$v0-$v1
    TmpRegTy,     //$t0-$t7
    FloatRegTy,   //$f0-$f31
    RetAddrRegTy, //$ra
    StkPtrRegTy,  //$sp
    FrmPtrRegTy,  //$fp
    OffsetTy      // offset，用于记录栈上没有分配reg的值的位置
};

class MipsReg {
    friend MipsManager;

public:
    int GetIndex() const { return index; };
    MipsRegType GetType() { return type; };
    virtual void PrintReg(std::ostream &out) {};

protected:
    MipsReg(int index, MipsRegType type) : index(index), type(type){};

private:
    int index;
    MipsRegType type;
};

class ZeroReg : public MipsReg {
    friend MipsManager;
    ZeroReg() : MipsReg(-1, ZeroRegTy){};
    void PrintReg(std::ostream &out) override;
};

class ArgumentReg : public MipsReg {
    friend MipsManager;

public:
    explicit ArgumentReg(int index) : MipsReg(index, ArgRegTy) {
        if (index > 3 || index < 0)
            TOLANG_DIE("Operation not supported.");
    };
    void PrintReg(std::ostream &out) override;
};

class ValueReg : public MipsReg {
    friend MipsManager;
    explicit ValueReg(int index) : MipsReg(index, ValueRegTy) {
        if (index > 1 || index < 0)
            TOLANG_DIE("MipsReg not supported.");
    };
    void PrintReg(std::ostream &out) override;
};

class TmpReg : public MipsReg {
    friend MipsManager;
    explicit TmpReg(int index) : MipsReg(index, TmpRegTy) {
        if (index > 7 || index < 0)
            TOLANG_DIE("MipsReg not supported.");
    };
    void PrintReg(std::ostream &out) override;
};

class FloatReg : public MipsReg {
    friend MipsManager;
    explicit FloatReg(int index) : MipsReg(index, FloatRegTy) {
        if (index > 31 || index < 0)
            TOLANG_DIE("MipsReg not supported.");
    };
    void PrintReg(std::ostream &out) override;
};

class RetAddrReg : public MipsReg {
    friend MipsManager;
    RetAddrReg() : MipsReg(-1, RetAddrRegTy){};
    void PrintReg(std::ostream &out) override;
};

class StkPtrReg : public MipsReg {
    friend MipsManager;
    StkPtrReg() : MipsReg(-1, StkPtrRegTy){};
    void PrintReg(std::ostream &out) override;
};

class FrmPtrReg : public MipsReg {
    friend MipsManager;
    FrmPtrReg() : MipsReg(-1, FrmPtrRegTy){};
    void PrintReg(std::ostream &out) override;
};

class OffsetReg : public MipsReg {
    friend MipsManager;
    explicit OffsetReg(int offset) : MipsReg(offset, MipsRegType::OffsetTy){};
    void PrintReg(std::ostream &out) override;
};
