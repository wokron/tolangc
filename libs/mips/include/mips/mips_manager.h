#pragma once

#include "mips/mips_forward.h"
#include "mips/mips_inst.h"
#include "mips/mips_reg.h"
#include "llvm/asm/AsmPrinter.h"
#include "llvm/ir/IrForward.h"
#include "llvm/ir/Llvm.h"
#include <map>
#include <set>
#include <vector>

#define TMPCOUNT 8
#define FLOATCOUNT 32

class MipsManager {
    friend Translator;

private:
    std::vector<MipsDataPtr> datas;
    int asciizCount = 0;
    int floatDataCount = 0;
    std::string functionName;
    int labelCount = 0;
    std::unordered_map<float, FloatDataPtr> floatMap;

    std::vector<MipsCodePtr> codes;

    std::unordered_map<int, TmpRegPtr> tmpRegPool;
    std::unordered_map<int, FloatRegPtr> floatRegPool;
    ZeroReg *zero;
    StkPtrReg *sp;
    RetAddrReg *ra;
    ArgumentReg *a0;
    ValueReg *v0;
    // f0 和 f12 保留，用于输入输出
    FloatReg *f0;
    FloatReg *f12;
    int currentOffset = 0;

    std::unordered_map<ValuePtr, MipsRegPtr> occupation;
    std::unordered_map<BasicBlockPtr, std::string *> blockNames;

    int tmpCount = 0;
    int floatCount = 0;
    void addCode(MipsCodePtr codePtr) { codes.emplace_back(codePtr); };
    void addData(MipsDataPtr dataPtr) { datas.emplace_back(dataPtr); };
    void addAsciiz(std::string);
    std::string addFloat(float f);
    std::string *newLabelName();
    std::string *getLabelName(BasicBlockPtr basicBlockPtr);
    void resetFrame(std::string name);
    void allocMem(AllocaInstPtr allocaInstPtr, int size);
    MipsRegPtr allocReg(ValuePtr valuePtr);
    MipsRegPtr getReg(ValuePtr valuePtr);
    MipsRegPtr loadConst(ValuePtr valuePtr, MipsRegType type);
    void tryRelease(UserPtr userPtr);
    void pushAll();

    TmpRegPtr getFreeTmp();
    FloatRegPtr getFreeFloat();
    MipsRegPtr getFree(Type::TypeID type);
    void release(ValuePtr valuePtr);
    void occupy(MipsRegPtr mipsRegPtr, ValuePtr valuePtr);
    void push(ValuePtr valuePtr);
    void load(ValuePtr valuePtr);

public:
    MipsManager();
    ~MipsManager();
    void PrintMips(std::ostream &_out);
};