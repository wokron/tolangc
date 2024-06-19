#include "llvm/ir/value/inst/ExtendedInstructions.h"
#include "llvm/ir/LlvmContext.h"
#include "llvm/ir/value/Value.h"

InputInstPtr InputInst::New(LlvmContextPtr context) {
    return context->SaveValue(new InputInst(context->GetFloatTy()));
}

InputInst::InputInst(TypePtr type) : Instruction(ValueType::InputInstTy, type) {
    SetName("get");
}

OutputInstPtr OutputInst::New(ValuePtr value) {
    return value->Context()->SaveValue(new OutputInst(value));
}

OutputInst::OutputInst(ValuePtr value)
    : UnaryInstruction(ValueType::OutputInstTy, value->Context()->GetVoidTy(),
                       value) {
    SetName("put");
}