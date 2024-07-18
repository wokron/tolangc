#include "pcode/PcodeSymbol.h"

PcodeSymbolTable::PcodeSymbolTable() : _current(-1), _last(-1) {
    auto node = PcodeSymbolTableNode::create(-1);
    _table.push_back(node);
    _current = 0;
    _last++;
}

void PcodeSymbolTable::pushScope() {
    auto node = PcodeSymbolTableNode::create(_current);
    _last++;
    _table[_current]->addChild(_last);
    _table.push_back(node);
    _current = _last;
}

void PcodeSymbolTable::popScope() {
    _current = _table[_current]->getParent();
}
