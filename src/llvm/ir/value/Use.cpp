#include "llvm/ir/value/Use.h"
#include "llvm/ir/LlvmContext.h"
#include "llvm/ir/value/User.h"

UsePtr Use::New(UserPtr user, ValuePtr value) {
    return user->Context()->SaveUse(new Use(user, value));
}