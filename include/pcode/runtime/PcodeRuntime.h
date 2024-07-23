#pragma once

#include "pcode/runtime/ActivityRecord.h"
#include "pcode/PcodeModule.h"
#include "pcode/PcodeInstructions.h"
#include <cmath>
#include <iostream>
#include <cstdio>
#include <stack>
#include <map>
#include <string>

#define eps 1e-7

class PcodeRuntime {
private:
    std::stack<ActivityRecord> _ars;

    PcodeModule &_module;

    PcodeBlockPtr _currentBlock;

    void excuteInst(const PcodeInstPtr &inst);

    void executeArg(const PcodeInstPtr &inst);
    void executeLi(const PcodeInstPtr &inst);
    void executeOpr(const PcodeInstPtr &inst);
    void executeLoad(const PcodeInstPtr &inst);
    void executeStore(const PcodeInstPtr &inst);
    void executeCall(const PcodeInstPtr &inst);
    void executeRet();
    void executeRead();
    void executeWrite();

public:
    PcodeRuntime(PcodeModule &pm) : 
        _module(pm), _currentBlock(pm.getMainBlock()) {}

    void run();
};