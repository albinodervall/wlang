utf8 const * disassemble(Array<Register> const & registers, Instruction const & instruction) {
    switch (instruction.type) {
        case INSTRUCTION_ADD: {
            return format("add %{} %{} %{}", instruction.left_register_index, instruction.right_register_index, instruction.result_register_index);
        } break;

        case INSTRUCTION_SUBTRACT: {
            return format("sub %{} %{} %{}", instruction.left_register_index, instruction.right_register_index, instruction.result_register_index);
        } break;

        case INSTRUCTION_MULTIPLY: {
            return format("mul %{} %{} %{}", instruction.left_register_index, instruction.right_register_index, instruction.result_register_index);
        } break;

        case INSTRUCTION_DIVIDE: {
            return format("div %{} %{} %{}", instruction.left_register_index, instruction.right_register_index, instruction.result_register_index);
        } break;

        case INSTRUCTION_NOT: {
            return format("not %{} %{}", instruction.a_register_index, instruction.b_register_index);
        } break;

        case INSTRUCTION_AND: {
            return format("and %{} %{} %{}", instruction.left_register_index, instruction.right_register_index, instruction.result_register_index);
        } break;

        case INSTRUCTION_OR: {
            return format("or %{} %{} %{}", instruction.left_register_index, instruction.right_register_index, instruction.result_register_index);
        } break;

        case INSTRUCTION_XOR: {
            return format("xor %{} %{} %{}", instruction.left_register_index, instruction.right_register_index, instruction.result_register_index);
        } break;

        case INSTRUCTION_SHIFT_LEFT: {
            return format("shl %{} %{} %{}", instruction.left_register_index, instruction.right_register_index, instruction.result_register_index);
        } break;

        case INSTRUCTION_SHIFT_RIGHT: {
            return format("shr %{} %{} %{}", instruction.left_register_index, instruction.right_register_index, instruction.result_register_index);
        } break;

        case INSTRUCTION_LESS_THAN: {
            return format("lt %{} %{} %{}", instruction.left_register_index, instruction.right_register_index, instruction.result_register_index);
        } break;

        case INSTRUCTION_GREATER_THAN: {
            return format("gt %{} %{} %{}", instruction.left_register_index, instruction.right_register_index, instruction.result_register_index);
        } break;

        case INSTRUCTION_LESS_THAN_OR_EQUAL_TO: {
            return format("lte %{} %{} %{}", instruction.left_register_index, instruction.right_register_index, instruction.result_register_index);
        } break;

        case INSTRUCTION_GREATER_THAN_OR_EQUAL_TO: {
            return format("gte %{} %{} %{}", instruction.left_register_index, instruction.right_register_index, instruction.result_register_index);
        } break;

        case INSTRUCTION_EQUALS: {
            return format("eq %{} %{} %{}", instruction.left_register_index, instruction.right_register_index, instruction.result_register_index);
        } break;

        case INSTRUCTION_NOT_EQUALS: {
            return format("neq %{} %{} %{}", instruction.left_register_index, instruction.right_register_index, instruction.result_register_index);
        } break;

        case INSTRUCTION_JUMP: {
            return format("jmp 0x{016x}", instruction.immediate);
        } break;

        case INSTRUCTION_CALL: {
            Symbol const * symbol = instruction.symbol;

            ASSERT(symbol->flags & DECLARATION_FLAGS_FUNCTION);

            i32 number_of_argument_register_indices = (i32)symbol->value.type->function.parameters.number_of_elements;

            if (number_of_argument_register_indices == 0) {
                return format("call {} %{}", name_for_symbol(symbol), instruction.b_register_index);
            } else if (number_of_argument_register_indices == 1) {
                return format("call {} %{} %{}", name_for_symbol(symbol), instruction.a_register_index, instruction.b_register_index);
            } else {
                return format("call {} %{}-%{} %{}", name_for_symbol(symbol), instruction.a_register_index, instruction.a_register_index + number_of_argument_register_indices - 1, instruction.b_register_index);
            }
        } break;

        case INSTRUCTION_CALL_POINTER: {
            Type * type = registers[instruction.c_register_index].type;
            i32 number_of_argument_register_indices = (i32)type->function.parameters.number_of_elements;

            if (number_of_argument_register_indices == 0) {
                return format("call %{} %{}", instruction.c_register_index, instruction.b_register_index);
            } else if (number_of_argument_register_indices == 1) {
                return format("call %{} %{} %{}", instruction.c_register_index, instruction.a_register_index, instruction.b_register_index);
            } else {
                return format("call %{} %{}-%{} %{}", instruction.c_register_index, instruction.a_register_index, instruction.a_register_index + number_of_argument_register_indices - 1, instruction.b_register_index);
            }
        } break;

        case INSTRUCTION_RETURN: {
            return format("ret %{}", instruction.a_register_index);
        } break;

        case INSTRUCTION_MOVE: {
            Formatter formatter;

            format(formatter, "move ");

            if (instruction.move.from.mode == ADDRESSING_MODE_IMMEDIATE) {
                format(formatter, "#{},{}", instruction.move.from.immediate, instruction.move.from.size);

                if (instruction.move.from.size < 8) {
                    if (instruction.move.from.sign_extend) {
                        format(formatter, "sx");
                    } else {
                        format(formatter, "zx");
                    }
                }
            } else {
                if (instruction.move.from.mode == ADDRESSING_MODE_INDIRECT) {
                    format(formatter, "[");
                }

                format(formatter, "%{}", instruction.move.from.base);

                if (instruction.move.from.index != INVALID_REGISTER_INDEX) {
                    format(formatter, " + %{} * #{}", instruction.move.from.index, instruction.move.from.scale);
                } else {
                    ASSERT(instruction.move.from.scale == 0);
                }

                if (instruction.move.from.displacement > 0) {
                    format(formatter, " + #{}", instruction.move.from.displacement);
                }

                format(formatter, ",{}", instruction.move.from.size);

                if (instruction.move.from.mode == ADDRESSING_MODE_INDIRECT) {
                    format(formatter, "]");
                }
            }

            format(formatter, " → ");

            ASSERT(instruction.move.to.mode != ADDRESSING_MODE_IMMEDIATE);
            ASSERT(instruction.move.to.index == INVALID_REGISTER_INDEX);
            ASSERT(instruction.move.to.scale == 0);
            ASSERT(instruction.move.to.sign_extend == 0);

            if (instruction.move.to.mode == ADDRESSING_MODE_INDIRECT) {
                format(formatter, "[");
            }

            format(formatter, "%{}", instruction.move.to.base);

            if (instruction.move.to.displacement > 0) {
                format(formatter, " + #{}", instruction.move.to.displacement);
            }

            format(formatter, ",{}", instruction.move.to.size);

            if (instruction.move.to.mode == ADDRESSING_MODE_INDIRECT) {
                format(formatter, "]");
            }

            return formatter.pointer;
        } break;

        case INSTRUCTION_MOVE_SYMBOL_POINTER: {
            if (instruction.move.to.displacement == 0) {
                return format("move {}, %{},{}", name_for_symbol(instruction.symbol), instruction.move.to.base, instruction.move.to.size);
            } else {
                return format("move {}, %{} + #{},{}", name_for_symbol(instruction.symbol), instruction.move.to.base, instruction.move.to.displacement, instruction.move.to.size);
            }
        } break;

        case INSTRUCTION_JUMP_IF_ZERO: {
            return format("jz %{} 0x{016x}", instruction.a_register_index, instruction.immediate);
        } break;

        case INSTRUCTION_JUMP_IF_NOT_ZERO: {
            return format("jnz %{} 0x{016x}", instruction.a_register_index, instruction.immediate);
        } break;

        case INSTRUCTION_POINTER: {
            return format("move &%{}, %{}", instruction.a_register_index, instruction.b_register_index);
        } break;
    }

    ASSERT_NOT_REACHED();
}

void disassemble_register(Formatter & formatter, u64 indent, Array<Register> const & registers, Register_Index register_index) {
    ASSERT(registers[register_index].type != NULL);
    format(formatter, "{*r}%{3} (D): {} (size = {}, alignment = {})\n", indent, " ", register_index, registers[register_index].type, registers[register_index].type->size, registers[register_index].type->alignment);
}

utf8 const * disassemble(utf8 const * label, Array<Register> const & registers, Code const & code) {
    ASSERT(utf8_compare(code.target, "intermediate"));

    Formatter formatter;

    Instruction const * instructions = (Instruction *)code.buffer.bytes;
    u64 const number_of_instructions = code.buffer.number_of_bytes / sizeof(Instruction);

    for (u64 instruction_index = 0; instruction_index < number_of_instructions; instruction_index += 1) {
        Instruction const & instruction = instructions[instruction_index];
        format(formatter, "  0x{016x}: {}\n", instruction_index, disassemble(registers, instruction));
    }

    for (u64 register_index = 0; register_index < registers.number_of_elements; register_index += 1) {
        disassemble_register(formatter, 2, registers, register_index);
    }

    if (formatter.pointer != NULL) {
        return formatter.pointer;
    } else {
        return "";
    }
}
