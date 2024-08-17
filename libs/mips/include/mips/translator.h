#pragma once

#include "mips_manager.h"
#include "llvm/ir/Llvm.h"

class Translator {
private:
    MipsManager *manager;
    void translate(FunctionPtr functionPtr);
    void translate(BasicBlockPtr basicBlockPtr);
    void translate(InstructionPtr instructionPtr);

    void translate(BinaryOperatorPtr binaryOperatorPtr);
    void translate(UnaryOperatorPtr unaryOperatorPtr);
    void translate(CompareInstructionPtr compareInstructionPtr);
    void translate(AllocaInstPtr allocaInstPtr);
    void translate(BranchInstPtr branchInstPtr);
    void translate(CallInstPtr callInstPtr);
    void translate(JumpInstPtr jumpInstPtr);
    void translate(LoadInstPtr loadInstPtr);
    void translate(StoreInstPtr storeInstPtr);
    void translate(ReturnInstPtr returnInstPtr);

    void translate(InputInstPtr inputInstPtr);
    void translate(OutputInstPtr outputInstPtr);

public:
    Translator() { manager = new MipsManager(); };
    ~Translator() { delete manager; };

    void translate(const ModulePtr &modulePtr);
    void print(std::ostream &_out);
};
