#pragma once

#include <string>

class PcodeVariable;

using PcodeVarPtr = std::shared_ptr<PcodeVariable>; 

class PcodeVariable {
private:
    std::string _name;

    float _value;

public:
    PcodeVariable(const std::string &name) : _name(name) {}

    void setValue(float value) { _value = value; }

    float getValue() const { return _value; }

    const std::string &getName() const { return _name; }

    static PcodeVarPtr create(const std::string &name) {
        return std::make_shared<PcodeVariable>(name);
    }
};