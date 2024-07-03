#pragma once

#include "asm/AsmWriter.h"
#include "llvm/ir/IrForward.h"
#include "mips/mips_forward.h"
#include <iostream>

class AsmPrinter {
public:
    void Print(ModulePtr module, std::ostream &out);
    void Print(MipsData* data, std::ostream &out);
    void Print(MipsCode* data, std::ostream &out);

private:
    void _PrintHeader(AsmWriterPtr out);
    void _PrintFooter(AsmWriterPtr out);

    void _PrintModule(AsmWriterPtr out, ModulePtr module);
    void _PrintDeclaration(AsmWriterPtr out);
};