#pragma once

#include "ir/IrForward.h"
#include "ir/Type.h"
#include <vector>


/// <summary>
/// One LLVM context per module, which holds all the types and values.
/// </summary>
class LlvmContext
{
    // Only Module can create a context.
    friend class Module;

public:
    ~LlvmContext();

    LlvmContext(const LlvmContext&) = delete;
    LlvmContext& operator=(const LlvmContext&) = delete;

    TypePtr GetVoidTy() { return &_voidTy; }
    TypePtr GetLabelTy() { return &_labelTy; }
    IntegerTypePtr GetInt32Ty() { return &_int32Ty; }

    FunctionTypePtr GetFunctionType(TypePtr returnType, const std::vector<TypePtr>& paramTypes);
    FunctionTypePtr GetFunctionType(TypePtr returnType);

    PointerTypePtr GetPointerType(TypePtr elementType);

    // Save all allocated values to avoid memory leak.
    template<typename _Ty>
    _Ty* SaveValue(_Ty* value)
    {
        _values.push_back(value);
        return value->template As<_Ty>();
    }


    UsePtr SaveUse(UsePtr use)
    {
        _uses.push_back(use);
        return use;
    }

private:
    LlvmContext() : _voidTy(this, Type::VoidTyID), _labelTy(this, Type::LabelTyID), _int32Ty(this, 32)
    {
    }


    Type _voidTy;
    Type _labelTy;

    IntegerType _int32Ty;

    std::vector<FunctionTypePtr> _functionTypes;
    std::vector<PointerTypePtr> _pointerTypes;

    std::vector<ValuePtr> _values;
    std::vector<UsePtr> _uses;
};
