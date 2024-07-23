#pragma once

#include <string>

class PcodeFunction;

using PcodeFuncPtr = std::shared_ptr<PcodeFunction>;

class PcodeFunction {
private:
    std::string _name;
    int _paramCounter;

public:
    PcodeFunction(const std::string &name, int paramCounter) :
        _name(name), _paramCounter(paramCounter) {}

    const std::string &getName() const { return _name; }

    int getParamCounter() const { return _paramCounter; }

    static PcodeFuncPtr create(const std::string &name, int paramCounter) {
        return std::make_shared<PcodeFunction>(name, paramCounter);
    }
};