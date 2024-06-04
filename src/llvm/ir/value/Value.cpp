#include "llvm/ir/value/Value.h"

#include "llvm/ir/value/Use.h"

void Value::AddUser(UserPtr user) { _userList.push_back(Use::New(user, this)); }

UserPtr Value::RemoveUser(UserPtr user) {
    for (auto it = _userList.begin(); it != _userList.end(); ++it) {
        if ((*it)->GetUser() == user) {
            _userList.erase(it);
            return user;
        }
    }

    return nullptr;
}