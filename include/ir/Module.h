#pragma once

#include "ir/IrForward.h"
#include <string>

class Module final
{
public:
    ~Module();

    static ModulePtr New(const char* name);

    const std::string Name() const { return _name; }
    LlvmContextPtr Context() const { return _context; }

public:
    using global_iterator = std::vector<GlobalVariablePtr>::iterator;
    using function_iterator = std::vector<FunctionPtr>::iterator;

    global_iterator GlobalBegin() { return _globalVariables.begin(); }
    global_iterator GlobalEnd() { return _globalVariables.end(); }
    int GlobalCount() const { return _globalVariables.size(); }

    function_iterator FunctionBegin() { return _functions.begin(); }
    function_iterator FunctionEnd() { return _functions.end(); }
    int FunctionCount() const { return _functions.size(); }

    FunctionPtr MainFunction() const { return _mainFunction; }

public:
    void AddGlobalVariable(GlobalVariablePtr globalVariable);
    void AddFunction(FunctionPtr function);
    void AddMainFunction(FunctionPtr function);

private:
    Module(const std::string name);

    std::string _name;
    LlvmContextPtr _context;

    // These will be managed by LlvmContext. So we don't need to delete them.
    std::vector<GlobalVariablePtr> _globalVariables;
    std::vector<FunctionPtr> _functions;
    FunctionPtr _mainFunction;
};
