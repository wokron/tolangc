#include "llvm/ir/SlotTracker.h"

#include "llvm/utils.h"

#include "llvm/ir/Type.h"
#include "llvm/ir/value/Argument.h"
#include "llvm/ir/value/BasicBlock.h"
#include "llvm/ir/value/Function.h"
#include "llvm/ir/value/inst/Instruction.h"

void SlotTracker::Trace(FunctionPtr function) {
    int slot = 0;

    // Clear slot history.
    _slot.clear();

    // First, add all parameters.
    for (auto arg = function->ArgBegin(); arg != function->ArgEnd(); ++arg) {
        _slot.emplace(*arg, slot++);
    }

    // Then add all basic blocks, and all instructions in each basic block.
    for (auto blockIter = function->BasicBlockBegin();
         blockIter != function->BasicBlockEnd(); ++blockIter) {
        BasicBlockPtr block = *blockIter;
        _slot.emplace(block, slot++);

        for (auto instIter = block->InstructionBegin();
             instIter != block->InstructionEnd(); ++instIter) {
            InstructionPtr inst = *instIter;

            // We only track non-void instructions.
            if (!inst->GetType()->IsVoidTy()) {
                _slot.emplace(inst, slot++);
            }
        }
    }
}

int SlotTracker::Slot(ValuePtr value) {
    auto iter = _slot.find(value);
    if (iter != _slot.end()) {
        return iter->second;
    }

    TOLANG_DIE("Value not found in slot tracker.");

    return 0;
}