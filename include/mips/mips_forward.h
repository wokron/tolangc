#pragma once

class MipsManager;
class Translator;

class MipsReg;
using MipsRegPtr = MipsReg *;
class ZeroReg;
class ArgumentReg;
class ValueReg;
class TmpReg;
using TmpRegPtr = TmpReg *;
class FloatReg;
using FloatRegPtr = FloatReg *;
class RetAddrReg;
class StkPtrReg;
class FrmPtrReg;
class OffsetReg;

class MipsData;
using MipsDataPtr = MipsData *;
class WordData;
using WordDataPtr = WordData *;
class FloatData;
using FloatDataPtr = FloatData *;
class AsciizData;
using AsciizDataPtr = AsciizData *;

class MipsCode;
using MipsCodePtr = MipsCode *;
class RCode;
class ICode;
class JCode;
class MipsLabel;
using MipsLabelPtr = MipsLabel *;
