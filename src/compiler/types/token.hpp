enum Token_Type : u32 {
    TOKEN_ADDITION,                 // +
    TOKEN_SUBTRACTION,              // -
    TOKEN_MULTIPLICATION,           // *
    TOKEN_DIVISION,                 // /

    TOKEN_BITWISE_NOT,              // ~
    TOKEN_BITWISE_AND,              // &
    TOKEN_BITWISE_OR,               // |
    TOKEN_BITWISE_XOR,              // ^
    TOKEN_BITWISE_SHIFT_LEFT,       // <<
    TOKEN_BITWISE_SHIFT_RIGHT,      // >>

    TOKEN_EQUALS,                   // ==
    TOKEN_NOT_EQUALS,               // !=
    TOKEN_LESS_THAN_OR_EQUAL_TO,    // <=
    TOKEN_LESS_THAN,                // <
    TOKEN_GREATER_THAN_OR_EQUAL_TO, // >=
    TOKEN_GREATER_THAN,             // >

    TOKEN_NOT,                      // !, not
    TOKEN_AND,                      // &&, and
    TOKEN_OR,                       // ||, or

    TOKEN_TRUE,                     // true
    TOKEN_FALSE,                    // false

    TOKEN_LITERAL_INTEGER,          // binary, decimal, or hexadecimal integer literals
    TOKEN_LITERAL_STRING,           // UTF-8 encoded string literal
    // TOKEN_LITERAL_FLOAT,       // IEEE-754 binary32 literal

    TOKEN_IDENTIFIER,

    TOKEN_LEFT_PARENTHESIS,         // (
    TOKEN_RIGHT_PARENTHESIS,        // )
    TOKEN_LEFT_BRACES,              // {
    TOKEN_RIGHT_BRACES,             // }

    TOKEN_ASSIGNMENT,               // =
    TOKEN_LEFT_BRACKETS,       // [
    TOKEN_RIGHT_BRACKETS,      // ]
    TOKEN_COMMA,                    // ,
    TOKEN_SEMICOLON,                // ;
    TOKEN_RANGE,                    // ..
    TOKEN_DOT,                      // .
    TOKEN_COLON,                    // : // TODO: Rename to TOKEN_IS?
    TOKEN_FOR,      // for
    TOKEN_WHILE,    // while
    TOKEN_IN,       // in
    TOKEN_IF,       // if
    TOKEN_ELSE,     // else
    TOKEN_RETURN,   // return
    TOKEN_AS,       // as

    // TOKEN_VARIADIC,         // ...
    // TOKEN_OPTIONAL,         // ?
    // TOKEN_OPTIONAL_UNWRAP,  // !
    // TOKEN_KEYWORD_BREAK,    // break
    // TOKEN_KEYWORD_CONTINUE, // continue
    // TOKEN_KEYWORD_NULL,     // null
    // TOKEN_KEYWORD_YIELD,    // yield
    // TOKEN_KEYWORD_VOID,     // void


    TOKEN_DIRECTIVE_ENTRY,
    TOKEN_DIRECTIVE_FOREIGN,
    TOKEN_DIRECTIVE_OPAQUE,
    TOKEN_DIRECTIVE_IMPORT,
    TOKEN_DIRECTIVE_LINKER,

    TOKEN_END,
};

struct Token {
    Token_Type type;

    Location location;
};

static void format(Formatter & formatter, Format const & specifier, Token const * argument) {
    switch (argument->type) {
        case TOKEN_ADDITION: {
            format(formatter, "+");
        } break;

        case TOKEN_SUBTRACTION: {
            format(formatter, "-");
        } break;

        case TOKEN_MULTIPLICATION: {
            format(formatter, "*");
        } break;

        case TOKEN_DIVISION: {
            format(formatter, "/");
        } break;

        case TOKEN_BITWISE_NOT: {
            format(formatter, "~");
        } break;

        case TOKEN_BITWISE_AND: {
            format(formatter, "&");
        } break;

        case TOKEN_BITWISE_OR: {
            format(formatter, "|");
        } break;

        case TOKEN_BITWISE_XOR: {
            format(formatter, "^");
        } break;

        case TOKEN_BITWISE_SHIFT_LEFT: {
            format(formatter, "<<");
        } break;

        case TOKEN_BITWISE_SHIFT_RIGHT: {
            format(formatter, ">>");
        } break;

        case TOKEN_EQUALS: {
            format(formatter, "==");
        } break;

        case TOKEN_NOT_EQUALS: {
            format(formatter, "!=");
        } break;

        case TOKEN_LESS_THAN_OR_EQUAL_TO: {
            format(formatter, "<=");
        } break;

        case TOKEN_LESS_THAN: {
            format(formatter, "<");
        } break;

        case TOKEN_GREATER_THAN_OR_EQUAL_TO: {
            format(formatter, ">=");
        } break;

        case TOKEN_GREATER_THAN: {
            format(formatter, ">");
        } break;

        case TOKEN_NOT: {
            format(formatter, "not");
        } break;

        case TOKEN_AND: {
            format(formatter, "and");
        } break;

        case TOKEN_OR: {
            format(formatter, "or");
        } break;

        case TOKEN_TRUE: {
            format(formatter, "true");
        } break;

        case TOKEN_FALSE: {
            format(formatter, "false");
        } break;

        case TOKEN_LITERAL_INTEGER: {
            format(formatter, "'{*}'", (u64)(argument->location.end.pointer - argument->location.begin.pointer + 1), argument->location.begin.pointer);
        } break;

        case TOKEN_IDENTIFIER: {
            format(formatter, "'{*}'", (u64)(argument->location.end.pointer - argument->location.begin.pointer + 1), argument->location.begin.pointer);
        } break;

        case TOKEN_LEFT_PARENTHESIS: {
            format(formatter, "(");
        } break;

        case TOKEN_RIGHT_PARENTHESIS: {
            format(formatter, ")");
        } break;

        case TOKEN_LEFT_BRACES: {
            format(formatter, "\\{");
        } break;

        case TOKEN_RIGHT_BRACES: {
            format(formatter, "}");
        } break;

        case TOKEN_LEFT_BRACKETS: {
            format(formatter, "[");
        } break;

        case TOKEN_RIGHT_BRACKETS: {
            format(formatter, "]");
        } break;

        case TOKEN_COMMA: {
            format(formatter, ",");
        } break;

        // case TOKEN_VARIADIC: {
        //     format(formatter, "...");
        // } break;

        case TOKEN_RANGE: {
            format(formatter, "..");
        } break;

        // case TOKEN_EXCLUSIVE_RANGE: {
        //     format(formatter, "..<");
        // } break;

        case TOKEN_DOT: {
            format(formatter, ".");
        } break;

        case TOKEN_SEMICOLON: {
            format(formatter, ";");
        } break;

        case TOKEN_ASSIGNMENT: {
            format(formatter, "=");
        } break;

        // case TOKEN_OPTIONAL: {
        //     format(formatter, "?");
        // } break;

        // case TOKEN_OPTIONAL_UNWRAP: {
        //     format(formatter, "!");
        // } break;

        case TOKEN_COLON: {
            format(formatter, ":");
        } break;

        case TOKEN_IF: {
            format(formatter, "if");
        } break;

        case TOKEN_ELSE: {
            format(formatter, "else");
        } break;

        case TOKEN_FOR: {
            format(formatter, "for");
        } break;

        case TOKEN_WHILE: {
            format(formatter, "while");
        } break;

        case TOKEN_RETURN: {
            format(formatter, "return");
        } break;

        case TOKEN_AS: {
            format(formatter, "as");
        } break;

        // case TOKEN_KEYWORD_YIELD: {
        //     format(formatter, "yield");
        // } break;

        // case TOKEN_KEYWORD_NULL: {
        //     format(formatter, "null");
        // } break;

        // case TOKEN_KEYWORD_BREAK: {
        //     format(formatter, "break");
        // } break;

        // case TOKEN_KEYWORD_CONTINUE: {
        //     format(formatter, "continue");
        // } break;

        // case TOKEN_KEYWORD_VOID: {
        //     format(formatter, "void");
        // } break;

        case TOKEN_IN: {
            format(formatter, "in");
        } break;

        // case TOKEN_KEYWORD_STRUCT: {
        //     format(formatter, "struct");
        // } break;

        case TOKEN_LITERAL_STRING: {
            // TODO: Escape
            format(formatter, "\"{*}\"", (u64)(argument->location.end.pointer - argument->location.begin.pointer - 1), argument->location.begin.pointer + 1); // TODO: Escaping!
        } break;

        // case TOKEN_LITERAL_FLOAT: {
        //     format(formatter, "<unimplemented>");
        // } break;

        case TOKEN_DIRECTIVE_ENTRY: {
            format(formatter, "#entry");
        } break;

        case TOKEN_DIRECTIVE_FOREIGN: {
            format(formatter, "#foreign");
        } break;

        case TOKEN_DIRECTIVE_OPAQUE: {
            format(formatter, "#opaque");
        } break;

        case TOKEN_DIRECTIVE_IMPORT: {
            format(formatter, "#import");
        } break;

        case TOKEN_DIRECTIVE_LINKER: {
            format(formatter, "#linker");
        } break;

        case TOKEN_END: {
            format(formatter, "END");
        } break;
    }
}
