#pragma once
#include <error.h>
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

#define TOKEN_EOF           -1  //
#define TOKEN_NAMESPACE     -2  // namespace i
#define TOKEN_IMPORT        -3  // import i
#define TOKEN_EXTERN        -4  // extern i
#define TOKEN_TABLE         -5  // table i
#define TOKEN_INTEGER       -6  // <integer literal>
#define TOKEN_FLOAT         -7  // <float literal>
#define TOKEN_IDENTIFIER    -8  // <identifier>
#define TOKEN_RETURN             -9  // return a

#define TOKEN_BITWISE_NOT        -11 // ~a
#define TOKEN_BANG               -12 // !a
#define TOKEN_PLUS               -13 // a + b OR +a
#define TOKEN_MINUS              -14 // a - b OR -a
#define TOKEN_STAR               -15 // a * b
#define TOKEN_SLASH              -16 // a / b
#define TOKEN_MODULO             -17 // a % b
#define TOKEN_BITWISE_AND        -18 // a & b
#define TOKEN_BITWISE_OR         -19 // a | b
#define TOKEN_BITWISE_XOR        -20 // a ^ b
#define TOKEN_LOGICAL_AND        -21 // a && b
#define TOKEN_LOGICAL_OR         -22 // a || b
#define TOKEN_GT                 -23 // a > b
#define TOKEN_LT                 -24 // a < b
#define TOKEN_GTE                -25 // a >= b
#define TOKEN_LTE                -26 // a <= b
#define TOKEN_POW                -27 // a %% b
#define TOKEN_SHOVE              -28 // -> or ->> or ->>> etc.
#define TOKEN_INTO_NAMESPACE     -29 // ::
#define TOKEN_HASH               -30 // hash a
#define TOKEN_CMP                -31 // a cmp b
#define TOKEN_EQUAL              -33 // i = b
#define TOKEN_NEW                -34 // new a
#define TOKEN_TERMINATE          -35 // a;
#define TOKEN_SHIFT_LEFT         -36 // a << b
#define TOKEN_SHIFT_RIGHT        -37 // a >> b
#define TOKEN_BLOCK_OPEN         -38 // {
#define TOKEN_BLOCK_CLOSE        -39 // }
#define TOKEN_THIS               -40 // this
