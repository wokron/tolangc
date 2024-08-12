#pragma once

#include "llvm/ir/SlotTracker.h"
#include "llvm/ir/value/GlobalValue.h"
#include <list>

class Function final : public GlobalValue {
public:
    ~Function() override = default;

    static bool classof(const ValueType type) {
        return type == ValueType::FunctionTy;
    }

    void PrintAsm(AsmWriterPtr out) override;

    static FunctionPtr New(TypePtr returnType, const std::string &name);
    static FunctionPtr New(TypePtr returnType, const std::string &name,
                           std::vector<ArgumentPtr> args);

    BasicBlockPtr NewBasicBlock();

    TypePtr ReturnType() const;

public:
    using block_iterator = std::list<BasicBlockPtr>::iterator;
    using argument_iterator = std::vector<ArgumentPtr>::iterator;

    int ArgCount() const { return static_cast<int>(_args.size()); }

    ArgumentPtr GetArg(int argNo) const { return _args[argNo]; }
    argument_iterator ArgBegin() { return _args.begin(); }
    argument_iterator ArgEnd() { return _args.end(); }

    int BasicBlockCount() const {
        return static_cast<int>(_basicBlocks.size());
    }

    // Insert a basic block at the end of the function.
    FunctionPtr InsertBasicBlock(BasicBlockPtr block);
    // Insert a basic block before the specified iterator.
    FunctionPtr InsertBasicBlock(block_iterator iter, BasicBlockPtr block);
    // Remove a basic block from the function.
    FunctionPtr RemoveBasicBlock(BasicBlockPtr block);

    block_iterator BasicBlockBegin() { return _basicBlocks.begin(); }
    block_iterator BasicBlockEnd() { return _basicBlocks.end(); }

    SlotTrackerPtr GetSlotTracker() { return &_slotTracker; }

private:
    Function(TypePtr type, const std::string &name);
    Function(TypePtr type, const std::string &name,
             std::vector<ArgumentPtr> args);

private:
    // We can generate arguments via its type.
    std::vector<ArgumentPtr> _args;
    std::list<BasicBlockPtr> _basicBlocks;

    SlotTracker _slotTracker;
};