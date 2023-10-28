[![Build Status](https://github.com/penguin-teal/shove/actions/workflows/Build.yml/badge.svg)](https://github.com/penguin-teal/shove/actions/workflows/Build.yml)

# shove

shove is still a work in progres. shove is a langauge and this project is the
shove compiler.

shove is meant to be zero-overhead and similar to C, but makes it much easier
to perform math operations and is simpler to read and write.

## Roadmap

- Lexer ✔
- Functions
    * No parameters ✔
    * Parameters ✔
    * Variadic
    * extern
- Statements 
    * return ✔
    * Variable Declarations ✔
    * if
    * while
    * switch (Statement)
    * for/foreach?...
    * Function Calls
- Expressions
    * Int/Float Literals ✔
    * Variable Reads ✔
    * All Operators
    * Implicit Casting
    * Explicit Hard and Soft Casting
    * switch (Expression)
    * hash
    * Ternary Conditional Operator
- Compile to Object Files ✔
- Custom Operators
- Compile to Executable ✔
- Cross-Platform Support
    * Cross-Compilation ✔
    * Compiler Linux Support ✔
    * Compiler Mac Support
    * Compiler Windows Support
    * Buildable with Clang ✔
    * Buildable with GCC


## Examples

Here is a piece of code that currently compiles:
```shove
namespace add;

i32 main
{
    i32 x = 0xA4;
    return x + 7;
}
```

Here is a possible future piece of code:
```shove
namespace misc;

// Comment

import std::limits as lim;
import std::dictionary;

// Maybe we could extend something or make it like 2D?
// Could be like an object but defines specific math operations
table Color
{
    u32 a;
    u32 r;
    u32 g;
    u32 b;
}

extern void send_colors;

operator ·(Color, Color) order *
Color dot -> Color c1 -> Color c2
{
    return
        c1.a * c2.a +
        c1.r * c2.r +
        c2.b * c2.b +
        c3.g * c2.g
    ;
}

Color darken -> Color col
{
    // No type inference problem since no inheritance
    return new
    {
        .a = col.a,
        .r = col.r / 2,
        .b = col.b / 2,
        .g = col.g / 2
    };
}

i32 main
{
    Color color = new
    {
        .a = lim::U32_MAX,
        .r = 5000,
        .g = 10000,
        .b = 0
    };

    Color darkened = darken -> color;

    send_colors -> color ->> darkened

    dictionary::Dictionary dict = dictionary::create
    {
        dict::def -> this ->> "key" ->>> "value"
    };

    String val = dictionary::get -> dict ->> "key"

    return 0;
}
```