// Forward declarations for the IR classes

#pragma once

#include <memory>
#include <vector>

class Module;
using ModulePtr = std::shared_ptr<Module>;

class LlvmContext;
using LlvmContextPtr = LlvmContext *;

class Type;
using TypePtr = Type *;

class IntegerType;
using IntegerTypePtr = IntegerType *;

class FloatType;
using FloatTypePtr = FloatType *;

class FunctionType;
using FunctionTypePtr = FunctionType *;

class PointerType;
using PointerTypePtr = PointerType *;

///////////////////////////////////////////////////////////
// Value Forward Declaration
class Value;
using ValuePtr = Value *;
using ValueSmartPtr = std::shared_ptr<Value>;

class Argument;
using ArgumentPtr = Argument *;

class Function;
using FunctionPtr = Function *;

class BasicBlock;
using BasicBlockPtr = BasicBlock *;

class User;
using UserPtr = User *;

///////////////////////////////////////////////////////////
// Constant Forward Declaration
class Constant;
using ConstantPtr = Constant *;

class ConstantData;
using ConstantDataPtr = ConstantData *;

class GlobalValue;
using GlobalValuePtr = GlobalValue *;

class Function;
using FunctionPtr = Function *;

///////////////////////////////////////////////////////////
// Instruction Forward Declaration
class Instruction;
using InstructionPtr = Instruction *;

class UnaryInstruction;
using UnaryInstructionPtr = UnaryInstruction *;

class UnaryOperator;
using UnaryOperatorPtr = UnaryOperator *;

class BinaryOperator;
using BinaryOperatorPtr = BinaryOperator *;

class CompareInstruction;
using CompareInstructionPtr = CompareInstruction *;

class AllocaInst;
using AllocaInstPtr = AllocaInst *;

class LoadInst;
using LoadInstPtr = LoadInst *;

class StoreInst;
using StoreInstPtr = StoreInst *;

class CallInst;
using CallInstPtr = CallInst *;

class BranchInst;
using BranchInstPtr = BranchInst *;

class JumpInst;
using JumpInstPtr = JumpInst *;

class ReturnInst;
using ReturnInstPtr = ReturnInst *;

class InputInst;
using InputInstPtr = InputInst *;

class OutputInst;
using OutputInstPtr = OutputInst *;

///////////////////////////////////////////////////////////
// Use Forward Declaration
class Use;
using UsePtr = Use *;
using UseList = std::vector<UsePtr>;
using UseListPtr = UseList *;

template <typename _Ty> class HasParent {
public:
    using _TyPtr = _Ty *;

    virtual ~HasParent() = default;

    void SetParent(_TyPtr parent) { _parent = parent; }
    void RemoveParent() { _parent = nullptr; }
    _TyPtr Parent() const { return _parent; }

protected:
    HasParent(_TyPtr parent = nullptr) : _parent(parent) {}

private:
    _TyPtr _parent;
};