#pragma once

#include <iostream>
#include <memory>
#include <type_traits>

/**
 *                        Pcode Instructions
 * 
 * Instruction          Usage               Meaning
 * 
 * Define Variable      DEF   var           Define a `var`-named variable
 * Argument Index       ARG   index         Get the index-th argument of current AR
 * Load Immediate       LI    imm           Load an immediate to stack top
 * Operation            OPR   op            Do calculation by `op`
 * Load                 LOAD  var           Load the value of `var` to stack top
 * Store                STORE var           Store the stack top value to `var`
 * Function Call        CALL  fn            Call function `fn`
 * Return               RET                 Return from current AR
 * Jump If True         JIT   label         Jump to `label` if the stack top value is true
 * Jump                 JUMP  label         Jump to `label` without any conditions
 * Read Number          READ                Read a number from `stdin` and save to stack top
 * Write Number         WRITE               Write the stack top value to `stdout`
 * Label Mark           LABEL label         Mark a `label`-named label
 */

class PcodeInstruction {
public:
    typedef enum {
        DEF = 0,
        ARG,
        LI,
        OPR,
        LOAD,
        STORE,
        CALL,
        RET,
        JIT,
        JUMP,
        READ,
        WRITE,
        LABEL
    } PcodeInstructionType;

private:
    PcodeInstructionType _type;

public:
    PcodeInstruction(PcodeInstructionType type) : _type(type) {}

    PcodeInstructionType getType() const { return _type; }
    
    virtual void print(std::ostream &out) const = 0;

    template<typename T, typename... Args>
    static std::shared_ptr<PcodeInstruction> create(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
};

using PcodeInstPtr = std::shared_ptr<PcodeInstruction>;