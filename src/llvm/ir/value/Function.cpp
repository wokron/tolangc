#include "llvm/ir/value/Function.h"
#include "llvm/ir/LlvmContext.h"
#include "llvm/ir/Type.h"
#include "llvm/ir/value/Argument.h"
#include "llvm/ir/value/BasicBlock.h"

FunctionPtr Function::New(TypePtr returnType, const std::string &name) {
    return returnType->Context()->SaveValue(
        new Function(FunctionType::Get(returnType), name));
}

FunctionPtr Function::New(TypePtr returnType, const std::string &name,
                          std::vector<ArgumentPtr> args) {
    std::vector<TypePtr> argTypes;
    for (auto arg : args) {
        argTypes.push_back(arg->GetType());
    }
    return returnType->Context()->SaveValue(
        new Function(FunctionType::Get(returnType, argTypes), name, args));
}

Function::Function(TypePtr type, const std::string &name)
    : GlobalValue(ValueType::FunctionTy, type, name) {}

Function::Function(TypePtr type, const std::string &name,
                   std::vector<ArgumentPtr> args)
    : GlobalValue(ValueType::FunctionTy, type, name), _args(args) {
    for (auto arg : args) {
        arg->SetParent(this);
    }
}

BasicBlockPtr Function::NewBasicBlock() {
    auto block = BasicBlock::New(this);
    InsertBasicBlock(block);
    return block;
}

TypePtr Function::ReturnType() const {
    return GetType()->As<FunctionType>()->ReturnType();
}

FunctionPtr Function::InsertBasicBlock(BasicBlockPtr block) {
    _basicBlocks.push_back(block);
    return this;
}

FunctionPtr Function::InsertBasicBlock(block_iterator iter,
                                       BasicBlockPtr block) {
    _basicBlocks.insert(iter, block);
    return this;
}

FunctionPtr Function::RemoveBasicBlock(BasicBlockPtr block) {
    _basicBlocks.remove(block);
    return this;
}