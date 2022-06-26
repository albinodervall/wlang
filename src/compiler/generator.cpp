struct Generator {
    Unit & unit;
    Code code;

    Array<Register> registers;
};

// These are used to generate symbol names... would be nice to remove them and generate
// the names a different way.
u64 string_counter = 0;
u64 array_counter = 0;

template<typename Function>
void traverse(Instruction const * instructions, u64 number_of_instructions, Function function) {
    for (u64 instruction_index = 0; instruction_index < number_of_instructions; instruction_index += 1) {
        Instruction const & instruction = instructions[instruction_index];
        function(instruction);
    }
}

Address here(Generator & generator) {
    return (Address)(generator.code.buffer.number_of_bytes / sizeof(Instruction));
}

Address generate(Generator & generator, Instruction const & instruction) {
    Address address = here(generator);

    buffer_write(generator.code.buffer, &instruction, sizeof(instruction));

    return address;
}

void generate_move(Generator & generator, Register_Index source_base, u32 source_displacement, Register_Index source_index, u32 source_scale, u32 source_dereference, u32 source_sign_extend, u32 source_size, Register_Index destination_base, u32 destination_displacement, u32 destination_dereference, u32 destination_size) {
    Addressing_Mode source_addressing_mode = ADDRESSING_MODE_DIRECT;
    if (source_dereference) {
        source_addressing_mode = ADDRESSING_MODE_INDIRECT;
    }

    Addressing_Mode destination_addressing_mode = ADDRESSING_MODE_DIRECT;
    if (destination_dereference) {
        destination_addressing_mode = ADDRESSING_MODE_INDIRECT;
    }

    Instruction instruction{
        .type = INSTRUCTION_MOVE,

        .move = {
            .from = {
                .mode = source_addressing_mode,
                .base = source_base,
                .index = source_index,
                .scale = source_scale,
                .displacement = source_displacement,
                // Since this is usually a result of a & with TYPE_FLAG_SIGNED, we lower it down to the first bit here
                .sign_extend = (u32)(source_sign_extend ? 1 : 0),
                .size = source_size,
            },

            .to = {
                .mode = destination_addressing_mode,
                .base = destination_base,
                .index = INVALID_REGISTER_INDEX,
                .scale = 0,
                .displacement = destination_displacement,
                .sign_extend = 0,
                .size = destination_size,
            },
        },
    };

    generate(generator, instruction);
}

void generate_move_immediate(Generator & generator, Register_Index destination_register_index, u32 destination_displacement, u32 destination_dereference, u32 destination_size, u64 immediate) {
    Addressing_Mode destination_addressing_mode = ADDRESSING_MODE_DIRECT;
    if (destination_dereference) {
        destination_addressing_mode = ADDRESSING_MODE_INDIRECT;
    }

    Instruction instruction{
        .type = INSTRUCTION_MOVE,

        .move = {
            .from = {
                .mode = ADDRESSING_MODE_IMMEDIATE,
                .immediate = immediate,
                .size = 8,
            },

            .to = {
                .mode = destination_addressing_mode,
                .base = destination_register_index,
                .index = INVALID_REGISTER_INDEX,
                .scale = 0,
                .displacement = destination_displacement,
                .size = destination_size,
            },
        },
    };

    generate(generator, instruction);
}

void generate_move_relocated_symbol_pointer(Generator & generator, Register_Index destination_register_index, u32 destination_displacement, Symbol * symbol) {
    ASSERT(generator.registers[destination_register_index].type->type == TYPE_POINTER);

    Instruction instruction{
        .type = INSTRUCTION_MOVE_SYMBOL_POINTER,
        .symbol = symbol,
        .move = {
            .to = {
                .mode = ADDRESSING_MODE_INDIRECT,
                .base = destination_register_index,
                .index = 0,
                .scale = 0,
                .displacement = destination_displacement,
                .size = 8
            }
        }
    };

    generate(generator, instruction);
}

Register_Index generate_register(Generator & generator, Type * type) {
    Register_Index register_index = generator.registers.number_of_elements;

    ASSERT(type == &type_none || type->alignment == 1 || type->alignment == 2 || type->alignment == 4 || type->alignment == 8);

    push(generator.registers, Register{
        .type = type,
    });

    return register_index;
}

Register_Index generate_immediate_register(Generator & generator, u64 immediate, Type * type) {
    Register_Index register_index = generator.registers.number_of_elements;

    push(generator.registers, Register{
        .type = type,
    });

    generate_move_immediate(generator, register_index, /* displacement */ 0, /* dereference */ 0, /* size */ type->size, immediate);

    return register_index;
}

Register_Index generate_pointer(Generator & generator, Register_Index value_register_index, Type * type) {
    Register_Index pointer_register_index = generate_register(generator, type_pointer(type));

    generate(generator, Instruction{
        .type = INSTRUCTION_POINTER,
        .a_register_index = value_register_index,
        .b_register_index = pointer_register_index,
    });

    return pointer_register_index;
}

Register_Index generate_empty_registers(Generator & generator, Type * type) {
    if ((type->flags & TYPE_FLAG_INTEGRAL) || type == &type_boolean) {
        return generate_register(generator, type);
    } else if (type->type == TYPE_STRING) {
        return generate_register(generator, type_string());
    } else if (type->type == TYPE_POINTER) {
        return generate_register(generator, type);
    } else if (type->type == TYPE_ARRAY) {
        return generate_register(generator, type);
    } else if (type->type == TYPE_SPAN) {
        return generate_register(generator, type);
    } else if (type == &type_none) {
        return INVALID_REGISTER_INDEX;
    } else if (type->type == TYPE_STRUCTURE) {
        return generate_register(generator, type);
    } else {
        // print("Type: {}\n", type);
        ASSERT_NOT_REACHED();
    }
}

void patch_immediate(Generator & generator, u64 instruction_index, u64 immediate) {
    Instruction & instruction = *(Instruction *)buffer_peek(generator.code.buffer, instruction_index * sizeof(Instruction), sizeof(Instruction));
    instruction.immediate = immediate;
}

// If the type is complex, we need to copy eacy member individually
// TODO: This should act more like memcpy instead and just copy the bytes!
void generate_copy(Generator & generator, Register_Index right_register_index, Type * source_type, Register_Index left_register_index, Type * destination_type) {
    if (type_is_complex(source_type)) {
        if (destination_type->type == TYPE_POINTER && destination_type->pointer.base == source_type) {
            Register_Index source_register_index = generate_pointer(generator, right_register_index, source_type);

            for (u64 field_index = 0; field_index < source_type->fields.number_of_elements; field_index += 1) {
                Structure_Field const & field = source_type->fields[field_index];
                generate_move(generator, source_register_index, /* displacement */ field.offset, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 1, /* sign_extend */ field.type->flags & TYPE_FLAG_SIGNED, /* size */ field.type->size, left_register_index, /* displacement */ field.offset, /* dereference */ 1, /* size */ field.type->size);
            }
        } else if (source_type->type == TYPE_ARRAY && destination_type->type == TYPE_POINTER && destination_type->pointer.base->type == TYPE_SPAN) {
            ASSERT(source_type->array.base == destination_type->pointer.base->span.base);

            Register_Index pointer_register_index = generate_pointer(generator, right_register_index, source_type->array.base);

            generate_move(generator, pointer_register_index, /* displacement */ 0, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 0, /* sign_extend */ 0, /* size */ 8, left_register_index, /* displacement */ 0, /* dereference */ 1, /* size */ 8);
            generate_move_immediate(generator, left_register_index, /* displacement */ 8, /* dereference */ 1, /* size */ 8, source_type->array.count); // count
        } else {
            ASSERT(destination_type == source_type);

            Register_Index source_register_index = generate_pointer(generator, right_register_index, source_type);
            Register_Index destination_register_index = generate_pointer(generator, left_register_index, source_type);

            for (u64 field_index = 0; field_index < source_type->fields.number_of_elements; field_index += 1) {
                Structure_Field const & field = source_type->fields[field_index];
                generate_move(generator, source_register_index, /* displacement */ field.offset, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 1, /* sign_extend */ field.type->flags & TYPE_FLAG_SIGNED, /* size */ field.type->size, destination_register_index, /* displacement */ field.offset, /* dereference */ 1, /* size */ field.type->size);
            }
        }
    } else {
        if (destination_type->type == TYPE_POINTER && destination_type->pointer.base == source_type) {
            generate_move(generator, right_register_index, /* displacement */ 0, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 0, /* sign extend */ source_type->flags & TYPE_FLAG_SIGNED, source_type->size, left_register_index, /* displacement */ 0, /* dereference */ 1, source_type->size);
        } else {
            ASSERT(source_type == destination_type);
            generate_move(generator, right_register_index, /* displacement */ 0, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 0, /* sign extend */ source_type->flags & TYPE_FLAG_SIGNED, source_type->size, left_register_index, /* displacement */ 0, /* dereference */ 0, source_type->size);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum Generate_Flags {
    GENERATE_COPY = 0,
    GENERATE_POINTER = (1 << 0),
};

Register_Index generate(Generator & generator, Expression * expression, Symbol * declaration, Generate_Flags flags);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Register_Index generate_integer(Generator & generator, Expression * expression, Symbol * declaration, Generate_Flags flags) {
    ASSERT(!(flags & GENERATE_POINTER));

    Type * type = expression->resolved_type;
    // ASSERT(type != &type_integral);

    Register_Index register_index = generate_register(generator, type);
    generate_move_immediate(generator, register_index, /* displacement */ 0, /* dereference */ 0, /* size */ type->size, expression->integer);

    // TODO: This can cause overlow! It should be detected in the parser!
    if (expression->flags & EXPRESSION_FLAGS_NEGATIVE) {
        Register_Index left_register_index = generate_register(generator, type);
        generate_move_immediate(generator, left_register_index, /* displacement */ 0, /* dereference */ 0, /* size */ type->size, /* immediate */ 0);

        generate(generator, Instruction{
            .type = INSTRUCTION_SUBTRACT,
            .a_register_index = left_register_index,
            .b_register_index = register_index,
            .c_register_index = register_index,
        });
    }

    return register_index;
}

Register_Index generate_string(Generator & generator, Expression * expression, Symbol * declaration, Generate_Flags flags) {
    utf8 const * string = expression->string;

    Register_Index register_index = generate_register(generator, type_string());

    Formatter formatter = utf8_unescape(string);
    u64 size = formatter.offset - 1; // Remove the null terminator
    utf8 const * value = formatter.pointer;

    Symbol & symbol = push(generator.unit.exports, Symbol{
        .flags = DECLARATION_FLAGS_CONSTANT | DECLARATION_FLAGS_NO_ADDRESS,
        .name = format("string.{}", string_counter), // TODO
        .value = {
            .type = type_string(),
            .size = size,
            .value = value
        }
    });


    string_counter += 1; // TODO

    Register_Index pointer_register_index = generate_pointer(generator, register_index, type_string());

    generate_move_relocated_symbol_pointer(generator, pointer_register_index, /* displacement */ 0, &symbol);

    generate_move_immediate(generator, pointer_register_index, /* displacement */8, /* dereference */ 1, /* size */ 8, /* immediate */ size);

    if (flags & GENERATE_POINTER) {
        return pointer_register_index;
    } else {
        return register_index;
    }
}

Register_Index generate_identifier(Generator & generator, Expression * expression, Symbol * declaration, Generate_Flags flags) {
    Symbol * identified = find_symbol(expression->environment, expression->identifier);

    if (flags & GENERATE_POINTER) {
        ASSERT(identified->value.register_index != INVALID_REGISTER_INDEX);
        return generate_pointer(generator, identified->value.register_index, generator.registers[identified->value.register_index].type);
    } else {
        if (identified->flags & DECLARATION_FLAGS_FUNCTION) {
            Register_Index register_index = generate_register(generator, identified->value.type);

            Register_Index pointer_register_index = generate_pointer(generator, register_index, generator.registers[register_index].type);
            generate_move_relocated_symbol_pointer(generator, pointer_register_index, /* displacement */ 0, identified);

            return register_index;
        } else if (identified->value.type->type == TYPE_ARRAY) {
            return identified->value.register_index;
        } else if (type_is_complex(generator.registers[identified->value.register_index].type)) {
            Type * type = generator.registers[identified->value.register_index].type;

            Register_Index source_register_index = generate_pointer(generator, identified->value.register_index, type);

            Register_Index register_index = generate_register(generator, type);
            Register_Index destination_register_index = generate_pointer(generator, register_index, generator.registers[register_index].type);

            for (u64 field_index = 0; field_index < type->fields.number_of_elements; field_index += 1) {
                Structure_Field const & field = type->fields[field_index];

                generate_move(generator, source_register_index, /* displacement */ field.offset, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 1, /* sign_extend */ field.type->flags & TYPE_FLAG_SIGNED, /* size */ field.type->size, destination_register_index, /* displacement */ field.offset, /* dereference */ 1, /* size */ field.type->size);
            }

            return register_index;
        } else {
            Type * type = generator.registers[identified->value.register_index].type;

            Register_Index register_index = generate_register(generator, type);
            generate_move(generator, identified->value.register_index, /* displacement */ 0, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 0, type->flags & TYPE_FLAG_SIGNED, type->size, register_index, /* displacement */ 0, /* dereference */ 0, generator.registers[register_index].type->size);

            return register_index;
        }
    }
}

Register_Index generate_unary_boolean_not(Generator & generator, Expression * expression, Symbol * declaration, Generate_Flags flags) {
    ASSERT(!(flags & GENERATE_POINTER));

    Register_Index left_register_index = generate(generator, expression->right, declaration, GENERATE_COPY);
    Register_Index right_register_index = generate_register(generator, expression->right->resolved_type);
    Register_Index result_register_index = generate_register(generator, expression->right->resolved_type);

    generate_move_immediate(generator, right_register_index, /* displacement */ 0, /* dereference */ 0, /* size */ type_boolean.size, /* immediate */ 1);

    generate(generator, Instruction{
        .type = INSTRUCTION_XOR,
        .a_register_index = left_register_index,
        .b_register_index = right_register_index,
        .c_register_index = result_register_index,
    });

    return result_register_index;
}

Register_Index generate_unary_minus(Generator & generator, Expression * expression, Symbol * declaration, Generate_Flags flags) {
    ASSERT(!(flags & GENERATE_POINTER));

    Register_Index left_register_index = generate_register(generator, expression->right->resolved_type);
    Register_Index right_register_index = generate(generator, expression->right, declaration, GENERATE_COPY);
    Register_Index result_register_index = generate_register(generator, expression->right->resolved_type);

    generate_move_immediate(generator, left_register_index, /* displacement */ 0, /* dereference */ 0, /* size */ expression->right->resolved_type->size, /* immediate */ 0);

    generate(generator, Instruction{
        .type = INSTRUCTION_SUBTRACT,
        .a_register_index = left_register_index,
        .b_register_index = right_register_index,
        .c_register_index = result_register_index,
    });

    return result_register_index;
}

Register_Index generate_unary_bitwise_not(Generator & generator, Expression * expression, Symbol * declaration, Generate_Flags flags) {
    ASSERT(!(flags & GENERATE_POINTER));

    Register_Index register_index = generate(generator, expression->right, declaration, GENERATE_COPY);
    Register_Index result_register_index = generate_register(generator, expression->right->resolved_type);

    generate(generator, Instruction{
        .type = INSTRUCTION_NOT,
        .a_register_index = register_index,
        .b_register_index = result_register_index,
    });

    return result_register_index;
}

Register_Index generate_binary(Generator & generator, Expression * expression, Symbol * declaration, Generate_Flags flags) {
    ASSERT(!(flags & GENERATE_POINTER));

    Register_Index left_register_index = generate(generator, expression->left, declaration, GENERATE_COPY);
    Register_Index right_register_index = generate(generator, expression->right, declaration, GENERATE_COPY);

    ASSERT(generator.registers[left_register_index].type == generator.registers[right_register_index].type);

    Instruction_Type instruction_type;
    switch (expression->type) {
        case EXPRESSION_BINARY_ADDITION: { instruction_type = INSTRUCTION_ADD; } break;
        case EXPRESSION_BINARY_SUBTRACTION: { instruction_type = INSTRUCTION_SUBTRACT; } break;
        case EXPRESSION_BINARY_BITWISE_OR: { instruction_type = INSTRUCTION_OR; } break;
        case EXPRESSION_BINARY_BITWISE_XOR: { instruction_type = INSTRUCTION_XOR; } break;
        case EXPRESSION_BINARY_COMPARISON_LESS_THAN: { instruction_type = INSTRUCTION_LESS_THAN; } break;
        case EXPRESSION_BINARY_COMPARISON_GREATER_THAN: { instruction_type = INSTRUCTION_GREATER_THAN; } break;
        case EXPRESSION_BINARY_COMPARISON_LESS_THAN_OR_EQUAL_TO: { instruction_type = INSTRUCTION_LESS_THAN_OR_EQUAL_TO; } break;
        case EXPRESSION_BINARY_COMPARISON_GREATER_THAN_OR_EQUAL_TO: { instruction_type = INSTRUCTION_GREATER_THAN_OR_EQUAL_TO; } break;
        case EXPRESSION_BINARY_COMPARISON_EQUALS: { instruction_type = INSTRUCTION_EQUALS; } break;
        case EXPRESSION_BINARY_COMPARISON_NOT_EQUALS: { instruction_type = INSTRUCTION_NOT_EQUALS; } break;
        case EXPRESSION_BINARY_MULTIPLICATION: { instruction_type = INSTRUCTION_MULTIPLY; } break;
        case EXPRESSION_BINARY_DIVISION: { instruction_type = INSTRUCTION_DIVIDE; } break;
        case EXPRESSION_BINARY_BITWISE_AND: { instruction_type = INSTRUCTION_AND; } break;
        case EXPRESSION_BINARY_SHIFT_LEFT: { instruction_type = INSTRUCTION_SHIFT_LEFT; } break;
        case EXPRESSION_BINARY_SHIFT_RIGHT: { instruction_type = INSTRUCTION_SHIFT_RIGHT; } break;
        case EXPRESSION_BINARY_BOOLEAN_AND: { instruction_type = INSTRUCTION_AND; } break;
        case EXPRESSION_BINARY_BOOLEAN_OR: { instruction_type = INSTRUCTION_OR; } break;

        default: {
            ASSERT_NOT_REACHED();
        } break;
    }

    ASSERT(expression->left->resolved_type == expression->right->resolved_type);
    Register_Index result_register_index = generate_register(generator, expression->resolved_type);

    generate(generator, Instruction{
        .type = instruction_type,
        .left_register_index = left_register_index,
        .right_register_index = right_register_index,
        .result_register_index = result_register_index,
    });

    return result_register_index;
}

Register_Index generate_invocation(Generator & generator, Expression_Invocation * expression, Symbol * declaration, Generate_Flags flags) {
    ASSERT(!(flags & GENERATE_POINTER));

    Register_Index result_register_index = generate_empty_registers(generator, expression->resolved_type);

    Type * invoked_type = expression->invoked->resolved_type;

    Type * invoked_function_type = NULL;
    if (invoked_type->type == TYPE_POINTER && invoked_type->pointer.base->type == TYPE_FUNCTION) {
        invoked_function_type = invoked_type->pointer.base;
    } else if (invoked_type->type == TYPE_FUNCTION) {
        invoked_function_type = invoked_type;
    } else {
        ASSERT_NOT_REACHED();
    }

    Array<Register_Index> argument_register_indices;

    // Generate code for each of the arguments
    u64 number_of_arguments_generated = 0;
    for (Argument & argument : expression->arguments) {
        // TODO: Rewrite this to a recursive function...
        Register_Index register_index = generate(generator, argument.value, declaration, GENERATE_COPY);

        Type * argument_type = generator.registers[register_index].type;
        Type * parameter_type = unwrap_type(invoked_function_type->function.parameters[number_of_arguments_generated]);

        // print("argument_type = {}\n", argument_type);
        // print("parameter_type = {}\n", parameter_type);

        if (argument_type->type == TYPE_POINTER && parameter_type->type == TYPE_POINTER && parameter_type->pointer.base == &type_void) {
            push(argument_register_indices, register_index);
        } else if (argument_type->type == TYPE_ARRAY && parameter_type->type == TYPE_SPAN) {
            // We allow passing arrays to spans, assuming their types match
            ASSERT(argument_type->array.base == parameter_type->span.base);

            Register_Index pointer_register_index = generate_pointer(generator, register_index, argument_type->array.base);

            Register_Index span_register_index = generate_register(generator, parameter_type);

            Register_Index pointer_span_register_index = generate_pointer(generator, span_register_index, parameter_type);
            generate_move(generator, pointer_register_index, /* displacement */ 0, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 0, /* sign_extend */ 0, /* size */ 8, pointer_span_register_index, /* displacement */ 0, /* dereference */ 1, /* size */ 8);
            generate_move_immediate(generator, pointer_span_register_index, /* displacement */ 8, /* dereference */ 1, /* size */ 8, argument_type->array.count); // count

            push(argument_register_indices, span_register_index);
        } else if (argument_type->type == TYPE_FUNCTION && parameter_type->type == TYPE_POINTER && parameter_type->pointer.base == argument_type) {
            push(argument_register_indices, register_index);
        } else if (argument_type->type == TYPE_POINTER && parameter_type->type == TYPE_POINTER) {
            ASSERT(argument_type->pointer.base == parameter_type->pointer.base || argument_type->pointer.base == &type_void || parameter_type->pointer.base == &type_void);
            push(argument_register_indices, register_index);
        } else {
            ASSERT(argument_type == parameter_type);
            push(argument_register_indices, register_index);
        }

        number_of_arguments_generated += 1;
    }

    // Generate the remaining arguments
    if (number_of_arguments_generated < invoked_function_type->function.parameters.number_of_elements) {
        ASSERT(invoked_type->type == TYPE_FUNCTION);
        ASSERT(expression->invoked->resolved_symbol != NULL);
        Symbol * invoked_symbol = expression->invoked->resolved_symbol;

        Type * invoked_type = expression->invoked->resolved_type;
        ASSERT(invoked_type != NULL);

        for (u64 argument_index = number_of_arguments_generated; argument_index < invoked_symbol->value.parameters.number_of_elements; argument_index += 1) {
            Symbol_Function_Parameter & default_parameter = invoked_symbol->value.parameters[argument_index];

            Register_Index register_index = generate(generator, default_parameter.default_value, declaration, GENERATE_COPY);
            push(argument_register_indices, register_index);
        }
    }

    Register_Index first_argument_register_index = INVALID_REGISTER_INDEX;

    // Copy each of the arguments into new registers
    if (argument_register_indices.number_of_elements > 0) {
        first_argument_register_index = generator.registers.number_of_elements;
        for (Register_Index argument_register_index : argument_register_indices) {
            Type * argument_type = generator.registers[argument_register_index].type;
            Register_Index copy_argument_register_index = generate_register(generator, argument_type);

            generate_copy(generator, argument_register_index, argument_type, copy_argument_register_index, argument_type);
        }
    }

    if (invoked_type->type == TYPE_POINTER && invoked_type->pointer.base->type == TYPE_FUNCTION) {
        Register_Index invoked_register_index = generate(generator, expression->invoked, declaration, GENERATE_COPY);

        Instruction instruction{
            .type = INSTRUCTION_CALL_POINTER,
            .a_register_index = first_argument_register_index,
            .b_register_index = result_register_index,
            .c_register_index = invoked_register_index,
        };

        generate(generator, instruction);
    } else if (invoked_type->type == TYPE_FUNCTION) {
        // TODO: We should really use relocations here instead of this weird thing
        //       where we pass the name of the symbol to call in the instruction...
        Symbol * symbol = expression->invoked->resolved_symbol;
        ASSERT(symbol != NULL);

        Instruction instruction{
            .type = INSTRUCTION_CALL,
            .a_register_index = first_argument_register_index,
            .b_register_index = result_register_index,
            .symbol = symbol,
        };

        generate(generator, instruction);
    } else {
        ASSERT_NOT_REACHED();
    }

    Register_Index register_index = result_register_index;

    if (register_index != INVALID_REGISTER_INDEX) {
        if (flags & GENERATE_POINTER) {
            return register_index;
        } else {
            Type * type = generator.registers[register_index].type;
            Register_Index destination_register_index = generate_register(generator, type);
            generate_move(generator, register_index, /* displacement */ 0, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 0, type->flags & TYPE_FLAG_SIGNED, type->size, destination_register_index, /* displacement */ 0, /* dereference */ 0, type->size);

            return destination_register_index;
        }
    } else {
        return register_index;
    }
}

Register_Index generate_field(Generator & generator, Expression_Field * expression, Symbol * declaration, Generate_Flags flags) {
    // print(">> EXPRESSION_FIELD\n");
    Expression * accessed = expression->accessed;
    Expression * accessor = expression->accessor;

    bool is_pointer = false;

    Type * accessed_type = accessed->resolved_type;
    if (accessed_type->type == TYPE_POINTER) {
        accessed_type = accessed_type->pointer.base;
        is_pointer = true;
    }

    for (int field_index = 0; field_index < (int)accessed_type->fields.number_of_elements; field_index += 1) {
        Structure_Field const & field = accessed_type->fields[field_index];

        if (utf8_compare(field.name, accessor->identifier)) {
            Register_Index pointer_register_index = INVALID_REGISTER_INDEX;

            if (is_pointer) {
                pointer_register_index = generate(generator, accessed, declaration, GENERATE_COPY);
                ASSERT(generator.registers[pointer_register_index].type == accessed->resolved_type);
            } else {
                pointer_register_index = generate(generator, accessed, declaration, GENERATE_POINTER);
                ASSERT(generator.registers[pointer_register_index].type == type_pointer(accessed->resolved_type));
            }

            if (flags & GENERATE_POINTER) {
                // TODO: There is an optimization to be done here if the offset is 0!
                Register_Index index_register_index = generate_register(generator, &type_u64);
                generate_move_immediate(generator, index_register_index, /* displacement */ 0, /* dereference */ 0, /* size */ 8, field.offset);

                Register_Index field_pointer_register_index = generate_register(generator, type_pointer(field.type));

                generate(generator, Instruction{
                    .type = INSTRUCTION_ADD,
                    .left_register_index = pointer_register_index,
                    .right_register_index = index_register_index,
                    .result_register_index = field_pointer_register_index,
                });

                return field_pointer_register_index;
            } else {
                ASSERT(expression->resolved_type == field.type);

                Register_Index value_register_index = generate_register(generator, expression->resolved_type);
                generate_move(generator, pointer_register_index, /* displacement */ field.offset, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 1, /* sign extend */ 0, /* size */ expression->resolved_type->size, value_register_index, /* displacement */ 0, /* dereference */ 0, /* size */ expression->resolved_type->size);

                return value_register_index;
            }
        }
    }

    // print("Could not find field {}\n", accessor->identifier);
    // print("Fields are:\n");
    // for (int field_index = 0; field_index < (int)accessed_type->fields.number_of_elements; field_index += 1) {
    //     Structure_Field const & field = accessed_type->fields[field_index];
    //     print("  #{}: {} ({})\n", field_index, field.name, field.type);
    // }

    ASSERT_NOT_REACHED();
}

Register_Index generate_index(Generator & generator, Expression_Index * expression, Symbol * declaration, Generate_Flags flags) {
    Expression * indexed = expression->indexed;
    // print("EXPRESSION_INDEX, indexed is of type {}\n", indexed->resolved_type);

    Expression * index = expression->index;

    ASSERT(indexed->resolved_type->type == TYPE_ARRAY || indexed->resolved_type->type == TYPE_SPAN || indexed->resolved_type->type == TYPE_STRING);

    Register_Index indexed_register_index = generate(generator, indexed, declaration, GENERATE_POINTER);
    ASSERT(indexed_register_index != INVALID_REGISTER_INDEX);

    Register_Index index_register_index = generate(generator, index, declaration, GENERATE_COPY);
    Type * index_type = generator.registers[index_register_index].type;

    // TODO: Generate optional array bounds checking!

    Type * element_type = NULL;
    if (indexed->resolved_type->type == TYPE_ARRAY) {
        element_type = indexed->resolved_type->array.base;
    } else if (indexed->resolved_type->type == TYPE_SPAN) {
        element_type = indexed->resolved_type->span.base;
    } else if (indexed->resolved_type->type == TYPE_STRING) {
        element_type = &type_u8;
    }

    Register_Index pointer_register_index = INVALID_REGISTER_INDEX;
    if (indexed->resolved_type->type == TYPE_SPAN || indexed->resolved_type->type == TYPE_STRING) {
        pointer_register_index = generate_register(generator, type_pointer(element_type));
        generate_move(generator, indexed_register_index, /* displacement */ 0, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 1, /* sign extend */ 0, /* size */ 8, pointer_register_index, /* displacement */ 0, /* dereference */ 0, /* size */ 8);
    } else if (indexed->resolved_type->type == TYPE_ARRAY) {
        pointer_register_index = generate_register(generator, type_pointer(element_type));
        generate_move(generator, indexed_register_index, /* displacement */ 0, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 0, /* sign extend */ 0, /* size */ 8, pointer_register_index, /* displacement */ 0, /* dereference */ 0, /* size */ 8);
    } else {
        ASSERT_NOT_REACHED();
    }

    ASSERT(pointer_register_index != INVALID_REGISTER_INDEX);

    if (flags & GENERATE_POINTER) {
        // If we want to generate the pointer itself, we cannot rely on the index * scale calculation of the move instruction

        // Strings are bytes so we don't need to do anything to the index
        if (element_type->size != 1) {
            Register_Index operand_register_index = generate_register(generator, index_type);
            generate_move_immediate(generator, operand_register_index, /* displacement */ 0, /* dereference */ 0, /* size */ index_type->size, element_type->size);

            generate(generator, Instruction{
                .type = INSTRUCTION_MULTIPLY,
                .left_register_index = index_register_index,
                .right_register_index = operand_register_index,
                .result_register_index = index_register_index,
            });
        }

        generate(generator, Instruction{
            .type = INSTRUCTION_ADD,
            .left_register_index = pointer_register_index,
            .right_register_index = index_register_index,
            .result_register_index = pointer_register_index,
        });

        return pointer_register_index;
    } else {
        Register_Index destination_register_index = generate_register(generator, element_type);
        generate_move(generator, pointer_register_index, /* displacement */ 0, /* index */ index_register_index, /* scale */ element_type->size, /* dereference */ 1, /* sign extend */ element_type->flags & TYPE_FLAG_SIGNED, /* size */ element_type->size, destination_register_index, /* displacement */ 0, /* dereference */ 0, element_type->size);

        return destination_register_index;
    }
}

Register_Index generate_true(Generator & generator, Expression * expression, Symbol * declaration, Generate_Flags flags) {
    ASSERT(!(flags & GENERATE_POINTER));

    Register_Index register_index = generate_register(generator, &type_boolean);
    generate_move_immediate(generator, register_index, /* displacement */ 0, /* dereference */ 0, /* size */ type_boolean.size, 1);

    return register_index;
}

Register_Index generate_false(Generator & generator, Expression * expression, Symbol * declaration, Generate_Flags flags) {
    ASSERT(!(flags & GENERATE_POINTER));

    Register_Index register_index = generate_register(generator, &type_boolean);
    generate_move_immediate(generator, register_index, /* displacement */ 0, /* dereference */ 0, /* size */ type_boolean.size, 0);

    return register_index;
}

Register_Index generate_function(Generator & generator, Expression_Function * expression, Symbol * declaration, Generate_Flags flags) {
    ASSERT(!(flags & GENERATE_POINTER));

    ASSERT(declaration != NULL);
    generate(generator, expression->body, declaration, GENERATE_COPY);

    return INVALID_REGISTER_INDEX;
}

Register_Index generate_if(Generator & generator, Expression_If * expression, Symbol * declaration, Generate_Flags flags) {
    ASSERT(!(flags & GENERATE_POINTER));

    Type * result_type = &type_none;

    Register_Index result_register_index = INVALID_REGISTER_INDEX;
    if (expression->resolved_type != &type_none) {
        result_register_index = generate_empty_registers(generator, expression->resolved_type);
        result_type = generator.registers[result_register_index].type;
    }

    Register_Index condition_register_index = generate(generator, expression->condition, declaration, GENERATE_COPY);

    u64 else_offset = generate(generator, Instruction{
        .type = INSTRUCTION_JUMP_IF_ZERO,
        .a_register_index = condition_register_index,
        .immediate = 0
    });

    Register_Index then_register_index = generate(generator, expression->then_expression, declaration, GENERATE_COPY);
    if (then_register_index != INVALID_REGISTER_INDEX) {
        ASSERT(generator.registers[then_register_index].type == result_type);
        generate_move(generator, then_register_index, /* displacement */ 0, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 0, /* sign */ result_type->flags & TYPE_FLAG_SIGNED, /* size */ result_type->size, result_register_index, /* displacement */ 0, /* dereference */ 0, /* size */ result_type->size);
    }

    u64 after_offset = generate(generator, Instruction{
        .type = INSTRUCTION_JUMP,
        .immediate = 0
    });

    Address else_address = here(generator);
    patch_immediate(generator, else_offset, else_address);

    if (expression->else_expression != NULL) {
        Register_Index else_register_index = generate(generator, expression->else_expression, declaration, GENERATE_COPY);
        if (else_register_index != INVALID_REGISTER_INDEX) {
            generate_move(generator, else_register_index, /* displacement */ 0, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 0, /* sign */ result_type->flags & TYPE_FLAG_SIGNED, /* size */ result_type->size, result_register_index, /* displacement */ 0, /* dereference */ 0, /* size */ result_type->size);
        }
    }

    Address after_address = here(generator);
    patch_immediate(generator, after_offset, after_address);

    return result_register_index;
}

Register_Index generate_for(Generator & generator, Expression_For * expression, Symbol * declaration, Generate_Flags flags) {
    ASSERT(!(flags & GENERATE_POINTER));

    Expression * iterable = expression->expression;
    Type * iterable_type = iterable->resolved_type;
    ASSERT(iterable_type->type == TYPE_RANGE || iterable_type->type == TYPE_SPAN || iterable_type->type == TYPE_STRING || iterable_type->type == TYPE_ARRAY);

    // Index
    Expression * index = expression->index;

    Type * index_type = NULL;
    if (index != NULL) {
        index_type = index->resolved_type;
    } else {
        if (iterable_type->type == TYPE_SPAN || iterable_type->type == TYPE_STRING || iterable_type->type == TYPE_ARRAY) {
            index_type = &type_u64;
        } else if (iterable_type->type == TYPE_RANGE) {
            index_type = iterable->left->resolved_type;
        } else {
            ASSERT_NOT_REACHED();
        }
    }

    Register_Index index_register_index =  generate_register(generator, index_type);
    generate_move_immediate(generator, index_register_index, /* displacement */ 0, /* dereference */ 0, /* size */ index_type->size, 0);

    if (index != NULL) {
        ASSERT(index->type == EXPRESSION_IDENTIFIER);
        index->resolved_symbol->value.register_index = index_register_index;
    }

    // Value
    Expression * value = expression->value;

    Type * value_type = value->resolved_type;

    Register_Index value_register_index = generate_register(generator, value_type);

    ASSERT(value->type == EXPRESSION_IDENTIFIER);
    value->resolved_symbol->value.register_index = value_register_index;

    // Loop
    Register_Index increment_register_index = generate_register(generator, index_type);
    generate_move_immediate(generator, increment_register_index, /* displacement */ 0, /* dereference */ 0, /* size */ index_type->size, 1);

    Address begin = here(generator);

    // 1. Condition
    Register_Index termination_register_index = generate_register(generator, index_type);

    if (iterable_type->type == TYPE_SPAN || iterable_type->type == TYPE_STRING) {
        Register_Index register_index = generate(generator, iterable, declaration, GENERATE_POINTER);
        generate_move(generator, register_index, /* displacement */ 8, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 1, /* sign */ 0, /* size */ 8, termination_register_index, /* displacement */ 0, /* dereference */ 0, /* size */ index_type->size);
    } else if (iterable_type->type == TYPE_RANGE) {
        Register_Index low_register_index = generate(generator, iterable->left, declaration, GENERATE_COPY);
        Register_Index high_register_index = generate(generator, iterable->right, declaration, GENERATE_COPY);

        generate(generator, Instruction{
            .type = INSTRUCTION_SUBTRACT,
            .left_register_index = high_register_index,
            .right_register_index = low_register_index,
            .result_register_index = termination_register_index,
        });
    } else if (iterable_type->type == TYPE_ARRAY) {
        generate_move_immediate(generator, termination_register_index, /* displacement */ 0, /* dereference */ 0, /* size */ index_type->size, iterable_type->array.count);
    } else {
        ASSERT_NOT_REACHED();
    }

    Register_Index condition_register_index = generate_register(generator, &type_boolean);

    if (iterable_type->type == TYPE_SPAN || iterable_type->type == TYPE_STRING || iterable_type->type == TYPE_ARRAY) {
        generate(generator, Instruction{
            .type = INSTRUCTION_LESS_THAN,
            .left_register_index = index_register_index,
            .right_register_index = termination_register_index,
            .result_register_index = condition_register_index,
        });
    } else if (iterable_type->type == TYPE_RANGE) {
        // Ranges are inclusive
        generate(generator, Instruction{
            .type = INSTRUCTION_LESS_THAN_OR_EQUAL_TO,
            .left_register_index = index_register_index,
            .right_register_index = termination_register_index,
            .result_register_index = condition_register_index,
        });
    } else {
        ASSERT_NOT_REACHED();
    }

    u64 after_offset = generate(generator, Instruction{
        .type = INSTRUCTION_JUMP_IF_ZERO,
        .a_register_index = condition_register_index,
        .immediate = 0
    });

    // 2. Load
    if (iterable_type->type == TYPE_SPAN || iterable_type->type == TYPE_STRING || iterable_type->type == TYPE_ARRAY) {
        Type * element_type = NULL;
        if (iterable_type->type == TYPE_SPAN) {
            element_type = iterable_type->span.base;
        } else if (iterable_type->type == TYPE_STRING) {
            element_type = &type_u8;
        } else if (iterable_type->type == TYPE_ARRAY) {
            element_type = iterable_type->array.base;
        } else {
            ASSERT_NOT_REACHED();
        }

        u64 element_size = element_type->size;

        // TODO: Generate optional array bounds checking!
        Register_Index pointer_register_index = INVALID_REGISTER_INDEX;
        if (iterable_type->type == TYPE_SPAN || iterable_type->type == TYPE_STRING) {
            Register_Index iterable_register_index = generate(generator, iterable, declaration, GENERATE_POINTER);
            pointer_register_index = generate_register(generator, type_pointer(element_type));
            generate_move(generator, iterable_register_index, /* displacement */ 0, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 1, /* sign */ 0, /* size */ 8, pointer_register_index, /* displacement */ 0, /* dereference */ 0, generator.registers[pointer_register_index].type->size);
        } else if (iterable_type->type == TYPE_ARRAY) {
            // TODO: Does this make a copy of the array...?
            pointer_register_index = generate(generator, iterable, declaration, GENERATE_POINTER);
        } else {
            ASSERT_NOT_REACHED();
        }

        ASSERT(pointer_register_index != INVALID_REGISTER_INDEX);

        // TODO: This only works when iterating over non complex types?
        ASSERT(!type_is_complex(element_type));

        generate_move(generator, pointer_register_index, /* displacement */ 0, /* index */ index_register_index, /* scale */ element_size, /* dereference */ 1, /* sign extend */ element_type->flags & TYPE_FLAG_SIGNED, /* size */ element_type->size, value_register_index, /* displacement */ 0, /* dereference */ 0, element_type->size);
    } else if (iterable_type->type == TYPE_RANGE) {
        Register_Index low_register_index = generate(generator, expression->expression->left, declaration, GENERATE_COPY);

        // TODO: Is there an issue here if the value type and index is of different types=?
        generate(generator, Instruction{
            .type = INSTRUCTION_ADD,
            .left_register_index = low_register_index,
            .right_register_index = index_register_index,
            .result_register_index = value_register_index,
        });
    } else {
        ASSERT_NOT_REACHED();
    }

    // 3. Execute
    generate(generator, expression->block, declaration, GENERATE_COPY);

    // 4. Increment
    generate(generator, Instruction{
        .type = INSTRUCTION_ADD,
        .left_register_index = index_register_index,
        .right_register_index = increment_register_index, // TODO: This should be an immediate
        .result_register_index = index_register_index,
    });

    // 5. Jump
    generate(generator, Instruction{
        .type = INSTRUCTION_JUMP,
        .immediate = begin
    });

    Address after = here(generator);
    patch_immediate(generator, after_offset, after);

    return INVALID_REGISTER_INDEX;
}

Register_Index generate_while(Generator & generator, Expression_While * expression, Symbol * declaration, Generate_Flags flags) {
    ASSERT(!(flags & GENERATE_POINTER));

    Address before = here(generator);

    Register_Index condition_register_index = generate(generator, expression->condition, declaration, GENERATE_COPY);

    u64 after_offset = generate(generator, Instruction{
        .type = INSTRUCTION_JUMP_IF_ZERO,
        .a_register_index = condition_register_index,
        .immediate = 0
    });

    generate(generator, expression->block, declaration, GENERATE_COPY);

    generate(generator, Instruction{
        .type = INSTRUCTION_JUMP,
        .immediate = before
    });

    Address after = here(generator);
    patch_immediate(generator, after_offset, after);

    return INVALID_REGISTER_INDEX;
}

Register_Index generate_block(Generator & generator, Expression_Block * expression, Symbol * declaration, Generate_Flags flags) {
    ASSERT(!(flags & GENERATE_POINTER));

    Register_Index result_register_index = INVALID_REGISTER_INDEX;

    Array<Expression *> & expressions = expression->expressions;
    for (Expression * expression : expressions) {
        Register_Index register_index = generate(generator, expression, declaration, GENERATE_COPY);

        if (expression == expressions[expressions.number_of_elements - 1]) {
            if (!(expression->flags & EXPRESSION_FLAGS_EXPLICIT_STATEMENT)) {
                result_register_index = register_index;
            }
        }
    }

    return result_register_index;
}

Register_Index generate_assignment(Generator & generator, Expression * expression, Symbol * declaration, Generate_Flags flags) {
    ASSERT(!(flags & GENERATE_POINTER));

    // print(">> EXPRESSION_ASSIGNMENT\n");

    Register_Index left_register_index = generate(generator, expression->left, declaration, GENERATE_POINTER);
    // This makes a copy of the RHS which is expensive if it's a complex type, we should instead
    // copy directly out of the register! Just do GENERATE_POINTER...
    Register_Index right_register_index = generate(generator, expression->right, declaration, GENERATE_COPY);

    Type * source_type = generator.registers[right_register_index].type;
    Type * destination_type = generator.registers[left_register_index].type;

    // print("want to move {} to {}\n", source_type, destination_type);

    // There is a special case where we allow a pointer to an array to be assigned to a pointer
    // to the array's underlying type.
    // TODO: Clean this up, there could be many pointers down until the array...
    if (source_type->type == TYPE_POINTER && source_type->pointer.base->type == TYPE_ARRAY) {
        Type * underlying_type = source_type->pointer.base->array.base;
        if (destination_type->type == TYPE_POINTER && destination_type->pointer.base->type == TYPE_POINTER && destination_type->pointer.base->pointer.base == underlying_type) {
            source_type = destination_type->pointer.base;
        }
    }

    generate_copy(generator, right_register_index, source_type, left_register_index, destination_type);

    return INVALID_REGISTER_INDEX;
}

Register_Index generate_return(Generator & generator, Expression * expression, Symbol * declaration, Generate_Flags flags) {
    ASSERT(!(flags & GENERATE_POINTER));

    Register_Index result_register_index = INVALID_REGISTER_INDEX;

    if (expression->expression != NULL) {
        result_register_index = generate(generator, expression->expression, declaration, GENERATE_COPY);
    }

    generate(generator, Instruction{
        .type = INSTRUCTION_RETURN,
        .a_register_index = result_register_index,
        .immediate = 1,
    });

    return INVALID_REGISTER_INDEX;
}

Register_Index generate_as(Generator & generator, Expression * expression, Symbol * declaration, Generate_Flags flags) {
    ASSERT(!(flags & GENERATE_POINTER));

    // print(">> EXPRESSION_AS\n");

    Register_Index from_register_index = generate(generator, expression->left, declaration, GENERATE_COPY);

    Type * from_type = expression->left->resolved_type;
    Type * to_type = unwrap_type(expression->right->resolved_type);

    ASSERT(from_type != &type_integral);

    // print(">>>>>> {} as {}\n", from_type, to_type);

    // For pointers and integers, casts don't really do anything. We have to do a move
    // since we generate a new register with the new type, but we'd like an indirection
    // register that can point to the same data but with a different type.
    Register_Index to_register_index = generate_register(generator, to_type);
    generate_move(generator, from_register_index, /* displacement */ 0, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 0, from_type->flags & TYPE_FLAG_SIGNED, from_type->size, to_register_index, /* displacement */ 0, /* dereference */ 0, to_type->size);

    if (from_type->type == TYPE_POINTER && to_type->type == TYPE_POINTER) {
        return to_register_index;
    } else if ((from_type->flags & TYPE_FLAG_INTEGRAL) && (to_type->flags & TYPE_FLAG_INTEGRAL)) {
        return to_register_index;
    } else if (from_type->type == TYPE_FUNCTION && to_type->type == TYPE_FUNCTION) {
        return to_register_index;
    } else if (from_type != to_type) {
        ASSERT_NOT_REACHED();
    }

    ASSERT_NOT_REACHED();
}

Register_Index generate_variable_declaration(Generator & generator, Expression_Declaration * expression, Symbol * declaration, Generate_Flags flags) {
    ASSERT(!(flags & GENERATE_POINTER));

    Expression * name = expression->declared_name;
    ASSERT(name->type == EXPRESSION_IDENTIFIER);

    Type * type = unwrap_type(expression->declared_type->resolved_type);

    Expression * value = expression->declared_value;

    Register_Index register_index = INVALID_REGISTER_INDEX;
    if (value != NULL) {
        Type * value_type = value->resolved_type;
        if (type->type == TYPE_SPAN) {
            Register_Index data_register_index = generate(generator, value, declaration, GENERATE_COPY);

            register_index = generate_register(generator, type);

            Register_Index pointer_register_index = generate_pointer(generator, data_register_index, value_type->array.base);
            generate_move(generator, pointer_register_index, /* displacement */ 0, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 0, generator.registers[pointer_register_index].type->flags & TYPE_FLAG_SIGNED, generator.registers[pointer_register_index].type->size, register_index, /* displacement */ 0, /* dereference */ 0, /* size */ 8);

            generate_move_immediate(generator, register_index, /* displacement */ 8, /* dereference */ 0, /* size */ 8, value_type->array.count);
        } else {
            register_index = generate(generator, value, declaration, GENERATE_COPY);
        }
    } else {
        if (type->type == TYPE_ARRAY) {
            u64 number_of_elements = 0;
            if (expression->declared_type->type == EXPRESSION_TYPE_ARRAY) {
                Expression_Type_Array * expression_type_array = (Expression_Type_Array *)expression->declared_type;
                if (expression_type_array->length != NULL) {
                    // We only allow literals here for now...
                    ASSERT(expression_type_array->length->type == EXPRESSION_INTEGER);
                    number_of_elements = expression_type_array->length->integer;
                }
            }

            Type * element_type = type->array.base;
            u64 element_count = number_of_elements;
            u64 element_size = element_type->size;

            register_index = generate_register(generator, type);

            Register_Index pointer_register_index = generate_pointer(generator, register_index, element_type);
            for (u64 element_index = 0; element_index < element_count; element_index += 1) {
                generate_move_immediate(generator, pointer_register_index, /* displacement */ element_index * element_size, /* dereference */ 1, element_type->size, /* immediate */ 0);
            }
        } else {
            register_index = generate_empty_registers(generator, type);
        }
    }

    expression->declared_name->resolved_symbol->value.register_index = register_index;

    return register_index;
}

Register_Index generate_function_declaration(Generator & generator, Expression_Declaration * expression, Symbol * declaration, Generate_Flags flags) {
    ASSERT(!(flags & GENERATE_POINTER));

    ASSERT(expression->declared_name->type == EXPRESSION_IDENTIFIER);
    ASSERT(expression->declared_type == NULL);

    ASSERT(expression->declared_value->type == EXPRESSION_FUNCTION);
    Expression_Function * expression_function = (Expression_Function *)expression->declared_value;

    if (expression_function->foreign) {
        // We don't generate anything for foreign functions
        push(generator.unit.exports, Symbol{
            .flags = DECLARATION_FLAGS_FUNCTION | DECLARATION_FLAGS_FOREIGN_FUNCTION | DECLARATION_FLAGS_NO_ADDRESS,
            .name = expression->declared_name->identifier,
            .value = {
                .type = expression->declared_value->resolved_type,
                .address = INVALID_ADDRESS,
            },
        });

        return INVALID_REGISTER_INDEX;
    }

    Symbol & symbol = push(generator.unit.exports, Symbol{
        .flags = DECLARATION_FLAGS_FUNCTION,
        .name = expression->declared_name->identifier,
        .value = {
            .type = expression->declared_value->resolved_type,
            .address = here(generator),
        },
    });

    if (expression_function->entry) {
        symbol.flags |= DECLARATION_FLAGS_ENTRY_FUNCTION;
    }

    Symbol * scope_symbol = expression->declared_name->resolved_symbol;
    ASSERT(scope_symbol != NULL);

    for (Function_Parameter & parameter : expression_function->parameters) {
        ASSERT(parameter.type->resolved_type->type == TYPE_TYPE);

        Type * type = unwrap_type(parameter.type->resolved_type);
        ASSERT((type->flags & TYPE_FLAG_INTEGRAL) || type == &type_boolean || type->type == TYPE_POINTER || type->type == TYPE_STRING || type->type == TYPE_SPAN || type->type == TYPE_STRUCTURE || type->type == TYPE_ARRAY);

        Register_Index register_index = generate_register(generator, type);

        parameter.name->resolved_symbol->value.register_index = register_index;
    }

    generate(generator, expression->declared_value, &symbol, GENERATE_COPY);

    symbol.value.size = here(generator) - symbol.value.address;
    scope_symbol->value.size = here(generator) - scope_symbol->value.address;

    // TODO: This needs to be per overload
    b32 leaf = TRUE;
    traverse((Instruction *)(generator.code.buffer.bytes) + symbol.value.address, symbol.value.size, [&leaf](Instruction const & instruction) {
        if (instruction.type == INSTRUCTION_CALL || instruction.type == INSTRUCTION_CALL_POINTER) {
            leaf = FALSE;
        }
    });

    if (leaf) {
        symbol.flags |= DECLARATION_FLAGS_LEAF_FUNCTION;
        scope_symbol->flags |= DECLARATION_FLAGS_LEAF_FUNCTION;
    }

    return INVALID_REGISTER_INDEX;
}

Register_Index generate_array(Generator & generator, Expression_Array * expression, Symbol * declaration, Generate_Flags flags) {
    ASSERT(!(flags & GENERATE_POINTER));

    ASSERT(expression->resolved_type->type == TYPE_ARRAY);

    Type * element_type = unwrap_type(expression->resolved_type)->array.base; // TODO: Why unwrap..?
    u64 element_count = expression->expressions.number_of_elements;
    u64 element_size = element_type->size;

    Register_Index array_register_index = generate_register(generator, expression->resolved_type);

    Register_Index pointer_register_index = generate_pointer(generator, array_register_index, element_type);

    u64 element_index = 0;
    for (Expression * expression : expression->expressions) {
        Register_Index register_index = generate(generator, expression, declaration, GENERATE_COPY);
        ASSERT(generator.registers[register_index].type == element_type);

        generate_move(generator, register_index, /* displacement */ 0, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 0, element_type->flags & TYPE_FLAG_SIGNED, element_type->size, pointer_register_index, /* displacement */ element_index * element_size, /* dereference */ 1, /* size */ element_type->size);

        element_index += 1;
    }

    return array_register_index;
}

Register_Index generate_address_of(Generator & generator, Expression * expression, Symbol * declaration, Generate_Flags flags) {
    ASSERT(!(flags & GENERATE_POINTER));

    Register_Index value_register_index = generate(generator, expression->right, declaration, GENERATE_POINTER);

    return value_register_index;
}

Register_Index generate_dereference(Generator & generator, Expression * expression, Symbol * declaration, Generate_Flags flags) {
    Register_Index pointer_register_index = generate(generator, expression->right, declaration, GENERATE_COPY);

    Type * pointer_type = generator.registers[pointer_register_index].type;
    ASSERT(pointer_type->type == TYPE_POINTER);

    if (flags & GENERATE_POINTER) {
        return pointer_register_index;
    } else {
        // Recreate the original register
        Type * value_type = pointer_type->pointer.base;
        Register_Index value_register_index = generate_empty_registers(generator, value_type);

        if (value_type->flags & TYPE_FLAG_INTEGRAL) {
            Type * source_type = pointer_type;
            Type * destination_type = pointer_type->pointer.base;

            ASSERT(source_type == type_pointer(destination_type));

            generate_move(generator, pointer_register_index, /* displacement */ 0, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 1, /* sign extend */ source_type->flags & TYPE_FLAG_SIGNED, /* size */ source_type->size, value_register_index, /* displacement */ 0, /* dereference */ 0, /* size */ destination_type->size);
        } else if (value_type->type == TYPE_STRUCTURE) {
            Register_Index destination_register_index = generate_pointer(generator, value_register_index, value_type);
            for (i32 field_index = 0; field_index < (i32)value_type->fields.number_of_elements; field_index += 1) {
                Structure_Field const & field = value_type->fields[field_index];
                generate_move(generator, pointer_register_index, /* displacement */ field.offset, /* index */ INVALID_REGISTER_INDEX, /* scale */ 0, /* dereference */ 1, /* sign extend */ field.type->flags & TYPE_FLAG_SIGNED, /* size */ field.type->size, destination_register_index, /* displacement */ field.offset, /* dereference */ 1, /* size */ field.type->size);
            }
        } else {
            ASSERT_NOT_REACHED();
        }

        return value_register_index;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Register_Index generate(Generator & generator, Expression * expression, Symbol * declaration, Generate_Flags flags) {
    // print("generating {} at 0x{x}\n", expression->type, generator.code.buffer.number_of_bytes / sizeof(Instruction));
    switch (expression->type) {
        case EXPRESSION_INTEGER: {
            return generate_integer(generator, expression, declaration, flags);
        } break;

        case EXPRESSION_STRING: {
            return generate_string(generator, expression, declaration, flags);
        } break;

        case EXPRESSION_IDENTIFIER: {
            return generate_identifier(generator, expression, declaration, flags);
        } break;

        case EXPRESSION_UNARY_BOOLEAN_NOT: {
            return generate_unary_boolean_not(generator, expression, declaration, flags);
        } break;

        case EXPRESSION_UNARY_MINUS: {
            return generate_unary_minus(generator, expression, declaration, flags);
        } break;

        case EXPRESSION_UNARY_BITWISE_NOT: {
            return generate_unary_bitwise_not(generator, expression, declaration, flags);
        } break;

        case EXPRESSION_BINARY_ADDITION:
        case EXPRESSION_BINARY_SUBTRACTION:
        case EXPRESSION_BINARY_BITWISE_OR:
        case EXPRESSION_BINARY_BITWISE_XOR:
        case EXPRESSION_BINARY_COMPARISON_LESS_THAN:
        case EXPRESSION_BINARY_COMPARISON_GREATER_THAN:
        case EXPRESSION_BINARY_COMPARISON_LESS_THAN_OR_EQUAL_TO:
        case EXPRESSION_BINARY_COMPARISON_GREATER_THAN_OR_EQUAL_TO:
        case EXPRESSION_BINARY_COMPARISON_EQUALS:
        case EXPRESSION_BINARY_COMPARISON_NOT_EQUALS:
        case EXPRESSION_BINARY_MULTIPLICATION:
        case EXPRESSION_BINARY_DIVISION:
        case EXPRESSION_BINARY_BITWISE_AND:
        case EXPRESSION_BINARY_SHIFT_LEFT:
        case EXPRESSION_BINARY_SHIFT_RIGHT:
        case EXPRESSION_BINARY_BOOLEAN_AND:
        case EXPRESSION_BINARY_BOOLEAN_OR: {
            return generate_binary(generator, expression, declaration, flags);
        } break;

        case EXPRESSION_INVOCATION: {
            return generate_invocation(generator, (Expression_Invocation *)expression, declaration, flags);
        } break;

        case EXPRESSION_FIELD: {
            return generate_field(generator, (Expression_Field *)expression, declaration, flags);
        } break;

        case EXPRESSION_INDEX: {
            return generate_index(generator, (Expression_Index *)expression, declaration, flags);
        } break;

        case EXPRESSION_TRUE: {
            return generate_true(generator, expression, declaration, flags);
        } break;

        case EXPRESSION_FALSE: {
            return generate_false(generator, expression, declaration, flags);
        } break;

        case EXPRESSION_FUNCTION: {
            return generate_function(generator, (Expression_Function *)expression, declaration, flags);
        } break;

        case EXPRESSION_STRUCTURE: {
            ASSERT_NOT_REACHED();
        } break;

        case EXPRESSION_IF: {
            return generate_if(generator, (Expression_If *)expression, declaration, flags);
        } break;

        case EXPRESSION_FOR: {
            return generate_for(generator, (Expression_For *)expression, declaration, flags);
        } break;

        case EXPRESSION_WHILE: {
            return generate_while(generator, (Expression_While *)expression, declaration, flags);
        } break;

        case EXPRESSION_RANGE: {
            ASSERT(!(flags & GENERATE_POINTER));

            ASSERT_NOT_REACHED();
        } break;

        case EXPRESSION_BLOCK: {
            return generate_block(generator, (Expression_Block *)expression, declaration, flags);
        } break;

        case EXPRESSION_ASSIGNMENT: {
            return generate_assignment(generator, expression, declaration, flags);
        } break;

        case EXPRESSION_RETURN: {
            return generate_return(generator, expression, declaration, flags);
        } break;

        case EXPRESSION_AS: {
            return generate_as(generator, expression, declaration, flags);
        } break;

        case EXPRESSION_VARIABLE_DECLARATION: {
            return generate_variable_declaration(generator, (Expression_Declaration *)expression, declaration, flags);
        } break;

        case EXPRESSION_FUNCTION_DECLARATION: {
            return generate_function_declaration(generator, (Expression_Declaration *)expression, declaration, flags);
        } break;

        case EXPRESSION_STRUCTURE_DECLARATION: {
            // These are not used for code generation, only type checking.
            // TODO: Should this not be ASSERT_NOT_REACHED then?
            return INVALID_REGISTER_INDEX;
        } break;

        case EXPRESSION_OPAQUE_DECLARATION: {
            // These are not used for code generation, only type checking.
            // TODO: Should this not be ASSERT_NOT_REACHED then?
            return INVALID_REGISTER_INDEX;
        } break;

        case EXPRESSION_TYPE_POINTER: {
            // TODO: Why is this ASSERT_NOT_REACHED? When is this used?
            ASSERT_NOT_REACHED();
        } break;

        case EXPRESSION_TYPE_ARRAY: {
            // TODO: Why is this ASSERT_NOT_REACHED? When is this used?
            ASSERT_NOT_REACHED();
        } break;

        case EXPRESSION_TYPE_FUNCTION: {
            // TODO: Why is this ASSERT_NOT_REACHED? When is this used?
            ASSERT_NOT_REACHED();
        } break;

        case EXPRESSION_ARRAY: {
            return generate_array(generator, (Expression_Array *)expression, declaration, flags);
        } break;

        case EXPRESSION_ADDRESS_OF: {
            return generate_address_of(generator, expression, declaration, flags);
        } break;

        case EXPRESSION_DEREFERENCE: {
            return generate_dereference(generator, expression, declaration, flags);
        } break;

        case EXPRESSION_IMPORT:
        case EXPRESSION_LINKER: {
            // EXPRESSION_IMPORT and EXPRESSION_LINKER are removed earlier in the compilation process
            ASSERT_NOT_REACHED();
        } break;
    }

    ASSERT_NOT_REACHED();
}

b32 generate(Unit & unit) {
    Generator generator{
        .unit = unit,
        .code = Code{
            .target = "intermediate",
        },
    };

    generate(generator, unit.tree, NULL, GENERATE_COPY);

    unit.intermediate_code = generator.code;
    unit.registers = generator.registers;

    return TRUE;
}
