#include "ir/Type.h"
#include "ir/value/GlobalVariable.h"
#include "ir/LlvmContext.h"


GlobalVariablePtr GlobalVariable::New(TypePtr type, const std::string& name)
{
    return type->Context()->SaveValue(new GlobalVariable(type, name));
}
