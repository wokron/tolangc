#pragma once

#include "ActivityRecord.h"
#include "../PcodeVisitor.h"
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

    std::vector<PcodeBlockPtr> &_blocks;
    std::map<std::string, PcodeVarPtr> &_variables;
    std::map<std::string, PcodeFuncPtr> &_functions;
    std::map<std::string, PcodeBlockPtr> &_labels;

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
    PcodeRuntime(PcodeVisitor &pv) :
        _blocks(pv._blocks), _variables(pv._variables), 
        _functions(pv._functions), _labels(pv._labels), 
        _currentBlock(_blocks[_functions.size()]) {}

    void run();
};