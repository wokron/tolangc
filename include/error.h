#pragma once

#include <algorithm>
#include <ostream>
#include <string>
#include <vector>

struct Error {
    int lineno;
    std::string msg;
};

class ErrorReporter {
public:
    static ErrorReporter &get() {
        static ErrorReporter instance;
        return instance;
    }

    static void error(int lineno, const std::string msg) {
        get().report_error(lineno, msg);
    }

    void report_error(int lineno, const std::string msg) {
        _errors.push_back({lineno, msg});
    }

    bool has_error() const { return !_errors.empty(); }

    void dump(std::ostream &out) {
        std::sort(
            _errors.begin(), _errors.end(),
            [](const Error &a, const Error &b) { return a.lineno < b.lineno; });

        for (const auto &err : _errors) {
            out << err.lineno << ": " << err.msg << std::endl;
        }
    }

private:
    ErrorReporter() = default;
    ErrorReporter(const ErrorReporter &) = delete;
    ErrorReporter &operator=(const ErrorReporter &) = delete;

    std::vector<Error> _errors;
};
