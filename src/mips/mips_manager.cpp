//
// Created by 荇子 on 2024/6/19.
//

#include "mips/mips_manager.h"
#include "llvm/ir/value/Use.h"
#include <cassert>

MipsManager::MipsManager(std::ostream &_out) : _out(_out) {
    printer = new AsmPrinter();
    for (int i=0; i<TMPCOUNT; i++) {
        tmpRegPool.insert(std::pair<int, TmpRegPtr>(i, new TmpReg(i)));
    }
    // f0 保留
    for (int i=1; i<FLOATCOUNT; i++) {
        if (i == 12)
            continue;
        floatRegPool.insert(std::pair<int, FloatRegPtr>(i, new FloatReg(i)));
    }
    zero = new ZeroReg();
    sp = new StkPtrReg();
    ra = new RetAddrReg();
    v0 = new ValueReg(0);
    f0 = new FloatReg(0);
    f12 = new FloatReg(12);
}

void MipsManager::PrintMips() {
    AsmWriterPtr writer = AsmWriter::New(_out);
    for (auto data: datas) {
        data->PrintData(writer);
    }

    for (auto code: codes) {
        code->PrintCode(writer);
    }

}

void MipsManager::addAsciiz(std::string value) {
    addData(new AsciizData("str"+std::to_string(asciizCount++), value));
}

std::string MipsManager::addFloat(float f) {
    if (floatMap.count(f) == 0){
        auto ptr = new FloatData("flt"+std::to_string(floatDataCount++), f);
        addData(ptr);
        floatMap.insert(std::pair<float, FloatDataPtr>(f, ptr));
    }
    return floatMap.find(f)->second->GetName();
}

std::string MipsManager::newLabelName() {
   return functionName + "_" +std::to_string(labelCount++);
}

std::string MipsManager::getLabelName(BasicBlockPtr basicBlockPtr) {
    if (blockNames.count(basicBlockPtr) != 0) {
        return *blockNames.find(basicBlockPtr)->second;
    }
    std::string name = newLabelName();
    blockNames.insert(std::pair<BasicBlockPtr, std::string*>(basicBlockPtr, &name));
    return name;
}

TmpRegPtr MipsManager::getFreeTmp(){
    for (int i=0; i<TMPCOUNT; i++){
        int index = (tmpCount+i) % TMPCOUNT;
        if (tmpRegPool.count(index) != 0) {
            auto tmpPtr = tmpRegPool.find(index);
            tmpCount = (index + 1) % TMPCOUNT;
            return tmpPtr->second;
        }
    }
    int index = tmpCount;
    tmpCount = (tmpCount+1) % TMPCOUNT;
    ValuePtr valuePtr;
    for (const auto& pair : occupation){
        if (pair.second->GetIndex() == index){
            valuePtr = pair.first;
            break;
        }
    }
    push(valuePtr);
    assert(tmpRegPool.count(index) == 1);
     return tmpRegPool.find(index)->second;
}

FloatRegPtr MipsManager::getFreeFloat(){
    for (int i=0; i<FLOATCOUNT; i++){
        int index = (floatCount + i) % FLOATCOUNT;
        if (floatRegPool.count(index) != 0) {
            auto floatPtr = floatRegPool.find(index);
            floatCount = (index + 1) % FLOATCOUNT;
            return floatPtr->second;
        }
    }
    int index = floatCount;
    floatCount = (floatCount + 1) % FLOATCOUNT;
    ValuePtr valuePtr;
    for (const auto& pair : occupation){
        if (pair.second->GetIndex() == index){
            valuePtr = pair.first;
            break;
        }
    }
    push(valuePtr);
    assert(floatRegPool.count(index) == 1);
    return floatRegPool.find(index)->second;
}

MipsRegPtr MipsManager::getFree(Type::TypeID type) {
    if (type == Type::FloatTyID) {
        return getFreeFloat();
    } else if (type == Type::IntegerTyID) {
        return getFreeTmp();
    } else {
        TOLANG_DIE("invalid MipsRegType");
    }
}

void MipsManager::resetFrame(std::string name) {
    this->functionName = name;
    addCode(new MipsLabel(name));
//    addCode(new ICode(Addiu, static_cast<MipsRegPtr>(sp), static_cast<MipsRegPtr>(fp), 0));
    labelCount = 0;
    currentOffset = 0;
    occupation.clear();
    tmpRegPool.clear();
    floatRegPool.clear();
    for (int i=0; i<TMPCOUNT; i++) {
        tmpRegPool.insert(std::pair<int, TmpRegPtr>(i, new TmpReg(i)));
    }
    for (int i=0; i<FLOATCOUNT; i++) {
        floatRegPool.insert(std::pair<int, FloatRegPtr>(i, new FloatReg(i)));
    }
    tmpCount = 0;
    floatCount = 0;
}

void MipsManager::allocMem(AllocaInstPtr allocaInstPtr, int size) {
    currentOffset -= 4*size;
    occupy(new OffsetReg(currentOffset + 4), allocaInstPtr);

}

MipsRegPtr MipsManager::allocReg(ValuePtr valuePtr) {
    MipsRegPtr mipsRegPtr = getFree(valuePtr->GetType()->TypeId());
    occupy(mipsRegPtr, valuePtr);
    return mipsRegPtr;
}

MipsRegPtr MipsManager::getReg(ValuePtr valuePtr) {
    if (occupation.count(valuePtr) == 0){
        if (valuePtr->Is<GlobalValue>()) {
            auto addr = allocReg(valuePtr);
            addCode(new ICode(LA, addr, valuePtr->GetName()));
        } else {
            return nullptr;
        }
    }
    if (occupation.find(valuePtr)->second->GetType() == OffsetTy) {
        // 存offset有两种情况：
        // 1. 临时寄存器池中变量推到栈中，occupation中存相对sp的偏移量；
        // 2. 指针变量所指的相对sp的位置
        if (!valuePtr->GetType()->IsPointerTy()){
            load(valuePtr);
        }
    }
    return occupation.find(valuePtr)->second;
}

MipsRegPtr MipsManager::loadConst(ValuePtr valuePtr, MipsRegType type) {
    if (!valuePtr->Is<ConstantData>()){
        return getReg(valuePtr);
    }
    MipsRegPtr newRegPtr = getFree(valuePtr->GetType()->TypeId());
    if (type == TmpRegTy){
        addCode(new ICode(Addiu, newRegPtr, zero, valuePtr->As<ConstantData>()->GetIntValue()));
    } else if (type == FloatRegTy) {
        std::string name = addFloat(valuePtr->As<ConstantData>()->GetFloatValue());
        addCode(new ICode(LS, newRegPtr, name));
    } else {
        TOLANG_DIE("wrong binary operator type");
    }
    occupy(newRegPtr, valuePtr);
    return newRegPtr;
}

void MipsManager::tryRelease(UserPtr userPtr) {
    for(UsePtr use: *(userPtr->GetUseList())){
        //TODO:完善寄存器的释放逻辑判断（基本块流图和活跃变量分析）
        release(use->GetValue());
    }
}

void MipsManager::release(ValuePtr valuePtr) {
    if (occupation.count(valuePtr)!=0){
        auto regPtr = occupation.find(valuePtr)->second;
        if (regPtr->GetType() == FloatRegTy){
            floatRegPool.insert(std::pair<int, FloatRegPtr>(regPtr->GetIndex(), (FloatRegPtr)regPtr));
        } else if (regPtr->GetType() == TmpRegTy){
            tmpRegPool.insert(std::pair<int, TmpRegPtr>(regPtr->GetIndex(), (TmpRegPtr)regPtr));
        }
        occupation.erase(valuePtr);
    }
}

void MipsManager::occupy(MipsRegPtr mipsRegPtr, ValuePtr valuePtr) {
    if (mipsRegPtr->GetType() == FloatRegTy){
        floatRegPool.erase(mipsRegPtr->GetIndex());
    } else if (mipsRegPtr->GetType() == TmpRegTy) {
        tmpRegPool.erase(mipsRegPtr->GetIndex());
    }
    occupation.insert(std::pair<ValuePtr, MipsRegPtr>(valuePtr, mipsRegPtr));
}

void MipsManager::push(ValuePtr valuePtr) {
    MipsCodeType codeType;
    if (valuePtr->GetType()->IsFloatTy()){
        codeType = SS;
    } else if (valuePtr->GetType()->IsIntegerTy()) {
        codeType = SW;
    } else {
        TOLANG_DIE("invalid value-mipsreg type");
    }
    addCode(new ICode(codeType, getReg(valuePtr), sp, currentOffset));

    release(valuePtr);
    occupation.insert(std::pair<ValuePtr, MipsRegPtr>(valuePtr, new OffsetReg(currentOffset)));
    currentOffset -= 4;
}

void MipsManager::pushAll() {
    for (auto occ: occupation) {
        if (occ.second->GetType() != OffsetTy) {
            push(occ.first);
        }
    }
}

void MipsManager::load(ValuePtr valuePtr) {
    MipsCodeType codeType;
    if (valuePtr->GetType()->IsFloatTy()){
        codeType = LS;
    } else if (valuePtr->GetType()->IsIntegerTy()) {
        codeType = LW;
    } else {
        TOLANG_DIE("invalid value-mipsreg type");
    }
    auto mipsRegPtr = getFree(valuePtr->GetType()->TypeId());
    int offset = occupation.find(valuePtr)->second->GetIndex();
    addCode(new ICode(codeType, mipsRegPtr, sp, offset));
    release(valuePtr);
    occupy(mipsRegPtr, valuePtr);
}