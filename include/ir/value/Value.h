#pragma once

#include "ir/IrForward.h"
#include "utils.h"
#include <string>

// All types used in LLVM for tolang.
enum ValueType
{
    // Value
    ArgumentTy,
    BasicBlockTy,

    // Value -> Constant
    ConstantTy,
    ConstantDataTy,

    // Value -> Constant -> GlobalValue
    FunctionTy,
    GlobalVariableTy,

    // Value -> User -> Instruction
    BinaryOperatorTy,
    CompareInstTy,
    BranchInstTy,
    ReturnInstTy,
    StoreInstTy,
    CallInstTy,
    InputInstTy,
    OutputInstTy,
    AllocaInstTy,
    LoadInstTy,
    UnaryOperatorTy,   
};

/// <summary>
/// Base class for all values in LLVM.
/// </summary>
class Value
{
public:
    virtual ~Value() = default;

    // This function is used for RTTI (RunTime Type Identification).
    static bool classof(ValueType type) { return true; }

    // Check if the value is a specific type.
    template<typename _Ty>
    bool Is() const { return _Ty::classof(_valueType); }

    // Cast this type to a specific type.
    // You should use this function only when you are sure that this type is
    // actually the type you want to cast to.
    template<typename _Ty>
    _Ty* As() { return static_cast<_Ty*>(this); }

    virtual bool IsInstruction() const { return false; }
    virtual bool IsUser() const { return false; }
    virtual bool IsFunction() const { return false; }
    virtual bool IsArgument() const { return false; }

public:
    ValueType GetValueType() const { return _valueType; }
    TypePtr GetType() const { return _type; }
    LlvmContextPtr GetContext() const;

    const std::string& GetName() const { return _name; }
    void SetName(const std::string& name) { _name = name; }

    void AddUser(UsePtr user);
    void AddUse(UsePtr use);

    UseListPtr GetUserList() { return &_userList; }
    UseListPtr GetUseList() { return &_useList; }

protected:
    Value(ValueType valueType, TypePtr type) : _type(type), _valueType(valueType) {}

protected:
    TypePtr _type;
    std::string _name;

    UseList _useList;
    UseList _userList;

private:
    ValueType _valueType;
};
