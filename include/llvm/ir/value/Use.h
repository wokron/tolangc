#pragma once

#include "llvm/ir/IrForward.h"

class Use {
public:
    // Prevent copying.
    Use(const Use &) = delete;
    Use &operator=(const Use &) = delete;

    static UsePtr New(UserPtr user, ValuePtr value);

    ValuePtr GetValue() const { return _value; }
    UserPtr GetUser() const { return _user; }

private:
    Use(UserPtr user, ValuePtr value) : _user(user), _value(value) {}

    UserPtr _user;
    ValuePtr _value;
};