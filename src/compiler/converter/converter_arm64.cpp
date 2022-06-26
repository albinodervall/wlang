// TODO: Does this sign extend or zero extend...? That should be a parameter...
void arm64_load_immediate(Buffer & buffer, u64 immediate, u32 destination_register) {
    if (immediate == 0) {
        arm64_mov_register(buffer, 1, xzr, destination_register);
    } if (immediate <= 0xFFFF) {
        arm64_mov(buffer, 1, 0, immediate, destination_register);
    } else if (immediate <= 0xFFFFFFFF) {
        arm64_movz(buffer, 1, 0, (immediate >> 0) & 0xFFFF, destination_register);
        arm64_movk(buffer, 1, 1, (immediate >> 16) & 0xFFFF, destination_register);
    } else {
        arm64_movz(buffer, 1, 0, (immediate >> 0) & 0xFFFF, destination_register);
        arm64_movk(buffer, 1, 1, (immediate >> 16) & 0xFFFF, destination_register);
        arm64_movk(buffer, 1, 2, (immediate >> 32) & 0xFFFF, destination_register);
        arm64_movk(buffer, 1, 3, (immediate >> 48) & 0xFFFF, destination_register);
    }
}

static void arm64_load(Buffer & buffer, u32 base, u32 index, u32 scale, u32 displacement, u32 destination, u32 size, u32 sign_extend) {
    // TODO: Since we manually calculate index * scale + displacement here, we can allow any scale. But if scale is 1, 2, 4, or 8
    //       we can use more specialized functions instead. This probably also requires that displacement is 0 on arm64 though.
    //
    //       Further, if scale is 1, we do not need to do the multiplication.

    if (size == 1) {
        if (index == xzr && displacement < 4096) {
            if (sign_extend) {
                arm64_ldrsb_64_unsigned_offset(buffer, displacement, base, destination);
            } else {
                arm64_ldrb_unsigned_offset(buffer, displacement, base, destination);
            }
        } else {
            arm64_load_immediate(buffer, displacement, x11);
            if (index != xzr) {
                arm64_load_immediate(buffer, scale, x12);
                arm64_mad(buffer, 1, index, x11, x12, x11);
            }

            if (sign_extend) {
                arm64_ldrsb_64_register(buffer, x11, 0b011, 0, base, destination);
            } else {
                arm64_ldrb_register(buffer, x11, 0b011, 0, base, destination);
            }
        }
    } else if (size == 2) {
        if (index == xzr && displacement < 8192) {
            if (sign_extend) {
                arm64_ldrsh_64_unsigned_offset(buffer, displacement, base, destination);
            } else {
                arm64_ldrh_unsigned_offset(buffer, displacement, base, destination);
            }
        } else {
            arm64_load_immediate(buffer, displacement, x11);
            if (index != xzr) {
                arm64_load_immediate(buffer, scale, x12);
                arm64_mad(buffer, 1, index, x11, x12, x11);
            }

            if (sign_extend) {
                arm64_ldrsh_64_register(buffer, x11, 0b011, 0, base, destination);
            } else {
                arm64_ldrh_register(buffer, x11, 0b011, 0, base, destination);
            }
        }
    } else if (size == 4) {
        if (index == xzr && displacement < 16384) {
            if (sign_extend) {
                arm64_ldrsw_unsigned_offset(buffer, displacement, base, destination);
            } else {
                arm64_ldr_32_immediate_unsigned_offset(buffer, displacement, base, destination);
            }
        } else {
            arm64_load_immediate(buffer, displacement, x11);
            if (index != xzr) {
                arm64_load_immediate(buffer, scale, x12);
                arm64_mad(buffer, 1, index, x11, x12, x11);
            }

            arm64_ldr_32_register(buffer, x11, 0b011, 0, base, destination);
        }
    } else if (size == 8) {
        // arm64_ldr_64_immediate_unsigned_offset does not work for unaligned reads. This happens when
        // we want to copy 8 bytes from a 16 byte argument to a register by the argument is not 8-byte
        // aligned...
        //
        // TODO: How should we handle this case? Is this a "proper" solution?
        if (index == xzr && displacement < 32768 && (displacement & 0b111) == 0) {
            arm64_ldr_64_immediate_unsigned_offset(buffer, displacement, base, destination);
        } else {
            arm64_load_immediate(buffer, displacement, x11);
            if (index != xzr) {
                arm64_load_immediate(buffer, scale, x12);
                arm64_mad(buffer, 1, index, x11, x12, x11);
            }

            arm64_ldr_64_register(buffer, x11, 0b011, 0, base, destination);
        }
    } else {
        ASSERT_NOT_REACHED();
    }
}

static void arm64_store(Buffer & buffer, u32 base, u32 index, u32 scale, u32 offset, u32 destination, u32 size) {
    ASSERT(index == xzr);
    ASSERT(scale == 0);

    if (size == 1) {
        if (offset < 4096) {
            arm64_strb_unsigned_offset(buffer, offset, base, destination);
        } else {
            arm64_load_immediate(buffer, offset, x11);
            arm64_strb_register(buffer, x11, 0b011, 0, base, destination);
        }
    } else if (size == 2) {
        if (offset < 8192) {
            arm64_strh_unsigned_offset(buffer, offset, base, destination);
        } else {
            arm64_load_immediate(buffer, offset, x11);
            arm64_strh_register(buffer, x11, 0b011, 0, base, destination);
        }
    } else if (size == 4) {
        if (offset < 16384) {
            arm64_str_32_immediate_unsigned_offset(buffer, offset, base, destination);
        } else {
            arm64_load_immediate(buffer, offset, x11);
            arm64_str_32_register(buffer, x11, 0b011, 0, base, destination);
        }
    } else if (size == 8) {
        if (offset < 32768) {
            arm64_str_64_immediate_unsigned_offset(buffer, offset, base, destination);
        } else {
            arm64_load_immediate(buffer, offset, x11);
            arm64_str_64_register(buffer, x11, 0b011, 0, base, destination);
        }
    } else {
        ASSERT_NOT_REACHED();
    }
}

u64 resolve_target(u64 * map, u64 input) {
    return map[input];
}

static u64 arm64_register_offset(Array<Register> const & registers, Register_Index register_index) {
    u64 offset = 0;
    for (Register_Index register_index_offset = 0; register_index_offset < register_index; register_index_offset += 1) {
        Register const & r = registers[register_index_offset];
        if (r.type != &type_none) {
            // offset must be aligned to the register's alignment
            offset = align_up(offset, r.type->alignment);
            offset += r.type->size;
        }
    }

    offset = align_up(offset, registers[register_index].type->alignment);

    ASSERT((offset % registers[register_index].type->alignment) == 0);

    return offset;
}

u64 generator_registers_size(Array<Register> const & registers) {
    u64 size = 0;
    for (u64 register_index_offset = 0; register_index_offset < registers.number_of_elements; register_index_offset += 1) {
        Register const & r = registers[register_index_offset];
        if (r.type != &type_none) {
            // offset must be aligned to the register's alignment
            size = align_up(size, r.type->alignment);
            size += r.type->size;
        }
    }

    return size;
}

static void convert_arm64_function(Array<Register> const & registers, Code const & input, Code & output, Symbol const & input_symbol, Object_Symbol & output_symbol, u64 * map, Buffer & buffer) {
    ASSERT(input_symbol.value.address != INVALID_ADDRESS);
    ASSERT(!(input_symbol.flags & DECLARATION_FLAGS_CONSTANT));
    ASSERT(!(input_symbol.flags & DECLARATION_FLAGS_DATA));

    u64 stack_space = generator_registers_size(registers);

    // Preamble
    {
        if (!(input_symbol.flags & DECLARATION_FLAGS_LEAF_FUNCTION)) {
            // stp  x29, x30, [sp, #-0x10]!
            arm64_stp_64_pre_index(buffer, -2, x30, sp, x29); // imm is a multiple of 8
            // mov  x29, sp
            arm64_mov_to_from_sp(buffer, 1, sp, x29);
        }

        if (stack_space > 0) {
            if (align_up(stack_space, 16) > 4095) {
                arm64_sub_immediate(buffer, 1, 1, align_up(stack_space, 4096) / 4096, sp, sp);
            } else {
                arm64_sub_immediate(buffer, 1, 0, align_up(stack_space, 16), sp, sp);
            }
        }

        ASSERT(input_symbol.flags & DECLARATION_FLAGS_FUNCTION);

        // See "Procedure Call Standard for the Arm® 64-bit Architecture (AArch64)""
        //   NOTE: We do not follow the parameter passing standard outlined in the above document fully!

        // arm64 allocates r0-r7 for parameter passing

        u64 register_index = 0;
        u64 register_offset = 0;
        for (u64 parameter_index = 0; parameter_index < input_symbol.value.type->function.parameters.number_of_elements; parameter_index += 1) {
            Type * type = unwrap_type(input_symbol.value.type->function.parameters[parameter_index]);
            ASSERT(registers[register_offset].type == type);

            if (type->size <= 8) {
                // Types of size <= 8 bytes are passed in one register
                ASSERT(register_index < 8);
                arm64_store(buffer, sp, xzr, 0, arm64_register_offset(registers, register_offset), x0 + register_index, type->size);
                register_index += 1;
            } else if (type->size <= 16) {
                // Types of size <= 16 bytes are passed in two registers
                ASSERT(register_index < 8);
                arm64_store(buffer, sp, xzr, 0, arm64_register_offset(registers, register_offset), x0 + register_index, 8);
                register_index += 1;

                ASSERT(register_index < 8);
                arm64_store(buffer, sp, xzr, 0, arm64_register_offset(registers, register_offset) + 8, x0 + register_index, type->size - 8);
                register_index += 1;
            } else {
                // Types of size > 16 bytes are passed on the stack
                ASSERT(register_index < 8);

                u64 offset = 0;
                u64 size_remaining = type->size;
                while (size_remaining > 0) {
                    arm64_load(buffer, x0 + register_index, xzr, 0, offset, x8, /* size */ size_remaining < 8 ? size_remaining : 8, /* sign extend */ 0);
                    arm64_store(buffer, sp, xzr, 0, arm64_register_offset(registers, register_offset) + offset, x8, size_remaining < 8 ? size_remaining : 8);

                    offset += 8;
                    size_remaining = size_remaining - (size_remaining > 8 ? 8 : size_remaining);
                }

                register_index += 1;
            }

            register_offset += 1;
        }
    }

    for (u64 instruction_offset = 0; instruction_offset < input_symbol.value.size; instruction_offset += 1) {
        u64 instruction_index = input_symbol.value.address + instruction_offset;
        map[instruction_index] = buffer.cursor; // Store off where this byte code instruction was generated

        Instruction const & instruction = *(Instruction const *)buffer_peek(input.buffer, instruction_index * sizeof(Instruction), sizeof(Instruction));

        switch (instruction.type) {
            case INSTRUCTION_ADD:
            case INSTRUCTION_SUBTRACT:
            case INSTRUCTION_MULTIPLY:
            case INSTRUCTION_DIVIDE:
            case INSTRUCTION_AND:
            case INSTRUCTION_OR:
            case INSTRUCTION_XOR:
            case INSTRUCTION_SHIFT_LEFT:
            case INSTRUCTION_SHIFT_RIGHT:
            case INSTRUCTION_LESS_THAN:
            case INSTRUCTION_GREATER_THAN:
            case INSTRUCTION_LESS_THAN_OR_EQUAL_TO:
            case INSTRUCTION_GREATER_THAN_OR_EQUAL_TO:
            case INSTRUCTION_NOT_EQUALS:
            case INSTRUCTION_EQUALS: {
                Type * left_type = registers[instruction.left_register_index].type;
                Type * right_type = registers[instruction.right_register_index].type;
                Type * result_type = registers[instruction.result_register_index].type;

                // TODO: Check register size and set sf accordingly, and also handle 8- and 16-bits
                u32 sf = 1;

                // We allow operations on pointers where the right hand size is a u64
                // TODO: This really only applies to some operations... Split these combined cases into separate cases.
                if (left_type->type == TYPE_POINTER) {
                    // left_type == right_type here should only be allowed if it's != and ==...
                    // TODO: Split these intro separate cases!!!!
                    ASSERT(right_type->flags & TYPE_FLAG_INTEGRAL || left_type == right_type);
                } else {
                    ASSERT(left_type == right_type);
                    if (instruction.type == INSTRUCTION_LESS_THAN || instruction.type == INSTRUCTION_GREATER_THAN || instruction.type == INSTRUCTION_LESS_THAN_OR_EQUAL_TO || instruction.type == INSTRUCTION_GREATER_THAN_OR_EQUAL_TO || instruction.type == INSTRUCTION_NOT_EQUALS || instruction.type == INSTRUCTION_EQUALS) {
                        ASSERT(result_type == &type_boolean);
                    } else {
                        ASSERT(result_type == right_type);
                    }

                    // TODO: Needs more special handling for 8- and 16-bits
                    if (left_type->size == 4) {
                        sf = 0;
                    }
                }

                arm64_load(buffer, sp, xzr, 0, arm64_register_offset(registers, instruction.left_register_index), x9, left_type->size, left_type->flags & TYPE_FLAG_SIGNED);
                arm64_load(buffer, sp, xzr, 0, arm64_register_offset(registers, instruction.right_register_index), x10, right_type->size, right_type->flags & TYPE_FLAG_SIGNED);

                switch (instruction.type) {
                    case INSTRUCTION_ADD: {
                        arm64_add_shifted_register(buffer, sf, 0, x10, 0, x9, x8);
                    } break;

                    case INSTRUCTION_SUBTRACT: {
                        arm64_sub_shifted_register(buffer, sf, 0, x10, 0, x9, x8);
                    } break;

                    case INSTRUCTION_MULTIPLY: {
                        arm64_mul(buffer, sf, x10, x9, x8);
                    } break;

                    case INSTRUCTION_DIVIDE: {
                        if (registers[instruction.left_register_index].type->flags & TYPE_FLAG_SIGNED) {
                            arm64_sdiv(buffer, sf, x10, x9, x8);
                        } else {
                            arm64_udiv(buffer, sf, x10, x9, x8);
                        }
                    } break;

                    case INSTRUCTION_AND: {
                        arm64_and_shifted_register(buffer, sf, 0, x10, 0, x9, x8);
                    } break;

                    case INSTRUCTION_OR: {
                        arm64_orr_shifted_register(buffer, sf, 0, x10, 0, x9, x8);
                    } break;

                    case INSTRUCTION_XOR: {
                        arm64_eor_shifted_register(buffer, sf, 0, x10, 0, x9, x8);
                    } break;

                    case INSTRUCTION_SHIFT_LEFT: {
                        arm64_lsl_register(buffer, sf, x10, x9, x8);
                    } break;

                    case INSTRUCTION_SHIFT_RIGHT: {
                        if (registers[instruction.left_register_index].type->flags & TYPE_FLAG_SIGNED) {
                            arm64_asr_register(buffer, sf, x10, x9, x8);
                        } else {
                            arm64_lsr_register(buffer, sf, x10, x9, x8);
                        }
                    } break;

                    case INSTRUCTION_LESS_THAN: {
                        arm64_subs_shifted_register(buffer, sf, 0, x10, 0, x9, x8);
                        if (registers[instruction.left_register_index].type->flags & TYPE_FLAG_SIGNED) {
                            arm64_cset(buffer, 1, lt, x8);
                        } else {
                            arm64_cset(buffer, 1, lo, x8);
                        }
                    } break;

                    case INSTRUCTION_GREATER_THAN: {
                        arm64_subs_shifted_register(buffer, sf, 0, x10, 0, x9, x8);
                        if (registers[instruction.left_register_index].type->flags & TYPE_FLAG_SIGNED) {
                            arm64_cset(buffer, 1, gt, x8);
                        } else {
                            arm64_cset(buffer, 1, hi, x8);
                        }
                    } break;

                    case INSTRUCTION_LESS_THAN_OR_EQUAL_TO: {
                        arm64_subs_shifted_register(buffer, sf, 0, x10, 0, x9, x8);
                        if (registers[instruction.left_register_index].type->flags & TYPE_FLAG_SIGNED) {
                            arm64_cset(buffer, 1, le, x8);
                        } else {
                            arm64_cset(buffer, 1, ls, x8);
                        }
                    } break;

                    case INSTRUCTION_GREATER_THAN_OR_EQUAL_TO: {
                        arm64_subs_shifted_register(buffer, sf, 0, x10, 0, x9, x8);
                        if (registers[instruction.left_register_index].type->flags & TYPE_FLAG_SIGNED) {
                            arm64_cset(buffer, 1, ge, x8);
                        } else {
                            arm64_cset(buffer, 1, hs, x8);
                        }
                    } break;

                    case INSTRUCTION_NOT_EQUALS: {
                        arm64_subs_shifted_register(buffer, sf, 0, x10, 0, x9, x8);
                        arm64_cset(buffer, 1, ne, x8);
                    } break;

                    case INSTRUCTION_EQUALS: {
                        arm64_subs_shifted_register(buffer, sf, 0, x10, 0, x9, x8);
                        arm64_cset(buffer, 1, eq, x8);
                    } break;

                    default: {
                        ASSERT_NOT_REACHED();
                    } break;
                }

                arm64_store(buffer, sp, xzr, 0, arm64_register_offset(registers, instruction.result_register_index), x8, result_type->size);
            } break;

            case INSTRUCTION_NOT: {
                Type * a_type = registers[instruction.a_register_index].type;
                Type * b_type = registers[instruction.b_register_index].type;

                ASSERT(a_type == b_type);

                arm64_load(buffer, sp, xzr, 0, arm64_register_offset(registers, instruction.a_register_index), x9, a_type->size, a_type->flags & TYPE_FLAG_SIGNED);
                arm64_mvn(buffer, 1, 0, x9, 0, x8);
                arm64_store(buffer, sp, xzr, 0, arm64_register_offset(registers, instruction.b_register_index), x8, b_type->size);
            } break;

            case INSTRUCTION_JUMP: {
                push(output.relocation, Relocation{
                    .type = RELOCATION_LABEL_ARM64_B,
                    .address = buffer.number_of_bytes,
                    .target = instruction.immediate,
                });

                arm64_b(buffer, 0);
            } break;

            case INSTRUCTION_CALL:
            case INSTRUCTION_CALL_POINTER: {
                Type * type = NULL;
                if (instruction.type == INSTRUCTION_CALL) {
                    ASSERT(instruction.symbol != NULL);

                    ASSERT(instruction.symbol->flags & DECLARATION_FLAGS_FUNCTION);
                    type = instruction.symbol->value.type;
                } else if (instruction.type == INSTRUCTION_CALL_POINTER) {
                    type = registers[instruction.c_register_index].type;
                } else {
                    ASSERT_NOT_REACHED();
                }

                // See "Procedure Call Standard for the Arm® 64-bit Architecture (AArch64)""
                //   NOTE: We do not follow the parameter passing standard outlined in the above document fully!

                // arm64 allocates r0-r7 for parameter passing

                u64 register_offset = 0;
                for (u64 parameter_index = 0; parameter_index < type->function.parameters.number_of_elements; parameter_index += 1) {
                    Type * parameter_type = unwrap_type(type->function.parameters[parameter_index]);

                    Register_Index register_index = instruction.a_register_index + register_offset;
                    if (registers[register_index].type != parameter_type) {
                        ASSERT(registers[register_index].type->type == TYPE_POINTER && parameter_type->type == TYPE_POINTER  && (registers[register_index].type->pointer.base == &type_void || parameter_type->pointer.base == &type_void) || (registers[register_index].type->type == TYPE_POINTER && parameter_type == type_pointer(&type_void)) || (registers[register_index].type->type == TYPE_FUNCTION && parameter_type == type_pointer(registers[register_index].type)));
                    }

                    if (parameter_type->size <= 8) {
                        // Types of size <= 8 bytes are passed in one register
                        ASSERT(register_offset < 8);
                        arm64_load(buffer, sp, xzr, 0, arm64_register_offset(registers, register_index), x0 + register_offset, parameter_type->size, parameter_type->flags & TYPE_FLAG_SIGNED);
                        register_offset += 1;
                    } else if (parameter_type->size <= 16) {
                        ASSERT(!(parameter_type->flags & TYPE_FLAG_SIGNED));

                        // Types of size <= 16 bytes are passed in two registers
                        ASSERT(register_offset < 8);
                        arm64_load(buffer, sp, xzr, 0, arm64_register_offset(registers, register_index), x0 + register_offset, 8, 0);
                        register_offset += 1;

                        ASSERT(register_offset < 8);
                        arm64_load(buffer, sp, xzr, 0, arm64_register_offset(registers, register_index) + 8, x0 + register_offset, parameter_type->size - 8, 0);
                        register_offset += 1;
                    } else {
                        // Types of size > 16 bytes are passed on the stack
                        ASSERT(register_offset < 8);
                        arm64_add_immediate(buffer, 1, 0, (u32)arm64_register_offset(registers, register_index), sp, x0 + register_offset);
                        register_offset += 1;
                    }
                }

                if (instruction.type == INSTRUCTION_CALL) {
                    push(output.relocation, Relocation{
                        .type = RELOCATION_BR26,
                        .address = buffer.number_of_bytes,
                        .symbol = name_for_symbol(instruction.symbol),
                    });

                    arm64_bl(buffer, 0);
                } else if (instruction.type == INSTRUCTION_CALL_POINTER) {
                    arm64_load(buffer, sp, xzr, 0, arm64_register_offset(registers, instruction.c_register_index), x8, /* size */ 8, /* sign extend */ 0);
                    arm64_blr(buffer, x8);
                } else {
                    ASSERT_NOT_REACHED();
                }

                if (unwrap_type(type->function.return_type) != &type_none) {
                    i32 number_of_result_register_indices = 0;
                    ASSERT((unwrap_type(type->function.return_type)->flags & TYPE_FLAG_INTEGRAL) || unwrap_type(type->function.return_type) == &type_boolean || unwrap_type(type->function.return_type)->type == TYPE_POINTER);
                    // if (unwrap_type(type->function.return_type)->type == TYPE_STRING) {
                    //     number_of_result_register_indices = 2;
                    // } else {
                        number_of_result_register_indices = 1;
                    // }

                    for (i32 register_index_offset = 0; register_index_offset < number_of_result_register_indices; register_index_offset += 1) {
                        Register_Index register_index = instruction.b_register_index + register_index_offset;
                        Type * type = registers[register_index].type;

                        arm64_store(buffer, sp, xzr, 0, arm64_register_offset(registers, register_index), x0 + register_index_offset, type->size);
                    }
                }
            } break;

            case INSTRUCTION_RETURN: {
                if (instruction.a_register_index != INVALID_REGISTER_INDEX) {
                    // TODO: Maybe we can use the type of the declaration instead of `immediate` here...
                    ASSERT(instruction.immediate < 0x7FFFFFFF);
                    for (i32 register_index_offset = 0; register_index_offset < (i32)instruction.immediate; register_index_offset += 1) {
                        Register_Index register_index = instruction.a_register_index + register_index_offset;
                        Type * type = registers[register_index].type;

                        arm64_load(buffer, sp, xzr, 0, arm64_register_offset(registers, register_index), x0 + register_index_offset, type->size, type->flags & TYPE_FLAG_SIGNED);
                    }
                }

                // If this is the last instruction, we can just skip the jump since
                // we would just jump to the next instruction anyway, but let's skip
                // that for now and maybe let the optimizer take care of that.
                push(output.relocation, Relocation{
                    .type = RELOCATION_LABEL_ARM64_B_END_OF_FUNCTION,
                    .address = buffer.number_of_bytes,
                    .target = output_symbol.address,
                });

                arm64_b(buffer, 0);
            } break;

            case INSTRUCTION_MOVE: {
                if (instruction.move.from.mode == ADDRESSING_MODE_IMMEDIATE) {
                    // ASSERT(instruction.move.from.sign_extend == 0); // TODO: Sign extend should be a thing for immediates too!
                    ASSERT(instruction.move.from.size == 8);

                    arm64_load_immediate(buffer, instruction.move.from.immediate, x8);
                } else if (instruction.move.from.mode == ADDRESSING_MODE_DIRECT) {
                    ASSERT(instruction.move.from.index == INVALID_REGISTER_INDEX);
                    ASSERT(instruction.move.from.scale == 0);

                    u64 offset = arm64_register_offset(registers, instruction.move.from.base);
                    arm64_load(buffer, sp, xzr, 0, offset + instruction.move.from.displacement, x8, instruction.move.from.size, instruction.move.from.sign_extend);
                } else if (instruction.move.from.mode == ADDRESSING_MODE_INDIRECT) {
                    arm64_load(buffer, sp, xzr, 0, arm64_register_offset(registers, instruction.move.from.base), x9, /* size */ 8, /* sign extend */ 0);

                    if (instruction.move.from.index != INVALID_REGISTER_INDEX) {
                        // TODO: We don't actually know what type the index is here, so we have to check the register...
                        arm64_load(buffer, sp, xzr, 0, arm64_register_offset(registers, instruction.move.from.index), x10, /* size */ registers[instruction.move.from.index].type->size, /* sign extend */ registers[instruction.move.from.index].type->flags & TYPE_FLAG_SIGNED);
                        arm64_load(buffer, x9, x10, instruction.move.from.scale, instruction.move.from.displacement, x8, instruction.move.from.size, instruction.move.from.sign_extend);
                    } else {
                        ASSERT(instruction.move.from.scale == 0);
                        arm64_load(buffer, x9, xzr, 0, instruction.move.from.displacement, x8, instruction.move.from.size, instruction.move.from.sign_extend);
                    }
                } else {
                    ASSERT_NOT_REACHED();
                }

                if (instruction.move.to.mode == ADDRESSING_MODE_DIRECT) {
                    ASSERT(instruction.move.to.index == INVALID_REGISTER_INDEX);
                    ASSERT(instruction.move.to.scale == 0);
                    ASSERT(instruction.move.to.sign_extend == 0);

                    u64 offset = arm64_register_offset(registers, instruction.move.to.base);
                    arm64_store(buffer, sp, xzr, 0, offset + instruction.move.to.displacement, x8, instruction.move.to.size);
                } else if (instruction.move.to.mode == ADDRESSING_MODE_INDIRECT) {
                    ASSERT(instruction.move.to.index == INVALID_REGISTER_INDEX);
                    ASSERT(instruction.move.to.scale == 0);
                    ASSERT(instruction.move.to.sign_extend == 0);

                    u64 offset = arm64_register_offset(registers, instruction.move.to.base);
                    arm64_load(buffer, sp, xzr, 0, offset, x9, /* size */ 8, /* sign extend */ 0);

                    arm64_store(buffer, x9, xzr, 0, instruction.move.to.displacement, x8, instruction.move.to.size);
                } else {
                    ASSERT_NOT_REACHED();
                }
            } break;

            case INSTRUCTION_MOVE_SYMBOL_POINTER: {
                Symbol const * symbol = instruction.symbol;
                ASSERT(symbol != NULL);

                if (symbol->flags & DECLARATION_FLAGS_FOREIGN_FUNCTION) {
                    push(output.relocation, Relocation{
                        .type = RELOCATION_GOTLDP,
                        .address = buffer.number_of_bytes,
                        .symbol = name_for_symbol(symbol)
                    });

                    arm64_adrp(buffer, 0, x8);

                    push(output.relocation, Relocation{
                        .type = RELOCATION_GOTLDPOF,
                        .address = buffer.number_of_bytes,
                        .symbol = name_for_symbol(symbol)
                    });

                    arm64_ldr_64_immediate_unsigned_offset(buffer, 0, x8, x8);
                } else {
                    push(output.relocation, Relocation{
                        .type = RELOCATION_PAGE21,
                        .address = buffer.number_of_bytes,
                        .symbol = name_for_symbol(symbol)
                    });

                    arm64_adrp(buffer, 0, x8);

                    push(output.relocation, Relocation{
                        .type = RELOCATION_PAGOF12,
                        .address = buffer.number_of_bytes,
                        .symbol = name_for_symbol(symbol)
                    });

                    arm64_add_immediate(buffer, 1, 0, 0, x8, x8);
                }

                ASSERT(instruction.move.to.mode == ADDRESSING_MODE_INDIRECT);

                u64 offset = arm64_register_offset(registers, instruction.move.to.base);
                arm64_load(buffer, sp, xzr, 0, offset, x9, /* size */ 8, /* sign extend */ 0);

                arm64_store(buffer, x9, xzr, 0, instruction.move.to.displacement, x8, instruction.move.to.size);
            } break;

            case INSTRUCTION_JUMP_IF_ZERO: {
                Register_Index register_index = instruction.a_register_index;
                Type * type = registers[register_index].type;

                arm64_load(buffer, sp, xzr, 0, arm64_register_offset(registers, register_index), x8, type->size, type->flags & TYPE_FLAG_SIGNED);

                push(output.relocation, Relocation{
                    .type = RELOCATION_LABEL_ARM64_CBZ,
                    .address = buffer.number_of_bytes,
                    .target = instruction.immediate,
                });

                arm64_cbz(buffer, 1, 0, x8);
            } break;

            case INSTRUCTION_JUMP_IF_NOT_ZERO: {
                Register_Index register_index = instruction.a_register_index;
                Type * type = registers[register_index].type;

                arm64_load(buffer, sp, xzr, 0, arm64_register_offset(registers, register_index), x8, type->size, type->flags & TYPE_FLAG_SIGNED);

                push(output.relocation, Relocation{
                    // .type = RELOCATION_LABEL_ARM64_B_COND,
                    .type = RELOCATION_LABEL_ARM64_CBNZ,
                    .address = buffer.number_of_bytes,
                    .target = instruction.immediate,
                });

                // arm64_b_cond(buffer, 0, ne);
                arm64_cbnz(buffer, 1, 0, x8);
            } break;

            case INSTRUCTION_POINTER: {
                Register_Index value_register_index = instruction.a_register_index;
                Register_Index pointer_register_index = instruction.b_register_index;

                // print("generating pointer to register of type {}\n", registers[value_register_index].type);
                // print("    the pointer itself is of type {}\n", registers[pointer_register_index].type);

                u64 offset = arm64_register_offset(registers, value_register_index);

                if (offset > 4095) {
                    // NOTE: We cannot use arm64_add_extended_register as it does not accept
                    //       sp as one of the operands.
                    arm64_load_immediate(buffer, offset, x9);
                    arm64_add_extended_register(buffer, 1, x9, 0b011, 0, sp, x8);
                } else {
                    arm64_add_immediate(buffer, 1, 0, (u32)offset, sp, x8);
                }

                arm64_store(buffer, sp, xzr, 0, arm64_register_offset(registers, pointer_register_index), x8, /* size */ 8);
            } break;
        }
    }

    output_symbol.postamble_address = buffer.number_of_bytes;

    map[input_symbol.value.size + input_symbol.value.address] = buffer.cursor;

    // Postamble
    {
        if (stack_space > 0) {
            if (align_up(stack_space, 16) > 4095) {
                arm64_add_immediate(buffer, 1, 1, align_up(stack_space, 4096) / 4096, sp, sp);
            } else {
                arm64_add_immediate(buffer, 1, 0, align_up(stack_space, 16), sp, sp);
            }
        }

        if (!(input_symbol.flags & DECLARATION_FLAGS_LEAF_FUNCTION)) {
            // ldp x29, x30, [sp], #0x10
            arm64_ldp_64_post_index(buffer, 2, x30, sp, x29); // imm is a multiple of 8
        }
    }

    // Return
    arm64_ret(buffer, x30);
}

b32 convert_arm64(Array<Symbol> const & exports, Array<Register> const & registers, Code const & input, Object_Environment & output_environment, Code & output) {
    ASSERT(utf8_compare(input.target, "intermediate"));

    output.target = "arm64";

    u64 * map = (u64 *)allocate(((input.buffer.number_of_bytes / sizeof(Instruction)) + 1) * sizeof(u64));

    Buffer buffer;

    for (Symbol const & input_symbol : exports) {
        Object_Symbol & output_symbol = push(output_environment.symbols, Object_Symbol{});

        output_symbol.flags = OBJECT_SYMBOL_FLAGS_NONE;

        if (input_symbol.flags & DECLARATION_FLAGS_FUNCTION) {
            output_symbol.flags |= OBJECT_SYMBOL_FUNCTION;
        }

        if (input_symbol.flags & DECLARATION_FLAGS_ENTRY_FUNCTION) {
            output_symbol.flags |= OBJECT_SYMBOL_ENTRY_FUNCTION;
        }

        if (input_symbol.flags & DECLARATION_FLAGS_FOREIGN_FUNCTION) {
            output_symbol.flags |= OBJECT_SYMBOL_FOREIGN_FUNCTION;
        }

        if (input_symbol.flags & DECLARATION_FLAGS_NO_ADDRESS) {
            output_symbol.flags |= OBJECT_SYMBOL_NO_ADDRESS;
        }

        if (input_symbol.flags & DECLARATION_FLAGS_CONSTANT) {
            output_symbol.flags |= OBJECT_SYMBOL_CONSTANT;
        }

        if (input_symbol.flags & DECLARATION_FLAGS_DATA) {
            output_symbol.flags |= OBJECT_SYMBOL_DATA;
        }

        output_symbol.name = name_for_symbol(&input_symbol);
        output_symbol.address = input_symbol.value.address;
        output_symbol.size = input_symbol.value.size;
        output_symbol.value = input_symbol.value.value;

        if ((input_symbol.flags & DECLARATION_FLAGS_FUNCTION) && !(input_symbol.flags & DECLARATION_FLAGS_FOREIGN_FUNCTION)) {
            u64 begin = buffer.cursor;

            output_symbol.address = buffer.cursor;
            output_symbol.size = 0;

            convert_arm64_function(registers, input, output, input_symbol, output_symbol, map, buffer);

            output_symbol.size = (u64)(buffer.cursor - begin);
        }
    }

    for (Relocation const & relocation : output.relocation) {
        u64 source = relocation.address;
        u32 instruction = buffer_peek_u32(buffer, source);

        if (relocation.type == RELOCATION_LABEL_ARM64_CBZ || relocation.type == RELOCATION_LABEL_ARM64_CBNZ) {
            u64 target = resolve_target(map, relocation.target);
            i64 relative = ((i64)target - (i64)source) / 4;

            instruction |= ((relative & 0b1111111111111111111) << 5);
            buffer_patch_u32(buffer, source, instruction);
        } else if (relocation.type == RELOCATION_LABEL_ARM64_B) {
            u64 target = resolve_target(map, relocation.target);
            i64 relative = ((i64)target - (i64)source) / 4;

            instruction |= ((relative & 0b11111111111111111111111111) << 0);
            buffer_patch_u32(buffer, source, instruction);
        } else if (relocation.type == RELOCATION_LABEL_ARM64_B_COND) {
            u64 target = resolve_target(map, relocation.target);
            i64 relative = ((i64)target - (i64)source) / 4;

            instruction |= ((relative & 0b1111111111111111111) << 5);
            buffer_patch_u32(buffer, source, instruction);
        } else if (relocation.type == RELOCATION_LABEL_ARM64_B_END_OF_FUNCTION) {
            // This is a special kind where we just want to branch to the end of the function
            i64 relative = 0;
            for (Object_Symbol const & symbol : output_environment.symbols) {
                if ((symbol.flags & OBJECT_SYMBOL_FUNCTION) && !(symbol.flags & OBJECT_SYMBOL_FOREIGN_FUNCTION)) {
                    if (symbol.address == relocation.target) {
                        relative = (symbol.postamble_address - relocation.address) / 4;
                    }
                }
            }

            // It must always be positive, the function postamble cannot be earlier than
            // any return, it must always be at the end of the generated machine code.
            ASSERT(relative > 0);

            instruction |= ((relative & 0b11111111111111111111111111) << 0);
            buffer_patch_u32(buffer, source, instruction);
        }
    }

    output.buffer = buffer;

    return TRUE;
}
