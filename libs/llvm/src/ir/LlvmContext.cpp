#include "llvm/ir/LlvmContext.h"

#include "llvm/ir/Type.h"
#include "llvm/ir/value/Use.h"
#include "llvm/ir/value/Value.h"

LlvmContext::~LlvmContext() {
    for (auto type : _functionTypes) {
        delete type;
    }

    for (auto type : _pointerTypes) {
        delete type;
    }

    for (auto value : _values) {
        delete value;
    }

    for (auto use : _uses) {
        delete use;
    }
}

FunctionTypePtr
LlvmContext::GetFunctionType(TypePtr returnType,
                             const std::vector<TypePtr> &paramTypes) {
    for (auto type : _functionTypes) {
        if (type->Equals(returnType, paramTypes)) {
            return type;
        }
    }
    auto functionType = new FunctionType(returnType, paramTypes);
    _functionTypes.push_back(functionType);
    return functionType;
}

FunctionTypePtr LlvmContext::GetFunctionType(TypePtr returnType) {
    for (auto type : _functionTypes) {
        if (type->Equals(returnType)) {
            return type;
        }
    }
    auto functionType = new FunctionType(returnType);
    _functionTypes.push_back(functionType);
    return functionType;
}

PointerTypePtr LlvmContext::GetPointerType(TypePtr elementType) {
    for (auto type : _pointerTypes) {
        if (type->ElementType() == elementType) {
            return type;
        }
    }
    auto pointerType = new PointerType(elementType);
    _pointerTypes.push_back(pointerType);
    return pointerType;
}

// ValuePtr LlvmContext::Save(ValuePtr value)
//{
//    _values.push_back(value);
//    return value;
//}

// UsePtr LlvmContext::Save(UsePtr use)
//{
//    _uses.push_back(use);
//    return use;
//}