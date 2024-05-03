#include "llvm/ir/LlvmContext.h"
#include "llvm/ir/value/inst/ExtendedInstructions.h"


InputInstPtr InputInst::New(LlvmContextPtr context)
{
    return context->SaveValue(new InputInst(context->GetInt32Ty()));
}


OutputInstPtr OutputInst::New(ValuePtr value)
{
    return value->Context()->SaveValue(new OutputInst(value));
}


OutputInst::OutputInst(ValuePtr value)
    : UnaryInstruction(ValueType::OutputInstTy, value->Context()->GetVoidTy(), value)
{
}
