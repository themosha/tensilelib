#include "tensile.h"

#include <sstream>
#include <string_view>
#include <iostream>
#include <iomanip>

namespace tensile {

class Comment : public ISQLFeature {
public:
    std::string name() override { return "comment"; }

    std::string GenerateSQL(size_t n) override {
        return "select /*" + std::string(n, '.') + "*/ 1";
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select /*...*/ 1", GenerateSQL(3));
    }
};

class Identifier : public ISQLFeature {
public:
    std::string name() override { return "identifier"; }

    std::string GenerateSQL(size_t n) override {
        return "select 1 as " + std::string(n, 'x');
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 1 as xxx", GenerateSQL(3));
    }
};

class Parenthesis : public ISQLFeature {
public:
    std::string name() override { return "parenthesis"; }

    std::string GenerateSQL(size_t n) override {
        return "select " + std::string(n, '(') + "1" + std::string(n, ')');
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select (((1)))", GenerateSQL(3));
    }
};

class PositiveIntegerLiteral : public ISQLFeature {
public:
    std::string name() override { return "positive integer"; }

    std::string GenerateSQL(size_t n) override {
        return "select " + std::to_string(n);
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 42", GenerateSQL(42));
    }
};

class NegativeIntegerLiteral : public ISQLFeature {
public:
    std::string name() override { return "negative integer"; }

    std::string GenerateSQL(size_t n) override {
        return "select -" + std::to_string(n);
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select -1001", GenerateSQL(1001));
    }
};

class NumericLiteral : public ISQLFeature {
public:
    std::string name() override { return "numeric literal"; }

    std::string GenerateSQL(size_t n) override {
        return "select numeric '" + std::string(n, '9') + "'";
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select numeric '9999'", GenerateSQL(4));
    }
};

class NumericPrecision : public ISQLFeature {
public:
    std::string name() override { return "numeric precision"; }

    std::string GenerateSQL(size_t n) override {
        return "select numeric(" + std::to_string(n) + ",0) '1'";
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select numeric(5,0) '1'", GenerateSQL(5));
    }
};

class FloatLiteral : public ISQLFeature {
public:
    std::string name() override { return "floating point literal"; }

    std::string GenerateSQL(size_t n) override {
        return "select " + std::to_string(n) + ".0";
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 42.0", GenerateSQL(42));
    }
};

class FloatPositiveExp : public ISQLFeature {
public:
    std::string name() override { return "float positive exponent"; }

    std::string GenerateSQL(size_t n) override {
        return "select 1E" + std::to_string(n);
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 1E10", GenerateSQL(10));
    }
};

class FloatNegativeExp : public ISQLFeature {
public:
    std::string name() override { return "float negative exponent"; }

    std::string GenerateSQL(size_t n) override {
        return "select 1E-" + std::to_string(n);
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 1E-4", GenerateSQL(4));
    }
};

class TextLiteral : public ISQLFeature {
public:
    std::string name() override { return "text literal"; }

    std::string GenerateSQL(size_t n) override {
        return "select '" + std::string(n, 'x') + "'";
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 'xxxxx'", GenerateSQL(5));
    }
};

class ByteaLiteral : public ISQLFeature {
public:
    std::string name() override { return "bytea literal"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select bytea '";
        for (size_t i = 0; i < n; i++) {
            sql_.append("\\001");
        }
        sql_.append("'");
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select bytea '\\001\\001\\001\\001\\001'", GenerateSQL(5));
    }
};

class FutureDateLiteral : public ISQLFeature {
public:
    std::string name() override { return "future date literal"; }

    std::string GenerateSQL(size_t n) override {
        std::stringstream ss;
        ss << std::setw(4) << std::setfill('0') << n;
        return "select date '" + ss.str() + "-12-31'";
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select date '0042-12-31'", GenerateSQL(42));
    }
};

class PastDateLiteral : public ISQLFeature {
public:
    std::string name() override { return "past date literal"; }

    std::string GenerateSQL(size_t n) override {
        std::stringstream ss;
        ss << std::setw(4) << std::setfill('0') << n;
        return "select date '" + ss.str() + "-01-01 BC'";
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select date '0042-01-01 BC'", GenerateSQL(42));
    }
};

class FutureTimestampLiteral : public ISQLFeature {
public:
    std::string name() override { return "future timestamp literal"; }

    std::string GenerateSQL(size_t n) override {
        std::stringstream ss;
        ss << std::setw(4) << std::setfill('0') << n;
        return "select timestamp '" + ss.str() + "-12-31 23:59:59.999999'";
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select timestamp '0042-12-31 23:59:59.999999'", GenerateSQL(42));
    }
};

class PastTimestampLiteral : public ISQLFeature {
public:
    std::string name() override { return "past timestamp literal"; }

    std::string GenerateSQL(size_t n) override {
        std::stringstream ss;
        ss << std::setw(4) << std::setfill('0') << n;
        return "select timestamp '" + ss.str() + "-01-01 BC 00:00:00'";
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select timestamp '0042-01-01 BC 00:00:00'", GenerateSQL(42));
    }
};

class Array : public ISQLFeature {
public:
    std::string name() override { return "array"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select array [1";
        for (size_t i = 0; i < n - 1; i++) {
            sql_.append(",1");
        }
        sql_.append("]");
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select array [1,1]", GenerateSQL(2));
    }
};

class NestedArray : public ISQLFeature {
public:
    std::string name() override { return "nested array"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select ";
        for (size_t i = 0; i < n; i++)
            sql_.append("array [");
        sql_.append("1");
        sql_.append(n, ']');
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select array [1]", GenerateSQL(1));
        cmp->ExpectEq("select array [array [array [array [array [1]]]]]", GenerateSQL(5));
    }
};

class Tuple : public ISQLFeature {
public:
    std::string name() override { return "tuple"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select (1";
        for (size_t i = 0; i < n - 1; i++) {
            sql_.append(",1");
        }
        sql_.append(")");
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select (1,1,1)", GenerateSQL(3));
    }
};

class NestedTuple : public ISQLFeature {
public:
    std::string name() override { return "nested tuple"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select (1";
        for (size_t i = 1; i < n; i++) {
            sql_.append(",(1");
        }
        sql_.append(n, ')');
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select (1)", GenerateSQL(1));
        cmp->ExpectEq("select (1,(1,(1)))", GenerateSQL(3));
    }
};

class SelectList : public ISQLFeature {
public:
    std::string name() override { return "select list"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select 1";
        for (size_t i = 0; i < n - 1; i++) {
            sql_.append(",1");
        }
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 1,1,1", GenerateSQL(3));
    }
};

class UnaryOperator : public ISQLFeature {
public:
    UnaryOperator(std::string_view op, std::string_view value) : op_(op), value_(value) {}

    std::string name() override { return "unary operator " + op_; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select ";
        for (size_t i = 0; i < n; i++) {
            sql_.append(op_);
        }
        sql_.append(value_);
        return sql_;
    }

private:
    const std::string op_;
    const std::string value_;
};

class UnaryPlus : public UnaryOperator {
public:
    UnaryPlus() : UnaryOperator("+", "1") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select ++1", GenerateSQL(2));
    }
};

// trailing space in "- " to prevent treating -- as a comment
class UnaryMinus : public UnaryOperator {
public:
    UnaryMinus() : UnaryOperator("- ", "1") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select - - - 1", GenerateSQL(3));
    }
};

class LogicalNot : public UnaryOperator {
public:
    LogicalNot() : UnaryOperator("NOT ", "true") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select NOT NOT NOT true", GenerateSQL(3));
    }
};

class BitwiseNot : public UnaryOperator {
public:
    BitwiseNot() : UnaryOperator("~ ", "1") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select ~ ~ ~ 1", GenerateSQL(3));
    }
};

class AbsOperator : public UnaryOperator {
public:
    AbsOperator() : UnaryOperator("@ ", "1") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select @ @ @ 1", GenerateSQL(3));
    }
};

class SquareRootOperator : public UnaryOperator {
public:
    SquareRootOperator() : UnaryOperator("|/ ", "1") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select |/ |/ |/ 1", GenerateSQL(3));
    }
};

class CubeRootOperator : public UnaryOperator {
public:
    CubeRootOperator() : UnaryOperator("||/ ", "1") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select ||/ ||/ ||/ 1", GenerateSQL(3));
    }
};

class BinaryOperator : public ISQLFeature {
public:
    explicit BinaryOperator(const std::string &op, std::string_view value) : op_(" " + op + " "), value_(value) {}

    std::string name() override { return "binary operator " + op_; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select " + value_;
        for (size_t i = 0; i < n; i++) {
            sql_.append(op_);
            sql_.append(value_);
        }
        return sql_;
    }

private:
    const std::string op_;
    const std::string value_;
};

class Plus : public BinaryOperator {
public:
    Plus() : BinaryOperator("+", "0") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 0 + 0 + 0 + 0 + 0", GenerateSQL(4));
    }
};

class Minus : public BinaryOperator {
public:
    Minus() : BinaryOperator("-", "0") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 0 - 0 - 0 - 0", GenerateSQL(3));
    }
};

class Multiply : public BinaryOperator {
public:
    Multiply() : BinaryOperator("*", "1") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 1 * 1 * 1 * 1", GenerateSQL(3));
    }
};

class Divide : public BinaryOperator {
public:
    Divide() : BinaryOperator("/", "1") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 1 / 1 / 1 / 1", GenerateSQL(3));
    }
};

class Modulo : public BinaryOperator {
public:
    Modulo() : BinaryOperator("%", "1") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 1 % 1 % 1 % 1", GenerateSQL(3));
    }
};

class Power : public BinaryOperator {
public:
    Power() : BinaryOperator("^", "1") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 1 ^ 1 ^ 1 ^ 1", GenerateSQL(3));
    }
};

class BitwiseAnd : public BinaryOperator {
public:
    BitwiseAnd() : BinaryOperator("&", "1") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 1 & 1 & 1 & 1", GenerateSQL(3));
    }
};

class BitwiseOr : public BinaryOperator {
public:
    BitwiseOr() : BinaryOperator("|", "1") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 1 | 1 | 1 | 1", GenerateSQL(3));
    }
};

class BitwiseXor : public BinaryOperator {
public:
    BitwiseXor() : BinaryOperator("#", "1") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 1 # 1 # 1 # 1", GenerateSQL(3));
    }
};

class BitwiseShiftLeft : public BinaryOperator {
public:
    BitwiseShiftLeft() : BinaryOperator("<<", "0") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 0 << 0 << 0 << 0", GenerateSQL(3));
    }
};

class BitwiseShiftRight : public BinaryOperator {
public:
    BitwiseShiftRight() : BinaryOperator(">>", "0") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 0 >> 0 >> 0 >> 0", GenerateSQL(3));
    }
};

class LogicalAnd : public BinaryOperator {
public:
    LogicalAnd() : BinaryOperator("AND", "true") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select true AND true AND true", GenerateSQL(2));
    }
};

class LogicalOr : public BinaryOperator {
public:
    LogicalOr() : BinaryOperator("OR", "true") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select true OR true OR true", GenerateSQL(2));
    }
};

class Is : public BinaryOperator {
public:
    Is() : BinaryOperator("IS", "true") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select true IS true IS true", GenerateSQL(2));
    }
};

class TextConcat : public BinaryOperator {
public:
    TextConcat() : BinaryOperator("||", "'x'") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 'x' || 'x' || 'x'", GenerateSQL(2));
    }
};

class ArrayConcat : public BinaryOperator {
public:
    ArrayConcat() : BinaryOperator("||", "array [1]") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select array [1] || array [1]", GenerateSQL(1));
    }
};

class UnaryFunction : public ISQLFeature {
public:
    explicit UnaryFunction(std::string func, std::string value) : func_(func + "("), value_(value) {}

    std::string name() override { return "function " + func_ + ")"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select ";
        for (size_t i = 0; i < n; i++) {
            sql_.append(func_);
        }
        sql_.append(value_);
        sql_.append(n, ')');
        return sql_;
    }

private:
    const std::string func_;
    const std::string value_;
};

class Abs : public UnaryFunction {
public:
    Abs() : UnaryFunction("abs", "-1") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select abs(abs(abs(-1)))", GenerateSQL(3));
    }
};

class Trim : public ISQLFeature {
public:
    std::string name() override { return "trim"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select ";
        for (size_t i = 0; i < n; i++) {
            sql_.append("trim(' ' from ");
        }
        sql_.append("'  x '");
        sql_.append(n, ')');
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select trim(' ' from trim(' ' from '  x '))", GenerateSQL(2));
    }
};

class DateTrunc : public ISQLFeature {
public:
    std::string name() override { return "date_trunc"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select ";
        for (size_t i = 0; i < n; i++) {
            sql_.append("date_trunc('minute', ");
        }
        sql_.append("timestamp '2000-01-01 10:20:30'");
        sql_.append(n, ')');
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select date_trunc('minute', date_trunc('minute', timestamp '2000-01-01 10:20:30'))",
                      GenerateSQL(2));
    }
};

class Repeat : public ISQLFeature {
    std::string name() override { return "repeat"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select repeat('x', " + std::to_string(n) + ")";
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select repeat('x', 5)", GenerateSQL(5));
    }
};

class Replace : public ISQLFeature {
    std::string name() override { return "replace"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select ";
        for (size_t i = 0; i < n; i++) {
            sql_.append("replace(");
        }
        sql_.append("'a'");
        for (size_t i = 0; i < n; i++) {
            sql_.append(", 'a', 'aa')");
        }
        return sql_;
    }

    bool is_exponential() const override { return true; }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select replace(replace('a', 'a', 'aa'), 'a', 'aa')", GenerateSQL(2));
    }
};

class LPad : public ISQLFeature {
    std::string name() override { return "lpad"; }

    std::string GenerateSQL(size_t n) override {
        return "select lpad('x', " + std::to_string(n) + ", ' ')";
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select lpad('x', 5, ' ')", GenerateSQL(5));
    }
};

class RPad : public ISQLFeature {
    std::string name() override { return "rpad"; }

    std::string GenerateSQL(size_t n) override {
        return "select rpad('x', " + std::to_string(n) + ", ' ')";
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select rpad('x', 5, ' ')", GenerateSQL(5));
    }
};

class Format : public ISQLFeature {
    std::string name() override { return "format"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select format('%" + std::to_string(n) + "s', 'a')";
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select format('%5s', 'a')", GenerateSQL(5));
    }
};
class AtTimeZone : public ISQLFeature {
    std::string name() override { return "at time zone"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select timestamp '2000-01-01 00:00:00'";
        for (size_t i = 0; i < n; i++) {
            sql_.append(" at time zone '+00'");
        }
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select timestamp '2000-01-01 00:00:00' at time zone '+00' at time zone '+00'",
                      GenerateSQL(2));
    }
};

class Cast : public ISQLFeature {
    std::string name() override { return "cast"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select ";
        for (size_t i = 0; i < n; i++) {
            sql_.append("cast(");
        }
        sql_.append("'1'");
        for (size_t i = 0; i < n; i++) {
            sql_.append(" as int)");
        }
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select cast(cast('1' as int) as int)", GenerateSQL(2));
    }
};

class CastNestedArray : public ISQLFeature {
    std::string name() override { return "cast as nested array"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select cast(NULL as int";
        for (size_t i = 0; i < n; i++) {
            sql_.append("[]");
        }
        sql_.append(")");
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select cast(NULL as int[][])", GenerateSQL(2));
    }
};

class CastOperator : public ISQLFeature {
    std::string name() override { return "cast ::"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select '1'";
        for (size_t i = 0; i < n; i++) {
            sql_.append("::int");
        }
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select '1'::int::int::int", GenerateSQL(3));
    }
};

class StringAgg : public ISQLFeature {
    std::string name() override { return "string_agg"; }

    std::string GenerateSQL(size_t n) override {
        return "select string_agg(x::text, '') from generate_series(1," + std::to_string(n) + ") as t(x)";
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select string_agg(x::text, '') from generate_series(1,3) as t(x)", GenerateSQL(3));
    }
};

class ArrayAgg : public ISQLFeature {
    std::string name() override { return "array_agg"; }

    std::string GenerateSQL(size_t n) override {
        return "select array_agg(x) from generate_series(1," + std::to_string(n) + ") as t(x)";
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select array_agg(x) from generate_series(1,3) as t(x)", GenerateSQL(3));
    }
};

class InList : public ISQLFeature {
public:
    std::string name() override { return "IN list"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select 1 in (2";
        for (size_t i = 1; i < n; i++) {
            sql_.append(",2");
        }
        sql_.append(")");
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 1 in (2,2,2,2,2)", GenerateSQL(5));
    }
};

class Coalesce : public ISQLFeature {
public:
    std::string name() override { return "coalesce"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select coalesce(null";
        for (size_t i = 1; i < n-1; i++) {
            sql_.append(",null");
        }
        sql_.append(",1)");
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select coalesce(null,null,null,1)", GenerateSQL(4));
    }
};

class Greatest : public ISQLFeature {
public:
    std::string name() override { return "greatest"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select greatest(0";
        for (size_t i = 1; i < n; i++) {
            sql_.append("," + std::to_string(i));
        }
        sql_.append(")");
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select greatest(0,1,2,3)", GenerateSQL(4));
    }
};

class SimpleCase : public ISQLFeature {
public:
    std::string name() override { return "simple CASE"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select case x ";
        for (size_t i = 0; i < n; i++) {
            sql_.append("when " + std::to_string(i) + " then " + std::to_string(i) + "+1 ");
        }
        sql_.append("else 0 end from (select 0 x) t");
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select case x when 0 then 0+1 when 1 then 1+1 else 0 end from (select 0 x) t",
                      GenerateSQL(2));
    }
};

class SearchedCase : public ISQLFeature {
public:
    std::string name() override { return "searched CASE"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select case ";
        for (size_t i = 0; i < n; i++) {
            sql_.append("when x > " + std::to_string(i) + " then " + std::to_string(i) + "+1 ");
        }
        sql_.append("else 0 end from (select 0 x) t");
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select case when x > 0 then 0+1 when x > 1 then 1+1 else 0 end from (select 0 x) t",
                      GenerateSQL(2));
    }
};

class SubSelectFrom : public ISQLFeature {
public:
    explicit SubSelectFrom() {}

    std::string name() override { return "subselect in FROM "; }

    std::string GenerateSQL(size_t n) override {
        sql_.clear();
        for (size_t i = 0; i < n; i++) {
            sql_.append("select * from (");
        }
        sql_.append("select 1 as x");
        for (size_t i = 0; i < n; i++) {
            sql_.append(") t" + std::to_string(i));
        }
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select * from (select * from (select 1 as x) t0) t1", GenerateSQL(2));
    }
};

class SubSelectScalar : public ISQLFeature {
public:
    explicit SubSelectScalar() {}

    std::string name() override { return "subselect nested scalar"; }

    std::string GenerateSQL(size_t n) override {
        sql_.clear();
        for (size_t i = 0; i < n; i++) {
            sql_.append("select (");
        }
        sql_.append("select 1 as x");
        for (size_t i = 0; i < n; i++) {
            sql_.append(")");
        }
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select (select (select (select (select 1 as x))))", GenerateSQL(4));
    }
};

class SubSelectInExpr : public ISQLFeature {
public:
    explicit SubSelectInExpr() {}

    std::string name() override { return "subselect in expression"; }

    std::string GenerateSQL(size_t n) override {
        sql_.clear();
        for (size_t i = 0; i < n; i++) {
            sql_.append("select 1 + (");
        }
        sql_.append("select 1 as x");
        for (size_t i = 0; i < n; i++) {
            sql_.append(") t" + std::to_string(i));
        }
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select 1 + (select 1 + (select 1 as x) t0) t1", GenerateSQL(2));
    }
};

class CTE : public ISQLFeature {
public:
    std::string name() override { return "CTE"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "with ";
        sql_.append("t0 as (select 1 as x) ");
        for (size_t i = 1; i < n; i++) {
            sql_.append(", t" + std::to_string(i) + " as (" + "select * from t" + std::to_string(i - 1) + ")");
        }
        sql_.append(" select * from t0");
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "with t0 as (select 1 as x) , t1 as (select * from t0), t2 as (select * from t1) select * from t0",
                GenerateSQL(3));
    }
};

class RecursiveCTE : public ISQLFeature {
public:
    std::string name() override { return "recursive CTE"; }

    std::string GenerateSQL(size_t n) override {
        return "with recursive r as (select 1 x union all select x + 1 from r where x < " + std::to_string(n) +
               ") select max(x) from r";
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "with recursive r as (select 1 x union all select x + 1 from r where x < 100) select max(x) from r",
                GenerateSQL(100));
    }
};

class GroupByList : public ISQLFeature {
public:
    std::string name() override { return "GROUP BY list"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select x from (select 1 x) t group by x";
        for (size_t i = 1; i < n; i++) {
            sql_.append(",x+");
            sql_.append(std::to_string(i));
        }
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select x from (select 1 x) t group by x,x+1,x+2", GenerateSQL(3));
    }
};

class OrderByList : public ISQLFeature {
public:
    std::string name() override { return "ORDER BY list"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select x from (select 1 x) t order by x";
        for (size_t i = 1; i < n; i++) {
            sql_.append(",x+" + std::to_string(i));
        }
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select x from (select 1 x) t order by x,x+1,x+2", GenerateSQL(3));
    }
};

class Aggregation : public ISQLFeature {
public:
    std::string name() override { return "aggregation"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select sum(x)";
        for (size_t i = 1; i < n; i++) {
            sql_.append(",sum(x+" + std::to_string(i) + ")");
        }
        sql_.append(" from (select 1 x) t");
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq("select sum(x),sum(x+1),sum(x+2) from (select 1 x) t", GenerateSQL(3));
    }
};


class RelOperator : public ISQLFeature {
public:
    explicit RelOperator(std::string_view op) : op_(op) {}

    std::string name() override { return op_; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select 1 x";
        for (size_t i = 0; i < n; i++) {
            sql_.append(" ");
            sql_.append(op_);
            sql_.append(" select 1 x");
        }
        return sql_;
    }

private:
    const std::string op_;
};

class UnionAll : public RelOperator {
public:
    UnionAll() : RelOperator("union all") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select 1 x union all select 1 x union all select 1 x",
                GenerateSQL(2));
    }
};

class Union : public RelOperator {
public:
    Union() : RelOperator("union") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select 1 x union select 1 x union select 1 x union select 1 x",
                GenerateSQL(3));
    }
};

class Except : public RelOperator {
public:
    Except() : RelOperator("except") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select 1 x except select 1 x",
                GenerateSQL(1));
    }
};

class JoinOperator : public ISQLFeature {
public:
    explicit JoinOperator(std::string_view type) : type_(type) {}

    std::string name() override { return type_.empty() ? "join" : type_ + " join"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select * from (select 1 x) t0";
        for (size_t i = 1; i <= n; i++) {
            if (!type_.empty()) {
                sql_.append(" ");
                sql_.append(type_);
            }
            sql_.append(" (select 1 x) t");
            sql_.append(std::to_string(i));
            std::string p = predicate(n, i - 1, i);
            if (!p.empty()) {
                sql_.append(" ");
                sql_.append(p);
            }
        }
        return sql_;
    }

protected:
    // Function to generate predicate
    virtual std::string predicate(size_t n, size_t i1, size_t i2) const = 0;

private:
    const std::string type_;
};

class CrossJoin : public JoinOperator {
public:
    CrossJoin() : JoinOperator("cross join") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select * from (select 1 x) t0 "
                "cross join (select 1 x) t1 "
                "cross join (select 1 x) t2 "
                "cross join (select 1 x) t3 "
                "cross join (select 1 x) t4",
                GenerateSQL(4));
    }

protected:
    std::string predicate(size_t n, size_t i1, size_t i2) const override { return std::string(); }
};

class NaturalJoin : public JoinOperator {
public:
    NaturalJoin() : JoinOperator("natural join") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select * from (select 1 x) t0 "
                "natural join (select 1 x) t1 "
                "natural join (select 1 x) t2 "
                "natural join (select 1 x) t3",
                GenerateSQL(3));
    }

protected:
    std::string predicate(size_t n, size_t i1, size_t i2) const override { return std::string(); }
};

class JoinChain : public JoinOperator {
public:
    JoinChain() : JoinOperator("join") {}

    std::string name() override { return "chain join"; }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select * from (select 1 x) t0 "
                "join (select 1 x) t1 on t0.x=t1.x "
                "join (select 1 x) t2 on t1.x=t2.x "
                "join (select 1 x) t3 on t2.x=t3.x",
                GenerateSQL(3));
    }

protected:
    std::string predicate(size_t n, size_t i1, size_t i2) const override {
        return "on t" + std::to_string(i1) + ".x=t" + std::to_string(i2) + ".x";
    }
};

class JoinChainRight : public ISQLFeature {
public:
    std::string name() override { return "join chain right"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select * from (select 1 x) t0";
        for (size_t i = 1; i < n; i++) {
            sql_.append(" inner join (select 1 x) t" + std::to_string(i));
        }
        for (size_t i = 1; i < n; i++) {
            sql_.append(" on true");
        }
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select * from (select 1 x) t0 inner join (select 1 x) t1 inner join (select 1 x) t2 on true on true",
                GenerateSQL(3));
    }
};

class JoinStar : public JoinOperator {
public:
    JoinStar() : JoinOperator("join") {}

    std::string name() override { return "star join"; }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select * from (select 1 x) t0 "
                "join (select 1 x) t1 on t0.x=t1.x "
                "join (select 1 x) t2 on t0.x=t2.x "
                "join (select 1 x) t3 on t0.x=t3.x",
                GenerateSQL(3));
    }

protected:
    std::string predicate(size_t n, size_t i1, size_t i2) const override {
        return "on t0.x=t" + std::to_string(i2) + ".x";
    }
};

class JoinUsingOperator : public JoinOperator {
public:
    explicit JoinUsingOperator(std::string_view type) : JoinOperator(type) {}

protected:
    std::string predicate(size_t n, size_t i1, size_t i2) const override {
        return "using (x)";
    }
};

class InnerJoin : public JoinUsingOperator {
public:
    InnerJoin() : JoinUsingOperator("inner join") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select * from (select 1 x) t0 "
                "inner join (select 1 x) t1 using (x) "
                "inner join (select 1 x) t2 using (x) "
                "inner join (select 1 x) t3 using (x)",
                GenerateSQL(3));
    }

protected:
    std::string predicate(size_t n, size_t i1, size_t i2) const override {
        return "using (x)";
    }
};

class LeftOuterJoin : public JoinUsingOperator {
public:
    LeftOuterJoin() : JoinUsingOperator("left outer join") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select * from (select 1 x) t0 left outer join (select 1 x) t1 using (x)",
                GenerateSQL(1));
    }
};

class RightOuterJoin : public JoinUsingOperator {
public:
    RightOuterJoin() : JoinUsingOperator("right outer join") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select * from (select 1 x) t0 "
                "right outer join (select 1 x) t1 using (x) "
                "right outer join (select 1 x) t2 using (x)",
                GenerateSQL(2));
    }
};

class FullOuterJoin : public JoinUsingOperator {
public:
    FullOuterJoin() : JoinUsingOperator("full outer join") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select * from (select 1 x) t0 "
                "full outer join (select 1 x) t1 using (x)",
                GenerateSQL(1));
    }
};

class LateralJoin : public JoinOperator {
public:
    LateralJoin() : JoinOperator(", lateral") {}

    std::string name() override { return "lateral join"; }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select * from (select 1 x) t0 , lateral (select 1 x) t1 , lateral (select 1 x) t2",
                GenerateSQL(2));
    }

protected:
    std::string predicate(size_t n, size_t i1, size_t i2) const override { return std::string(); }
};

class WhereSemiJoin : public ISQLFeature {
public:
    explicit WhereSemiJoin(std::string_view type) : type_(type) {}

    std::string name() override { return type_ + " semijoin"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select * from (select 1 as x) t0";
        for (size_t i = 0; i < n; i++) {
            sql_.append(" where " + type_ + " (select * from (select 1 as x) t" + std::to_string(i + 1));
        }
        sql_.append(n, ')');
        return sql_;
    }

private:
    std::string type_;
};

class InSemiJoin : public WhereSemiJoin {
public:
    InSemiJoin() : WhereSemiJoin("x in") {}

    std::string name() override { return "in semijoin"; }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select * from (select 1 as x) t0 "
                "where x in (select * from (select 1 as x) t1 "
                "where x in (select * from (select 1 as x) t2 "
                "where x in (select * from (select 1 as x) t3)))",
                GenerateSQL(3));
    }
};

class ExistsSemiJoin : public WhereSemiJoin {
public:
    ExistsSemiJoin() : WhereSemiJoin("exists") {}

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select * from (select 1 as x) t0 "
                "where exists (select * from (select 1 as x) t1 "
                "where exists (select * from (select 1 as x) t2))",
                GenerateSQL(2));
    }
};

class AnySemiJoin : public WhereSemiJoin {
public:
    AnySemiJoin() : WhereSemiJoin("x > any") {}

    std::string name() override { return "any semijoin"; }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select * from (select 1 as x) t0 where x > any (select * from (select 1 as x) t1 where x > any (select * from (select 1 as x) t2))",
                GenerateSQL(2));
    }
};

class AllSemiJoin : public WhereSemiJoin {
public:
    AllSemiJoin() : WhereSemiJoin("x < all") {}

    std::string name() override { return "all semijoin"; }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select * from (select 1 as x) t0 where x < all (select * from (select 1 as x) t1 where x < all (select * from (select 1 as x) t2))",
                GenerateSQL(2));
    }
};

class Unnest : public ISQLFeature {
public:
    std::string name() override { return "unnest"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select * from (select 1 x) t";
        for (size_t i = 0; i < n; i++) {
            sql_.append(", unnest(array [x]) t" + std::to_string(i));
        }
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select * from (select 1 x) t, unnest(array [x]) t0, unnest(array [x]) t1",
                GenerateSQL(2));
    }
};

class UnnestList : public ISQLFeature {
public:
    std::string name() override { return "unnest list"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select * from unnest(array[1]";
        for (size_t i = 1; i < n; i++) {
            sql_.append(", array [1]");
        }
        sql_.append(")");
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select * from unnest(array[1], array [1], array [1])",
                GenerateSQL(3));
    }
};

class Windows : public ISQLFeature {
public:
    std::string name() override { return "windows"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select x";
        for (size_t i = 1; i <= n; i++) {
            sql_.append(", row_number() over (order by x+" + std::to_string(i) + ")");
        }
        sql_.append(" from (select 1 x) t");
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select x, row_number() over (order by x+1), row_number() over (order by x+2) from (select 1 x) t",
                GenerateSQL(2));
    }
};

class NamedWindow : public ISQLFeature {
public:
    std::string name() override { return "named window"; }

    std::string GenerateSQL(size_t n) override {
        sql_ = "select row_number() over w" + std::to_string(n) + " window w0 as ()";
        for (size_t i = 1; i <= n; i++) {
            sql_.append(" ,w" + std::to_string(i) + " as (w" + std::to_string(i - 1) + ")");
        }
        return sql_;
    }

    void SelfTest(ITestComparer *cmp) override {
        cmp->ExpectEq(
                "select row_number() over w2 window w0 as () ,w1 as (w0) ,w2 as (w1)",
                GenerateSQL(2));
    }
};

std::vector<std::unique_ptr<ISQLFeature>> GetBuiltinFeatures() {
    std::vector<std::unique_ptr<ISQLFeature>> features;
    features.emplace_back(std::make_unique<Comment>());
    features.emplace_back(std::make_unique<Identifier>());
    features.emplace_back(std::make_unique<Parenthesis>());
    features.emplace_back(std::make_unique<PositiveIntegerLiteral>());
    features.emplace_back(std::make_unique<NegativeIntegerLiteral>());
    features.emplace_back(std::make_unique<NumericLiteral>());
    features.emplace_back(std::make_unique<NumericPrecision>());
    features.emplace_back(std::make_unique<FloatLiteral>());
    features.emplace_back(std::make_unique<FloatPositiveExp>());
    features.emplace_back(std::make_unique<FloatNegativeExp>());
    features.emplace_back(std::make_unique<TextLiteral>());
    features.emplace_back(std::make_unique<ByteaLiteral>());
    features.emplace_back(std::make_unique<FutureDateLiteral>());
    features.emplace_back(std::make_unique<PastDateLiteral>());
    features.emplace_back(std::make_unique<FutureTimestampLiteral>());
    features.emplace_back(std::make_unique<PastTimestampLiteral>());
    features.emplace_back(std::make_unique<Array>());
    features.emplace_back(std::make_unique<NestedArray>());
    features.emplace_back(std::make_unique<Tuple>());
    features.emplace_back(std::make_unique<NestedTuple>());
    features.emplace_back(std::make_unique<SelectList>());
    features.emplace_back(std::make_unique<UnaryPlus>());
    features.emplace_back(std::make_unique<UnaryMinus>());
    features.emplace_back(std::make_unique<LogicalNot>());
    features.emplace_back(std::make_unique<BitwiseNot>());
    features.emplace_back(std::make_unique<AbsOperator>());
    features.emplace_back(std::make_unique<SquareRootOperator>());
    features.emplace_back(std::make_unique<CubeRootOperator>());
    features.emplace_back(std::make_unique<Plus>());
    features.emplace_back(std::make_unique<Minus>());
    features.emplace_back(std::make_unique<Multiply>());
    features.emplace_back(std::make_unique<Divide>());
    features.emplace_back(std::make_unique<Modulo>());
    features.emplace_back(std::make_unique<Power>());
    features.emplace_back(std::make_unique<LogicalAnd>());
    features.emplace_back(std::make_unique<LogicalOr>());
    features.emplace_back(std::make_unique<Is>());
    features.emplace_back(std::make_unique<BitwiseAnd>());
    features.emplace_back(std::make_unique<BitwiseOr>());
    features.emplace_back(std::make_unique<BitwiseXor>());
    features.emplace_back(std::make_unique<BitwiseShiftLeft>());
    features.emplace_back(std::make_unique<BitwiseShiftRight>());
    features.emplace_back(std::make_unique<TextConcat>());
    features.emplace_back(std::make_unique<ArrayConcat>());
    features.emplace_back(std::make_unique<Abs>());
    features.emplace_back(std::make_unique<Repeat>());
    features.emplace_back(std::make_unique<Replace>());
    features.emplace_back(std::make_unique<LPad>());
    features.emplace_back(std::make_unique<RPad>());
    features.emplace_back(std::make_unique<Format>());
    features.emplace_back(std::make_unique<Trim>());
    features.emplace_back(std::make_unique<DateTrunc>());
    features.emplace_back(std::make_unique<AtTimeZone>());
    features.emplace_back(std::make_unique<Cast>());
    features.emplace_back(std::make_unique<CastNestedArray>());
    features.emplace_back(std::make_unique<CastOperator>());
    features.emplace_back(std::make_unique<StringAgg>());
    features.emplace_back(std::make_unique<ArrayAgg>());
    features.emplace_back(std::make_unique<InList>());
    features.emplace_back(std::make_unique<Coalesce>());
    features.emplace_back(std::make_unique<Greatest>());
    features.emplace_back(std::make_unique<SimpleCase>());
    features.emplace_back(std::make_unique<SearchedCase>());
    features.emplace_back(std::make_unique<SubSelectFrom>());
    features.emplace_back(std::make_unique<SubSelectScalar>());
    features.emplace_back(std::make_unique<SubSelectInExpr>());
    features.emplace_back(std::make_unique<CTE>());
    features.emplace_back(std::make_unique<RecursiveCTE>());
    features.emplace_back(std::make_unique<GroupByList>());
    features.emplace_back(std::make_unique<OrderByList>());
    features.emplace_back(std::make_unique<Aggregation>());
    features.emplace_back(std::make_unique<UnionAll>());
    features.emplace_back(std::make_unique<Union>());
    features.emplace_back(std::make_unique<Except>());
    features.emplace_back(std::make_unique<CrossJoin>());
    features.emplace_back(std::make_unique<NaturalJoin>());
    features.emplace_back(std::make_unique<JoinChain>());
    features.emplace_back(std::make_unique<JoinChainRight>());
    features.emplace_back(std::make_unique<JoinStar>());
    features.emplace_back(std::make_unique<InnerJoin>());
    features.emplace_back(std::make_unique<LeftOuterJoin>());
    features.emplace_back(std::make_unique<RightOuterJoin>());
    features.emplace_back(std::make_unique<FullOuterJoin>());
    features.emplace_back(std::make_unique<LateralJoin>());
    features.emplace_back(std::make_unique<InSemiJoin>());
    features.emplace_back(std::make_unique<ExistsSemiJoin>());
    features.emplace_back(std::make_unique<AnySemiJoin>());
    features.emplace_back(std::make_unique<AllSemiJoin>());
    features.emplace_back(std::make_unique<Unnest>());
    features.emplace_back(std::make_unique<UnnestList>());
    features.emplace_back(std::make_unique<Windows>());
    features.emplace_back(std::make_unique<NamedWindow>());

    return features;
}

}   // namespace tensile
