#include "tolang/ast.h"
#include "tolang/error.h"
#include "tolang/lexer.h"
#include "tolang/parser.h"
#include "tolang/visitor.h"
#include "tolang/utils.h"

#include <fstream>
#include <getopt.h>
#include <iostream>
#include <vector>

#if TOLANG_BACKEND == LLVM
#include "mips/translator.h"
#include "llvm/asm/AsmPrinter.h"
#include "llvm/ir/Module.h"
#elif TOLANG_BACKEND == PCODE
#include "pcode/PcodeModule.h"
#include "pcode/runtime/PcodeRuntime.h"
#else
#error "unknown backend"
#endif

struct Options {
    bool emit_ast = false;
    bool emit_ir = false;
    bool emit_asm = false;
    std::string output;
};

void usage(const char *name) {
    std::cerr << "Usage: " << name << " [options] [file]" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -h, --help: Show this help message" << std::endl;
    std::cerr << "  --emit-ast: Emit AST" << std::endl;
    std::cerr << "  --emit-ir: Emit IR" << std::endl;
    std::cerr << "  -S, --emit-asm: Emit assembly" << std::endl;
    std::cerr << "  -o, --output: Output file" << std::endl;
}

void cmd_error(const char *name, const std::string &msg) {
    std::cerr << name << ": " << msg << std::endl;
    exit(1);
}

extern FILE *yyin;

#if TOLANG_BACKEND == LLVM

void compile(const char *name, const Options &options, const std::string &input,
             std::unique_ptr<CompUnit> root) {
    std::ofstream outfile;
    auto output = options.output;

    ModulePtr module = Module::New(input);
    auto visitor = Visitor(module);
    visitor.visit(*root);

    if (ErrorReporter::get().has_error()) {
        ErrorReporter::get().dump(std::cerr);
        cmd_error(name, "compilation failed");
    }

    if (options.emit_ir) {
        if (output.length() == 0) {
            output = "out.ll";
        }
        outfile.open(output, std::ios::out);

        AsmPrinter printer;
        printer.Print(module, outfile);
        return;
    }

    if (options.emit_asm) {
        if (output.length() == 0) {
            output = "out.s";
        }
        outfile.open(output, std::ios::out);

        Translator translator;
        translator.translate(module);
        translator.print(outfile);
        return;
    }

    cmd_error(name, "nothing to do");
}

#elif TOLANG_BACKEND == PCODE

void compile(const char *name, const Options &options, const std::string &input,
             std::unique_ptr<CompUnit> root) {
    std::ofstream outfile;
    auto output = options.output;
    
    Module module;
    auto visitor = Visitor(module);
    visitor.visit(*root);

    if (ErrorReporter::get().has_error()) {
        ErrorReporter::get().dump(std::cerr);
        cmd_error(name, "compilation failed");
    }

    if (options.emit_ir || options.emit_asm) {
        if (output.length() == 0) {
            output = "out.pcode";
        }
        outfile.open(output, std::ios::out);

        module.print(outfile);
        return;
    }

    PcodeRuntime runtime(module);
    runtime.run();
}

#else

#error "unknown backend"

#endif

void compile(const char *name, const Options &options,
             const std::string &input) {
    std::ofstream outfile;
    auto output = options.output;

    std::ifstream infile(input, std::ios::in);

    Lexer lexer(infile);
    Parser parser(lexer);
    auto root = parser.parse();

    if (options.emit_ast) {
        if (output.length() == 0) {
            output = "out.ast";
        }
        outfile.open(output, std::ios::out);
        root->print(outfile);
        return;
    }

    compile(name, options, input, std::move(root));
}

int main(int argc, char *argv[]) {
    enum {
        HELP = 256,
        EMIT_IR,
        EMIT_AST,
        EMIT_ASM,
        OUTPUT,
    };
    const struct option long_options[] = {
        {"help", no_argument, 0, HELP},
        {"emit-ast", no_argument, 0, EMIT_AST},
        {"emit-ir", no_argument, 0, EMIT_IR},
        {"emit-asm", no_argument, 0, EMIT_ASM},
        {"output", required_argument, 0, OUTPUT},
        {0, 0, 0, 0}};

    Options options;
    int opt;

    while ((opt = getopt_long_only(argc, argv, "hSo:", long_options, NULL)) !=
           -1) {
        switch (opt) {
        case 'h':
        case HELP:
            usage(argv[0]);
            return 0;
        case EMIT_AST:
            options.emit_ast = true;
            break;
        case EMIT_IR:
            options.emit_ir = true;
            break;
        case 'S':
        case EMIT_ASM:
            options.emit_asm = true;
            break;
        case 'o':
        case OUTPUT:
            options.output = optarg;
            break;
        case '?':
            cmd_error(argv[0], "unknown option");
            return 1;
        }
    }

    if (argv[optind] == nullptr) {
        cmd_error(argv[0], "no input file");
        return 1;
    }

    compile(argv[0], options, argv[optind]);

    return 0;
}
