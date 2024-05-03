#include "llvm/ir/value/Use.h"
#include "llvm/ir/value/User.h"


void User::AddOperand(ValuePtr value)
{
    UsePtr use = Use::New(this, value);

    AddUse(use);
    value->AddUser(use);
}


ValuePtr User::OperandAt(int index)
{
    return _useList[index]->GetValue();
}


ValuePtr User::OperandAt(int index) const
{
    return _useList[index]->GetValue();
}


int User::OperandCount() const
{
    return _useList.size();
}
