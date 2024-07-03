//

#pragma once
#include "asm/AsmPrinter.h"
#include "utils.h"

enum MipsRegType {
    ZeroRegTy,    //$zero
    ArgRegTy,     //$a0-$a3
    ValueRegTy,   //$v0-$v1
    TmpRegTy,     //$t0-$t7
    FloatRegTy,   //$f0-$f9
    RetAddrRegTy, //$ra
    StkPtrRegTy,  //$sp
    FrmPtrRegTy,  //$fp
    OffsetTy      // offset，用于记录栈上没有分配reg的值的位置
};

class MipsReg {
    friend MipsManager;

public:
    int GetIndex() { return index; };
    MipsRegType GetType() { return type; };
    virtual void PrintReg(AsmWriterPtr out);

protected:
    MipsReg(int index, MipsRegType type) : index(index), type(type){};

private:
    int index;
    MipsRegType type;
};

class ZeroReg : public MipsReg {
    friend MipsManager;

public:
    ZeroReg() : MipsReg(-1, ZeroRegTy){};
};

class ArgumentReg : public MipsReg {
    friend MipsManager;

public:
    ArgumentReg(int index) : MipsReg(index, ArgRegTy) {
        if (index > 3 || index < 0)
            TOLANG_DIE("Operation not supported.");
    };
};

class ValueReg : public MipsReg {
    friend MipsManager;
    ValueReg(int index) : MipsReg(index, ValueRegTy) {
        if (index > 1 || index < 0)
            TOLANG_DIE("MipsReg not supported.");
    };
};

class TmpReg : public MipsReg {
    friend MipsManager;
    TmpReg(int index) : MipsReg(index, TmpRegTy) {
        if (index > 7 || index < 0)
            TOLANG_DIE("MipsReg not supported.");
    };
};

class FloatReg : public MipsReg {
    friend MipsManager;
    FloatReg(int index) : MipsReg(index, FloatRegTy) {
        if (index > 9 || index < 0)
            TOLANG_DIE("MipsReg not supported.");
    };
};

class RetAddrReg : public MipsReg {
    friend MipsManager;
    RetAddrReg() : MipsReg(-1, RetAddrRegTy){};
};

class StkPtrReg : public MipsReg {
    friend MipsManager;
    StkPtrReg() : MipsReg(-1, StkPtrRegTy){};
};

class FrmPtrReg : public MipsReg {
    friend MipsManager;
    FrmPtrReg() : MipsReg(-1, FrmPtrRegTy){};
};

class Offset : public MipsReg {
    friend MipsManager;
    Offset(int offset) : MipsReg(offset, MipsRegType::OffsetTy){};
};
