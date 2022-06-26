struct Lexer {
    Context & context;

    utf8 const * file = NULL;
    utf8 const * source = NULL;

    Location location;
    Array<Token> tokens;
};

static void advance(Lexer & lexer) {
    if (*lexer.location.end.pointer == 0x0A) {
        lexer.location.end.line += 1;
        lexer.location.end.column = 1;
        lexer.location.end.pointer += utf8_length(lexer.location.end.pointer);
    } else {
        lexer.location.end.column += 1;
        lexer.location.end.pointer += utf8_length(lexer.location.end.pointer);
    }
}

static void advance(Lexer & lexer, utf8 const * value) {
    while (*value) {
        advance(lexer);
        value += 1;
    }
}

static b32 peek(Lexer & lexer) {
    return *lexer.location.end.pointer == '\0';
}

static b32 peek(Lexer & lexer, utf8 const * value) {
    if (utf8_compare(lexer.location.end.pointer, value, utf8_length_string(value))) {
        return TRUE;
    }

    return FALSE;
}

static b32 match(Lexer & lexer, utf8 const * value) {
    if (peek(lexer, value)) {
        advance(lexer, value);

        return TRUE;
    }

    return FALSE;
}

static b32 match_not(Lexer & lexer, utf8 const * value) {
    if (!peek(lexer, value)) {
        advance(lexer, value);

        return TRUE;
    }

    return FALSE;
}

template<class ... T>
static b32 match_any(Lexer & lexer, T ... expected) {
    return (... || match(lexer, expected));
}

static void token(Lexer & lexer, Token_Type const type) {
    Location location = lexer.location;

    // TODO: This is a bit hacky, we need to go one step back...
    //       I am unsure why we need to do this though..?
    if (location.end.pointer > location.source) {
        // TODO: This crashes the program if we try to lex an empty string!
        if (*utf8_previous(location.source, location.end.pointer) == '\n') {
            // If the previos character was a new line, we need to step backwards
            ASSERT(location.end.line > 0);
            location.end.line -= 1;
            location.end.column = utf8_range_length(line_from_begin(location.source, location.end.pointer), location.end.pointer);
        }

        location.end.pointer -= 1;
    }

    push(lexer.tokens, Token{
        .type = type,

        .location = location
    });
}

// TODO: Since chances are this is an ASCII character in the range 65-90 or 97-122, we
//       should optimize for that case and only fall back to the (way) more expensive
//       UTF-8 check.
//
//       Checking whether an ASCII character is in A-Z or a-z should be able to be done
//       with one mask and two comparisons since uppercase and lowercase are 32 off each
//       other:
//
//         Mask off the "32" bit and check if the value is between 65 and 90.

// These two generates the _exact_ same code on x64-64 clang 16.0.0 with -O2
//     int isalpha(unsigned char c) {
//         return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
//     }

//     int isalpha2(unsigned char c) {
//         unsigned char c2 = c & ~0b100000;
//         return (c2 >= 'A' && c2 <= 'Z');
//     }

static b32 is_identifier_head(utf8 const * string) {
    // TODO: These is_ functions should take the Unicode codepoint directly... Here
    //       we convert between UTF-8 and Unicode 6 times in the worst case.
    u32 const U = utf8_to_utf32(string);
    return U == 0x005F || is_Ll(string) || is_Lm(string) || is_Lo(string) || is_Lt(string) || is_Lu(string);
}

static b32 is_identifier_body(utf8 const * string) {
    return is_identifier_head(string) || is_Nd(string);
}

static b32 match_identifier_head(Lexer * const lexer) {
    if (is_identifier_head(lexer->location.end.pointer)) {
        advance(*lexer);

        return TRUE;
    }

    return FALSE;
}

static b32 match_identifier_body(Lexer * const lexer) {
    if (is_identifier_body(lexer->location.end.pointer)) {
        advance(*lexer);

        return TRUE;
    }

    return FALSE;
}

struct Lex_Result {
    b32 success;
    Array<Token> tokens;
};

Lex_Result lex(Context & context, utf8 const * file, utf8 const * source) {
    Lexer lexer = {
        .context = context,

        .file = file,
        .source = source,

        .location = {
            .file = file,
            .source = source,

            .begin = {
                .line = 1,
                .column = 1,
                .pointer = source
            },

            .end = {
                .line = 1,
                .column = 1,
                .pointer = source
            }
        },
    };

    while (*lexer.location.end.pointer) {
        lexer.location.begin.line = lexer.location.end.line;
        lexer.location.begin.column = lexer.location.end.column;
        lexer.location.begin.pointer = lexer.location.end.pointer;

        if (match_any(lexer, "\u0009", "\u000a", "\u000d", "\u0020")) {
        } else if (match(lexer, "//")) {
            while (match_not(lexer, "\n"));
        } else if (match(lexer, "+")) {
            if (match_any(lexer, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9")) {
                while (match_any(lexer, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"));
                token(lexer, TOKEN_LITERAL_INTEGER);
            } else {
                token(lexer, TOKEN_ADDITION);
            }
        } else if (match(lexer, "-")) {
            if (match_any(lexer, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9")) {
                while (match_any(lexer, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"));
                token(lexer, TOKEN_LITERAL_INTEGER);
            } else {
                token(lexer, TOKEN_SUBTRACTION);
            }
        } else if (match(lexer, "*")) {
            token(lexer, TOKEN_MULTIPLICATION);
        } else if (match(lexer, "/")) {
            token(lexer, TOKEN_DIVISION);
        } else if (match(lexer, "~")) {
            token(lexer, TOKEN_BITWISE_NOT);
        } else if (match(lexer, "&")) {
            token(lexer, TOKEN_BITWISE_AND);
        } else if (match(lexer, "|")) {
            token(lexer, TOKEN_BITWISE_OR);
        } else if (match(lexer, "^")) {
            token(lexer, TOKEN_BITWISE_XOR);
        } else if (match(lexer, "<<")) {
            token(lexer, TOKEN_BITWISE_SHIFT_LEFT);
        } else if (match(lexer, ">>")) {
            token(lexer, TOKEN_BITWISE_SHIFT_RIGHT);
        } else if (match(lexer, "==")) {
            token(lexer, TOKEN_EQUALS);
        } else if (match(lexer, "!=")) {
            token(lexer, TOKEN_NOT_EQUALS);
        } else if (match(lexer, "<=")) {
            token(lexer, TOKEN_LESS_THAN_OR_EQUAL_TO);
        } else if (match(lexer, "<")) {
            token(lexer, TOKEN_LESS_THAN);
        } else if (match(lexer, ">=")) {
            token(lexer, TOKEN_GREATER_THAN_OR_EQUAL_TO);
        } else if (match(lexer, ">")) {
            token(lexer, TOKEN_GREATER_THAN);
        } else if (match(lexer, "!") || match(lexer, "not")) {
            token(lexer, TOKEN_NOT);
        } else if (match(lexer, "&&") || match(lexer, "and")) {
            token(lexer, TOKEN_AND);
        } else if (match(lexer, "||") || match(lexer, "or")) {
            token(lexer, TOKEN_OR);
        } else if (match(lexer, "true")) {
            token(lexer, TOKEN_TRUE);
        } else if (match(lexer, "false")) {
            token(lexer, TOKEN_FALSE);
        } else if (match(lexer, "(")) {
            token(lexer, TOKEN_LEFT_PARENTHESIS);
        } else if (match(lexer, ")")) {
            token(lexer, TOKEN_RIGHT_PARENTHESIS);
        // } else if (match(lexer, "!")) {
        //     token(lexer, TOKEN_OPTIONAL_UNWRAP);
        } else if (match(lexer, "\"")) {
            while (match_not(lexer, "\""));

            if (match(lexer, "\"")) {
                token(lexer, TOKEN_LITERAL_STRING);
            } else {
                report(lexer.context, ERROR_UNTERMINATED_STRING_LITERAL, lexer.location, "unterminated string literal");
                return Lex_Result{
                    .success = FALSE,
                };
            }
        // } else if (match(lexer, "$")) {
        //     // Polymorphism directive
        //     ASSERT_NOT_REACHED();
        } else if (match(lexer, ",")) {
            token(lexer, TOKEN_COMMA);
        } else if (match(lexer, "..")) {
            token(lexer, TOKEN_RANGE);
        } else if (match(lexer, ".")) {
            token(lexer, TOKEN_DOT);
        // } else if (match(lexer, "...")) {
        //     token(lexer, TOKEN_VARIADIC);
        // }
        } else if (match(lexer, ":")) {
            token(lexer, TOKEN_COLON);
        } else if (match(lexer, ";")) {
            token(lexer, TOKEN_SEMICOLON);
        } else if (match(lexer, "=")) {
            token(lexer, TOKEN_ASSIGNMENT);
        // } else if (match(lexer, "?")) {
        //     token(lexer, TOKEN_OPTIONAL);
        } else if (match(lexer, "[")) {
            token(lexer, TOKEN_LEFT_BRACKETS);
        } else if (match(lexer, "]")) {
            token(lexer, TOKEN_RIGHT_BRACKETS);
        } else if (match(lexer, "{")) {
            token(lexer, TOKEN_LEFT_BRACES);
        } else if (match(lexer, "}")) {
            token(lexer, TOKEN_RIGHT_BRACES);
        } else if (match_identifier_head(&lexer)) {
            while (match_identifier_body(&lexer));

            utf8 const * identifier = utf8_copy(lexer.location.begin.pointer, lexer.location.end.pointer);

            if (utf8_compare(identifier, "if")) {
                token(lexer, TOKEN_IF);
            } else if (utf8_compare(identifier, "else")) {
                token(lexer, TOKEN_ELSE);
            } else if (utf8_compare(identifier, "for")) {
                token(lexer, TOKEN_FOR);
            } else if (utf8_compare(identifier, "while")) {
                token(lexer, TOKEN_WHILE);
            } else if (utf8_compare(identifier, "in")) {
                token(lexer, TOKEN_IN);
            } else if (utf8_compare(identifier, "return")) {
                token(lexer, TOKEN_RETURN);
            } else if (utf8_compare(identifier, "as")) {
                token(lexer, TOKEN_AS);
            } else {
                token(lexer, TOKEN_IDENTIFIER);
            }
        } else if (match(lexer, "0b")) {
            while (match_any(lexer, "0", "1"));
            token(lexer, TOKEN_LITERAL_INTEGER);
        } else if (match(lexer, "0x")) {
            while (match_any(lexer, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "A", "b", "B", "c", "C", "d", "D", "e", "E", "f", "F"));
            token(lexer, TOKEN_LITERAL_INTEGER);
        } else if (match_any(lexer, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9")) {
            while (match_any(lexer, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"));
            token(lexer, TOKEN_LITERAL_INTEGER);
        } else if (match(lexer, "#")) {
            while (match_identifier_body(&lexer));

            utf8 const * directive = utf8_copy(utf8_next(lexer.location.begin.pointer), lexer.location.end.pointer);
            if (utf8_compare(directive, "entry")) {
                token(lexer, TOKEN_DIRECTIVE_ENTRY);
            } else if (utf8_compare(directive, "foreign")) {
                token(lexer, TOKEN_DIRECTIVE_FOREIGN);
            } else if (utf8_compare(directive, "opaque")) {
                token(lexer, TOKEN_DIRECTIVE_OPAQUE);
            } else if (utf8_compare(directive, "import")) {
                token(lexer, TOKEN_DIRECTIVE_IMPORT);
            } else if (utf8_compare(directive, "linker")) {
                token(lexer, TOKEN_DIRECTIVE_LINKER);
            } else if (utf8_compare(directive, "")) {
                report(lexer.context, ERROR_EMPTY_DIRECTIVE, lexer.location, "empty directive");
                return Lex_Result{
                    .success = FALSE,
                };
            } else {
                report(lexer.context, ERROR_UNKNOWN_DIRECTIVE, lexer.location, "unkown directive '{}'", directive);
                return Lex_Result{
                    .success = FALSE,
                };
            }
        } else {
            report(lexer.context, ERROR_UNKNOWN_CHARACTER, lexer.location, "unknown character '{*}' (U+{04x})", (u64)utf8_length(lexer.location.end.pointer), lexer.location.end.pointer, utf8_to_utf32(lexer.location.end.pointer));
            return Lex_Result{
                .success = FALSE,
            };
        }
    }

    token(lexer, TOKEN_END);

    return Lex_Result{
        .success = TRUE,
        .tokens = lexer.tokens,
    };
}
