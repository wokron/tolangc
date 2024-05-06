#include "llvm/asm/AsmPrinter.h"
#include "llvm/ir/Module.h"
#include "llvm/ir/value/Function.h"
#include "llvm/ir/value/GlobalVariable.h"

void AsmPrinter::Print(ModulePtr module, std::ostream &out) {
    AsmWriterPtr writer = AsmWriter::New(out);
    _PrintHeader(writer);
    _PrintModule(writer, std::move(module));
    _PrintFooter(writer);
}

/*
 * ; tolang LLVM IR
 * (blank line)
 */
void AsmPrinter::_PrintHeader(AsmWriterPtr out) {
    out->PushComment("tolang LLVM IR").PushNewLine();
}

/*
 * ; End of LLVM IR
 */
void AsmPrinter::_PrintFooter(AsmWriterPtr out) {
    out->PushNewLine().PushComment("End of LLVM IR");
}

/*
 * ; LLVM IR Module: {module-name}
 * (blank line)
 * {global variables}
 * (blank line)
 * {functions}
 */
void AsmPrinter::_PrintModule(AsmWriterPtr out, ModulePtr module) {
    // Module name.
    out->CommentBegin()
        .Push("Module ID = '%s'", module->Name().c_str())
        .CommentEnd();
    out->Push("source_filename = \"%s\"", module->Name().c_str())
        .PushNewLine()
        .PushNewLine();

    // Library function declaration.
    _PrintDeclaration(out);

    // Global variables.
    for (auto it = module->GlobalBegin(); it != module->GlobalEnd(); ++it) {
        (*it)->PrintAsm(out);
    }

    // Functions.
    for (auto it = module->FunctionBegin(); it != module->FunctionEnd(); ++it) {
        (*it)->PrintAsm(out);
    }

    // Main function.
    if (module->MainFunction()) {
        module->MainFunction()->PrintAsm(out);
    }
}

/*
 * This only prints the library function declaration... :(
 *
 * declare dso_local i32 @getint()
 * declare dso_local void @putstr(i8*)
 * declare dso_local void @putint(i32)
 */
void AsmPrinter::_PrintDeclaration(AsmWriterPtr out) {
    out->Push("declare i32 @get()")
        .PushNewLine()
        .Push("declare void @put(i32)")
        .PushNewLine()
        .PushNewLine();
}