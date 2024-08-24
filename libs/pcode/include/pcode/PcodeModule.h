#pragma once

#include "pcode/PcodeBlock.h"
#include "pcode/PcodeVariable.h"
#include "pcode/PcodeFunction.h"
#include <map>

class PcodeModule {
private:
    /**
     * Here `_variables`, `_functions` and `_labels` are 
     * something like `Value` in llvm based on specific
     * `_blocks`. So combine them together in `PcodeModule`.
     */
    std::vector<PcodeBlockPtr> _blocks;

    std::map<std::string, PcodeVarPtr> _variables;
    std::map<std::string, PcodeFuncPtr> _functions;
    std::map<std::string, PcodeBlockPtr> _labels;

public:
    void addBlock(const PcodeBlockPtr &block) { _blocks.push_back(block); }

    void addVariable(const std::string &content, const PcodeVarPtr &var) {
        _variables.emplace(content, var);
    }

    void addFunction(const std::string &funcName, const PcodeFuncPtr &func) {
        _functions.emplace(funcName, func);
    }

    void addLabel(const std::string &content, const PcodeBlockPtr &block) {
        _labels.emplace(content, block);
    }

    PcodeVarPtr &getVariable(const std::string &content) {
        return _variables.at(content);
    }

    PcodeFuncPtr &getFunction(const std::string &funcName) {
        return _functions.at(funcName);
    }

    PcodeBlockPtr &getLabel(const std::string &content) {
        return _labels.at(content);
    }

    PcodeBlockPtr &getMainBlock() { return _blocks[_functions.size()]; }

    void print(std::ostream &out) const {
        for (auto &b : _blocks) {
            for (auto &i : b->instructions) {
                i->print(out);
            }
            out << "\n";
        }
    }
};

using Module = PcodeModule;
