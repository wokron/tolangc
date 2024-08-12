#include "tolang/ast.h"
#include <iostream>

void Ident::print(std::ostream &out) {
    out << "IDENFR " << value << std::endl;
    out << "<Ident>" << std::endl;
}

void CompUnit::print(std::ostream &out) {
    for (const auto &funcDef : func_defs) {
        (*funcDef).print(out);
    }
    for (const auto &varDecl : var_decls) {
        (*varDecl).print(out);
    }
    for (const auto &stmt : stmts) {
        std::visit(
            [&out](auto &s) {
                Node &node_ref = static_cast<Node &>(s);
                node_ref.print(out);
            },
            (*stmt));
    }
    out << "<CompUnit>" << std::endl;
}

void FuncDef::print(std::ostream &out) {
    out << "FN fn" << std::endl;
    ident->print(out);
    out << "LPARENT (" << std::endl;
    if (func_f_params.size() > 0) {
        func_f_params[0]->print(out);
        for (int i = 1; i < func_f_params.size(); i++) {
            out << "COMMA ," << std::endl;
            func_f_params[i]->print(out);
        }
    }
    out << "<FuncFParams>" << std::endl;
    out << "RPARENT )" << std::endl;
    out << "RARROW =>" << std::endl;
    std::visit(
        [&out](auto &s) {
            Node &node_ref = static_cast<Node &>(s);
            node_ref.print(out);
        },
        (*exp));
    out << "SEMICN ;" << std::endl;
    out << "<FuncDef>" << std::endl;
}

void VarDecl::print(std::ostream &out) {
    out << "VARTK var" << std::endl;
    ident->print(out);
    out << "SEMICN ;" << std::endl;
    out << "<VarDecl>" << std::endl;
}

void GetStmt::print(std::ostream &out) {
    out << "GETTK get" << std::endl;
    ident->print(out);
    out << "SEMICN ;" << std::endl;
    out << "<Stmt>" << std::endl;
}

void PutStmt::print(std::ostream &out) {
    out << "PUTTK put" << std::endl;
    std::visit(
        [&out](auto &s) {
            Node &node_ref = static_cast<Node &>(s);
            node_ref.print(out);
        },
        (*exp));
    out << "SEMICN ;" << std::endl;
    out << "<Stmt>" << std::endl;
}

void TagStmt::print(std::ostream &out) {
    out << "TAGTK tag" << std::endl;
    ident->print(out);
    out << "SEMICN ;" << std::endl;
    out << "<Stmt>" << std::endl;
}

void LetStmt::print(std::ostream &out) {
    out << "LETTK let" << std::endl;
    ident->print(out);
    out << "ASSIGN =" << std::endl;
    std::visit(
        [&out](auto &s) {
            Node &node_ref = static_cast<Node &>(s);
            node_ref.print(out);
        },
        (*exp));
    out << "SEMICN ;" << std::endl;
    out << "<Stmt>" << std::endl;
}

void IfStmt::print(std::ostream &out) {
    out << "IFTK if" << std::endl;
    (*cond).print(out);
    out << "TOTK to" << std::endl;
    ident->print(out);
    out << "SEMICN ;" << std::endl;
    out << "<Stmt>" << std::endl;
}

void ToStmt::print(std::ostream &out) {
    out << "TOTK to" << std::endl;
    ident->print(out);
    out << "SEMICN ;" << std::endl;
    out << "<Stmt>" << std::endl;
}

void BinaryExp::print(std::ostream &out) {
    std::visit(
        [&out](auto &s) {
            Node &node_ref = static_cast<Node &>(s);
            node_ref.print(out);
        },
        (*lhs));
    bool isAdd = op == PLUS || op == MINU;
    if (rhs == nullptr) {
        if (isAdd) {
            out << "<AddExp>" << std::endl;
        } else {
            out << "<MulExp>" << std::endl;
        }
        return;
    }
    switch (op) {
    case PLUS: {
        out << "PLUS +" << std::endl;
        break;
    }
    case MINU: {
        out << "MINU -" << std::endl;
        break;
    }
    case MULT: {
        out << "MULT *" << std::endl;
        break;
    }
    case DIV: {
        out << "DIV /" << std::endl;
        break;
    }
    case MOD: {
        out << "MOD %" << std::endl;
        break;
    }
    }
    std::visit(
        [&out](auto &s) {
            Node &node_ref = static_cast<Node &>(s);
            node_ref.print(out);
        },
        (*rhs));
    if (isAdd) {
        out << "<AddExp>" << std::endl;
    } else {
        out << "<MulExp>" << std::endl;
    }
}

void CallExp::print(std::ostream &out) {
    ident->print(out);
    out << "LPARENT (" << std::endl;
    if (func_r_params.size() > 0) {
        std::visit(
            [&out](auto &s) {
                Node &node_ref = static_cast<Node &>(s);
                node_ref.print(out);
            },
            (*(func_r_params[0])));
        for (int i = 1; i < func_r_params.size(); i++) {
            out << "COMMA ," << std::endl;
            std::visit(
                [&out](auto &s) {
                    Node &node_ref = static_cast<Node &>(s);
                    node_ref.print(out);
                },
                (*(func_r_params[i])));
        }
    }
    out << "<FuncRParams>" << std::endl;
    out << "RPARENT )" << std::endl;
    out << "<CallExp>" << std::endl;
}

void UnaryExp::print(std::ostream &out) {
    bool hasParent = false;
    if (std::holds_alternative<BinaryExp>(*exp))
        hasParent = true;
    if (hasParent)
        out << "LPARENT (" << std::endl;
    std::visit(
        [&out](auto &s) {
            Node &node_ref = static_cast<Node &>(s);
            node_ref.print(out);
        },
        (*exp));
    if (hasParent)
        out << "RPARENT )" << std::endl;
    out << "<UnaryExp>" << std::endl;
}

void Cond::print(std::ostream &out) {
    std::visit(
        [&out](auto &s) {
            Node &node_ref = static_cast<Node &>(s);
            node_ref.print(out);
        },
        *lhs);
    switch (op) {
    case LT: {
        out << "LSS <" << std::endl;
        break;
    }
    case GT: {
        out << "GRE >" << std::endl;
        break;
    }
    case LE: {
        out << "LEQ <=" << std::endl;
        break;
    }
    case GE: {
        out << "GEQ >=" << std::endl;
        break;
    }
    case EQ: {
        out << "EQL ==" << std::endl;
        break;
    }
    case NE: {
        out << "NEQ !=" << std::endl;
        break;
    }
    }
    std::visit(
        [&out](auto &s) {
            Node &node_ref = static_cast<Node &>(s);
            node_ref.print(out);
        },
        *rhs);
    out << "<Cond>" << std::endl;
}

void Number::print(std::ostream &out) {
    out << "Number " << value << std::endl;
    out << "<Number>" << std::endl;
}

void IdentExp::print(std::ostream &out) { ident->print(out); }