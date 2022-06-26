struct Environment {
    Environment * enclosing = NULL;

    Array<utf8 const *> symbols;
};

Environment * get_global_environment(Environment * environment) {
    if (environment->enclosing != NULL) {
        return get_global_environment(environment->enclosing);
    }

    return environment;
}

b32 has_symbol(Environment * environment, utf8 const * name) {
    for (utf8 const * symbol : environment->symbols) {
        if (utf8_compare(symbol, name)) {
            return TRUE;
        }
    }

    if (environment->enclosing != NULL) {
        return has_symbol(environment->enclosing, name);
    }

    return FALSE;
}

Environment * push_scope(Environment * enclosing) {
    Environment * environment = (Environment *)allocate(sizeof(Environment));
    environment->enclosing = enclosing;
    return environment;
}

b32 is_global_scope(Environment * environment) {
    return environment->enclosing == NULL;
}

void push_symbol(Environment * environment, utf8 const * symbol) {
    push(environment->symbols, symbol);
}

Environment * make_global_environment() {
    Environment * environment = (Environment *)allocate(sizeof(Environment));
    push_symbol(environment, "void");
    push_symbol(environment, "u8");
    push_symbol(environment, "u16");
    push_symbol(environment, "u32");
    push_symbol(environment, "u64");
    push_symbol(environment, "i8");
    push_symbol(environment, "i16");
    push_symbol(environment, "i32");
    push_symbol(environment, "i64");
    push_symbol(environment, "boolean");
    push_symbol(environment, "string");

    return environment;
}

void find_dependencies(Unit & unit, Environment * enclosing_environment, Environment * environment, Expression * expression) {
    switch (expression->type) {
        case EXPRESSION_INTEGER:
        case EXPRESSION_STRING:
        case EXPRESSION_TRUE:
        case EXPRESSION_FALSE: {
        } break;

        case EXPRESSION_IDENTIFIER: {
            if (!has_symbol(environment, expression->identifier)) {
                push(unit.dependencies, Dependency{ .fulfilled = FALSE, .label = expression->identifier, .usage = expression->location });
            }
        } break;

        case EXPRESSION_UNARY_MINUS:
        case EXPRESSION_UNARY_BITWISE_NOT:
        case EXPRESSION_UNARY_BOOLEAN_NOT: {
            find_dependencies(unit, enclosing_environment, environment, expression->right);
        } break;

        case EXPRESSION_RANGE:
        case EXPRESSION_BINARY_ADDITION:
        case EXPRESSION_BINARY_SUBTRACTION:
        case EXPRESSION_BINARY_BITWISE_OR:
        case EXPRESSION_BINARY_BITWISE_XOR:
        case EXPRESSION_BINARY_MULTIPLICATION:
        case EXPRESSION_BINARY_DIVISION:
        case EXPRESSION_BINARY_BITWISE_AND:
        case EXPRESSION_BINARY_SHIFT_LEFT:
        case EXPRESSION_BINARY_SHIFT_RIGHT:
        case EXPRESSION_BINARY_BOOLEAN_AND:
        case EXPRESSION_BINARY_BOOLEAN_OR:
        case EXPRESSION_BINARY_COMPARISON_LESS_THAN:
        case EXPRESSION_BINARY_COMPARISON_GREATER_THAN:
        case EXPRESSION_BINARY_COMPARISON_LESS_THAN_OR_EQUAL_TO:
        case EXPRESSION_BINARY_COMPARISON_GREATER_THAN_OR_EQUAL_TO:
        case EXPRESSION_BINARY_COMPARISON_EQUALS:
        case EXPRESSION_BINARY_COMPARISON_NOT_EQUALS:
        case EXPRESSION_ASSIGNMENT: {
            find_dependencies(unit, enclosing_environment, environment, expression->left);
            find_dependencies(unit, enclosing_environment, environment, expression->right);
        } break;

        case EXPRESSION_INVOCATION: {
            Expression_Invocation * expression_invocation = (Expression_Invocation *)expression;

            find_dependencies(unit, enclosing_environment, environment, expression_invocation->invoked);

            for (Argument & argument : expression_invocation->arguments) {
                find_dependencies(unit, enclosing_environment, environment, argument.value);
            }
        } break;

        case EXPRESSION_FUNCTION: {
            Expression_Function * expression_function = (Expression_Function *)expression;

            Environment * environment = push_scope(get_global_environment(enclosing_environment));

            for (Function_Parameter & parameter : expression_function->parameters) {
                Expression * name = parameter.name;
                ASSERT(name->type == EXPRESSION_IDENTIFIER);

                find_dependencies(unit, enclosing_environment, environment, parameter.type);

                push_symbol(environment, name->identifier);
            }

            if (expression_function->return_type != NULL) {
                find_dependencies(unit, enclosing_environment, environment, expression_function->return_type);
            }

            if (expression_function->body != NULL) {
                find_dependencies(unit, enclosing_environment, environment, expression_function->body);
            }
        } break;

        case EXPRESSION_STRUCTURE: {
            Expression_Structure * expression_structure = (Expression_Structure *)expression;

            for (Field & field : expression_structure->fields) {
                find_dependencies(unit, enclosing_environment, environment, field.type);
            }
        } break;

        case EXPRESSION_FIELD: {
            Expression_Field * expression_field = (Expression_Field *)expression;

            find_dependencies(unit, enclosing_environment, environment, expression_field->accessed);

            ASSERT(expression_field->accessor->type == EXPRESSION_IDENTIFIER);
        } break;

        case EXPRESSION_INDEX: {
            Expression_Index * expression_index = (Expression_Index *)expression;

            find_dependencies(unit, enclosing_environment, environment, expression_index->indexed);
            find_dependencies(unit, enclosing_environment, environment, expression_index->index);
        } break;

        case EXPRESSION_IF: {
            Expression_If * expression_if = (Expression_If *)expression;

            find_dependencies(unit, enclosing_environment, environment, expression_if->condition);
            find_dependencies(unit, enclosing_environment, environment, expression_if->then_expression);
            if (expression_if->else_expression != NULL) {
                find_dependencies(unit, enclosing_environment, environment, expression_if->else_expression);
            }
        } break;

        case EXPRESSION_FOR: {
            Expression_For * expression_for = (Expression_For *)expression;

            find_dependencies(unit, enclosing_environment, environment, expression_for->expression);

            Environment * new_environment = push_scope(environment);

            if (expression_for->index != NULL) {
                push_symbol(new_environment, expression_for->index->identifier);
            }

            push_symbol(new_environment, expression_for->value->identifier);

            find_dependencies(unit, enclosing_environment, new_environment, expression_for->block);
        } break;

        case EXPRESSION_WHILE: {
            Expression_While * expression_while = (Expression_While *)expression;

            find_dependencies(unit, enclosing_environment, environment, expression_while->condition);
            find_dependencies(unit, enclosing_environment, environment, expression_while->block);
        } break;

        case EXPRESSION_BLOCK: {
            Expression_Block * expression_block = (Expression_Block *)expression;

            Environment * new_environment = push_scope(environment);

            for (Expression * expression : expression_block->expressions) {
                find_dependencies(unit, enclosing_environment, new_environment, expression);
            }
        } break;

        case EXPRESSION_RETURN: {
            if (expression->expression != NULL) {
                find_dependencies(unit, enclosing_environment, environment, expression->expression);
            }
        } break;

        case EXPRESSION_AS: {
            find_dependencies(unit, enclosing_environment, environment, expression->left);
            find_dependencies(unit, enclosing_environment, environment, expression->right);
        } break;

        case EXPRESSION_ADDRESS_OF:
        case EXPRESSION_DEREFERENCE: {
            find_dependencies(unit, enclosing_environment, environment, expression->right);
        } break;

        case EXPRESSION_FUNCTION_DECLARATION:
        case EXPRESSION_STRUCTURE_DECLARATION:
        case EXPRESSION_OPAQUE_DECLARATION:
        case EXPRESSION_VARIABLE_DECLARATION: {
            Expression_Declaration * expression_declaration = (Expression_Declaration *)expression;

            if (expression_declaration->declared_type != NULL) {
                find_dependencies(unit, enclosing_environment, environment, expression_declaration->declared_type);
            }

            if (expression->type == EXPRESSION_FUNCTION_DECLARATION) {
                push_symbol(enclosing_environment, expression_declaration->declared_name->identifier);
            }

            if (expression_declaration->declared_value != NULL) {
                find_dependencies(unit, enclosing_environment, environment, expression_declaration->declared_value);
            }

            if (expression->type != EXPRESSION_FUNCTION_DECLARATION) {
                push_symbol(environment, expression_declaration->declared_name->identifier);
            }
        } break;

        case EXPRESSION_TYPE_POINTER: {
            find_dependencies(unit, enclosing_environment, environment, expression->expression);
        } break;

        case EXPRESSION_TYPE_ARRAY: {
            Expression_Type_Array * expression_type_array = (Expression_Type_Array *)expression;

            find_dependencies(unit, enclosing_environment, environment, expression_type_array->type);

            if (expression_type_array->length != NULL) {
                find_dependencies(unit, enclosing_environment, environment, expression_type_array->length);
            }
        } break;

        case EXPRESSION_TYPE_FUNCTION: {
            Expression_Type_Function * expression_type_function = (Expression_Type_Function *)expression;

            for (Function_Parameter const & parameter : expression_type_function->parameters) {
                find_dependencies(unit, enclosing_environment, environment, parameter.type);
            }

            if (expression_type_function->return_type != NULL) {
                find_dependencies(unit, enclosing_environment, environment, expression_type_function->return_type);
            }
        } break;

        case EXPRESSION_ARRAY: {
            Expression_Array * expression_array = (Expression_Array *)expression;

            for (Expression * expression : expression_array->expressions) {
                find_dependencies(unit, enclosing_environment, environment, expression);
            }
        } break;

        case EXPRESSION_IMPORT:
        case EXPRESSION_LINKER: {
            ASSERT_NOT_REACHED();
        } break;
    }
}

b32 find_dependencies(Unit & unit) {
    Environment * global_environment = make_global_environment();

    Environment * environment = push_scope(global_environment);

    find_dependencies(unit, global_environment, environment, unit.tree);

    return TRUE;
}

b32 are_all_dependencies_fulfilled(Unit & unit, Resolver & resolver) {
    b32 are_all_dependencies_fulfilled = TRUE;
    for (Dependency & dependency : unit.dependencies) {
        if (!dependency.fulfilled) {
            if (find_symbol(resolver.tail, dependency.label) != NULL) {
                dependency.fulfilled = TRUE;
            } else {
                are_all_dependencies_fulfilled = FALSE;
            }
        }
    }

    return are_all_dependencies_fulfilled;
}
