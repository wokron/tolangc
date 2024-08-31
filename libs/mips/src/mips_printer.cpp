#include "mips/mips_inst.h"
#include "mips/mips_manager.h"
#include "mips/mips_reg.h"
#include <iostream>

void MipsManager::PrintMips(std::ostream &_out) {
    _out << ".data" << std::endl;
    for (auto data : datas) {
        data->PrintData(_out);
    }

    _out << ".text" << std::endl << "j main" << std::endl << "nop" << std::endl;
    for (auto code : codes) {
        code->PrintCode(_out);
    }
}

void ZeroReg::PrintReg(std::ostream &out) { out << "$zero"; }

void ArgumentReg::PrintReg(std::ostream &out) { out << "$a" << GetIndex(); }

void ValueReg::PrintReg(std::ostream &out) { out << "$v" << GetIndex(); }

void TmpReg::PrintReg(std::ostream &out) { out << "$t" << GetIndex(); }

void FloatReg::PrintReg(std::ostream &out) { out << "$f" << GetIndex(); }

void RetAddrReg::PrintReg(std::ostream &out) { out << "$ra"; }

void StkPtrReg::PrintReg(std::ostream &out) { out << "$sp"; }

void FrmPtrReg::PrintReg(std::ostream &out) { out << "$fp"; }

void OffsetReg::PrintReg(std::ostream &out) { out << GetIndex(); }

void WordData::printValue(std::ostream &out) { out << w_value; }

void WordData::PrintData(std::ostream &out) {
    printName(out);
    out << ": .word ";
    printValue(out);
    out << std::endl;
}

void FloatData::printValue(std::ostream &out) { out << f_value; }

void FloatData::PrintData(std::ostream &out) {
    printName(out);
    out << ": .float ";
    printValue(out);
    out << std::endl;
}

void AsciizData::printValue(std::ostream &out) { out << "\"" << str << "\""; }

void AsciizData::PrintData(std::ostream &out) {
    printName(out);
    out << ": .asciiz ";
    printValue(out);
    out << std::endl;
}

void RCode::PrintCode(std::ostream &out) {
    std::string Ope;
    if (op >= Div && op <= Sne) {
        Ope = op == Div    ? "div"
              : op == Mul  ? "mul"
              : op == Rem  ? "rem"
              : op == Addu ? "addu"
              : op == Subu ? "subu"
              : op == AddS ? "add.s"
              : op == SubS ? "sub.s"
              : op == MulS ? "mul.s"
              : op == DivS ? "div.s"
              : op == Slt  ? "slt"
              : op == Sle  ? "sle"
              : op == Sgt  ? "sgt"
              : op == Sge  ? "sge"
              : op == Seq  ? "seq"
              : op == Sne  ? "sne"
                           : "";
        out << Ope << " ";
        rd->PrintReg(out);
        out << ", ";
        rs->PrintReg(out);
        out << ", ";
        rt->PrintReg(out);
        out << std::endl;
        return;
    } else if (op <= TruncWS && op >= CEqS) {
        Ope = op == CEqS      ? "c.eq.s"
              : op == CLeS    ? "c.le.s"
              : op == CLtS    ? "c.lt.s"
              : op == CvtSW   ? "cvt.s.w"
              : op == CvtWS   ? "cvt.w.s"
              : op == TruncWS ? "trunc.w.s"
                              : "";
        out << Ope << " ";
        rs->PrintReg(out);
        out << ", ";
        rt->PrintReg(out);
        out << std::endl;
        return;
    } else if (op == Sll) {
        out << "sll ";
        rd->PrintReg(out);
        out << ", ";
        rt->PrintReg(out);
        out << ", " << intermediate << std::endl;
        return;
    } else if (op == Jr) {
        out << "jr ";
        rs->PrintReg(out);
        out << std::endl;
        return;
    } else if (op == Syscall || op == Nop) {
        Ope = op == Syscall ? "syscall" : op == Nop ? "nop" : "";
        out << Ope << std::endl;
        return;
    } else {
        TOLANG_DIE("RCode not supported.");
    }
}

void ICode::PrintCode(std::ostream &out) {
    std::string Ope;
    if (op >= LW && op <= Rem && rt != nullptr) {
        Ope = op == LW      ? "lw"
              : op == SW    ? "sw"
              : op == LS    ? "l.s"
              : op == SS    ? "s.s"
              : op == Addiu ? "addiu"
              : op == Subiu ? "subiu"
              : op == Div   ? "div"
              : op == Mul   ? "mul"
              : op == Rem   ? "rem"
                            : "";
        out << Ope << " ";
        if (op >= LW && op <= SS) {
            rt->PrintReg(out);
            out << ", " << intermediate << "(";
            rs->PrintReg(out);
            out << ")" << std::endl;
        } else {
            rt->PrintReg(out);
            out << ", ";
            rs->PrintReg(out);
            out << ", " << intermediate << std::endl;
        }
        return;
    } else if (op <= SS && op >= LA && rt == nullptr) {
        Ope = op == LA     ? "la"
              : op == Bnez ? "bnez"
              : op == LW   ? "lw"
              : op == SW   ? "sw"
              : op == LS   ? "l.s"
              : op == SS   ? "s.s"
                           : "";
        out << Ope << " ";
        rs->PrintReg(out);
        out << ", " << label << std::endl;
    } else if (op <= BC1T && op >= BC1F) {
        Ope = op == BC1F ? "bc1f" : op == BC1T ? "bc1t" : "";
        out << Ope << " " << label << std::endl;
    } else {
        TOLANG_DIE("ICode not supported.");
    }
}

void JCode::PrintCode(std::ostream &out) {
    std::string Ope = op == J ? "j" : op == Jal ? "jal" : "";
    out << Ope << " " << label << std::endl;
}

void MipsLabel::PrintCode(std::ostream &out) {
    out << std::endl << label << ":" << std::endl;
}
