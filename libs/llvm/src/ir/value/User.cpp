#include "llvm/ir/value/User.h"
#include "llvm/ir/value/Use.h"

void User::AddOperand(ValuePtr value) {
    AddUse(value);
    value->AddUser(this);
}

ValuePtr User::RemoveOperand(ValuePtr value) {
    ValuePtr old = RemoveUse(value);
    if (old) {
        old->RemoveUser(this);
    }
    return old;
}

ValuePtr User::ReplaceOperand(ValuePtr oldValue, ValuePtr newValue) {
    ValuePtr old = ReplaceUse(oldValue, newValue);
    if (old) {
        old->RemoveUser(this);
        newValue->AddUser(this);
    }
    return old;
}

ValuePtr User::OperandAt(int index) { return _useList[index]->GetValue(); }

ValuePtr User::OperandAt(int index) const {
    return _useList[index]->GetValue();
}

int User::OperandCount() const { return _useList.size(); }

void User::AddUse(ValuePtr use) { _useList.push_back(Use::New(this, use)); }

ValuePtr User::RemoveUse(ValuePtr use) {
    for (auto it = _useList.begin(); it != _useList.end(); ++it) {
        if ((*it)->GetValue() == use) {
            _useList.erase(it);
            return use;
        }
    }
    return nullptr;
}

ValuePtr User::ReplaceUse(ValuePtr oldValue, ValuePtr newValue) {
    for (auto it = _useList.begin(); it != _useList.end(); ++it) {
        if ((*it)->GetValue() == oldValue) {
            *it = Use::New(this, newValue);
            return oldValue;
        }
    }
    return nullptr;
}