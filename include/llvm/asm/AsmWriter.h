#pragma once

#include <iostream>
#include <memory>
#include <string>

class AsmWriter;
using AsmWriterPtr = std::shared_ptr<AsmWriter>;

class AsmWriter {
public:
    static AsmWriterPtr New(std::ostream &out);

    const AsmWriter &Push(char ch) const;
    const AsmWriter &Push(const char *format, ...) const;
    const AsmWriter &Push(const std::string &str) const;

    const AsmWriter &PushNext(char ch) const;
    const AsmWriter &PushNext(const char *format, ...) const;
    const AsmWriter &PushNext(const std::string &str) const;

    const AsmWriter &PushSpace() const;
    const AsmWriter &PushSpaces(int repeat) const;
    const AsmWriter &PushNewLine() const;
    const AsmWriter &PushNewLines(int repeat) const;

    const AsmWriter &PushComment(const char *format, ...) const;
    const AsmWriter &CommentBegin() const;
    const AsmWriter &CommentEnd() const;

private:
    AsmWriter(std::ostream &out) : _out(out) {}

    std::ostream &_out;
};