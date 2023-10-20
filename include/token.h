#pragma once
#include <shvError.h>
#include <stdint.h>

/**
 * Represents a token and associated information.
 */
struct Token
{
    /**
     * The int ID of this token. Should be a TOKEN_* macro.
     */
    int32_t symbol;
    /**
     * Holds extra metadata about the token.
     * Might be a binary representation of a `TOKEN_FLOAT`
     * or `TOKEN_INTEGER` literal, or the offset in the
     * string table of a `TOKEN_IDENTIFIER`.
     * Should just be `0` if irrelevant.
     */
    uint64_t value;

    struct FilePos fpos;
};

enum ShvToken
{
    TOKEN_EOF             =   -1,  //
    TOKEN_NAMESPACE       =   -2,  // namespace i
    TOKEN_IMPORT          =   -3,  // import i
    TOKEN_EXTERN          =   -4,  // extern i
    TOKEN_TABLE           =   -5,  // table i
    TOKEN_INTEGER         =   -6,  // <integer literal>
    TOKEN_FLOAT           =   -7,  // <float literal>
    TOKEN_IDENTIFIER      =   -8,  // <identifier>
    TOKEN_RETURN          =   -9,  // return a

    TOKEN_BITWISE_NOT     =   -11, // ~a
    TOKEN_BANG            =   -12, // !a
    TOKEN_PLUS            =   -13, // a + b OR +a
    TOKEN_MINUS           =   -14, // a - b OR -a
    TOKEN_STAR            =   -15, // a * b
    TOKEN_SLASH           =   -16, // a / b
    TOKEN_MODULO          =   -17, // a % b
    TOKEN_BITWISE_AND     =   -18, // a & b
    TOKEN_BITWISE_OR      =   -19, // a | b
    TOKEN_BITWISE_XOR     =   -20, // a ^ b
    TOKEN_LOGICAL_AND     =   -21, // a && b
    TOKEN_LOGICAL_OR      =   -22, // a || b
    TOKEN_GT              =   -23, // a > b
    TOKEN_LT              =   -24, // a < b
    TOKEN_GTE             =   -25, // a >= b
    TOKEN_LTE             =   -26, // a <= b
    TOKEN_POW             =   -27, // a %% b
    TOKEN_SHOVE           =   -28, // -> or ->> or ->>> etc.
    TOKEN_INTO_NAMESPACE  =   -29, // ::
    TOKEN_HASH            =   -30, // hash a
    TOKEN_CMP             =   -31, // a cmp b
    TOKEN_EQUAL           =   -33, // i = b
    TOKEN_NEW             =   -34, // new a
    TOKEN_TERMINATE       =   -35, // a;
    TOKEN_SHIFT_LEFT      =   -36, // a << b
    TOKEN_SHIFT_RIGHT     =   -37, // a >> b
    TOKEN_BLOCK_OPEN      =   -38, // {
    TOKEN_BLOCK_CLOSE     =   -39, // }
    TOKEN_THIS            =   -40, // this
    TOKEN_VOID            =   -41, // void
    TOKEN_PAREN_OPEN      =   -42, // (
    TOKEN_PAREN_CLOSE     =   -43, // )
};