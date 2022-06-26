struct Resolver {
    Context & context;
    Symbol * root;
    Symbol * tail;
};

struct Resolution {
    Type * type;
    Symbol * symbol;
};

bool operator==(Resolution const & a, Resolution const & b) {
    return a.type == b.type && a.symbol == b.symbol;
}

static const Resolution no_resolution = Resolution{ .type = NULL, .symbol = NULL };

Resolution resolve_expression(Resolver & resolver, Symbol * previous, Expression_Function * function, Expression * expression, Type * hinted_type);

Resolution resolve_expression_integer(Resolver & resolver, Expression * expression, Type * hinted_type) {
    if (hinted_type != NULL) {
        if (hinted_type->flags & TYPE_FLAG_INTEGRAL) {
            expression->resolved_type = hinted_type;
        } else if (hinted_type->type == TYPE_POINTER && expression->integer == 0) {
            expression->resolved_type = hinted_type;
        } else {
            expression->resolved_type = &type_integral;
        }
    } else {
        expression->resolved_type = &type_integral;
    }

    return Resolution{
        expression->resolved_type,
        expression->environment,
    };
}

Resolution resolve_expression_identifier(Resolver & resolver, Expression * expression, Type * hinted_type) {
    Symbol * symbol = find_symbol(expression->environment, expression->identifier);
    if (symbol == NULL) {
        // print_symbols(symbols);

        report(resolver.context, ERROR_UNKNOWN_NAME, expression->location, "unknown name '{}'", expression->identifier);
        return no_resolution;
    } else if (!(symbol->flags & DECLARATION_FLAGS_DEFINED)) {
        report(resolver.context, ERROR_UNKNOWN_NAME, expression->location, "name '{}' has been declared but not yet defined", expression->identifier);
        return no_resolution;
    }

    expression->resolved_type = symbol->value.type;

    expression->resolved_symbol = symbol;

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression_unary(Resolver & resolver, Expression * expression, Type * hinted_type) {
    Resolution resolution = resolve_expression(resolver, expression->environment, NULL, expression->right, hinted_type);;
    if (resolution == no_resolution) {
        return no_resolution;
    }

    Type * type = resolution.type;

    expression->resolved_type = type;

    if (expression->type == EXPRESSION_UNARY_BOOLEAN_NOT) {
        if (expression->resolved_type != &type_boolean) {
            report(resolver.context, ERROR_INVALID_EXPRESSION_IN_CONTEXT, expression->location, "expected boolean");
            return no_resolution;
        }
    } else {
        if (!(expression->resolved_type == &type_integral || (expression->resolved_type->flags & TYPE_FLAG_INTEGRAL))) {
            report(resolver.context, ERROR_INVALID_EXPRESSION_IN_CONTEXT, expression->location, "expected integer");
            return no_resolution;
        }
    }

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression_range(Resolver & resolver, Expression_Function * function, Expression * expression, Type * hinted_type) {
    ASSERT(expression->type == EXPRESSION_RANGE);
    ASSERT(hinted_type == NULL || hinted_type->type == TYPE_RANGE || hinted_type->type == TYPE_ITERABLE);

    Type * hinted_base_type = NULL;
    if (hinted_type != NULL) {
        if (hinted_type->type == TYPE_ITERABLE) {
            hinted_base_type = hinted_type->iterable.base;
        } else if (hinted_type->type == TYPE_RANGE) {
            hinted_base_type = hinted_type->range.base;
        } else {
            ASSERT_NOT_REACHED();
        }
    }

    Resolution a_resolution = resolve_expression(resolver, expression->environment, NULL, expression->left, hinted_base_type);
    if (a_resolution == no_resolution) {
        return no_resolution;
    }

    Type * a_type = a_resolution.type;

    Resolution b_resolution = resolve_expression(resolver, expression->environment, NULL, expression->right, hinted_base_type);
    if (b_resolution == no_resolution) {
        return no_resolution;
    }

    Type * b_type = b_resolution.type;

    if (a_type == &type_integral && (b_type->flags & TYPE_FLAG_INTEGRAL)) {
        expression->left->resolved_type = b_type;
        a_type = expression->left->resolved_type;
    }

    if (b_type == &type_integral && (a_type->flags & TYPE_FLAG_INTEGRAL)) {
        expression->right->resolved_type = a_type;
        b_type = expression->right->resolved_type;
    }

    if (a_type == b_type) {
        if (!(a_type == &type_integral || a_type->flags & TYPE_FLAG_INTEGRAL)) {
            report(resolver.context, ERROR_INVALID_EXPRESSION_IN_CONTEXT, expression->location, "expected integral type, got {}", a_type);
            return no_resolution;
        }
    } else {
        report(resolver.context, ERROR_TYPE_MISMATCH, expression->location, "cannot create range on two values of different types (left hand side has type '{}' while right hand side has type '{}'", a_type, b_type);
        return no_resolution;
    }

    expression->resolved_type = type_range(a_type);

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression_binary(Resolver & resolver, Expression_Function * function, Expression * expression, Type * hinted_type) {
    Resolution a_resolution = resolve_expression(resolver, expression->environment, NULL, expression->left, hinted_type);
    if (a_resolution == no_resolution) {
        return no_resolution;
    }

    Type * a_type = a_resolution.type;

    Resolution b_resolution = resolve_expression(resolver, expression->environment, NULL, expression->right, hinted_type);
    if (b_resolution == no_resolution) {
        return no_resolution;
    }

    Type * b_type = b_resolution.type;

    if (a_type == &type_integral && (b_type->flags & TYPE_FLAG_INTEGRAL)) {
        expression->left->resolved_type = b_type;
        a_type = expression->left->resolved_type;
    }

    if (b_type == &type_integral && (a_type->flags & TYPE_FLAG_INTEGRAL)) {
        expression->right->resolved_type = a_type;
        b_type = expression->right->resolved_type;
    }

    if (a_type == b_type) {
        if (expression->type == EXPRESSION_BINARY_ADDITION) {
            // TODO: Should look up a function overload instead or do we want specific code paths for build in types?
            if (a_type == &type_integral || a_type->flags & TYPE_FLAG_INTEGRAL) {
                expression->resolved_type = a_type;
            } else if (a_type->type == TYPE_STRING) {
                expression->resolved_type = type_string();
            } else {
                report(resolver.context, ERROR_INVALID_EXPRESSION_IN_CONTEXT, expression->location, "expected integral type or string, got {}", a_type);
                return no_resolution;
            }
        } else {
            if (a_type == &type_integral || a_type->flags & TYPE_FLAG_INTEGRAL) {
                expression->resolved_type = a_type;
            } else {
                report(resolver.context, ERROR_INVALID_EXPRESSION_IN_CONTEXT, expression->location, "expected integral type, got {}", a_type);
                return no_resolution;
            }
        }
    } else {
        report(resolver.context, ERROR_TYPE_MISMATCH, expression->location, "cannot perform arithmetic on two values of different types (left hand side has type '{}' while right hand side has type '{}'", a_type, b_type);
        return no_resolution;
    }

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression_assignment(Resolver & resolver, Expression_Function * function, Expression * expression, Type * hinted_type) {
    // Update the type of the symbol's value, so we can recognize that we've assigned the variable at least once
    if (expression->left->type == EXPRESSION_IDENTIFIER) {
        Symbol * symbol = find_symbol(expression->environment, expression->left->identifier);
        symbol->flags |= DECLARATION_FLAGS_DEFINED;
    }

    // TODO: We need to check that the left hand side here evaluates to an L-value...
    Resolution a_resolution = resolve_expression(resolver, expression->environment, NULL, expression->left, NULL);
    if (a_resolution == no_resolution) {
        return no_resolution;
    }

    Type * a_type = a_resolution.type;

    Resolution b_resolution = resolve_expression(resolver, expression->environment, NULL, expression->right, a_type);
    if (b_resolution == no_resolution) {
        return no_resolution;
    }

    Type * b_type = b_resolution.type;

    if (!types_are_compatible(a_type, b_type)) {
        report(resolver.context, ERROR_ASSIGNMENT_TYPE_MISMATCH, expression->location, "cannot assign variable of type '{}' with value of type '{}'", expression->left->resolved_type, expression->right->resolved_type);
        return no_resolution;
    }

    if (a_type->flags & TYPE_FLAG_INTEGRAL) {
        if (expression->right->type == EXPRESSION_INTEGER) {
            if (a_type->flags & TYPE_FLAG_SIGNED) {
                // TODO: We need constant folder for unary minus before this will work properly for signed types...
                if (expression->right->integer > a_type->integral.maximum) {
                    report(resolver.context, ERROR_INTEGER_LITERAL_TOO_LARGE, expression->right->location, "integer literal too large");
                    return no_resolution;
                }
            } else {
                if (expression->right->flags & EXPRESSION_FLAGS_NEGATIVE) {
                    report(resolver.context, ERROR_INTEGER_LITERAL_NEGATIVE, expression->right->location, "integer literal is negative");
                    return no_resolution;
                }

                if (expression->right->integer > a_type->integral.maximum) {
                    report(resolver.context, ERROR_INTEGER_LITERAL_TOO_LARGE, expression->right->location, "integer literal too large");
                    return no_resolution;
                }
            }
        }
    }

    expression->resolved_type = &type_none;

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression_binary_comparison(Resolver & resolver, Expression_Function * function, Expression * expression, Type * hinted_type) {
    // TODO: This allows us to only have integer literals on the right hand side, which is not what we want in the end
    Resolution a_resolution = resolve_expression(resolver, expression->environment, NULL, expression->left, NULL);
    if (a_resolution == no_resolution) {
        return no_resolution;
    }

    Type * a_type = a_resolution.type;

    Resolution b_resolution = resolve_expression(resolver, expression->environment, NULL, expression->right, a_type);
    if (b_resolution == no_resolution) {
        return no_resolution;
    }

    Type * b_type = b_resolution.type;

    if (a_type == &type_integral && (b_type->flags & TYPE_FLAG_INTEGRAL)) {
        expression->left->resolved_type = b_type;
        a_type = expression->left->resolved_type;
    }

    if (b_type == &type_integral && (a_type->flags & TYPE_FLAG_INTEGRAL)) {
        expression->right->resolved_type = a_type;
        b_type = expression->right->resolved_type;
    }

    if (a_type == b_type) {
        if (a_type == &type_integral || a_type->flags & TYPE_FLAG_INTEGRAL) {
            expression->resolved_type = &type_boolean;
        } else if (a_type == &type_boolean) {
            expression->resolved_type = &type_boolean;
        } else if (a_type->type == TYPE_POINTER) {
            expression->resolved_type = &type_boolean;
        } else {
            report(resolver.context, ERROR_INVALID_EXPRESSION_IN_CONTEXT, expression->location, "expected integral type, got {}", a_type);
            return no_resolution;
        }
    } else {
        report(resolver.context, ERROR_INVALID_EXPRESSION_IN_CONTEXT, expression->location, "cannot compare two values of different types (left hand side has type '{}' while right hand side has type '{}'", a_type, b_type);
        return no_resolution;
    }

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression_binary_boolean(Resolver & resolver, Expression_Function * function, Expression * expression, Type * hinted_type) {
    // TODO: This allows us to only have integer literals on the right hand side, which is not what we want in the end
    Resolution a_resolution = resolve_expression(resolver, expression->environment, NULL, expression->left, NULL);
    if (a_resolution == no_resolution) {
        return no_resolution;
    }

    Type * a_type = a_resolution.type;

    Resolution b_resolution = resolve_expression(resolver, expression->environment, NULL, expression->right, a_type);
    if (b_resolution == no_resolution) {
        return no_resolution;
    }

    Type * b_type = b_resolution.type;

    if (a_type == b_type && a_type == &type_boolean) {
        expression->resolved_type = &type_boolean;
    } else {
        report(resolver.context, ERROR_INVALID_EXPRESSION_IN_CONTEXT, expression->location, "expected boolean");
        return no_resolution;
    }

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_invocation(Resolver & resolver, Expression_Invocation * expression) {
    Resolution resolved_resolution = resolve_expression(resolver, expression->environment, NULL, expression->invoked, NULL);
    Type * resolved_type = resolved_resolution.type;

    if (resolved_type != NULL) {
        Type * function_type = resolved_type;
        if (function_type->type == TYPE_POINTER) {
            function_type = resolved_type->pointer.base;
        }

        if (function_type->type != TYPE_FUNCTION) {
            report(resolver.context, ERROR_INVOKED_EXPRESSION_NOT_FUNCTION_OR_STRUCT, expression->invoked->location, "expected function or function pointer");
            return no_resolution;
        }

        ASSERT(expression->invoked->resolved_symbol != NULL);
        Symbol * invoked_symbol = expression->invoked->resolved_symbol;

        ASSERT(function_type->function.return_type == &type_none || function_type->function.return_type->type == TYPE_TYPE);

        // TODO: Here we need to go over all overloads etc.
        ASSERT(expression->invoked->resolved_symbol != NULL);
        ASSERT(function_type->function.parameters.number_of_elements == invoked_symbol->value.parameters.number_of_elements);

        b32 * has_argument_for_parameter = (b32 *)allocate(function_type->function.parameters.number_of_elements * sizeof(b32));

        {
            i32 index = 0;
            b32 in_named = FALSE;

            for (Argument & argument : expression->arguments) {
                i32 parameter_index = -1;
                if (argument.name == NULL) {
                    if (in_named) {
                        report(resolver.context, ERROR_UNNAMED_ARGUMENT_AFTER_NAMED_ARGUMENT, argument.value->location, "an unnamed argument cannot follow a named argument");
                        return no_resolution;
                    }

                    // We can rely on the index to find the correct parameter!
                    parameter_index = index;
                } else {
                    ASSERT(argument.name->type == EXPRESSION_IDENTIFIER);

                    if (!in_named) {
                        in_named = TRUE;
                    }

                    // We need to find the parameter by name
                    i32 candidate_parameter_index = 0;
                    for (Symbol_Function_Parameter & parameter : invoked_symbol->value.parameters) {
                        if (utf8_compare(parameter.name, argument.name->identifier)) {
                            parameter_index = candidate_parameter_index;
                            break;
                        }

                        candidate_parameter_index += 1;
                    }
                }

                if (parameter_index == -1) {
                    ASSERT(argument.name != NULL);

                    report(resolver.context, ERROR_NO_SUCH_PARAMETER, argument.name->location, "no argument named {}", argument.name->identifier);
                    return no_resolution;
                }

                ASSERT(parameter_index >= 0);
                Type * parameter_type = function_type->function.parameters[(u64)parameter_index];

                if (has_argument_for_parameter[parameter_index]) {
                    report(resolver.context, ERROR_PARAMETER_SPECIFIED_MULTIPLE_TIMES, argument.name->location, "parameter #{} was specified multiple times", parameter_index);
                    return no_resolution;
                }

                has_argument_for_parameter[parameter_index] = TRUE;

                Resolution argument_resolution = resolve_expression(resolver, expression->environment, NULL, argument.value, unwrap_type(parameter_type));
                if (argument_resolution == no_resolution) {
                    return no_resolution;
                }

                Type * argument_type = argument_resolution.type;
                if (!types_are_compatible(unwrap_type(parameter_type), argument_type)) {
                    utf8 const * parameter_name = invoked_symbol->value.parameters[parameter_index].name;
                    report(resolver.context, ERROR_PARAMETERS_ARGUMENTS_MISMATCH, expression->invoked->location, "expected argument to parameter #{} '{}' to be of type {}, but got argument of type {}", index, parameter_name, unwrap_type(parameter_type), argument_type);
                    return no_resolution;
                }

                index += 1;
            }
        }

        // Fill in the default values (could be merged with loop below...)
        {
            for (u64 parameter_index = 0; parameter_index < function_type->function.parameters.number_of_elements; parameter_index += 1) {
                if (!has_argument_for_parameter[parameter_index]) {
                    Symbol_Function_Parameter & parameter = invoked_symbol->value.parameters[parameter_index];
                    if (parameter.default_value != NULL) {
                        has_argument_for_parameter[parameter_index] = TRUE;

                    }
                }
            }
        }

        // Check that we have all parameters
        {
            for (u64 parameter_index = 0; parameter_index < function_type->function.parameters.number_of_elements; parameter_index += 1) {
                if (!has_argument_for_parameter[parameter_index]) {
                    report(resolver.context, ERROR_MISSING_ARGUMENT_FOR_PARAMETER, expression->invoked->location, "missing argument for parameter #{}", parameter_index);
                    return no_resolution;
                }
            }
        }

        if (function_type->function.return_type == &type_none) {
            expression->resolved_type = &type_none;
        } else {
            expression->resolved_type = unwrap_type(function_type->function.return_type);
        }

        return Resolution{
            expression->resolved_type,
            expression->environment
        };
    } else {
        ASSERT_NOT_REACHED();
    }
}

Resolution resolve_function_signature_raw(Resolver & resolver, Symbol * current, Array<Function_Parameter> const & parameters, Expression * return_type_expression, Array<Symbol_Function_Parameter>& parameters_additional_info) {
    Array<Type *> parameter_types;

    b32 in_defaults = FALSE;
    for (Function_Parameter const & parameter : parameters) {
        Expression * name = parameter.name;
        ASSERT(name->type == EXPRESSION_IDENTIFIER);

        Resolution resolution = resolve_expression(resolver, current, NULL, parameter.type, NULL);
        if (resolution == no_resolution) {
            return no_resolution;
        }

        Type * type = resolution.type;

        if (type == &type_none) {
            report(resolver.context, ERROR_INVALID_EXPRESSION_IN_CONTEXT, parameter.type->location, "expected non-none but got '{}'", parameter.type);
            return no_resolution;
        }

        if (parameter.value != NULL) {
            in_defaults = TRUE;

            // TODO: Here we have a more complex interdependency between units. These default arguments needs to be
            //       generated in the environment that the function signature itself was generated in. We don't have
            //       that available right now, so we just use an empty environment instead.
            Resolution value_resolution = resolve_expression(resolver, NULL, NULL, parameter.value, unwrap_type(type));
            if (value_resolution == no_resolution) {
                return no_resolution;
            }

            Type * value_type = value_resolution.type;

            if (!types_are_compatible(value_type, unwrap_type(type))) {
                report(resolver.context, ERROR_DEFAULT_VALUE_NOT_MATCHING_PARAMETER_TYPE, parameter.type->location, "parameter default value type {} does not match parameter type {}", value_type, unwrap_type(type));
                return no_resolution;
            }
        } else {
            if (in_defaults) {
                report(resolver.context, ERROR_NON_DEFAULT_VALUE_FOLLOWS_DEFAULT_VALUE, parameter.type->location, "non-defaulted parameter {} cannot follow defaulted parameter", parameter);
                return no_resolution;
            }
        }

        push(parameters_additional_info, Symbol_Function_Parameter{
            .name = name->identifier,
            .default_value = parameter.value,
        });

        push(parameter_types, type);
    }

    Type * return_type = &type_none;
    if (return_type_expression != NULL) {
        Resolution return_resolution = resolve_expression(resolver, current, NULL, return_type_expression, NULL);
        if (return_resolution == no_resolution) {
            return no_resolution;
        }

        return_type = return_resolution.type;

        if (return_type == &type_none) {
            report(resolver.context, ERROR_INVALID_EXPRESSION_IN_CONTEXT, return_type_expression->location, "expected non-none but got '{}'", return_type_expression);
            return no_resolution;
        } else if (unwrap_type(return_type)->type == TYPE_OPAQUE) {
            report(resolver.context, ERROR_INVALID_EXPRESSION_IN_CONTEXT, return_type_expression->location, "function cannot return opaque type {}", return_type_expression);
            return no_resolution;
        }
    }

    return Resolution{
        type_function(return_type, parameter_types),
        current
    };
}

Resolution resolve_function_signature(Resolver & resolver, Expression_Function * expression, Array<Symbol_Function_Parameter>& parameters) {
    Resolution resolution = resolve_function_signature_raw(resolver, expression->environment, expression->parameters, expression->return_type, parameters);
    if (resolution == no_resolution) {
        return no_resolution;
    }

    Type * type = resolution.type;
    expression->resolved_type = type;

    if (expression->entry) {
        if (type->function.parameters.number_of_elements > 0) {
            report(resolver.context, ERROR_ENTRY_FUNCTION_PARAMETERS, expression->location, "functions marked #entry cannot have any parameters");
            return no_resolution;
        }

        if (type->function.return_type != &type_none) {
            report(resolver.context, ERROR_ENTRY_FUNCTION_RETURN, expression->location, "functions marked #entry cannot have a return value");
            return no_resolution;
        }
    }

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

b32 check_return(Resolver & resolver, Expression * expression, Type * type) {
    if (expression->type == EXPRESSION_BLOCK) {
        Expression_Block * expression_block = (Expression_Block *)expression;
        for (Expression * expression : expression_block->expressions) {
            if (!check_return(resolver, expression, type)) {
                return FALSE;
            }
        }
    } else if (expression->type == EXPRESSION_IF) {
        Expression_If * expression_if = (Expression_If *)expression;
        if (!check_return(resolver, expression_if->then_expression, type)) {
            return FALSE;
        } else if (expression_if->else_expression != NULL) {
            if (!check_return(resolver, expression_if->else_expression, type)) {
                return FALSE;
            }
        }
    } else if (expression->type == EXPRESSION_FOR) {
        Expression_For * expression_for = (Expression_For *)expression;
        if (!check_return(resolver, expression_for->block, type)) {
            return FALSE;
        }
    } else if (expression->type == EXPRESSION_RETURN) {
        if (expression->expression != NULL) {
            if (type == &type_none) {
                report(resolver.context, ERROR_FUNCTION_RETURN_MISMATCH, expression->location, "return {} does not match function return type {}", expression->expression, type);
                return FALSE;
            } else if (!types_are_compatible(unwrap_type(type), expression->expression->resolved_type)) {
                report(resolver.context, ERROR_FUNCTION_RETURN_MISMATCH, expression->location, "return {} does not match function return type {}", expression->expression->resolved_type, unwrap_type(type));
                return FALSE;
            }
        } else if (expression->expression == NULL) {
            if (type != &type_none) {
                report(resolver.context, ERROR_FUNCTION_RETURN_MISMATCH, expression->location, "return does not match function return type {}", type);
                return FALSE;
            }
        }
    }

    return TRUE;
}

b32 ensure_return(Resolver & resolver, Expression * expression) {
    if (expression->type == EXPRESSION_BLOCK) {
        Expression_Block * expression_block = (Expression_Block *)expression;
        for (Expression * expression : expression_block->expressions) {
            if (ensure_return(resolver, expression)) {
                return TRUE;
            }
        }
    } else if (expression->type == EXPRESSION_IF) {
        Expression_If * expression_if = (Expression_If *)expression;
        if (expression_if->else_expression != NULL) {
            return ensure_return(resolver, expression_if->then_expression) && ensure_return(resolver, expression_if->else_expression);
        }
    } else if (expression->type == EXPRESSION_RETURN) {
        return TRUE;
    }

    return FALSE;
}

Resolution resolve_function(Resolver & resolver, Expression_Function * expression) {
    ASSERT(expression->type == EXPRESSION_FUNCTION);

    Symbol * current = find_first_global_symbol(expression->environment);
    ASSERT(current != NULL);

    Array<Type *> parameter_types;

    // TODO: Isn't this the same as resolve_function_signature...?
    for (Function_Parameter & parameter : expression->parameters) {
        Expression * name = parameter.name;
        ASSERT(name->type == EXPRESSION_IDENTIFIER);

        Resolution resolution = resolve_expression(resolver, current, expression, parameter.type, NULL);
        if (resolution == no_resolution) {
            return no_resolution;
        }

        Type * type = resolution.type;

        if (type == &type_none) {
            report(resolver.context, ERROR_INVALID_EXPRESSION_IN_CONTEXT, parameter.type->location, "expected non-none but got '{}'", parameter.type);
            return no_resolution;
        }

        push(parameter_types, type);

        current = push_symbol(current, Symbol{
            .flags = DECLARATION_FLAGS_DEFINED,
            .name = name->identifier,
            .value = {
                .type = unwrap_type(type),
                // TODO: put the resolved parameters here!
            }
        });

        ASSERT(current != NULL);

        name->resolved_symbol = current;
    }

    Type * return_type = &type_none;
    if (expression->return_type != NULL) {
        Resolution resolution = resolve_expression(resolver, current, NULL, expression->return_type, NULL);
        if (resolution == no_resolution) {
            return no_resolution;
        }

        return_type = resolution.type;

        if (return_type == &type_none) {
            report(resolver.context, ERROR_INVALID_EXPRESSION_IN_CONTEXT, expression->return_type->location, "expected non-none but got '{}'", expression->return_type);
            return no_resolution;
        }
    }

    if (expression->body != NULL) {
        Resolution resolution = resolve_expression(resolver, current, expression, expression->body, NULL);
        if (resolution == no_resolution) {
            return no_resolution;
        }

        if (!check_return(resolver, expression->body, return_type)) {
            return no_resolution;
        }

        if (return_type != &type_none && !ensure_return(resolver, expression->body)) {
            report(resolver.context, ERROR_FUNCTION_RETURN_MISMATCH, expression->body->location, "not all paths return");
            return no_resolution;
        }
    }

    expression->resolved_type = type_function(return_type, parameter_types);

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_structure(Resolver & resolver, Expression_Structure * expression) {
    ASSERT(expression->type == EXPRESSION_STRUCTURE);

    u64 size = 0;
    u64 alignment = 0;
    u64 offset = 0;

    Array<Structure_Field> fields;
    for (Field & field : expression->fields) {
        ASSERT(field.name->type == EXPRESSION_IDENTIFIER);
        utf8 const * name = field.name->identifier;

        Resolution field_resolution = resolve_expression(resolver, expression->environment, NULL, field.type, NULL);
        if (field_resolution == no_resolution) {
            return no_resolution;
        }

        Type * field_type = field_resolution.type;

        if (field_type == &type_none) {
            report(resolver.context, ERROR_INVALID_EXPRESSION_IN_CONTEXT, field.type->location, "expected non-none but got '{}'", field_type);
            return no_resolution;
        }

        ASSERT(field_type->type == TYPE_TYPE);
        field_type = unwrap_type(field_type);

        ASSERT(field_type->size != 0);
        ASSERT(field_type->alignment != 0);
        ASSERT((field_type->size & (field_type->alignment - 1)) == 0);

        offset = align_up(offset, field_type->alignment);
        size = align_up(size, field_type->alignment);
        alignment = max(alignment, field_type->alignment);

        push(fields, Structure_Field{
            .name = name,
            .type = field_type,
            .offset = offset,
        });

        offset += field_type->size;
        size += field_type->size;
    }

    size = align_up(size, alignment);

    expression->resolved_type = type_structure(size, alignment, fields);

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression_field(Resolver & resolver, Expression_Field * expression, Type * hinted_type) {
    Resolution resolution = resolve_expression(resolver, expression->environment, NULL, expression->accessed, NULL);
    if (resolution == no_resolution) {
        return no_resolution;
    }

    Type * accessed_type = resolution.type;

    if (accessed_type->type == TYPE_POINTER) {
        accessed_type = accessed_type->pointer.base;
    }

    if (accessed_type->type == TYPE_STRUCTURE) {
        b32 found = FALSE;
        for (Structure_Field & field : accessed_type->fields) {
            if (utf8_compare(field.name, expression->accessor->identifier)) {
                expression->resolved_type = field.type;

                found = TRUE;
                break;
            }
        }

        if (found) {
            return Resolution{
                expression->resolved_type,
                expression->environment
            };
        } else {
            report(resolver.context, ERROR_NO_SUCH_FIELD, expression->location, "no such field '{}' on type '{}'", expression->accessor->identifier, accessed_type);
            return no_resolution;
        }
    } else if (accessed_type->type == TYPE_STRING) {
        if (utf8_compare(expression->accessor->identifier, "pointer")) {
            expression->resolved_type = type_pointer(&type_u8);
        } else if (utf8_compare(expression->accessor->identifier, "number_of_bytes")) {
            expression->resolved_type = &type_u64;
        } else {
            report(resolver.context, ERROR_NO_SUCH_FIELD, expression->location, "no such field '{}' on type '{}'", expression->accessor->identifier, accessed_type);
            return no_resolution;
        }
    } else if (accessed_type->type == TYPE_SPAN) {
        if (utf8_compare(expression->accessor->identifier, "pointer")) {
            expression->resolved_type = type_pointer(&type_u8);
        } else if (utf8_compare(expression->accessor->identifier, "number_of_elements")) {
            expression->resolved_type = &type_u64;
        } else {
            report(resolver.context, ERROR_NO_SUCH_FIELD, expression->location, "no such field '{}' on type '{}'", expression->accessor->identifier, accessed_type);
            return no_resolution;
        }
    } else if (accessed_type->type == TYPE_OPAQUE) {
        report(resolver.context, ERROR_INVALID_EXPRESSION_IN_CONTEXT, expression->accessed->location, "opaque type {} cannot be indexed", accessed_type);
        return no_resolution;
    } else {
        report(resolver.context, ERROR_INVALID_EXPRESSION_IN_CONTEXT, expression->accessed->location, "expected structure or string but got '{}'", accessed_type);
        return no_resolution;
    }

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression_index(Resolver & resolver, Expression_Index * expression, Type * hinted_type) {
    Resolution indexed_resolution = resolve_expression(resolver, expression->environment, NULL, expression->indexed, NULL);
    if (indexed_resolution == no_resolution) {
        return no_resolution;
    }

    Type * indexed_type = indexed_resolution.type;

    if (indexed_type->type != TYPE_ARRAY && indexed_type->type != TYPE_SPAN && indexed_type != type_string()) {
        report(resolver.context, ERROR_INDEXED_TYPE_IS_NOT_ARRAY, expression->indexed->location, "tried to index '{}', but it is not an array, it is '{}'", expression->indexed, expression->indexed->resolved_type);
        return no_resolution;
    }

    Resolution index_resolution = resolve_expression(resolver, expression->environment, NULL, expression->index, &type_u64);
    if (index_resolution == no_resolution) {
        return no_resolution;
    }

    Type * index_type = index_resolution.type;

    if (!(index_type->flags & TYPE_FLAG_INTEGRAL)) {
        report(resolver.context, ERROR_ARRAY_INDEX_NOT_INTEGER, expression->index->location, "tried to index '{}', but the index is not an integral type, it is '{}'", expression->indexed, expression->indexed->resolved_type);
        return no_resolution;
    }

    if (indexed_type->type == TYPE_ARRAY) {
        expression->resolved_type = indexed_type->array.base;
    } else if (indexed_type->type == TYPE_SPAN) {
        expression->resolved_type = indexed_type->span.base;
    } else if (indexed_type->type == TYPE_STRING) {
        expression->resolved_type = &type_u8;
    } else {
        ASSERT_NOT_REACHED();
    }

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression_if(Resolver & resolver, Expression_Function * function, Expression_If * expression, Type * hinted_type) {
    Resolution condition_resolution = resolve_expression(resolver, expression->environment, NULL, expression->condition, NULL);
    if (condition_resolution == no_resolution) {
        return no_resolution;
    }

    Type * condition_type = condition_resolution.type;

    if (condition_type != &type_boolean) {
        report(resolver.context, ERROR_INVALID_EXPRESSION_IN_CONTEXT, expression->condition->location, "expected boolean but got '{}'", condition_type);
        return no_resolution;
    }

    Resolution then_resolution = resolve_expression(resolver, expression->environment, function, expression->then_expression, hinted_type);
    if (then_resolution == no_resolution) {
        return no_resolution;
    }

    Type * then_type = then_resolution.type;

    if (expression->else_expression) {
        Resolution else_resolution = resolve_expression(resolver, expression->environment, function, expression->else_expression, hinted_type);
        if (else_resolution == no_resolution) {
            return no_resolution;
        }

        Type * else_type = else_resolution.type;

        if (then_type != else_type) {
            report(resolver.context, ERROR_YIELD_MISMATCH, expression->location, "else clause ({}) and then clause ({}) does not yield the same type", then_type, else_type);
            return no_resolution;
        }

        expression->resolved_type = then_type;
    } else {
        // If there's no else, then we cannot evaluate to anything other than "nothing"
        expression->resolved_type = &type_none;
    }

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression_for(Resolver & resolver, Expression_Function * function, Expression_For * expression) {
    Type * index_type = NULL;
    if (expression->index_type != NULL) {
        Resolution index_resolution = resolve_expression(resolver, expression->environment, NULL, expression->index_type, NULL);
        if (index_resolution == no_resolution) {
            return no_resolution;
        }

        index_type = index_resolution.type;

        if (index_type->type != TYPE_TYPE) {
            report(resolver.context, ERROR_EXPRESSION_DOES_NOT_DENOTE_TYPE, expression->index_type->location, "expression {} does not denote a type", expression->index_type);
            return no_resolution;
        }

        index_type = unwrap_type(index_type);
    } else {
        index_type = &type_u64;
    }

    Type * value_type = NULL;
    if (expression->value_type != NULL) {
        Resolution value_resolution = resolve_expression(resolver, expression->environment, NULL, expression->value_type, NULL);
        if (value_resolution == no_resolution) {
            return no_resolution;
        }

        value_type = value_resolution.type;

        if (value_type->type != TYPE_TYPE) {
            report(resolver.context, ERROR_EXPRESSION_DOES_NOT_DENOTE_TYPE, expression->value_type->location, "expression {} does not denote a type", expression->value_type);
            return no_resolution;
        }

        value_type = unwrap_type(value_type);
    }

    Type * hinted_type = NULL;
    if (value_type != NULL) {
        hinted_type = type_iterable(value_type);
    }

    Resolution iterable_resolution = resolve_expression(resolver, expression->environment, NULL, expression->expression, hinted_type);
    if (iterable_resolution == no_resolution) {
        return no_resolution;
    }

    Type * iterable_type = iterable_resolution.type;

    if (iterable_type->type != TYPE_RANGE && iterable_type->type != TYPE_SPAN && iterable_type->type != TYPE_ARRAY && iterable_type->type != TYPE_STRING) {
        report(resolver.context, ERROR_INVALID_EXPRESSION_IN_CONTEXT, expression->expression->location, "expected range, span, array, or string but got {}", expression->expression->resolved_type);
        return no_resolution;
    }

    Type * base_type = NULL;
    if (iterable_type->type == TYPE_RANGE) {
        base_type = iterable_type->range.base;
    } else if (iterable_type->type == TYPE_SPAN) {
        base_type = iterable_type->span.base;
    } else if (iterable_type->type == TYPE_ARRAY) {
        base_type = iterable_type->array.base;
    } else if (iterable_type->type == TYPE_STRING) {
        base_type = &type_u8;
    } else {
        ASSERT_NOT_REACHED();
    }

    if (value_type == NULL) {
        if (base_type == &type_integral) {
            report(resolver.context, ERROR_INTEGRAL_RANGE, expression->location, "iterable is of integral type but iterator does not have an explicit type");
            return no_resolution;
        }

        value_type = base_type;
    } else if (value_type != NULL) {
        if (value_type != base_type) {
            if (!(base_type == &type_integral && (value_type->flags & TYPE_FLAG_INTEGRAL))) { // TODO: maybe we should have a type_casts_to_type function?
                report(resolver.context, ERROR_INTEGRAL_RANGE, expression->value_type->location, "explicit iterator type {} does not match iterable base type base {}", value_type, base_type);
                return no_resolution;
            }
        }
    }

    Symbol * block_scope = expression->environment;

    if (expression->index != NULL) {
        ASSERT(index_type != NULL);

        expression->index->resolved_type = index_type;
        block_scope = push_symbol(block_scope, Symbol{
            .flags = DECLARATION_FLAGS_DEFINED,
            .name = expression->index->identifier,
            .value = {
                .type = index_type,
            }
        });

        expression->index->resolved_symbol = block_scope;
    }

    ASSERT(value_type != NULL);

    expression->value->resolved_type = value_type;
    block_scope = push_symbol(block_scope, Symbol{
        .flags = DECLARATION_FLAGS_DEFINED,
        .name = expression->value->identifier,
        .value = {
            .type = value_type,
        }
    });

    expression->value->resolved_symbol = block_scope;

    {
        Resolution resolution = resolve_expression(resolver, block_scope, function, expression->block, NULL);
        if (resolution == no_resolution) {
            return no_resolution;
        }
    }

    expression->resolved_type = &type_none;

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression_while(Resolver & resolver, Expression_Function * function, Expression_While * expression) {
    {
        Resolution resolution = resolve_expression(resolver, expression->environment, NULL, expression->condition, NULL);
        if (resolution == no_resolution) {
            return no_resolution;
        }
    }

    if (expression->condition->resolved_type != &type_boolean) {
        report(resolver.context, ERROR_INVALID_EXPRESSION_IN_CONTEXT, expression->condition->location, "expected boolean but got '{}'", expression->condition->resolved_type);
        return no_resolution;
    }

    {
        Resolution resolution = resolve_expression(resolver, expression->environment, function, expression->block, NULL);
        if (resolution == no_resolution) {
            return no_resolution;
        }
    }

    expression->resolved_type = &type_none;

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression_block(Resolver & resolver, Expression_Function * function, Expression_Block * expression, Type * hinted_type) {
    expression->resolved_type = &type_none;

    Symbol * block_scope = expression->environment;
    for (Expression * subexpression : expression->expressions) {
        b32 last = FALSE;
        if (subexpression == expression->expressions[expression->expressions.number_of_elements - 1]) {
            last = TRUE;
        }

        Resolution resolution = resolve_expression(resolver, block_scope, function, subexpression, last ? hinted_type : NULL);
        if (resolution == no_resolution) {
            return no_resolution;
        }

        ASSERT(resolution.symbol != NULL);

        block_scope = resolution.symbol;

        if (last) {
            if (!(subexpression->flags & EXPRESSION_FLAGS_EXPLICIT_STATEMENT)) {
                expression->resolved_type = subexpression->resolved_type;
            }
        }
    }

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression_return(Resolver & resolver, Expression_Function * function, Expression * expression) {
    if (expression->expression != NULL) {
        Resolution resolution = resolve_expression(resolver, expression->environment, NULL, expression->expression, unwrap_type(function->resolved_type->function.return_type));
        if (resolution == no_resolution) {
            return no_resolution;
        }
    }

    expression->resolved_type = &type_none;

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression_as(Resolver & resolver, Expression * expression) {
    Resolution right_resolution = resolve_expression(resolver, expression->environment, NULL, expression->right, NULL);
    if (right_resolution == no_resolution) {
        return no_resolution;
    }

    Type * right_type = right_resolution.type;

    if (right_type->type != TYPE_TYPE) {
        report(resolver.context, ERROR_EXPRESSION_DOES_NOT_DENOTE_TYPE, expression->right->location, "expression {} does not denote a type", expression->right);
        return no_resolution;
    }

    Resolution left_resolution = resolve_expression(resolver, expression->environment, NULL, expression->left, unwrap_type(right_type));
    if (left_resolution == no_resolution) {
        return no_resolution;
    }

    Type * left_type = left_resolution.type;

    if (!type_can_be_casted_to_type(left_type, unwrap_type(right_type))) {
        report(resolver.context, ERROR_INCOMPATIBLE_CAST, expression->location, "cannot cast {} to {}", left_type, unwrap_type(right_type));
        return no_resolution;
    }

    expression->resolved_type = unwrap_type(right_type);

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression_variable_declaration(Resolver & resolver, Expression_Declaration * expression) {
    ASSERT(expression != NULL);
    ASSERT(expression->type == EXPRESSION_VARIABLE_DECLARATION);

    ASSERT(expression->declared_type != NULL);
    Resolution resolution = resolve_expression(resolver, expression->environment, NULL, expression->declared_type, NULL);
    if (resolution == no_resolution) {
        return no_resolution;
    }

    Type * type = resolution.type;

    if (type->type != TYPE_TYPE) {
        report(resolver.context, ERROR_EXPRESSION_DOES_NOT_DENOTE_TYPE, expression->declared_type->location, "expression {} does not denote a type", expression->declared_type);
        return no_resolution;
    } else if (unwrap_type(type)->type == TYPE_FUNCTION) {
        report(resolver.context, ERROR_VARIABLE_TYPE_FUNCTION, expression->declared_type->location, "cannot declare variable of function type ({}), use &{} to declare it as a function pointer instead", expression->declared_type, expression->declared_type);
        return no_resolution;
    } else if(unwrap_type(type)->type == TYPE_OPAQUE) {
        report(resolver.context, ERROR_CANNOT_INSTANTIATE_OPAQUE_TYPE, expression->declared_type->location, "cannot instantiate opaque type '{}'", expression->declared_type);
        return no_resolution;
    }

    if (expression->declared_value != NULL) {
        Resolution value_resolution = resolve_expression(resolver, expression->environment, NULL, expression->declared_value, unwrap_type(type));
        if (value_resolution == no_resolution) {
            return no_resolution;
        }

        Type * value_type = value_resolution.type;

        if (!types_are_compatible(unwrap_type(type), value_type)) {
            report(resolver.context, ERROR_ASSIGNMENT_TYPE_MISMATCH, expression->location, "cannot assign variable of type '{}' with value of type '{}'", unwrap_type(type), value_type);
            return no_resolution;
        }

        if (unwrap_type(type)->flags & TYPE_FLAG_INTEGRAL) {
            Type * integral_type = unwrap_type(type);
            if (expression->declared_value->type == EXPRESSION_INTEGER) {
                if (integral_type->flags & TYPE_FLAG_SIGNED) {
                    if (expression->declared_value->flags & EXPRESSION_FLAGS_NEGATIVE) {
                        if (expression->declared_value->integer > integral_type->integral.maximum + 1) {
                            report(resolver.context, ERROR_INTEGER_LITERAL_TOO_LARGE, expression->declared_value->location, "integer literal '{}{}' is too large for type {}, it must be between -{} and {}", expression->declared_value->flags & EXPRESSION_FLAGS_NEGATIVE ? "-" : "", expression->declared_value->integer, integral_type, integral_type->integral.minimum, integral_type->integral.maximum);
                            return no_resolution;
                        }
                    } else {

                        if (expression->declared_value->integer > integral_type->integral.maximum) {
                            report(resolver.context, ERROR_INTEGER_LITERAL_TOO_LARGE, expression->declared_value->location, "integer literal '{}' is too large for type {}, it must be between -{} and {}", expression->declared_value->integer, integral_type, integral_type->integral.minimum, integral_type->integral.maximum);
                            return no_resolution;
                        }
                    }
                } else {
                    if (expression->declared_value->flags & EXPRESSION_FLAGS_NEGATIVE) {
                        report(resolver.context, ERROR_INTEGER_LITERAL_NEGATIVE, expression->declared_value->location, "integer literal '{}' is negative", expression->declared_value->integer);
                        return no_resolution;
                    }

                    if (expression->declared_value->integer > integral_type->integral.maximum) {
                        report(resolver.context, ERROR_INTEGER_LITERAL_TOO_LARGE, expression->declared_value->location, "integer literal '{}' is too large for type {}, it must be smaller than {}", expression->declared_value->integer, integral_type, integral_type->integral.maximum);
                        return no_resolution;
                    }
                }
            }
        }
    }

    b32 defined = FALSE;
    if (expression->declared_value != NULL) {
        defined = TRUE;
    } else if (unwrap_type(type)->type == TYPE_STRING) {
        defined = TRUE;
    } else if (unwrap_type(type)->type == TYPE_STRUCTURE) {
        defined = TRUE;
    } else if (unwrap_type(type)->type == TYPE_SPAN) {
        defined = TRUE;
    } else if (unwrap_type(type)->type == TYPE_ARRAY) {
        defined = TRUE;
    }

    Symbol * new_scope = push_symbol(expression->environment, Symbol{
        .flags = DECLARATION_FLAGS_NONE,
        .name = expression->declared_name->identifier,
        .value = {
            .type = unwrap_type(type),
        }
    });

    if (defined) {
        new_scope->flags |= DECLARATION_FLAGS_DEFINED;
    }

    expression->symbol = new_scope;

    if (expression->declared_type->type == EXPRESSION_TYPE_POINTER) {
        if (expression->declared_type->expression->type == EXPRESSION_TYPE_FUNCTION) {
            new_scope->value.parameters = ((Expression_Type_Function *)expression->declared_type->expression)->resolved_parameters;
        }
    }

    expression->declared_name->resolved_symbol = new_scope;

    expression->resolved_type = &type_none;
    return Resolution{
        expression->resolved_type,
        new_scope
    };
}

Resolution resolve_expression_function_declaration(Resolver & resolver, Expression_Declaration * expression) {
    ASSERT(expression->type == EXPRESSION_FUNCTION_DECLARATION);

    ASSERT(expression->declared_type == NULL);

    ASSERT(expression->declared_value != NULL);
    ASSERT(expression->declared_value->type == EXPRESSION_FUNCTION);
    Expression_Function * expression_function = (Expression_Function *)expression->declared_value;
    expression_function->environment = expression->environment; // TODO: Why is this expression not resolved...?

    Array<Symbol_Function_Parameter> function_parameters;

    // We need to resolve the function signature before the body since the
    // function itself can be referenced in the body. This is different to
    // how variable declarations work.
    Resolution signature_resolution = resolve_function_signature(resolver, expression_function, function_parameters);
    if (signature_resolution == no_resolution) {
        return no_resolution;
    }

    Type * signature_type = signature_resolution.type;

    if (signature_type->type != TYPE_FUNCTION) {
        report(resolver.context, ERROR_EXPRESSION_DOES_NOT_DENOTE_TYPE, expression->declared_name->location, "expression {} does not denote a function type", expression->declared_name);
        return no_resolution;
    }

    Symbol * symbol = find_symbol(expression->environment, expression->declared_name->identifier);
    if (symbol) {
        ASSERT_NOT_REACHED();
    } else {
        symbol = push_symbol(expression->environment, Symbol{
            .flags = DECLARATION_FLAGS_FUNCTION | DECLARATION_FLAGS_DEFINED,
            .name = expression->declared_name->identifier,
            .value = {
                .type = signature_type,
                .parameters = function_parameters,
            },
        });

        // TODO: This is a hack. We need a global table and then chaining after that instead...
        symbol->global = TRUE;

        ASSERT(symbol != NULL);

        if (expression_function->foreign) {
            symbol->flags |= DECLARATION_FLAGS_FOREIGN_FUNCTION;
        }

        if (expression_function->entry) {
            symbol->flags |= DECLARATION_FLAGS_ENTRY_FUNCTION;
        }

        expression->symbol = symbol;

        expression->declared_name->resolved_symbol = symbol;
    }

    Resolution body_resolution = resolve_expression(resolver, symbol, NULL, expression_function, NULL); // this can be used to infer the declaration type if we want to
    if (body_resolution == no_resolution) {
        return no_resolution;
    }

    Type * body_type = body_resolution.type;

    ASSERT(signature_type == body_type);

    expression->resolved_type = &type_none;
    return Resolution{
        expression->resolved_type,
        symbol
    };
}

Resolution resolve_expression_structure_declaration(Resolver & resolver, Expression_Declaration * expression) {
    ASSERT(expression->type == EXPRESSION_STRUCTURE_DECLARATION);

    ASSERT(expression->declared_type == NULL);

    Expression_Structure * expression_structure = (Expression_Structure *)expression->declared_value;

    Resolution structure_resolution = resolve_expression(resolver, expression->environment, NULL, expression->declared_value, NULL); // this can be used to infer the declaration type if we want to
    if (structure_resolution == no_resolution) {
        return no_resolution;
    }

    Type * structure_type = structure_resolution.type;

    Symbol * scope = push_symbol(expression->environment, Symbol{
        .flags = DECLARATION_FLAGS_DEFINED,
        .name = expression->declared_name->identifier,
        .value = {
            .type = type_type(structure_type),
        }
    });

    // TODO: This is a hack. We need a global table and then chaining after that instead...
    scope->global = TRUE;

    expression->symbol = scope;

    expression->declared_name->resolved_symbol = scope;

    expression->resolved_type = &type_none;
    return Resolution{
        expression->resolved_type,
        scope
    };
}

Resolution resolve_expression_opaque_declaration(Resolver & resolver, Expression_Declaration * expression) {
    ASSERT(expression->type == EXPRESSION_OPAQUE_DECLARATION);

    ASSERT(expression->declared_type == NULL);
    ASSERT(expression->declared_value == NULL);

    if (resolver.context.verbose) {
        print("adding opaque symbol to scope: {}\n", expression->declared_name->identifier);
    }

    Symbol * scope = push_symbol(expression->environment, Symbol{
        .flags = DECLARATION_FLAGS_DEFINED,
        .name = expression->declared_name->identifier,
        .value = {
            .type = type_type(type_opaque(((Expression_Declaration *)expression)->declared_name->identifier)),
        }
    });

    expression->symbol = scope;

    expression->declared_name->resolved_symbol = scope;

    expression->resolved_type = &type_none;
    return Resolution{
        expression->resolved_type,
        scope
    };
}

Resolution resolve_expression_type_pointer(Resolver & resolver, Expression * expression, Type * hinted_type) {
    Resolution resolution = resolve_expression(resolver, expression->environment, NULL, expression->expression, NULL);
    if (resolution == no_resolution) {
        return no_resolution;
    }

    Type * type = resolution.type;

    expression->resolved_type = type_type(type_pointer(type));

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression_type_array(Resolver & resolver, Expression_Type_Array * expression, Type * hinted_type) {
    Resolution array_type_resolution = resolve_expression(resolver, expression->environment, NULL, expression->type, NULL);
    if (array_type_resolution == no_resolution) {
        return no_resolution;
    }

    Type * array_type_type = array_type_resolution.type;

    if (expression->length != NULL) {
        Resolution length_resolution = resolve_expression(resolver, expression->environment, NULL, expression->length, NULL);
        if (length_resolution == no_resolution) {
            return no_resolution;
        }

        Type * length_type = length_resolution.type;

        if (length_type != &type_integral && !(length_type->flags & TYPE_FLAG_INTEGRAL)) {
            report(resolver.context, ERROR_NON_INTEGRAL_ARRAY_LENGTH, expression->length->location, "expected integral type but got", expression->length->resolved_type);
            return no_resolution;
        }

        // TODO: Here we should really check if it's a compile time constant, but for now
        //       we just check for an integer literal.
        if (expression->length->type != EXPRESSION_INTEGER) {
            report(resolver.context, ERROR_NON_CONSTANT_ARRAY_LENGTH, expression->length->location, "expected constant");
            return no_resolution;
        }
    }

    if (expression->length != NULL) {
        ASSERT(expression->length->type == EXPRESSION_INTEGER);
        expression->resolved_type = type_type(type_array(expression->length->integer, array_type_type));
    } else {
        expression->resolved_type = type_type(type_span(array_type_type));
    }

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression_type_function(Resolver & resolver, Expression_Type_Function * expression, Type * hinted_type) {
    Resolution resolution = resolve_function_signature_raw(resolver, expression->environment, expression->parameters, expression->return_type, expression->resolved_parameters);
    if (resolution == no_resolution) {
        return no_resolution;
    }

    Type * type = resolution.type;

    expression->resolved_type = type_type(type);

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression_array(Resolver & resolver, Expression_Array * expression, Type * hinted_type) {
    ASSERT(hinted_type != NULL && (hinted_type->type == TYPE_ARRAY || hinted_type->type == TYPE_SPAN || hinted_type->type == TYPE_ITERABLE));

    Type * hinted_element_type = NULL;
    if (hinted_type->type == TYPE_ARRAY) {
        hinted_element_type = hinted_type->array.base;
    } else if (hinted_type->type == TYPE_SPAN) {
        hinted_element_type = hinted_type->span.base;
    } else if (hinted_type->type == TYPE_ITERABLE) {
        hinted_element_type = hinted_type->iterable.base;
    } else {
        ASSERT_NOT_REACHED();
    }

    u64 element_count = expression->expressions.number_of_elements;

    Type * element_type = NULL;
    if (expression->expressions.number_of_elements > 0) {
        Resolution element_resolution = resolve_expression(resolver, expression->environment, NULL, expression->expressions[0], hinted_element_type);
        if (element_resolution == no_resolution) {
            return no_resolution;
        }

        element_type = element_resolution.type;

        for (u64 index = 1; index < expression->expressions.number_of_elements; index += 1) {
            Resolution resolution = resolve_expression(resolver, expression->environment, NULL, expression->expressions[index], hinted_element_type);
            if (resolution == no_resolution) {
                return no_resolution;
            }

            Type * type = resolution.type;

            if (type != element_type) {
                report(resolver.context, ERROR_INCOMPATIBLE_ARRAY_ELEMENTS, expression->expressions[index]->location, "element {} does not match type of first element {}", expression->expressions[index]->resolved_type, expression->expressions[0]->resolved_type);
                return no_resolution;
            }
        }
    } else {
        element_type = hinted_element_type;
    }

    expression->resolved_type = type_array(element_count, element_type);
    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression_address_of(Resolver & resolver, Expression * expression, Type * hinted_type) {
    Resolution resolution = resolve_expression(resolver, expression->environment, NULL, expression->right, hinted_type);
    if (resolution == no_resolution) {
        return no_resolution;
    }

    Type * type = resolution.type;

    if (type->type == TYPE_ARRAY) {
        expression->resolved_type = type_pointer(type->array.base);
    } else {
        expression->resolved_type = type_pointer(type);
    }

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression_dereference(Resolver & resolver, Expression * expression, Type * hinted_type) {
    Resolution resolution = resolve_expression(resolver, expression->environment, NULL, expression->right, hinted_type);
    if (resolution == no_resolution) {
        return no_resolution;
    }

    Type * type = resolution.type;

    if (type->type != TYPE_POINTER) {
        report(resolver.context, ERROR_INVALID_EXPRESSION_IN_CONTEXT, expression->location, "expected pointer type, cannot derefence type '{}'", type);
        return no_resolution;
    }

    expression->resolved_type = type->pointer.base;

    return Resolution{
        expression->resolved_type,
        expression->environment
    };
}

Resolution resolve_expression(Resolver & resolver, Symbol * current, Expression_Function * function, Expression * expression, Type * hinted_type) {
    expression->environment = current;
    switch (expression->type) {
        case EXPRESSION_INTEGER: {
            return resolve_expression_integer(resolver, expression, hinted_type);
        } break;

        case EXPRESSION_STRING: {
            expression->resolved_type = type_string();
            return Resolution{
                expression->resolved_type,
                expression->environment
            };
        } break;

        case EXPRESSION_IDENTIFIER: {
            return resolve_expression_identifier(resolver, expression, hinted_type);
        } break;

        case EXPRESSION_UNARY_BOOLEAN_NOT:
        case EXPRESSION_UNARY_MINUS:
        case EXPRESSION_UNARY_BITWISE_NOT: {
            return resolve_expression_unary(resolver, expression, hinted_type);
        } break;

        case EXPRESSION_RANGE: {
            return resolve_expression_range(resolver, function, expression, hinted_type);
        } break;

        case EXPRESSION_BINARY_ADDITION:
        case EXPRESSION_BINARY_SUBTRACTION:
        case EXPRESSION_BINARY_BITWISE_OR:
        case EXPRESSION_BINARY_BITWISE_XOR:
        case EXPRESSION_BINARY_MULTIPLICATION:
        case EXPRESSION_BINARY_DIVISION:
        case EXPRESSION_BINARY_BITWISE_AND:
        case EXPRESSION_BINARY_SHIFT_LEFT:
        case EXPRESSION_BINARY_SHIFT_RIGHT: {
            return resolve_expression_binary(resolver, function, expression, hinted_type);
        } break;

        case EXPRESSION_ASSIGNMENT: {
            return resolve_expression_assignment(resolver, function, expression, hinted_type);
        } break;

        case EXPRESSION_BINARY_COMPARISON_LESS_THAN:
        case EXPRESSION_BINARY_COMPARISON_GREATER_THAN:
        case EXPRESSION_BINARY_COMPARISON_LESS_THAN_OR_EQUAL_TO:
        case EXPRESSION_BINARY_COMPARISON_GREATER_THAN_OR_EQUAL_TO:
        case EXPRESSION_BINARY_COMPARISON_EQUALS:
        case EXPRESSION_BINARY_COMPARISON_NOT_EQUALS: {
            return resolve_expression_binary_comparison(resolver, function, expression, hinted_type);
        } break;

        case EXPRESSION_BINARY_BOOLEAN_AND:
        case EXPRESSION_BINARY_BOOLEAN_OR: {
            return resolve_expression_binary_boolean(resolver, function, expression, hinted_type);
        } break;

        case EXPRESSION_INVOCATION: {
            return resolve_invocation(resolver, (Expression_Invocation *)expression);
        } break;

        case EXPRESSION_TRUE: {
            expression->resolved_type = &type_boolean;
            return Resolution{
                expression->resolved_type,
                expression->environment,
            };
        } break;

        case EXPRESSION_FALSE: {
            expression->resolved_type = &type_boolean;
            return Resolution{
                expression->resolved_type,
                expression->environment,
            };
        } break;

        case EXPRESSION_FUNCTION: {
            return resolve_function(resolver, (Expression_Function *)expression);
        } break;

        case EXPRESSION_STRUCTURE: {
            return resolve_structure(resolver, (Expression_Structure *)expression);
        } break;

        case EXPRESSION_FIELD: {
            return resolve_expression_field(resolver, (Expression_Field *)expression, hinted_type);
        } break;

        case EXPRESSION_INDEX: {
            return resolve_expression_index(resolver, (Expression_Index *)expression, hinted_type);
        } break;

        case EXPRESSION_IF: {
            return resolve_expression_if(resolver, function, (Expression_If *)expression, hinted_type);
        } break;

        case EXPRESSION_FOR: {
            return resolve_expression_for(resolver, function, (Expression_For *)expression);
        } break;

        case EXPRESSION_WHILE: {
            return resolve_expression_while(resolver, function, (Expression_While *)expression);
        } break;

        case EXPRESSION_BLOCK: {
            return resolve_expression_block(resolver, function, (Expression_Block *)expression, hinted_type);
        } break;

        case EXPRESSION_RETURN: {
            return resolve_expression_return(resolver, function, expression);
        } break;

        case EXPRESSION_AS: {
            return resolve_expression_as(resolver, expression);
        } break;

        case EXPRESSION_VARIABLE_DECLARATION: {
            return resolve_expression_variable_declaration(resolver, (Expression_Declaration *)expression);
        } break;

        case EXPRESSION_FUNCTION_DECLARATION: {
            return resolve_expression_function_declaration(resolver, (Expression_Declaration *)expression);
        } break;

        case EXPRESSION_STRUCTURE_DECLARATION: {
            return resolve_expression_structure_declaration(resolver, (Expression_Declaration *)expression);
        } break;

        case EXPRESSION_OPAQUE_DECLARATION: {
            return resolve_expression_opaque_declaration(resolver, (Expression_Declaration *)expression);
        } break;

        case EXPRESSION_TYPE_POINTER: {
            return resolve_expression_type_pointer(resolver, expression, hinted_type);
        } break;

        case EXPRESSION_TYPE_ARRAY: {
            return resolve_expression_type_array(resolver, (Expression_Type_Array *)expression, hinted_type);
        } break;

        case EXPRESSION_TYPE_FUNCTION: {
            return resolve_expression_type_function(resolver, (Expression_Type_Function *)expression, hinted_type);
        } break;

        case EXPRESSION_ARRAY: {
            return resolve_expression_array(resolver, (Expression_Array *)expression, hinted_type);
        } break;

        case EXPRESSION_ADDRESS_OF: {
            return resolve_expression_address_of(resolver, expression, hinted_type);
        } break;

        case EXPRESSION_DEREFERENCE: {
            return resolve_expression_dereference(resolver, expression, hinted_type);
        } break;

        case EXPRESSION_IMPORT:
        case EXPRESSION_LINKER: {
            ASSERT_NOT_REACHED();
        } break;
    }
}

b32 resolve(Resolver & resolver, Expression * expression) {
    Resolution resolution = resolve_expression(resolver, resolver.tail, NULL, expression, NULL);
    if (resolution == no_resolution) {
        return FALSE;
    }

    resolver.tail = resolution.symbol;

    return TRUE;
}
