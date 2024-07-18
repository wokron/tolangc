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
                    auto jit = std::dynamic_pointer_cast<JumpIfTrueInst>(inst);
                    _currentBlock = _labels.at(jit->getLabel());
                    flag = true;
                    break;
                } else {
                    continue;
                }
            } else if (inst->getType() == PcodeInstruction::JUMP) {
                auto jump = std::dynamic_pointer_cast<JumpInst>(inst);
                _currentBlock = _labels.at(jump->getLabel());
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
        case PcodeInstruction::DEF: // No need to handle this
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
    auto arg = std::dynamic_pointer_cast<ArgumentInst>(inst);
    ar.pushTmp(ar.getNthArg(arg->getIndex()));
}

void PcodeRuntime::executeLi(const PcodeInstPtr &inst) {
    auto &ar = _ars.top();
    auto li = std::dynamic_pointer_cast<LoadImmediateInst>(inst);
    ar.pushTmp(li->getImm());
}

void PcodeRuntime::executeOpr(const PcodeInstPtr &inst) {
    auto &ar = _ars.top();
    auto opr = std::dynamic_pointer_cast<OperationInst>(inst);
    auto op = opr->getOp();
    // Only negative operation needs one operand
    if (op == OperationInst::NEG) {
        auto operand = ar.popTmp();
        ar.pushTmp(-operand);
    } else {
        auto second = ar.popTmp();
        auto first = ar.popTmp();
        switch (op) {
            case OperationInst::ADD:
                ar.pushTmp(first + second); break;
            case OperationInst::SUB:
                ar.pushTmp(first - second); break;
            case OperationInst::MUL:
                ar.pushTmp(first * second); break;
            case OperationInst::DIV:
                ar.pushTmp(first / second); break;
            // case OperationInst::MOD:
            //     ar.pushTmp((int)first % (int)second); 
            //     break;
            case OperationInst::LES:
                ar.pushTmp(first < second); break;
            case OperationInst::LEQ:
                ar.pushTmp(first <= second); break;
            case OperationInst::GRE:
                ar.pushTmp(first > second); break;
            case OperationInst::GEQ:
                ar.pushTmp(first >= second); break;
            case OperationInst::EQL:
                ar.pushTmp(first == second); break;
            case OperationInst::NEQ:
                ar.pushTmp(first != second); break;
        }
    }
}

void PcodeRuntime::executeLoad(const PcodeInstPtr &inst) {
    auto &ar = _ars.top();
    auto &var = std::dynamic_pointer_cast<LoadInst>(inst)->getVar();
    ar.pushTmp(var->getValue());
}

void PcodeRuntime::executeStore(const PcodeInstPtr &inst) {
    auto &ar = _ars.top();
    auto &var = std::dynamic_pointer_cast<StoreInst>(inst)->getVar();
    var->setValue(ar.popTmp());
}

void PcodeRuntime::executeCall(const PcodeInstPtr &inst) {
    auto &prevAr = _ars.top();
    auto call = std::dynamic_pointer_cast<CallInst>(inst);
    
    // Parameter counter of the function 
    auto cnt = call->getFn()->getParamCounter();

    // Create a new AR for the comming function call
    _ars.push(ActivityRecord());
    auto &ar = _ars.top();

    for (int i = 0; i < cnt; i++) {
        // Add parameters from the previous AR
        ar.addArg(prevAr.popTmp());
    }
    for (auto &inst : _labels.at(call->getFn()->getName())->instructions) {
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
    std::cin >> val;
    ar.pushTmp(val);
}

void PcodeRuntime::executeWrite() {
    auto &ar = _ars.top();
    std::cout << ar.popTmp() << std::endl;
}
