#include "llvm/ir/LlvmContext.h"
#include "llvm/ir/Type.h"
#include "llvm/ir/value/Function.h"
#include "llvm/ir/value/inst/Instructions.h"

#pragma region AllocaInst


AllocaInstPtr AllocaInst::New(TypePtr type)
{
    return type->Context()->SaveValue(new AllocaInst(type));
}


AllocaInst::AllocaInst(TypePtr type)
    : Instruction(ValueType::AllocaInstTy, type->Context()->GetPointerType(type))
{
}


TypePtr AllocaInst::AllocatedType() const
{
    return GetType()->As<PointerType>()->ElementType();
}

#pragma endregion

#pragma region LoadInst

// %4 = load i32, i32* %2, align 4
LoadInstPtr LoadInst::New(ValuePtr address)
{
    TOLANG_DIE_IF_NOT(address->GetType()->IsPointerTy(), "Address must be a pointer!");
    auto type = address->GetType()->As<PointerType>()->ElementType();
    return address->Context()->SaveValue(new LoadInst(type, address));
}


ValuePtr LoadInst::Address() const
{
    return OperandAt(0);
}

#pragma endregion

#pragma region StoreInst

// store i32 0, i32* %1, align 4
// store i32 %4, i32* %3, align 4
StoreInstPtr StoreInst::New(ValuePtr value, ValuePtr address)
{
    TOLANG_DIE_IF_NOT(address->GetType()->IsPointerTy(), "Address must be a pointer!");
    return address->Context()->SaveValue(new StoreInst(value, address));
}


StoreInst::StoreInst(ValuePtr value, ValuePtr address)
    : BinaryInstruction(ValueType::StoreInstTy, value->Context()->GetVoidTy(), value, address)
{
}

#pragma endregion

#pragma region ReturnInst

// ret i32 0
// ret

ReturnInstPtr ReturnInst::New(ValuePtr value)
{
    return value->Context()->SaveValue(new ReturnInst(value->Context()->GetVoidTy(), value));
}


ReturnInstPtr ReturnInst::New(LlvmContextPtr context)
{
    return context->SaveValue(new ReturnInst(context->GetVoidTy()));

}


ReturnInst::ReturnInst(TypePtr type, ValuePtr value)
    : Instruction(ValueType::ReturnInstTy, type)
{
    if (!value->GetType()->IsVoidTy())
    {
        AddOperand(value);
    }
}


ReturnInst::ReturnInst(TypePtr type) : Instruction(ValueType::ReturnInstTy, type)
{
}


ValuePtr ReturnInst::ReturnValue() const
{
    if (OperandCount() == 0)
    {
        return nullptr;
    }
    return OperandAt(0);
}

#pragma endregion

#pragma region CallInst

// %6 = call i32 @add(i32 %4, i32 %5)
// The parameters are the operands of the call instruction.
CallInstPtr CallInst::New(FunctionPtr function, const std::vector<ValuePtr>& params)
{
    return function->Context()->SaveValue(new CallInst(function, params));
}


CallInstPtr CallInst::New(FunctionPtr function)
{
    return function->Context()->SaveValue(new CallInst(function));
}


CallInst::CallInst(FunctionPtr function, const std::vector<ValuePtr>& parameters)
    : Instruction(ValueType::CallInstTy, function->ReturnType()), _function(function)

{
    for (auto param : parameters)
    {
        AddOperand(param);
    }
}


CallInst::CallInst(FunctionPtr function)
    : Instruction(ValueType::CallInstTy, function->ReturnType()), _function(function)
{
}

#pragma endregion
