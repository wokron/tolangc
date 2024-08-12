#pragma once

#include <algorithm>
#include <ostream>
#include <string>
#include <vector>

struct Error {
    int lineno;
    std::string msg;
};

/**
 * @brief `ErrorReporter` is a singleton class that collects error messages
 * during parsing and prints them out at the end.
 */
class ErrorReporter {
public:
    /**
     * @brief Get the singleton instance of `ErrorReporter`.
     */
    static ErrorReporter &get() {
        static ErrorReporter instance;
        return instance;
    }

    /**
     * @brief Report an error message.
     * @param lineno The line number where the error occurred.
     * @param msg The error message.
     */
    static void error(int lineno, const std::string msg) {
        get().report_error(lineno, msg);
    }

    /**
     * @brief Report an error message.
     * @param lineno The line number where the error occurred.
     * @param msg The error message.
     */
    void report_error(int lineno, const std::string msg) {
        _errors.push_back({lineno, msg});
    }

    /**
     * @brief Check if there are any error messages.
     * @return `true` if there are error messages, `false` otherwise.
     */
    bool has_error() const { return !_errors.empty(); }

    /**
     * @brief Dump all error messages to the given output stream.
     * @param out The output stream.
     * @note The error messages are sorted by line number before being printed.
     */
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
