#include "llvm/utils.h"
#include "llvm/ir/Type.h"
#include <string>

void Type::PrintAsm(AsmWriterPtr out) {
    switch (_typeId) {
    case VoidTyID:
        out->Push("void");
        break;
    case LabelTyID:
        out->Push("label");
        break;
    default:
        TOLANG_DIE("Unimplemented type");
    }
}

// i32
void IntegerType::PrintAsm(AsmWriterPtr out) {
    out->Push('i').Push(std::to_string(_bitWidth));
}

// float
void FloatType::PrintAsm(AsmWriterPtr out) {
    if (_bitWidth == 32) {
        out->Push("float");
    } else if (_bitWidth == 64) {
        out->Push("double");
    } else {
        TOLANG_DIE("Unsupport float bit width: %u", _bitWidth);
    }
}

// return type ( arg1, arg2, ... )
void FunctionType::PrintAsm(AsmWriterPtr out) {
    ReturnType()->PrintAsm(out);
    out->PushNext('(');
    bool first = true;
    for (auto type : _paramTypes) {
        if (!first) {
            out->Push(", ");
        } else {
            first = false;
        }
        type->PrintAsm(out);
    }
    out->Push(')');
}

// type*
void PointerType::PrintAsm(AsmWriterPtr out) {
    _elementType->PrintAsm(out);
    out->Push('*');
}