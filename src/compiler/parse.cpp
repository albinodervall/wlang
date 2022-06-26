struct Parser {
    Context & context;

    utf8 const * label = NULL;
    utf8 const * source = NULL;

    Array<Token> tokens;
    Token * previous;
    u64 current_index;
    Token * current;
};

Location here(Parser & parser) {
    ASSERT(parser.current != NULL);
    return parser.current->location;
}

b32 parser_peek(Parser & parser, Token_Type type) {
    return (parser.current->type == type);
}

template<class ... T>
b32 parser_peek_any(Parser & parser, T ... expected) {
    return (... || parser_peek(parser, expected));
}

b32 parser_peek_next(Parser & parser, Token_Type type) {
    ASSERT(parser.current->type != TOKEN_END);
    return ((parser.tokens[parser.current_index + 1]).type == type);
}

void parser_advance(Parser & parser) {
    if (parser.current->type != TOKEN_END) {
        parser.previous = parser.current;
        parser.current_index += 1;
        parser.current = &parser.tokens[parser.current_index];
    }
}

b32 parser_match(Parser & parser, Token_Type type) {
    if (parser_peek(parser, type)) {
        parser_advance(parser);

        return TRUE;
    }

    return FALSE;
}

template<class ... T>
b32 parser_match_any(Parser & parser, T ... expected) {
    return (... || parser_match(parser, expected));
}

struct Function_Signature {
    b32 success = FALSE;
    Array<Function_Parameter> parameters;
    Expression * return_type = NULL;
};

Expression * parse_expression_0(Parser & parser);
Expression * parse_expression(Parser & parser);
Expression * parse_statement_or_expression(Parser & parser);
Function_Signature parse_function_signature(Parser & parser);

Expression * parse_type(Parser & parser) {
    Location const & begin = parser.current->location;

    if (parser_match(parser, TOKEN_BITWISE_AND)) {
        Expression * expression = parse_type(parser);
        if (expression == NULL) {
            return NULL;
        }

        Location const & end = parser.previous->location;
        return make_expression_type_pointer(span(begin, end), expression);
    } else if (parser_match(parser, TOKEN_LEFT_BRACKETS)) {
        Expression * length_expression = NULL;
        if (!parser_peek(parser, TOKEN_RIGHT_BRACKETS)) {
            length_expression = parse_expression_0(parser);
        }

        if (!parser_match(parser, TOKEN_RIGHT_BRACKETS)) {
            report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected ']', but got '{}'", parser.current);
            return NULL;
        }

        Expression * type_expression = parse_type(parser);
        if (type_expression == NULL) {
            return NULL;
        }

        Location const & end = parser.previous->location;
        return make_expression_type_array(span(begin, end), type_expression, length_expression);
    } else if (parser_match(parser, TOKEN_IDENTIFIER)) {
        Location const & end = parser.previous->location;
        return make_expression_identifier(span(begin, end), parser.previous);
    } else if (parser_peek(parser, TOKEN_LEFT_PARENTHESIS)) {
        Function_Signature signature = parse_function_signature(parser);
        if (!signature.success) {
            return NULL;
        }

        Location const & end = parser.previous->location;
        return make_expression_type_function(span(begin, end), signature.parameters, signature.return_type);
    } else {
        report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected identifier, but got '{}'", parser.current);
        return NULL;
    }
}

Expression * parse_block(Parser & parser) {
    if (parser_match(parser, TOKEN_LEFT_BRACES)) {
        Location const & begin = parser.previous->location;

        Array<Expression *> expressions;
        while (!parser_peek(parser, TOKEN_RIGHT_BRACES)) {
            if (expressions.number_of_elements > 0 && !(expressions[expressions.number_of_elements - 1]->flags & EXPRESSION_FLAGS_STATEMENT)) {
                report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expressions may only appear as the last expression in a block");
                return NULL;
            }

            Expression * expression = parse_statement_or_expression(parser);
            if (expression == NULL) {
                return NULL;
            }

            push(expressions, expression);
        }

        if (!parser_match(parser, TOKEN_RIGHT_BRACES)) {
            report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected '}', but got '{}'", parser.current);
            return NULL;
        }

        Location const & end = parser.previous->location;
        return make_expression_block(span(begin, end), expressions);
    } else {
        report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected '\\{', but got '{}'", parser.current);
        return NULL;
    }

    ASSERT_NOT_REACHED();
}

Expression * parse_if_expression(Parser & parser) {
    Location const & begin = parser.current->location;

    if (!parser_match(parser, TOKEN_IF)) {
        report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected if, but got '{}'", parser.current);
        return NULL;
    }

    Expression * condition = parse_expression_0(parser);
    if (condition == NULL) {
        return NULL;
    }

    if (!parser_peek(parser, TOKEN_LEFT_BRACES)) {
        report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected '\\{', but got '{}'", parser.current);
        return NULL;
    }

    Expression * then_expression = parse_block(parser);
    if (then_expression == NULL) {
        return NULL;
    }

    Expression * else_expression = NULL;
    if (parser_match(parser, TOKEN_ELSE)) {
        if (parser_peek(parser, TOKEN_IF)) {
            else_expression = parse_if_expression(parser);
        } else if (parser_peek(parser, TOKEN_LEFT_BRACES)) {
            else_expression = parse_block(parser);
        } else {
            report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected 'if' or '{', but got '{}'", parser.current);
            return NULL;
        }
    }

    Location const & end = parser.previous->location;
    return make_expression_if(span(begin, end), condition, then_expression, else_expression);
}

// for x in e { ... }
// for x, y in e { ... }
Expression * parse_for_expression(Parser & parser) {
    Location const & begin = parser.current->location;

    if (!parser_match(parser, TOKEN_FOR)) {
        report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected for, but got '{}'", parser.current);
        return NULL;
    }

    Expression * index = NULL;
    Expression * index_type = NULL;

    if (!parser_match(parser, TOKEN_IDENTIFIER)) {
        report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected identifier, but got '{}'", parser.current);
        return NULL;
    }

    Expression * value = make_expression_identifier(parser.previous->location, parser.previous);

    Expression * value_type = NULL;
    if (parser_match(parser, TOKEN_COLON)) {
        value_type = parse_type(parser);
        if (value_type == NULL) {
            return NULL;
        }
    }

    if (parser_match(parser, TOKEN_COMMA)) {
        index = value;
        index_type = value_type;

        if (!parser_match(parser, TOKEN_IDENTIFIER)) {
            report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected identifier, but got '{}'", parser.current);
            return NULL;
        }

        value = make_expression_identifier(parser.previous->location, parser.previous);

        value_type = NULL;
        if (parser_match(parser, TOKEN_COLON)) {
            value_type = parse_type(parser);
            if (value_type == NULL) {
                return NULL;
            }
        }
    }

    if (!parser_match(parser, TOKEN_IN)) {
        report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected 'in', but got '{}'", parser.current);
        return NULL;
    }

    Expression * expression = parse_expression_0(parser);
    if (expression == NULL) {
        return NULL;
    }

    Expression * block = parse_block(parser);
    if (block == NULL) {
        return NULL;
    }

    Location const & end = parser.previous->location;

    return make_expression_for(span(begin, end), index, index_type, value, value_type, expression, block);
}

Expression * parse_while_expression(Parser & parser) {
    Location const & begin = parser.current->location;

    if (!parser_match(parser, TOKEN_WHILE)) {
        report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected while, but got '{}'", parser.current);
        return NULL;
    }

    Expression * condition = parse_expression_0(parser);
    if (condition == NULL) {
        return NULL;
    }

    Expression * block = parse_block(parser);
    if (block == NULL) {
        return NULL;
    }

    Location const & end = parser.previous->location;
    return make_expression_while(span(begin, end), condition, block);
}

Expression * parse_return_expression(Parser & parser) {
    Location const & begin = parser.current->location;

    if (!parser_match(parser, TOKEN_RETURN)) {
        report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected return, but got '{}'", parser.current);
        return NULL;
    }

    Expression * expression = NULL;

    if (!parser_peek(parser, TOKEN_SEMICOLON)) {
        expression = parse_expression_0(parser);
        if (expression == NULL) {
            return NULL;
        }
    }

    Location const & end = parser.previous->location;
    return make_expression_return(span(begin, end), expression);
}

Expression * parse_array(Parser & parser) {
    Location const & begin = parser.current->location;

    if (!parser_match(parser, TOKEN_LEFT_BRACKETS)) {
        report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected '[', but got '{}'", parser.current);
        return NULL;
    }

    Array<Expression *> expressions;
    while (!parser_peek(parser, TOKEN_RIGHT_BRACKETS)) {
        Expression * expression = parse_expression_0(parser);
        if (expression == NULL) {
            return NULL;
        }

        push(expressions, expression);

        if (!parser_match(parser, TOKEN_COMMA)) {
            report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected ',', but got '{}'", parser.current);
            return NULL;
        }
    }

    if (!parser_match(parser, TOKEN_RIGHT_BRACKETS)) {
        report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected ']', but got '{}'", parser.current);
        return NULL;
    }

    Location const & end = parser.previous->location;
    return make_expression_array(span(begin, end), expressions);
}

Expression * parse_expression_primary(Parser & parser) {
    if (parser_match(parser, TOKEN_LITERAL_INTEGER)) {
        u32 negative = 0;
        u32 base = 0;
        u64 value = 0;

        {
            Token * token = parser.previous;
            u64 length = token->location.end.pointer - token->location.begin.pointer + 1;
            utf8 const * string = utf8_copy(token->location.begin.pointer, length);

            if (length > 2) {
                if (token->location.begin.pointer[0] == '0' && token->location.begin.pointer[1] == 'b') {
                    base = 2;
                    if (!utf8_to_u64(string + 2, base, value)) {
                        report(parser.context, ERROR_INTEGER_LITERAL_TOO_LARGE, token->location, "integer literal too large");
                        return NULL;
                    }
                } else if (token->location.begin.pointer[0] == '0' && token->location.begin.pointer[1] == 'x') {
                    base = 16;
                    if (!utf8_to_u64(string + 2, base, value)) {
                        report(parser.context, ERROR_INTEGER_LITERAL_TOO_LARGE, token->location, "integer literal too large");
                        return NULL;
                    }
                } else {
                    if (token->location.begin.pointer[0] == '-') {
                        negative = 1;
                    }

                    base = 10;
                    if (!utf8_to_u64(string + negative, base, value)) {
                        report(parser.context, ERROR_INTEGER_LITERAL_TOO_LARGE, token->location, "integer literal too large");
                        return NULL;
                    }
                }
            } else {
                u64 skip = 0;
                if (token->location.begin.pointer[0] == '-') {
                    negative = 1;
                    skip = 1;
                } else if (token->location.begin.pointer[0] == '+') {
                    skip = 1;
                }

                base = 10;
                if (!utf8_to_u64(string + skip, base, value)) {
                    report(parser.context, ERROR_INTEGER_LITERAL_TOO_LARGE, token->location, "integer literal too large");
                    return NULL;
                }
            }
        }

        return make_expression_integer(parser.previous->location, parser.previous, negative, base, value);
    } else if (parser_match(parser, TOKEN_LITERAL_STRING)) {
        return make_expression_string(parser.previous->location, parser.previous);
    } else if (parser_match(parser, TOKEN_IDENTIFIER)) {
        return make_expression_identifier(parser.previous->location, parser.previous);
    } else if (parser_match(parser, TOKEN_TRUE)) {
        return make_expression_true(parser.previous->location, parser.previous);
    } else if (parser_match(parser, TOKEN_FALSE)) {
        return make_expression_false(parser.previous->location, parser.previous);
    } else if (parser_peek(parser, TOKEN_IF)) {
        return parse_if_expression(parser);
    } else if (parser_peek(parser, TOKEN_FOR)) {
        return parse_for_expression(parser);
    } else if (parser_peek(parser, TOKEN_WHILE)) {
        return parse_while_expression(parser);
    } else if (parser_peek(parser, TOKEN_RETURN)) {
        return parse_return_expression(parser);
    } else if (parser_match(parser, TOKEN_LEFT_PARENTHESIS)) {
        Expression * expression = parse_expression_0(parser);
        if (!parser_match(parser, TOKEN_RIGHT_PARENTHESIS)) {
            report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected ')', but got '{}'", parser.current);
            return NULL;
        }

        return expression;
    } else if (parser_peek(parser, TOKEN_LEFT_BRACES)) {
        return parse_block(parser);
    } else if (parser_peek(parser, TOKEN_LEFT_BRACKETS)) {
        return parse_array(parser);
    } else {
        report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected integer literal, string literal, identifier, or '(', but got '{}'", parser.current);
        return NULL;
    }
}

Expression * parse_invocation(Parser & parser) {
    Expression * expression = parse_expression_primary(parser);
    if (expression == NULL) {
        return NULL;
    }

    while (parser_peek_any(parser, TOKEN_LEFT_PARENTHESIS, TOKEN_DOT, TOKEN_LEFT_BRACKETS)) {
        if (parser_peek(parser, TOKEN_LEFT_PARENTHESIS)) {
            Location const & begin = parser.current->location;

            if (!parser_match(parser, TOKEN_LEFT_PARENTHESIS)) {
                report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected '(', but got '{}'", parser.current);
                return NULL;
            }

            Array<Argument> arguments;

            if (!parser_peek(parser, TOKEN_RIGHT_PARENTHESIS)) {
                do {
                    Expression * name = NULL;

                    if (parser_peek(parser, TOKEN_IDENTIFIER) && parser_peek_next(parser, TOKEN_ASSIGNMENT)) {
                        parser_match(parser, TOKEN_IDENTIFIER);
                        name = make_expression_identifier(parser.previous->location, parser.previous);

                        parser_match(parser, TOKEN_ASSIGNMENT);
                    }

                    Expression * argument = parse_expression_0(parser);
                    if (argument == NULL) {
                        return NULL;
                    }

                    push(arguments, Argument{
                        .name = name,
                        .value = argument
                    });
                } while (parser_match(parser, TOKEN_COMMA));
            }

            if (!parser_match(parser, TOKEN_RIGHT_PARENTHESIS)) {
                report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected ')', but got '{}'", parser.current);
                return NULL;
            }

            Location const & end = parser.previous->location;
            expression = make_expression_invocation(span(begin, end), expression, arguments);
        } else if (parser_peek(parser, TOKEN_DOT)) {
            Location const & begin = parser.current->location;

            if (!parser_match(parser, TOKEN_DOT)) {
                report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected '.', but got '{}'", parser.current);
                return NULL;
            }

            if (!parser_match(parser, TOKEN_IDENTIFIER)) {
                report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected identifier, but got '{}'", parser.current);
                return NULL;
            }

            Location const & end = parser.previous->location;
            expression = make_expression_field(span(begin, end), expression, make_expression_identifier(parser.previous->location, parser.previous));
        } else if (parser_peek(parser, TOKEN_LEFT_BRACKETS)) {
            Location const & begin = parser.current->location;

            if (!parser_match(parser, TOKEN_LEFT_BRACKETS)) {
                report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected '[', but got '{}'", parser.current);
                return NULL;
            }

            Expression * index = parse_expression_0(parser);
            if (index == NULL) {
                return NULL;
            }

            if (!parser_match(parser, TOKEN_RIGHT_BRACKETS)) {
                report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected ']', but got '{}'", parser.current);
                return NULL;
            }

            Location const & end = parser.previous->location;
            expression = make_expression_index(span(begin, end), expression, index);
        }
    }

    return expression;
}

Expression * parse_expression_unary(Parser & parser) {
    if (parser_match_any(parser, TOKEN_SUBTRACTION, TOKEN_BITWISE_NOT, TOKEN_NOT, TOKEN_BITWISE_AND, TOKEN_MULTIPLICATION)) {
        Location const & begin = parser.previous->location;

        Token_Type type = parser.previous->type;

        Expression * expression = parse_expression_unary(parser);
        if (expression == NULL) {
            return NULL;
        }

        Location const & end = parser.previous->location;
        if (type == TOKEN_SUBTRACTION) {
            expression = make_expression_unary(span(begin, end), EXPRESSION_UNARY_MINUS, expression);
        } else if (type == TOKEN_BITWISE_NOT) {
            expression = make_expression_unary(span(begin, end), EXPRESSION_UNARY_BITWISE_NOT, expression);
        } else if (type == TOKEN_NOT) {
            expression = make_expression_unary(span(begin, end), EXPRESSION_UNARY_BOOLEAN_NOT, expression);
        } else if (type == TOKEN_BITWISE_AND) {
            expression = make_expression_unary(span(begin, end), EXPRESSION_ADDRESS_OF, expression);
        } else if (type == TOKEN_MULTIPLICATION) {
            expression = make_expression_unary(span(begin, end), EXPRESSION_DEREFERENCE, expression);
        } else {
            ASSERT_NOT_REACHED();
        }

        return expression;
    } else {
        return parse_invocation(parser);
    }
}

Expression * parse_declaration(Parser & parser);

Expression * parse_variable_declaration(Parser & parser, Expression * declaration_name) {
    Location const & begin = parser.current->location;

    Expression * declaration_type = parse_type(parser);
    if (declaration_type == NULL) {
        return NULL;
    }

    Expression * right = NULL;
    if (parser_match(parser, TOKEN_ASSIGNMENT)) {
        right = parse_expression_0(parser);
        if (right == NULL) {
            return NULL;
        }
    }

    Location const & end = parser.previous->location;
    return make_expression_variable_declaration(span(begin, end), declaration_name, declaration_type, right);
}

Function_Signature parse_function_signature(Parser & parser) {
    if (!parser_match(parser, TOKEN_LEFT_PARENTHESIS)) {
        report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected '(', but got '{}'", parser.current);
        return Function_Signature{ .success = FALSE };
    }

    Array<Function_Parameter> parameters;

    if (!parser_peek(parser, TOKEN_RIGHT_PARENTHESIS)) {
        do {
            if (!parser_match(parser, TOKEN_IDENTIFIER)) {
                report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected identifier, but got '{}'", parser.current);
                return Function_Signature{ .success = FALSE };
            }

            Expression * name = make_expression_identifier(parser.previous->location, parser.previous);

            if (!parser_match(parser, TOKEN_COLON)) {
                report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected ':', but got '{}'", parser.current);
                return Function_Signature{ .success = FALSE };
            }

            Expression * type = parse_type(parser);
            if (type == NULL) {
                return Function_Signature{ .success = FALSE };
            }

            Expression * value = NULL;
            if (parser_match(parser, TOKEN_ASSIGNMENT)) {
                value = parse_expression_0(parser);
                if (value == NULL) {
                    return Function_Signature{ .success = FALSE };
                }
            }

            push(parameters, Function_Parameter{
                .name = name,
                .type = type,
                .value = value,
            });
        } while (parser_match(parser, TOKEN_COMMA));
    }

    if (!parser_match(parser, TOKEN_RIGHT_PARENTHESIS)) {
        report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected ')', ',' or '=', but got '{}'", parser.current);
        return Function_Signature{ .success = FALSE };
    }

    Expression * return_type = NULL;
    if (parser_match(parser, TOKEN_COLON)) {
        return_type = parse_type(parser);
        if (return_type == NULL) {
            return Function_Signature{ .success = FALSE };
        }
    }

    return Function_Signature{
        .success = TRUE,
        .parameters = parameters,
        .return_type = return_type,
    };
}

Expression * parse_function_declaration(Parser & parser, Expression * declaration_name) {
    Location declaration_begin = here(parser);

    Function_Signature signature = parse_function_signature(parser);
    if (!signature.success) {
        return NULL;
    }

    Expression_Function * function = NULL;

    {
        Location function_begin = here(parser);

        b32 foreign = FALSE;
        b32 entry = FALSE;

        Expression * body = NULL;

        if (parser_match(parser, TOKEN_DIRECTIVE_FOREIGN)) {
            foreign = TRUE;
            if (parser_peek(parser, TOKEN_LEFT_BRACES)) {
                report(parser.context, ERROR_BODY_ON_FOREIGN_FUNCTION, parser.current->location, "functions marked #foreign can not have a body");
                return NULL;
            }
        } else {
            if (parser_match(parser, TOKEN_DIRECTIVE_ENTRY)) {
                entry = TRUE;
            }

            body = parse_block(parser);
            if (body == NULL) {
                return NULL;
            }
        }

        Location function_end = here(parser);

        function = make_expression_function(span(function_begin, function_end), signature.parameters, signature.return_type, body);
        function->foreign = foreign;
        function->entry = entry;
    }

    Location declaration_end = here(parser);

    return make_expression_function_declaration(span(declaration_begin, declaration_end), declaration_name, function);
}

Expression * parse_structure_declaration(Parser & parser, Expression * declaration_name) {
    Location const & begin = parser.current->location;

    if (!parser_match(parser, TOKEN_LEFT_BRACES)) {
        report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected '\\{', but got '{}'", parser.current);
        return NULL;
    }

    Array<Field> fields;
    while (!parser_peek(parser, TOKEN_RIGHT_BRACES)) {
        if (!parser_match(parser, TOKEN_IDENTIFIER)) {
            report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected identifier, but got '{}'", parser.current);
            return NULL;
        }

        Expression * field_name = make_expression_identifier(parser.previous->location, parser.previous);

        if (!parser_match(parser, TOKEN_COLON)) {
            report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected ':', but got '{}'", parser.current);
            return NULL;
        }

        Expression * field_type = parse_type(parser);
        if (field_type == NULL) {
            return NULL;
        }

        if (!parser_match(parser, TOKEN_SEMICOLON)) {
            report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected ';', but got '{}'", parser.current);
            return NULL;
        }

        push(fields, Field{
            .name = field_name,
            .type = field_type
        });
    }

    if (!parser_match(parser, TOKEN_RIGHT_BRACES)) {
        report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected '}', but got '{}'", parser.current);
        return NULL;
    }

    Location const & end = parser.previous->location;
    Expression * value = make_expression_struct(span(begin, end), fields);

    return make_expression_struct_declaration(span(begin, end), declaration_name, value);
}

Expression * parse_opaque_declaration(Parser & parser, Expression * declaration_name) {
    Location const & begin = parser.current->location;

    if (!parser_match(parser, TOKEN_DIRECTIVE_OPAQUE)) {
        report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected '#opaque', but got '{}'", parser.current);
        return NULL;
    }

    Location const & end = parser.previous->location;
    return make_expression_opaque_declaration(span(begin, end), declaration_name);
}

Expression * parse_declaration(Parser & parser) {
    // TODO: This essentially checks if the value is assignable (or an "l-value"),
    //       we want a more sophisticated check in the future

    // TODO: This should all be speculative and the respective functions should do the full parsing later!
    if (!parser_match(parser, TOKEN_IDENTIFIER)) {
        report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected identifier, but got '{}'", parser.current);
        return NULL;
    }

    Expression * declaration_name = make_expression_identifier(parser.previous->location, parser.previous);

    if (!parser_match(parser, TOKEN_COLON)) {
        report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected ':', but got '{}'", parser.current);
        return NULL;
    }

    if (parser_match(parser, TOKEN_COLON)) {
        if (parser_peek(parser, TOKEN_LEFT_PARENTHESIS)) {
            return parse_function_declaration(parser, declaration_name);
        } else if (parser_peek(parser, TOKEN_LEFT_BRACES)) {
            return parse_structure_declaration(parser, declaration_name);
        } else if (parser_peek(parser, TOKEN_DIRECTIVE_OPAQUE)) {
            return parse_opaque_declaration(parser, declaration_name);
        } else {
            report(parser.context, ERROR_UNEXPECTED_TOKEN, parser.current->location, "expected '(' or '\\{', but got '{}'", parser.current);
            return NULL;
        }
    } else {
        return parse_variable_declaration(parser, declaration_name);
    }
}

Expression * parse_expression_7(Parser & parser) {
    Expression * expression = parse_expression_unary(parser);
    if (expression == NULL) {
        return NULL;
    }

    while (parser_match(parser, TOKEN_RANGE)) {
        Expression * left = expression;

        Expression * right = parse_expression_unary(parser);
        if (right == NULL) {
            return NULL;
        }

        expression = make_expression_binary(span(left->location, right->location), EXPRESSION_RANGE, left, right);
    }

    return expression;
}

Expression * parse_expression_6(Parser & parser) {
    Expression * expression = parse_expression_7(parser);
    if (expression == NULL) {
        return NULL;
    }

    while (parser_match(parser, TOKEN_AS)) {
        Token_Type type = parser.previous->type;

        Expression * left = expression;

        Expression * right = parse_type(parser);
        if (right == NULL) {
            return NULL;
        }

        expression = make_expression_as(span(left->location, right->location), left, right);
    }

    return expression;
}

Expression * parse_expression_5(Parser & parser) {
    Expression * expression = parse_expression_6(parser);
    if (expression == NULL) {
        return NULL;
    }

    while (parser_match_any(parser, TOKEN_MULTIPLICATION, TOKEN_DIVISION, TOKEN_BITWISE_AND, TOKEN_BITWISE_SHIFT_LEFT, TOKEN_BITWISE_SHIFT_RIGHT)) {
        Token_Type type = parser.previous->type;

        Expression * left = expression;

        Expression * right = parse_expression_6(parser);
        if (right == NULL) {
            return NULL;
        }

        if (type == TOKEN_MULTIPLICATION) {
            expression = make_expression_binary(span(left->location, right->location), EXPRESSION_BINARY_MULTIPLICATION, left, right);
        } else if (type == TOKEN_DIVISION) {
            expression = make_expression_binary(span(left->location, right->location), EXPRESSION_BINARY_DIVISION, left, right);
        } else if (type == TOKEN_BITWISE_AND) {
            expression = make_expression_binary(span(left->location, right->location), EXPRESSION_BINARY_BITWISE_AND, left, right);
        } else if (type == TOKEN_BITWISE_SHIFT_LEFT) {
            expression = make_expression_binary(span(left->location, right->location), EXPRESSION_BINARY_SHIFT_LEFT, left, right);
        } else if (type == TOKEN_BITWISE_SHIFT_RIGHT) {
            expression = make_expression_binary(span(left->location, right->location), EXPRESSION_BINARY_SHIFT_RIGHT, left, right);
        } else {
            ASSERT_NOT_REACHED();
        }
    }

    return expression;
}

Expression * parse_expression_4(Parser & parser) {
    Expression * expression = parse_expression_5(parser);
    if (expression == NULL) {
        return NULL;
    }

    while (parser_match_any(parser, TOKEN_ADDITION, TOKEN_SUBTRACTION, TOKEN_BITWISE_OR, TOKEN_BITWISE_XOR)) {
        Token_Type type = parser.previous->type;

        Expression * left = expression;

        Expression * right = parse_expression_5(parser);
        if (right == NULL) {
            return NULL;
        }

        if (type == TOKEN_ADDITION) {
            expression = make_expression_binary(span(left->location, right->location), EXPRESSION_BINARY_ADDITION, left, right);
        } else if (type == TOKEN_SUBTRACTION) {
            expression = make_expression_binary(span(left->location, right->location), EXPRESSION_BINARY_SUBTRACTION, left, right);
        } else if (type == TOKEN_BITWISE_OR) {
            expression = make_expression_binary(span(left->location, right->location), EXPRESSION_BINARY_BITWISE_OR, left, right);
        } else if (type == TOKEN_BITWISE_XOR) {
            expression = make_expression_binary(span(left->location, right->location), EXPRESSION_BINARY_BITWISE_XOR, left, right);
        } else {
            ASSERT_NOT_REACHED();
        }
    }

    return expression;
}

Expression * parse_expression_3(Parser & parser) {
    Expression * expression = parse_expression_4(parser);
    if (expression == NULL) {
        return NULL;
    }

    while (parser_match_any(parser, TOKEN_LESS_THAN, TOKEN_GREATER_THAN, TOKEN_LESS_THAN_OR_EQUAL_TO, TOKEN_GREATER_THAN_OR_EQUAL_TO, TOKEN_EQUALS, TOKEN_NOT_EQUALS)) {
        Token_Type type = parser.previous->type;

        Expression * left = expression;

        Expression * right = parse_expression_4(parser);
        if (right == NULL) {
            return NULL;
        }

        if (type == TOKEN_LESS_THAN) {
            expression = make_expression_binary(span(left->location, right->location), EXPRESSION_BINARY_COMPARISON_LESS_THAN, left, right);
        } else if (type == TOKEN_GREATER_THAN) {
            expression = make_expression_binary(span(left->location, right->location), EXPRESSION_BINARY_COMPARISON_GREATER_THAN, left, right);
        } else if (type == TOKEN_LESS_THAN_OR_EQUAL_TO) {
            expression = make_expression_binary(span(left->location, right->location), EXPRESSION_BINARY_COMPARISON_LESS_THAN_OR_EQUAL_TO, left, right);
        } else if (type == TOKEN_GREATER_THAN_OR_EQUAL_TO) {
            expression = make_expression_binary(span(left->location, right->location), EXPRESSION_BINARY_COMPARISON_GREATER_THAN_OR_EQUAL_TO, left, right);
        } else if (type == TOKEN_EQUALS) {
            expression = make_expression_binary(span(left->location, right->location), EXPRESSION_BINARY_COMPARISON_EQUALS, left, right);
        } else if (type == TOKEN_NOT_EQUALS) {
            expression = make_expression_binary(span(left->location, right->location), EXPRESSION_BINARY_COMPARISON_NOT_EQUALS, left, right);
        } else {
            ASSERT_NOT_REACHED();
        }
    }

    return expression;
}

Expression * parse_expression_2(Parser & parser) {
    Expression * expression = parse_expression_3(parser);
    if (expression == NULL) {
        return NULL;
    }

    while (parser_match(parser, TOKEN_AND)) {
        Expression * left = expression;

        Expression * right = parse_expression_3(parser);
        if (right == NULL) {
            return NULL;
        }

        expression = make_expression_binary(span(left->location, right->location), EXPRESSION_BINARY_BOOLEAN_AND, left, right);
    }

    return expression;
}

Expression * parse_expression_1(Parser & parser) {
    Expression * expression = parse_expression_2(parser);
    if (expression == NULL) {
        return NULL;
    }

    while (parser_match(parser, TOKEN_OR)) {
        Expression * left = expression;

        Expression * right = parse_expression_2(parser);
        if (right == NULL) {
            return NULL;
        }

        expression = make_expression_binary(span(left->location, right->location), EXPRESSION_BINARY_BOOLEAN_OR, left, right);
    }

    return expression;
}

Expression * parse_expression_0(Parser & parser) {
    Expression * expression = parse_expression_1(parser);
    if (expression == NULL) {
        return NULL;
    }

    while (parser_match(parser, TOKEN_ASSIGNMENT)) {
        Expression * left = expression;

        Expression * right = parse_expression_1(parser);
        if (right == NULL) {
            return NULL;
        }

        expression = make_expression_binary(span(left->location, right->location), EXPRESSION_ASSIGNMENT, left, right);
    }

    return expression;
}

Expression * parse_expression(Parser & parser) {
    if (parser_peek(parser, TOKEN_IDENTIFIER) && parser_peek_next(parser, TOKEN_COLON)) {
        return parse_declaration(parser);
    } else {
        Expression * expression = parse_expression_0(parser);
        if (expression == NULL) {
            return NULL;
        }

        return expression;
    }
}

Expression * parse_statement_or_expression(Parser & parser) {
    Expression * expression = parse_expression(parser);
    if (expression == NULL) {
        return NULL;
    }

    // Some expressions are considered statements even if they don't end with a semi-colon
    if (expression->type == EXPRESSION_FUNCTION_DECLARATION || expression->type == EXPRESSION_STRUCTURE_DECLARATION) {
        expression->flags |= EXPRESSION_FLAGS_STATEMENT;
    } if (expression->type == EXPRESSION_IF || expression->type == EXPRESSION_FOR || expression->type == EXPRESSION_WHILE || expression->type == EXPRESSION_BLOCK) {
        expression->flags |= EXPRESSION_FLAGS_STATEMENT;
    }

    if (parser_match(parser, TOKEN_SEMICOLON)) {
        expression->flags |= EXPRESSION_FLAGS_STATEMENT;
        expression->flags |= EXPRESSION_FLAGS_EXPLICIT_STATEMENT;
    }

    return expression;
}

Expression * parse_statement(Parser & parser) {
    Expression * expression = parse_statement_or_expression(parser);
    if (expression == NULL) {
        return NULL;
    }

    if (!(expression->flags & EXPRESSION_FLAGS_STATEMENT)) {
        report(parser.context, ERROR_UNEXPECTED_TOKEN, expression->location, "expected statement, but got expression (did you forget a ';'?)");
        return NULL;
    }

    return expression;
}

Expression * parse_top_level_statement(Parser & parser) {
    Location const & begin = parser.current->location;

    if (parser_match(parser, TOKEN_DIRECTIVE_IMPORT)) {
        // TODO: This can probably be simplified by explicitly parsing a string...
        Expression * expression = parse_expression(parser);
        if (expression == NULL) {
            return NULL;
        }

        if (expression->type != EXPRESSION_STRING) {
            report(parser.context, ERROR_UNEXPECTED_TOKEN, expression->location, "expected string, but got {}", expression);
            return NULL;
        }

        Location const & end = parser.previous->location;
        return make_expression_import(span(begin, end), expression);
    } else if (parser_match(parser, TOKEN_DIRECTIVE_LINKER)) {
        // TODO: This can probably be simplified by explicitly parsing a string...
        Expression * expression = parse_expression(parser);
        if (expression == NULL) {
            return NULL;
        }

        if (expression->type != EXPRESSION_STRING) {
            report(parser.context, ERROR_UNEXPECTED_TOKEN, expression->location, "expected string, but got {}", expression);
            return NULL;
        }

        Location const & end = parser.previous->location;
        return make_expression_linker(span(begin, end), expression);
    } else {
        Expression * statement = parse_statement(parser);
        if (statement == NULL) {
            return NULL;
        }

        if (statement->type != EXPRESSION_FUNCTION_DECLARATION && statement->type != EXPRESSION_STRUCTURE_DECLARATION && statement->type != EXPRESSION_OPAQUE_DECLARATION) {
            report(parser.context, ERROR_UNEXPECTED_TOKEN, statement->location, "expected declaration, but got {}", statement);
            return NULL;
        }

        return statement;
    }
}

struct Parse_Result {
    b32 success;
    Array<Expression *> expressions;
};

Parse_Result parse(Context & context, utf8 const * label, utf8 const * source) {
    Lex_Result result = lex(context, label, source);
    if (result.success) {
        Parser parser = {
            .context = context,

            .label = label,
            .source = source,

            .tokens = result.tokens,
            .previous = NULL,
            .current_index = 0,
            .current = &result.tokens[0],
        };

        Array<Expression *> expressions;
        while (parser.current->type != TOKEN_END) {
            Expression * expression = parse_top_level_statement(parser);
            if (expression == NULL) {
                return Parse_Result{
                    .success = FALSE,
                };
            }

            push(expressions, expression);
        }

        return Parse_Result{
            .success = TRUE,
            .expressions = expressions,
        };
    }

    return Parse_Result{
        .success = FALSE,
    };
}
