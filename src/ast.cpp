#include<ast.h>


#include<iostream>
using namespace std;

void Ident::print(std::ostream &out) {
    out << "IDENFR " << value << endl;
    out << "<Ident>" << endl;
}

void CompUnit::print(std::ostream &out) {
    for (const auto& funcDef : funcDefs) {
        (*funcDef).print(out);
    }
    for (const auto & varDecl : varDecls) {
        (*varDecl).print(out);
    }
    for (const auto & stmt : stmts) {
        std::visit([&out](auto& s) {
            Node& node_ref = static_cast<Node&>(s);
            node_ref.print(out);
        }, (*stmt));
    }
    out << "<CompUnit>" << endl;
}

void FuncDef::print(std::ostream &out) {
    out << "FN fn" << endl;
    ident.print(out);
    out << "LPARENT (" << endl;
    (*funcFParams).print(out);
    out << "RPARENT )" << endl;
    out << "RARROW =>" << endl;
    std::visit([&out](auto& s) {
        Node& node_ref = static_cast<Node&>(s);
        node_ref.print(out);
    }, (*exp));
    out << "SEMICN ;" << endl;
    out << "<FuncDef>" << endl;
}

void FuncFParams::print(std::ostream &out) {
    if (idents.size() > 0) {
        idents[0].print(out);
        for (int i = 1;i < idents.size();i++) {
            out << "COMMA ," << endl;
            idents[i].print(out);
        }
    }
    out << "<FuncFParams>" << endl;
}

void VarDecl::print(std::ostream &out) {
    out << "VARTK var" << endl;
    ident.print(out);
    out << "SEMICN ;" << endl;
    out << "<VarDecl>" << endl;
}

void GetStmt::print(std::ostream &out) {
    out << "GETTK get" << endl;
    ident.print(out);
    out << "SEMICN ;" << endl;
    out << "<Stmt>" << endl;
}

void PutStmt::print(std::ostream &out) {
    out << "PUTTK put" << endl;
    std::visit([&out](auto& s) {
        Node& node_ref = static_cast<Node&>(s);
        node_ref.print(out);
    }, (*exp));
    out << "SEMICN ;" << endl;
    out << "<Stmt>" << endl;
}


void TagStmt::print(std::ostream &out) {
    out << "TAGTK tag" << endl;
    ident.print(out);
    out << "SEMICN ;" << endl;
    out << "<Stmt>" << endl;
}

void LetStmt::print(std::ostream &out) {
    out << "LETTK let" << endl;
    ident.print(out);
    out << "ASSIGN =" << endl;
    std::visit([&out](auto& s) {
        Node& node_ref = static_cast<Node&>(s);
        node_ref.print(out);
    }, (*exp));
    out << "SEMICN ;" << endl;
    out << "<Stmt>" << endl;
}

void IfStmt::print(std::ostream &out) {
    out << "IFTK if" << endl;
    (*cond).print(out);
    out << "TOTK to" << endl;
    ident.print(out);
    out << "SEMICN ;" << endl;
    out << "<Stmt>" << endl;
}

void ToStmt::print(std::ostream &out) {
    out << "TOTK to" << endl;
    ident.print(out);
    out << "SEMICN ;" << endl;
    out << "<Stmt>" << endl;
}

void BinaryExp::print(std::ostream &out) {
    std::visit([&out](auto& s) {
        Node& node_ref = static_cast<Node&>(s);
        node_ref.print(out);
    }, (*lexp));
    bool isAdd = op == PLUS || op == MINU;
    if (rexp == nullptr) {
        if (isAdd) {
            out << "<AddExp>" << endl;
        } else {
            out << "<MulExp>" << endl;
        }
        return;
    }
    switch (op) {
    case PLUS: {
        out << "PLUS +" << endl;
        break;
    }
    case MINU: {
        out << "MINU -" << endl;
        break;
    }
    case MULT: {
        out << "MULT *" << endl;
        break;
    }
    case DIV: {
        out << "DIV /" << endl;
        break;
    }
    case MOD: {
        out << "MOD %" << endl;
        break;
    }
    }
    std::visit([&out](auto& s) {
        Node& node_ref = static_cast<Node&>(s);
        node_ref.print(out);
    }, (*rexp));
    if (isAdd) {
        out << "<AddExp>" << endl;
    } else {
        out << "<MulExp>" << endl;
    }
}

void CallExp::print(std::ostream &out) {
    ident.print(out);
    out << "LPARENT (" << endl;
    (*funcRParams).print(out);
    out << "RPARENT )" << endl;
    out << "<CallExp>" << endl;
}


void UnaryExp::print(std::ostream &out) {
    if (hasOp) {
        if (op == PLUS)
            out << "PLUS +" << endl;
        else
            out << "MINUS -" << endl;
    }
    bool hasParent = false;
    if (std::holds_alternative<BinaryExp>(*exp))
        hasParent = true;
    if (hasParent)
        out << "LPARENT (" << endl;
    std::visit([&out](auto& s) {
        Node& node_ref = static_cast<Node&>(s);
        node_ref.print(out);
    }, (*exp));
    if (hasParent)
        out << "RPARENT )" << endl;
    out << "<UnaryExp>" << endl;
}

void FuncRParams::print(std::ostream &out) {
    if (exps.size() > 0) {
        std::visit([&out](auto& s) {
            Node& node_ref = static_cast<Node&>(s);
            node_ref.print(out);
        }, (*(exps[0])));
        for (int i = 1;i < exps.size();i++) {
            out << "COMMA ," << endl;
            std::visit([&out](auto& s) {
                Node& node_ref = static_cast<Node&>(s);
                node_ref.print(out);
            }, (*(exps[i])));
        }
    }
    out << "<FuncRParams>" << endl;
}

void Cond::print(std::ostream &out) {
    std::visit([&out](auto &s) {
        Node &node_ref = static_cast<Node &>(s);
        node_ref.print(out);
    },*left);
    switch (op) {
    case LSS: {
        out << "LSS <" << endl;
        break;
    }
    case GRE: {
        out << "GRE >" << endl;
        break;
    }
    case LEQ: {
        out << "LEQ <=" << endl;
        break;
    }
    case GEQ: {
        out << "GEQ >=" << endl;
        break;
    }
    case EQL: {
        out << "EQL ==" << endl;
        break;
    }
    case NEQ: {
        out << "NEQ !=" << endl;
        break;
    }
    }
    std::visit([&out](auto &s) {
        Node &node_ref = static_cast<Node &>(s);
        node_ref.print(out);
    },*right);
    out << "<Cond>" << endl;
}

void Number::print(std::ostream &out) {
    out << "Number " << value << endl;
    out << "<Number>" << endl;
}