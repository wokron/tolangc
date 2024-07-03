//
#pragma once
#include "asm/AsmPrinter.h"
#include "mips/mips_forward.h"
#include "mips/mips_inst.h"
#include "mips/mips_reg.h"
#include "translator.h"
#include "llvm/ir/IrForward.h"
#include "llvm/ir/Llvm.h"
#include <map>
#include <set>
#include <vector>

#define TMPCOUNT 8
#define FLOATCOUNT 10

class MipsManager {
    friend Translator;

private:
    std::vector<MipsDataPtr> datas;
    int asciizCount = 0;
    int floatDataCount = 0;
    int labelCount = 0;
    std::unordered_map<float, FloatDataPtr> floatMap;

    std::vector<MipsCodePtr> codes;
    AsmPrinter *printer;
    std::ostream &_out;

    std::unordered_map<int, TmpRegPtr> tmpRegPool;
    std::unordered_map<int, FloatRegPtr> floatRegPool;
    ZeroReg *zero;
    FrmPtrReg *fp;
    StkPtrReg *sp;
    int currentOffset = 0;

    std::unordered_map<ValuePtr, MipsRegPtr> occupation;

    int tmpCount = 0;
    int floatCount = 0;
    void addCode(MipsCodePtr codePtr) { codes.emplace_back(codePtr); };
    void addData(MipsDataPtr dataPtr) { datas.emplace_back(dataPtr); };
    void addAsciiz(std::string);
    std::string addFloat(float f);
    void resetFrame();
    MipsRegPtr allocReg(ValuePtr valuePtr);
    MipsRegPtr getReg(ValuePtr valuePtr);
    void tryRelease(UserPtr userPtr);

    TmpRegPtr getFreeTmp();
    FloatRegPtr getFreeFloat();
    MipsRegPtr getFree(Type::TypeID type);
    void release(ValuePtr valuePtr);
    void occupy(MipsRegPtr mipsRegPtr, ValuePtr valuePtr);
    void push(ValuePtr valuePtr);
    void load(ValuePtr valuePtr);

public:
    explicit MipsManager(std::ostream &_out);
    void PrintMips();
};