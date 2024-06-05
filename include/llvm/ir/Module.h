#pragma once

#include "llvm/ir/IrForward.h"
#include "llvm/ir/LlvmContext.h"
#include <string>

class Module final {
public:
    static ModulePtr New(const std::string &name);

    const std::string Name() const { return _name; }
    LlvmContextPtr Context() { return &_context; }

public:
    using function_iterator = std::vector<FunctionPtr>::iterator;

    function_iterator FunctionBegin() { return _functions.begin(); }
    function_iterator FunctionEnd() { return _functions.end(); }
    int FunctionCount() const { return static_cast<int>(_functions.size()); }

    FunctionPtr MainFunction() const { return _mainFunction; }

public:
    void AddFunction(FunctionPtr function);
    void AddMainFunction(FunctionPtr function);

private:
    Module(const std::string &name);

    std::string _name;
    LlvmContext _context;

    // These will be managed by LlvmContext. So we don't need to delete them.
    std::vector<FunctionPtr> _functions;
    FunctionPtr _mainFunction = nullptr;
};