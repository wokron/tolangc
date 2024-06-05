#pragma once

#include "llvm/asm/AsmWriter.h"
#include "llvm/ir/IrForward.h"

/*
 * Base class for types in LLVM. Can only be accessed by LlvmContext.
 */
class Type {
    friend class LlvmContext;

public:
    enum TypeID {
        // Primitive types
        VoidTyID,
        LabelTyID,

        // Derived types
        IntegerTyID,
        FloatTyID,
        FunctionTyID,
        PointerTyID
    };

    // Always use virtual destructor for base class.
    virtual ~Type() = default;

    static TypePtr GetVoidTy(LlvmContextPtr context);
    static TypePtr GetLabelTy(LlvmContextPtr context);

    TypeID TypeId() const { return _typeId; }
    LlvmContextPtr Context() const { return _context; }

    bool IsVoidTy() const { return _typeId == VoidTyID; }
    bool IsLabelTy() const { return _typeId == LabelTyID; }

    bool IsIntegerTy() const { return _typeId == IntegerTyID; }
    bool IsFloatTy() const { return _typeId == FloatTyID; }
    bool IsArithmeticTy() const { return IsIntegerTy() || IsFloatTy(); }
    bool IsPointerTy() const { return _typeId == PointerTyID; }

    template <typename _Ty> _Ty *As() { return static_cast<_Ty *>(this); }

    virtual void PrintAsm(AsmWriterPtr out);

protected:
    // Prohibit direct instantiation.
    Type(LlvmContextPtr context, TypeID typeId)
        : _typeId(typeId), _context(context) {}

private:
    TypeID _typeId;
    LlvmContextPtr _context;
};

/*
 * Represent an integer. In tolang, it is the only type used
 * in arithmetic operations.
 */
class IntegerType : public Type {
    friend class LlvmContext;

public:
    ~IntegerType() override = default;

    void PrintAsm(AsmWriterPtr out) override;

    static IntegerTypePtr Get(LlvmContextPtr context, unsigned bitWidth);

    unsigned BitWidth() const { return _bitWidth; }

protected:
    IntegerType(LlvmContextPtr context, unsigned bitWidth)
        : Type(context, IntegerTyID), _bitWidth(bitWidth) {}

private:
    unsigned _bitWidth;
};

class FloatType : public Type {
    friend class LlvmContext;

public:
    ~FloatType() override = default;

    void PrintAsm(AsmWriterPtr out) override;

    static FloatTypePtr Get(LlvmContextPtr context, unsigned bitWidth);

    unsigned BitWidth() const { return _bitWidth; }

private:
    FloatType(LlvmContextPtr context, unsigned bitWidth)
        : Type(context, FloatTyID), _bitWidth(bitWidth) {}

    unsigned _bitWidth;
};

/*
 * A function's type consists of a return type and a list of parameter
 * types.
 */
class FunctionType : public Type {
    friend class LlvmContext;

public:
    ~FunctionType() override = default;

    void PrintAsm(AsmWriterPtr out) override;

    static FunctionTypePtr Get(TypePtr returnType,
                               const std::vector<Type *> &paramTypes);
    static FunctionTypePtr Get(TypePtr returnType);

    TypePtr ReturnType() const { return _returnType; }
    const std::vector<TypePtr> &ParamTypes() const { return _paramTypes; }

    bool Equals(TypePtr returnType,
                const std::vector<TypePtr> &paramTypes) const;
    bool Equals(TypePtr returnType) const;

private:
    FunctionType(TypePtr returnType, const std::vector<TypePtr> &paramTypes);
    FunctionType(TypePtr returnType);

    TypePtr _returnType;
    std::vector<TypePtr> _paramTypes;
};

/*
 * A pointer type represents a pointer to another type.
 * It is mostly used as the return type of alloca instruction.
 */
class PointerType : public Type {
    friend class LlvmContext;

public:
    ~PointerType() override = default;

    void PrintAsm(AsmWriterPtr out) override;
    static PointerTypePtr Get(TypePtr elementType);

    TypePtr ElementType() const { return _elementType; }

private:
    PointerType(TypePtr elementType);

    TypePtr _elementType;
};