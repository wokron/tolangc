#include "ast.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
// #include "visitor.h"
// #include "llvm/asm/AsmPrinter.h"
// #include "llvm/ir/Module.h"
#include "pcode/PcodeVisitor.h"
#include "pcode/runtime/PcodeRuntime.h"
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <vector>

struct Options {
    bool emit_ast = false;
    bool emit_ir = false;
    bool emit_asm = false;
    bool emit_pcode = false;
    std::string output;
};

void usage(const char *name) {
    std::cerr << "Usage: " << name << " [options] [file]" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -h, --help: Show this help message" << std::endl;
    std::cerr << "  --emit-ast: Emit AST as JSON" << std::endl;
    std::cerr << "  --emit-ir: Emit IR as JSON" << std::endl;
    std::cerr << "  --emit-pcode: Emit Pcode as JSON" << std::endl;
    std::cerr << "  -S, --emit-asm: Emit assembly" << std::endl;
    std::cerr << "  -o, --output: Output file" << std::endl;
}

void cmd_error(const char *name, const std::string &msg) {
    std::cerr << name << ": " << msg << std::endl;
    exit(1);
}

extern FILE *yyin;

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

    // ModulePtr module = Module::New(input);
    // auto visitor = Visitor(module);
    // visitor.visit(*root);

    if (ErrorReporter::get().has_error()) {
        ErrorReporter::get().dump(std::cerr);
        cmd_error(name, "compilation failed");
    }

    // if (options.emit_ir) {
    //     if (output.length() == 0) {
    //         output = "out.ll";
    //     }
    //     outfile.open(output, std::ios::out);

    //     AsmPrinter printer;
    //     printer.Print(module, outfile);
    //     return;
    // }

    // TODO: ir to asm
    // if (options.emit_asm) {
    //     cmd_error(name, "ir to asm not implemented");
    // }
    // Added: Generate pcode
    if (options.emit_pcode) {
        // auto
        PcodeVisitor pv;
        pv.visit(*root);
        
        if (output.length() == 0) {
            output = "out.pcode";
        }
        outfile.open(output, std::ios::out);
        pv.print(outfile);
        PcodeRuntime pr(pv);
        pr.run();

        return;
        // cmd_error(name, "pcode not implemented");
    }

    if (options.emit_asm) {
        if (output.length() == 0) {
            output = "out.s";
        }
        outfile.open(output, std::ios::out);
        // TODO: emit assembly
        cmd_error(name, "emit assembly not implemented");
    }

    cmd_error(name, "nothing to do");
}

int main(int argc, char *argv[]) {
    enum {
        HELP = 256,
        EMIT_IR,
        EMIT_AST,
        EMIT_ASM,
        EMIT_PCODE,
        OUTPUT,
    };
    const struct option long_options[] = {
        {"help", no_argument, 0, HELP},
        {"emit-ast", no_argument, 0, EMIT_AST},
        {"emit-ir", no_argument, 0, EMIT_IR},
        {"emit-asm", no_argument, 0, EMIT_ASM},
        {"emit-pcode", no_argument, 0, EMIT_PCODE},
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
        case EMIT_PCODE:
            options.emit_pcode = true;
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
