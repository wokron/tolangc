#include "ir/Type.h"
#include "ir/LlvmContext.h"
#include "utils.h"

#pragma region Type

TypePtr Type::GetVoidTy(LlvmContextPtr context)
{
    return context->GetVoidTy();
}

TypePtr Type::GetLabelTy(LlvmContextPtr context)
{
    return context->GetLabelTy();
}

#pragma endregion

#pragma region IntegerType

IntegerTypePtr IntegerType::Get(LlvmContextPtr context, unsigned bitWidth)
{
    switch (bitWidth)
    {
    case 8:
        return context->GetInt8Ty();
    case 32:
        return context->GetInt32Ty();
    default:
        TOLANG_DIE("Unsupported bit width %u", bitWidth);
    }

    return nullptr;
}

#pragma endregion

#pragma region FunctionType

FunctionTypePtr FunctionType::Get(LlvmContextPtr context, TypePtr returnType, const std::vector<TypePtr>& paramTypes)
{
    return context->GetFunctionType(returnType, paramTypes);
}

FunctionTypePtr FunctionType::Get(LlvmContextPtr context, TypePtr returnType)
{
    return context->GetFunctionType(returnType);
}

bool FunctionType::Equals(TypePtr returnType, const std::vector<TypePtr>& paramTypes) const
{
    if (_returnType != returnType)
    {
        return false;
    }
    if (_paramTypes.size() != paramTypes.size())
    {
        return false;
    }
    for (int i = 0; i < _paramTypes.size(); i++)
    {
        if (_paramTypes[i] != paramTypes[i])
        {
            return false;
        }
    }
    return true;
}

bool FunctionType::Equals(TypePtr returnType) const
{
    return _returnType == returnType && _paramTypes.empty();
}

FunctionType::FunctionType(TypePtr returnType, const std::vector<TypePtr>& paramTypes)
    : Type(FunctionTyID), _returnType(returnType), _paramTypes(paramTypes)
{
}

FunctionType::FunctionType(TypePtr returnType)
    : Type(FunctionTyID), _returnType(returnType)
{
}

#pragma endregion

#pragma region PointerType

PointerTypePtr PointerType::Get(LlvmContextPtr context, TypePtr elementType)
{
    return context->GetPointerType(elementType);
}

PointerType::PointerType(TypePtr elementType)
    : Type(PointerTyID), _elementType(elementType)
{
}

#pragma endregion
