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
    for (int i=0; i<FLOATCOUNT; i++) {
        floatRegPool.insert(std::pair<int, FloatRegPtr>(i, new FloatReg(i)));
    }
    zero = new ZeroReg();
    fp = new FrmPtrReg();
    sp = new StkPtrReg();
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

void MipsManager::resetFrame() {
    addCode(new ICode(Addiu, static_cast<MipsRegPtr>(sp), static_cast<MipsRegPtr>(fp), 0));
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

MipsRegPtr MipsManager::allocReg(ValuePtr valuePtr) {
    MipsRegPtr mipsRegPtr = getFree(valuePtr->GetType()->TypeId());
    occupy(mipsRegPtr, valuePtr);
    return mipsRegPtr;
}

MipsRegPtr MipsManager::getReg(ValuePtr valuePtr) {
    if (occupation.count(valuePtr) == 0) return nullptr;
    if (occupation.find(valuePtr)->second->GetType() == OffsetTy) {
        load(valuePtr);
    }
    return occupation.find(valuePtr)->second;
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
    addCode(new ICode(codeType, sp, sp, currentOffset));

    release(valuePtr);
    occupation.insert(std::pair<ValuePtr, MipsRegPtr>(valuePtr, new Offset(currentOffset)));
    currentOffset -= 4;
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