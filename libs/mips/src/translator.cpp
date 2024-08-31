#include "mips/translator.h"
#include "llvm/utils.h"
#include "llvm/asm/AsmWriter.h"
#include "llvm/ir/value/Use.h"
#include <cassert>

void Translator::print(std::ostream &out) { manager->PrintMips(out); }

void Translator::translate(const ModulePtr &modulePtr) {
    auto func = modulePtr->FunctionBegin();
    while (func != modulePtr->FunctionEnd()) {
        translate(*func);
        func++;
    }
    translate(modulePtr->MainFunction());
}

void Translator::translate(FunctionPtr functionPtr) {
    manager->resetFrame(functionPtr->GetName());
    auto block = functionPtr->BasicBlockBegin();
    while (block != functionPtr->BasicBlockEnd()) {
        translate(*block);
        block++;
    }
}

void Translator::translate(BasicBlockPtr basicBlockPtr) {
    auto name = *manager->getLabelName(basicBlockPtr);
    manager->addCode(new MipsLabel(name));
    auto instr = basicBlockPtr->InstructionBegin();
    while (instr != basicBlockPtr->InstructionEnd()) {
        translate(*instr);
        instr++;
    }
    manager->pushAll();
}

void Translator::translate(InstructionPtr instructionPtr) {
    if (instructionPtr->Is<AllocaInst>()) {
        translate(instructionPtr->As<AllocaInst>());
    } else if (instructionPtr->Is<BranchInst>()) {
        translate(instructionPtr->As<BranchInst>());
    } else if (instructionPtr->Is<CallInst>()) {
        translate(instructionPtr->As<CallInst>());
    } else if (instructionPtr->Is<JumpInst>()) {
        translate(instructionPtr->As<JumpInst>());
    } else if (instructionPtr->Is<LoadInst>()) {
        translate(instructionPtr->As<LoadInst>());
    } else if (instructionPtr->Is<StoreInst>()) {
        translate(instructionPtr->As<StoreInst>());
    } else if (instructionPtr->Is<ReturnInst>()) {
        translate(instructionPtr->As<ReturnInst>());
    } else if (instructionPtr->Is<InputInst>()) {
        translate(instructionPtr->As<InputInst>());
    } else if (instructionPtr->Is<OutputInst>()) {
        translate(instructionPtr->As<OutputInst>());
    } else if (instructionPtr->Is<CompareInstruction>()) {
        translate(instructionPtr->As<CompareInstruction>());
    } else if (instructionPtr->Is<UnaryOperator>()) {
        translate(instructionPtr->As<UnaryOperator>());
    } else if (instructionPtr->Is<BinaryOperator>()) {
        translate(instructionPtr->As<BinaryOperator>());
    } else {
        TOLANG_DIE("invalid instruction");
    }
    manager->tryRelease(instructionPtr);
}

void Translator::translate(BinaryOperatorPtr binaryOperatorPtr) {
    auto type =
        binaryOperatorPtr->GetType()->IsIntegerTy() ? TmpRegTy : FloatRegTy;
    manager->loadConst(binaryOperatorPtr->LeftOperand(), type);
    manager->loadConst(binaryOperatorPtr->RightOperand(), type);
    auto leftRegPtr = manager->getReg(binaryOperatorPtr->LeftOperand());
    auto rightRegPtr = manager->getReg(binaryOperatorPtr->RightOperand());
    if (rightRegPtr == nullptr || leftRegPtr == nullptr) {
        TOLANG_DIE("invalid binary operation.");
    }
    MipsRegPtr resultRegPtr = manager->allocReg(binaryOperatorPtr);

    MipsCodeType op = Error;
    if (binaryOperatorPtr->GetType()->TypeId() == Type::TypeID::IntegerTyID) {
        switch (binaryOperatorPtr->OpType()) {
        case BinaryOpType::Add:
            op = MipsCodeType::Addu;
            break;
        case BinaryOpType::Sub:
            op = MipsCodeType::Subu;
            break;
        case BinaryOpType::Mul:
            op = MipsCodeType::Mul;
            break;
        case BinaryOpType::Div:
            op = MipsCodeType::Div;
            break;
        case BinaryOpType::Mod:
            op = MipsCodeType::Rem;
            break;
        default:
            TOLANG_DIE("invalid binary int instruction.");
        }
    } else if (binaryOperatorPtr->GetType()->IsFloatTy()) {
        switch (binaryOperatorPtr->OpType()) {
        case BinaryOpType::Add:
            op = MipsCodeType::AddS;
            break;
        case BinaryOpType::Sub:
            op = MipsCodeType::SubS;
            break;
        case BinaryOpType::Mul:
            op = MipsCodeType::MulS;
            break;
        case BinaryOpType::Div:
            op = MipsCodeType::DivS;
            break;
        default:
            // rem for float is not supported
            TOLANG_DIE("invalid binary float instruction.");
        }
    } else {
        TOLANG_DIE("invalid binary instruction typeId.");
    }
    manager->addCode(new RCode(op, resultRegPtr, leftRegPtr, rightRegPtr));
    manager->occupy(resultRegPtr, binaryOperatorPtr);
}

void Translator::translate(UnaryOperatorPtr unaryOperatorPtr) {
    auto operandRegPtr = manager->loadConst(
        unaryOperatorPtr->Operand(),
        unaryOperatorPtr->GetType()->IsIntegerTy() ? TmpRegTy : FloatRegTy);

    if (unaryOperatorPtr->OpType() == UnaryOpType::Pos) { // +
        manager->occupy(operandRegPtr, unaryOperatorPtr);
    } else if (unaryOperatorPtr->OpType() == UnaryOpType::Neg) { // -
        auto result = manager->allocReg(unaryOperatorPtr);
        if (unaryOperatorPtr->GetType()->IsIntegerTy()) {
            manager->addCode(
                new RCode(Subu, result, manager->zero, operandRegPtr));
        } else if (unaryOperatorPtr->GetType()->IsIntegerTy()) {
            auto reg0 = manager->getFreeFloat();
            std::string name0 = manager->addFloat(0);
            manager->addCode(new ICode(LS, reg0, name0));
            manager->addCode(new RCode(SubS, result, reg0, operandRegPtr));
        }
        auto optype = unaryOperatorPtr->GetType()->IsFloatTy() ? SubS : Subu;

    } else { // not !
        assert(unaryOperatorPtr->OpType() == UnaryOpType::Not &&
               "invalid unary operator");
        auto result = manager->allocReg(unaryOperatorPtr);
        if (unaryOperatorPtr->GetType()->IsFloatTy()) {
            manager->addCode(new RCode(CEqS, operandRegPtr, manager->zero));
            std::string label1 = *manager->newLabelName(),
                        label2 = *manager->newLabelName();
            manager->addCode(new ICode(BC1T, label1));
            manager->addCode(new RCode(Nop));

            manager->addCode(new ICode(Addiu, result, manager->zero, 0));
            manager->addCode(new JCode(J, label2));
            manager->addCode(new RCode(Nop));

            manager->addCode(new MipsLabel(label1));
            manager->addCode(new ICode(Addiu, result, manager->zero, 1));
            manager->addCode(new MipsLabel(label2));

        } else if (unaryOperatorPtr->GetType()->IsIntegerTy()) {
            manager->addCode(
                new RCode(Seq, result, manager->zero, operandRegPtr));
        }
    }
}

void Translator::translate(CompareInstructionPtr compareInstructionPtr) {
    MipsRegType type =
        compareInstructionPtr->OperandAt(0)->GetType()->IsIntegerTy() &&
                compareInstructionPtr->OperandAt(1)->GetType()->IsIntegerTy()
            ? TmpRegTy
            : FloatRegTy;
    auto leftRegPtr =
        manager->loadConst(compareInstructionPtr->LeftOperand(), type);
    auto rightRegPtr =
        manager->loadConst(compareInstructionPtr->RightOperand(), type);

    auto resultRegPtr = manager->allocReg(compareInstructionPtr);
    MipsCodeType op;
    if (type == TmpRegTy) {
        switch (compareInstructionPtr->OpType()) {
        case CompareOpType::Equal:
            op = Seq;
            break;
        case CompareOpType::NotEqual:
            op = Sne;
            break;
        case CompareOpType::GreaterThan:
            op = Sgt;
            break;
        case CompareOpType::GreaterThanOrEqual:
            op = Sge;
            break;
        case CompareOpType::LessThan:
            op = Slt;
            break;
        case CompareOpType::LessThanOrEqual:
            op = Sle;
            break;
        default:
            op = Error;
            TOLANG_DIE("Invalid Compare operator");
        }
        manager->addCode(new RCode(op, resultRegPtr, leftRegPtr, rightRegPtr));
    } else {
        bool doOpposite = false;
        switch (compareInstructionPtr->OpType()) {
        case CompareOpType::Equal:
            op = CEqS;
            break;
        case CompareOpType::NotEqual:
            op = CEqS;
            doOpposite = true;
            break;
        case CompareOpType::GreaterThan:
            op = CLeS;
            doOpposite = true;
            break;
        case CompareOpType::GreaterThanOrEqual:
            op = CLtS;
            doOpposite = true;
            break;
        case CompareOpType::LessThan:
            op = CLtS;
            break;
        case CompareOpType::LessThanOrEqual:
            op = CLeS;
            break;
        default:
            op = Error;
            TOLANG_DIE("Invalid Compare operator");
        }
        MipsCodeType btype = doOpposite ? BC1F : BC1T;
        std::string label1 = *manager->newLabelName(),
                    label2 = *manager->newLabelName();

        manager->addCode(new RCode(op, leftRegPtr, rightRegPtr));
        manager->addCode(new ICode(btype, label1));
        manager->addCode(new RCode(Nop));

        manager->addCode(new ICode(Addiu, resultRegPtr, manager->zero, 0));
        manager->addCode(new JCode(J, label2));
        manager->addCode(new RCode(Nop));

        manager->addCode(new MipsLabel(label1));
        manager->addCode(new ICode(Addiu, resultRegPtr, manager->zero, 1));
        manager->addCode(new MipsLabel(label2));
    }
}
void Translator::translate(AllocaInstPtr allocaInstPtr) {
    manager->allocMem(allocaInstPtr, 1);
}

void Translator::translate(BranchInstPtr branchInstPtr) {
    MipsRegPtr cond = manager->loadConst(branchInstPtr->Condition(), TmpRegTy);
    std::string trueLabel = *manager->getLabelName(branchInstPtr->TrueBlock());
    std::string falseLabel =
        *manager->getLabelName(branchInstPtr->FalseBlock());

    manager->addCode(new ICode(Bnez, cond, trueLabel));
    manager->addCode(new RCode(Nop));

    manager->addCode(new JCode(J, falseLabel));
    manager->addCode(new RCode(Nop));
}

void Translator::translate(CallInstPtr callInstPtr) {
    std::set<ValuePtr> pushSet;
    for (auto occ : manager->occupation) {
        if (occ.second->GetType() != OffsetTy) {
            pushSet.insert(occ.first);
        }
    }
    for (UsePtr use : *(callInstPtr->GetUseList())) {
        pushSet.erase(use->GetValue());
    }

    int pos = manager->currentOffset - 4 -
              4 * pushSet.size();
    for (UsePtr use : *(callInstPtr->GetUseList())) {
        MipsCodeType codeType =
            use->GetValue()->GetType()->IsFloatTy() ? SS : SW;
        auto reg = manager->loadConst(
            use->GetValue(),
            use->GetValue()->GetType()->IsFloatTy() ? FloatRegTy : TmpRegTy);
        manager->addCode(new ICode(codeType, reg, manager->sp, pos));
        pos -= 4;
    }

    for (auto valuePtr : pushSet) {
        manager->push(valuePtr);
    }
    manager->addCode(
        new ICode(SW, manager->ra, manager->sp, manager->currentOffset));
    manager->currentOffset -= 4;

    manager->addCode(
        new ICode(Addiu, manager->sp, manager->sp, manager->currentOffset));
    manager->addCode(new JCode(Jal, callInstPtr->GetFunction()->GetName()));
    manager->addCode(new RCode(Nop));

    manager->addCode(
        new ICode(Subiu, manager->sp, manager->sp, manager->currentOffset));
    manager->currentOffset += 4;
    manager->addCode(
        new ICode(LW, manager->ra, manager->sp, manager->currentOffset));

    if (!callInstPtr->GetType()->IsVoidTy()) {
        auto resultReg = manager->allocReg(callInstPtr);
        if (callInstPtr->GetType()->IsIntegerTy()) {
            // returned int has been stored in $v0
            manager->addCode(
                new RCode(Addu, resultReg, manager->v0, manager->zero));
        } else if (callInstPtr->GetType()->IsFloatTy()) {
            // returned float has been stored in $f0
            auto reg0 = manager->getFreeFloat();
            std::string name0 = manager->addFloat(0);
            manager->addCode(new ICode(LS, reg0, name0));
            manager->addCode(new RCode(AddS, resultReg, manager->f0, reg0));
        }
    }
}

void Translator::translate(JumpInstPtr jumpInstPtr) {
    std::string label = *manager->getLabelName(jumpInstPtr->Target());
    manager->addCode(new JCode(J, label));
    manager->addCode(new RCode(Nop));
}

void Translator::translate(LoadInstPtr loadInstPtr) {
    // Operand -> pointerty
    auto offsetptr = manager->getReg(loadInstPtr->Operand());

    auto resultReg = manager->allocReg(loadInstPtr);
    MipsCodeType op = loadInstPtr->GetType()->IsFloatTy() ? LS : LW;
    manager->addCode(
        new ICode(op, resultReg, manager->sp, offsetptr->GetIndex()));
}

void Translator::translate(StoreInstPtr storeInstPtr) {
    // LeftOperand -> IntegerTy / FloatTy
    // RightOperand -> PointerTy
    auto operand = manager->loadConst(
        storeInstPtr->LeftOperand(),
        storeInstPtr->LeftOperand()->GetType()->IsFloatTy() ? FloatRegTy
                                                            : TmpRegTy);
    if (operand == nullptr)
        return;
    MipsCodeType op =
        storeInstPtr->LeftOperand()->GetType()->IsFloatTy() ? SS : SW;

    auto offsetptr = manager->getReg(storeInstPtr->RightOperand());
    if (storeInstPtr->RightOperand()->Is<GlobalValue>()) {
        manager->addCode(new ICode(op, operand, offsetptr, 0));
    } else {
        manager->addCode(
            new ICode(op, operand, manager->sp, offsetptr->GetIndex()));
    }
}

void Translator::translate(ReturnInstPtr returnInstPtr) {
    if (returnInstPtr->ReturnValue() != nullptr) {
        if (manager->functionName == "main") {
            manager->addCode(new ICode(Addiu, manager->v0, manager->zero, 10));
            manager->addCode(new RCode(Syscall));
            return;
        }
        if (returnInstPtr->ReturnValue()->GetType()->IsIntegerTy()) {
            // returned int stored in $v0
            auto reg =
                manager->loadConst(returnInstPtr->ReturnValue(), TmpRegTy);
            manager->addCode(new RCode(Addu, manager->v0, reg, manager->zero));
        } else {
            // returned float stored in $f0
            auto reg =
                manager->loadConst(returnInstPtr->ReturnValue(), FloatRegTy);

            auto reg0 = manager->getFreeFloat();
            std::string name0 = manager->addFloat(0);
            manager->addCode(new ICode(LS, reg0, name0));
            manager->addCode(new RCode(AddS, manager->f0, reg, reg0));
        }
        manager->addCode(new RCode(Jr, manager->ra));
        manager->addCode(new RCode(Nop));
    }
}

void Translator::translate(InputInstPtr inputInstPtr) {
    // input number-float
    // v0 = 6, syscall, f0 = input
    manager->addCode(new ICode(Addiu, manager->v0, manager->zero, 6));
    manager->addCode(new RCode(Syscall));

    auto reg0 = manager->getFreeFloat();
    std::string name0 = manager->addFloat(0);
    manager->addCode(new ICode(LS, reg0, name0));
    auto result = manager->allocReg(inputInstPtr);
    manager->addCode(new RCode(AddS, result, manager->f0, reg0));
}

void Translator::translate(OutputInstPtr outputInstPtr) {
    // print number-float
    // v0 = 2, f12 = output, syscall
    auto reg0 = manager->getFreeFloat();
    std::string name0 = manager->addFloat(0);

    auto reg = manager->loadConst(outputInstPtr->Operand(), FloatRegTy);

    manager->addCode(new ICode(LS, reg0, name0));
    manager->addCode(new RCode(AddS, manager->f12, reg, reg0));
    manager->addCode(new ICode(Addiu, manager->v0, manager->zero, 2));
    manager->addCode(new RCode(Syscall));
    // for test: put '\n'
    manager->addCode(new ICode(Addiu, manager->a0, manager->zero, 10));
    manager->addCode(new ICode(Addiu, manager->v0, manager->zero, 11));
    manager->addCode(new RCode(Syscall));
}
