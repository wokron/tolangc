#pragma once

#include "llvm/ir/IrForward.h"
#include <unordered_map>

/*
 * SlotTracker is used to tag all values with ordered number in a function.
 * So, it is easy to get a standard looking LLVM IR.
 */
class SlotTracker final {
public:
    // Trace all values in a function, should be called before print.
    void Trace(FunctionPtr function);

    // Resolve, must be called after Trace.
    int Slot(ValuePtr value);

private:
    std::unordered_map<ValuePtr, int> _slot;
};

using SlotTrackerPtr = SlotTracker *;