#include "pcode/runtime/PcodeRuntime.h"

void PcodeRuntime::run() {
    // Create the AR for main
    _ars.push(ActivityRecord());
    // Execute every instruction in main
    while (_currentBlock != nullptr) {
        // If _currentBlock is set by JIT or JUMP
        // flag should be true
        bool flag = false;
        for (auto &inst : _currentBlock->instructions) {
            // Handle JIT and JUMP here!
            if (inst->getType() == PcodeInstruction::JIT) {
                auto &ar = _ars.top();
                if (::fabs(ar.popTmp() - 1) < eps) {
                    auto jit = std::dynamic_pointer_cast<PcodeJumpIfTrueInst>(inst);
                    _currentBlock = _module.getLabel((jit->getLabel()));
                    flag = true;
                    break;
                } else {
                    continue;
                }
            } else if (inst->getType() == PcodeInstruction::JUMP) {
                auto jump = std::dynamic_pointer_cast<PcodeJumpInst>(inst);
                _currentBlock = _module.getLabel((jump->getLabel()));
                flag = true;
                break;
            } else {
                excuteInst(inst);
            }
        }
        if (!flag) {
            _currentBlock = _currentBlock->next;
        }
    }
}

void PcodeRuntime::excuteInst(const PcodeInstPtr &inst) {
    switch (inst->getType()) {
        case PcodeInstruction::DEF: 
        case PcodeInstruction::JIT: 
        case PcodeInstruction::JUMP: 
        case PcodeInstruction::LABEL: 
            break;
        case PcodeInstruction::ARG: 
            executeArg(inst); break;
        case PcodeInstruction::LI:
            executeLi(inst); break;
        case PcodeInstruction::OPR:
            executeOpr(inst); break;
        case PcodeInstruction::LOAD:
            executeLoad(inst); break;
        case PcodeInstruction::STORE:
            executeStore(inst); break;
        case PcodeInstruction::CALL:
            executeCall(inst); break;
        case PcodeInstruction::RET:
            executeRet(); break;
        // case PcodeInstruction::JIT:
        //     executeJit(inst); break;
        // case PcodeInstruction::JUMP:
        //     executeJump(inst); break;
        case PcodeInstruction::READ:
            executeRead(); break;
        case PcodeInstruction::WRITE:
            executeWrite(); break;
    }
}

void PcodeRuntime::executeArg(const PcodeInstPtr &inst) {
    auto &ar = _ars.top();
    auto arg = std::dynamic_pointer_cast<PcodeArgumentInst>(inst);
    ar.pushTmp(ar.getNthArg(arg->getIndex()));
}

void PcodeRuntime::executeLi(const PcodeInstPtr &inst) {
    auto &ar = _ars.top();
    auto li = std::dynamic_pointer_cast<PcodeLoadImmediateInst>(inst);
    ar.pushTmp(li->getImm());
}

void PcodeRuntime::executeOpr(const PcodeInstPtr &inst) {
    auto &ar = _ars.top();
    auto opr = std::dynamic_pointer_cast<PcodeOperationInst>(inst);
    auto op = opr->getOp();
    // Only negative operation needs one operand
    if (op == PcodeOperationInst::NEG) {
        auto operand = ar.popTmp();
        ar.pushTmp(-operand);
    } else {
        auto second = ar.popTmp();
        auto first = ar.popTmp();
        switch (op) {
            case PcodeOperationInst::ADD:
                ar.pushTmp(first + second); break;
            case PcodeOperationInst::SUB:
                ar.pushTmp(first - second); break;
            case PcodeOperationInst::MUL:
                ar.pushTmp(first * second); break;
            case PcodeOperationInst::DIV:
                ar.pushTmp(first / second); break;
            case PcodeOperationInst::LES:
                ar.pushTmp(first < second); break;
            case PcodeOperationInst::LEQ:
                ar.pushTmp(first <= second); break;
            case PcodeOperationInst::GRE:
                ar.pushTmp(first > second); break;
            case PcodeOperationInst::GEQ:
                ar.pushTmp(first >= second); break;
            case PcodeOperationInst::EQL:
                ar.pushTmp(first == second); break;
            case PcodeOperationInst::NEQ:
                ar.pushTmp(first != second); break;
            case PcodeOperationInst::NEG: 
                break;
        }
    }
}

void PcodeRuntime::executeLoad(const PcodeInstPtr &inst) {
    auto &ar = _ars.top();
    auto &var = std::dynamic_pointer_cast<PcodeLoadInst>(inst)->getVar();
    ar.pushTmp(var->getValue());
}

void PcodeRuntime::executeStore(const PcodeInstPtr &inst) {
    auto &ar = _ars.top();
    auto &var = std::dynamic_pointer_cast<PcodeStoreInst>(inst)->getVar();
    var->setValue(ar.popTmp());
}

void PcodeRuntime::executeCall(const PcodeInstPtr &inst) {
    auto &prevAr = _ars.top();
    auto call = std::dynamic_pointer_cast<PcodeCallInst>(inst);
    
    // Parameter counter of the function 
    auto cnt = call->getFn()->getParamCounter();

    // Create a new AR for the comming function call
    _ars.push(ActivityRecord());
    auto &ar = _ars.top();

    for (int i = 0; i < cnt; i++) {
        // Add parameters from the previous AR
        auto num = prevAr.popTmp();
        ar.addArg(num);
    }
    for (auto &inst : _module.getLabel(call->getFn()->getName())->instructions) {
        excuteInst(inst);
    }
}

void PcodeRuntime::executeRet() {
    auto &funcAr = _ars.top();
    // Get return value
    float retVal = funcAr.popTmp();
    // Pop current AR
    _ars.pop();

    auto &ar = _ars.top();
    // Push the return value to the previous AR
    ar.pushTmp(retVal);
}

void PcodeRuntime::executeRead() {
    auto &ar = _ars.top();
    float val;
    scanf("%f", &val);
    ar.pushTmp(val);
}

void PcodeRuntime::executeWrite() {
    auto &ar = _ars.top();
    printf("%f\n", ar.popTmp());
}
