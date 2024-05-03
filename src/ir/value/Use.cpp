#include "ir/LlvmContext.h"
#include "ir/value/Use.h"
#include "ir/value/User.h"


UsePtr Use::New(UserPtr user, ValuePtr value)
{
    return user->Context()->SaveUse(new Use(user, value));
}
