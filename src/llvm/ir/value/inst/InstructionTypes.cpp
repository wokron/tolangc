#include "llvm/ir/value/inst/InstructionTypes.h"
#include "utils.h"
#include "llvm/ir/LlvmContext.h"
#include "llvm/ir/Type.h"

UnaryInstruction::UnaryInstruction(ValueType valueType, TypePtr type,
                                   ValuePtr operand)
    : Instruction(valueType, type) {
    AddOperand(operand);
}

UnaryOperatorPtr UnaryOperator::New(UnaryOpType opType, ValuePtr operand) {
    auto type = operand->GetType();
    return operand->Context()->SaveValue(
        new UnaryOperator(type, operand, opType));
}

UnaryOperator::UnaryOperator(TypePtr type, ValuePtr operand, UnaryOpType opType)
    : UnaryInstruction(ValueType::UnaryOperatorTy, type, operand),
      _opType(opType) {}

BinaryInstruction::BinaryInstruction(ValueType valueType, TypePtr type,
                                     ValuePtr lhs, ValuePtr rhs)
    : Instruction(valueType, type) {
    AddOperand(lhs);
    AddOperand(rhs);
}

BinaryOperatorPtr BinaryOperator::New(BinaryOpType opType, ValuePtr lhs,
                                      ValuePtr rhs) {
    TOLANG_DIE_IF_NOT(lhs->GetType()->IsIntegerTy() &&
                          rhs->GetType()->IsIntegerTy(),
                      "BinaryOperator operands must be of integer type");
    auto type = lhs->GetType();

    return lhs->Context()->SaveValue(
        new BinaryOperator(type, lhs, rhs, opType));
}

CompareInstructionPtr CompareInstruction::New(CompareOpType opType,
                                              ValuePtr lhs, ValuePtr rhs) {
    TOLANG_DIE_IF_NOT(lhs->GetType()->IsIntegerTy() &&
                          rhs->GetType()->IsIntegerTy(),
                      "CompareInstruction operands must be of integer type");
    auto type = lhs->Context()->GetInt32Ty();
    return lhs->Context()->SaveValue(
        new CompareInstruction(type, lhs, rhs, opType));
}