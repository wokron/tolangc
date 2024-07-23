#pragma once

#include "pcode/PcodeInstruction.h"
#include <string>
#include <map>
#include <vector>

struct PcodeBlock;
using PcodeBlockPtr = std::shared_ptr<PcodeBlock>; 

struct PcodeBlock {
    std::vector<PcodeInstPtr> instructions;
    PcodeBlockPtr next = nullptr;

    void insertInst(PcodeInstPtr &inst) {
        instructions.push_back(inst);
    }

    static PcodeBlockPtr create() {
        return std::make_shared<PcodeBlock>();
    }
};