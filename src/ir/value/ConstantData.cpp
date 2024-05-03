#include "ir/value/ConstantData.h"
#include "ir/LlvmContext.h"
#include "ir/Type.h"
#include "utils.h"


ConstantDataPtr ConstantData::New(TypePtr type, int value)
{
    TOLANG_DIE_IF_NOT(type->IsIntegerTy(), "ConstantData must be of integer type");
    return type->Context()->SaveValue(new ConstantData(type, value));
}
