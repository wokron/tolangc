#pragma once

#include "llvm/ir/IrForward.h"
#include "llvm/ir/Type.h"
#include <vector>

/// <summary>
/// One LLVM context per module, which holds all the types and values.
/// </summary>
class LlvmContext {
    // Only Module can create a context.
    friend class Module;

public:
    ~LlvmContext();

    LlvmContext(const LlvmContext &) = delete;
    LlvmContext &operator=(const LlvmContext &) = delete;

    TypePtr GetVoidTy() { return &_voidTy; }
    TypePtr GetLabelTy() { return &_labelTy; }
    IntegerTypePtr GetInt1Ty() { return &_int1Ty; }
    IntegerTypePtr GetInt32Ty() { return &_int32Ty; }
    FloatTypePtr GetFloatTy() { return &_floatTy; }

    FunctionTypePtr GetFunctionType(TypePtr returnType,
                                    const std::vector<TypePtr> &paramTypes);
    FunctionTypePtr GetFunctionType(TypePtr returnType);

    PointerTypePtr GetPointerType(TypePtr elementType);

    // Save all allocated values to avoid memory leak.
    template <typename _Ty> _Ty *SaveValue(_Ty *value) {
        _values.push_back(value);
        return value->template As<_Ty>();
    }

    UsePtr SaveUse(UsePtr use) {
        _uses.push_back(use);
        return use;
    }

private:
    LlvmContext()
        : _voidTy(this, Type::VoidTyID), _labelTy(this, Type::LabelTyID),
          _int1Ty(this, 1), _int32Ty(this, 32), _floatTy(this, 32) {}

    Type _voidTy;
    Type _labelTy;

    IntegerType _int1Ty;
    IntegerType _int32Ty;
    FloatType _floatTy;

    std::vector<FunctionTypePtr> _functionTypes;
    std::vector<PointerTypePtr> _pointerTypes;

    std::vector<ValuePtr> _values;
    std::vector<UsePtr> _uses;
};