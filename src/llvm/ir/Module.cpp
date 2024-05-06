#include "llvm/ir/Module.h"
#include "utils.h"
#include "llvm/ir/IrForward.h"
#include "llvm/ir/LlvmContext.h"
#include "llvm/ir/value/Function.h"
#include "llvm/ir/value/GlobalVariable.h"

ModulePtr Module::New(const std::string &name) {
    return std::shared_ptr<Module>(new Module(name));
}

Module::Module(const std::string &name) : _name(name), _context() {}

void Module::AddGlobalVariable(GlobalVariablePtr globalVariable) {
    _globalVariables.push_back(globalVariable);
}

void Module::AddFunction(FunctionPtr function) {
    _functions.push_back(function);
}

void Module::AddMainFunction(FunctionPtr function) {
    TOLANG_DIE_IF_NOT(_mainFunction, "Main function already exists.");
    _mainFunction = function;
}