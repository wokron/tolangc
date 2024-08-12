#pragma once

#include "llvm/asm/AsmWriter.h"
#include "llvm/ir/IrForward.h"
#include "llvm/ir/Type.h"
#include <string>

// All types used in LLVM for tolang.
enum class ValueType {
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
    JumpInstTy,
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
class Value {
    friend class User; // to access protected methods
public:
    virtual ~Value() = default;

    // This function is used for RTTI (RunTime Type Identification).
    static bool classof(ValueType type) { return true; }

    // Check if the value is a specific type.
    template <typename _Ty> bool Is() const { return _Ty::classof(_valueType); }

    // Cast this type to a specific type.
    // You should use this function only when you are sure that this type is
    // actually the type you want to cast to.
    template <typename _Ty> _Ty *As() { return static_cast<_Ty *>(this); }

    /*
     * Print the complete asm code of this value. This is used to print
     * the value itself on its occurrence.
     */
    virtual void PrintAsm(AsmWriterPtr out);

    /*
     * Print only the name of this value. For example, only %1.
     */
    virtual void PrintName(AsmWriterPtr out);

    /*
     * Print the use of this value. Usually, the type and name.
     */
    virtual void PrintUse(AsmWriterPtr out);

public:
    using use_iterator = UseList::iterator;

    ValueType GetValueType() const { return _valueType; }
    TypePtr GetType() const { return _type; }
    LlvmContextPtr Context() const { return GetType()->Context(); }

    const std::string &GetName() const { return _name; }
    void SetName(const std::string &name) { _name = name; }

    use_iterator UserBegin() { return _userList.begin(); }
    use_iterator UserEnd() { return _userList.end(); }
    UseListPtr GetUserList() { return &_userList; }

protected:
    void AddUser(UserPtr user);
    UserPtr RemoveUser(UserPtr user);

protected:
    Value(ValueType valueType, TypePtr type)
        : _type(type), _valueType(valueType) {}

    Value(ValueType ValueType, TypePtr type, const std::string &name)
        : _type(type), _name(name), _valueType(ValueType) {}

protected:
    TypePtr _type;
    std::string _name;

    UseList _userList;

private:
    ValueType _valueType;
};