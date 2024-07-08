#include "llvm/asm/AsmWriter.h"

#include <cstdarg>
#include <cstdio>

static char buffer[1024];

AsmWriterPtr AsmWriter::New(std::ostream &out) {
    return std::shared_ptr<AsmWriter>(new AsmWriter(out));
}

const AsmWriter &AsmWriter::Push(char ch) const {
    _out << ch;
    return *this;
}

const AsmWriter &AsmWriter::Push(const char *format, ...) const {
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    _out << buffer;
    return *this;
}

const AsmWriter &AsmWriter::Push(const std::string &str) const {
    _out << str;
    return *this;
}

const AsmWriter &AsmWriter::PushNext(char ch) const {
    return PushSpace().Push(ch);
}

const AsmWriter &AsmWriter::PushNext(const char *format, ...) const {
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    return PushSpace().Push(buffer);
}

const AsmWriter &AsmWriter::PushNext(const std::string &str) const {
    return PushSpace().Push(str);
}

const AsmWriter &AsmWriter::PushSpace() const { return Push(' '); }

const AsmWriter &AsmWriter::PushSpaces(int repeat) const {
    for (int i = 0; i < repeat; i++) {
        Push(' ');
    }
    return *this;
}

const AsmWriter &AsmWriter::PushNewLine() const { return Push('\n'); }

const AsmWriter &AsmWriter::PushNewLines(int repeat) const {
    for (int i = 0; i < repeat; i++) {
        Push('\n');
    }
    return *this;
}

const AsmWriter &AsmWriter::PushComment(const char *format, ...) const {
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    return CommentBegin().Push(buffer).CommentEnd();
}

const AsmWriter &AsmWriter::CommentBegin() const {
    return Push(';').PushSpace();
}

const AsmWriter &AsmWriter::CommentEnd() const { return PushNewLine(); }