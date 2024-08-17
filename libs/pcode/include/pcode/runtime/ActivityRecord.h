#pragma once

#include <vector>

class ActivityRecord {
public:
    std::vector<float> _args;
    std::vector<float> _tmps;

public:
    void addArg(float arg) { _args.emplace(_args.begin(), arg); }
    float getNthArg(int n) const { return _args[n - 1]; }

    void pushTmp(float tmp) { _tmps.push_back(tmp); } 
    float popTmp() { 
        float tmp = _tmps[_tmps.size() - 1];
        _tmps.pop_back();
        return tmp;
    } 
};
