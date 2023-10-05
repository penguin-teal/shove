#pragma once

#define TOKEN_EOF           -1  //
#define TOKEN_NAMESPACE     -2  // namespace i
#define TOKEN_IMPORT        -3  // import i
#define TOKEN_EXTERN        -4  // extern i
#define TOKEN_TABLE         -5  // table i
#define TOKEN_INTEGER       -6  // <integer literal>
#define TOKEN_FLOAT         -7  // <float literal>
#define TOKEN_IDENTIFIER    -8  // <identifier>
#define TOKEN_POSITIVE      -9  // +a
#define TOKEN_NEGATIVE      -10 // -a
#define TOKEN_BITWISE_NOT   -11 // ~a
#define TOKEN_LOGICAL_NOT   -12 // !a
#define TOKEN_ADD           -13 // a + b
#define TOKEN_SUBTRACT      -14 // a - b
#define TOKEN_MULTIPLY      -15 // a * b
#define TOKEN_DIVIDE        -16 // a / b
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
#define TOKEN_BANG_CMP           -32 // a !cmp b
#define TOKEN_SET                -33 // i = b
#define TOKEN_NEW                -34 // new a
#define TOKEN_TERMINATE          -35 // a;
#define TOKEN_SHIFT_LEFT         -36 // a << b
#define TOKEN_SHIFT_RIGHT        -37 // a >> b