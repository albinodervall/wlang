static u64 x64_register_offset(Array<Register> const & registers, Register_Index register_index) {
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

static void x64_store(Buffer & buffer, Array<Register> const & registers, Register_Index register_index, u8 r) {
    u64 offset = x64_register_offset(registers, register_index);
    x64_mov_r_ro(buffer, registers[register_index].type->size, rsp, offset, r);
}

static void x64_load(Buffer & buffer, Array<Register> const & registers, Register_Index register_index, u8 r) {
    u64 offset = x64_register_offset(registers, register_index);
    x64_mov_ro_r(buffer, registers[register_index].type->size, rsp, offset, r);
}

static void convert_x64_function(Array<Register> const & registers, Code const & input, Code & output, Symbol const & input_symbol, Object_Symbol & output_symbol, u64 * map, Buffer & buffer) {
    ASSERT(input_symbol.value.address != INVALID_ADDRESS);
    ASSERT(!(input_symbol.flags & DECLARATION_FLAGS_CONSTANT));
    ASSERT(!(input_symbol.flags & DECLARATION_FLAGS_DATA));

    u64 stack_space = generator_registers_size(registers);

    // Preamble
    {
        // We don't depend on the frame pointer at all, but the stack needs to be aligned to 16 bytes,
        // and when macOS calls us it seems like it is 8 byte aligned...?
        x64_push_r(buffer, rbp);
        x64_mov_r_r(buffer, rsp, rbp);

        if (stack_space > 0) {
            x64_sub_immediate(buffer, rsp, align_up(stack_space, 16));
        }

        // Now, we need to put all the parameters onto the stack
        ASSERT(input_symbol.flags & DECLARATION_FLAGS_FUNCTION);

        // TODO: This uses the Linux/macOS calling convention!

        i32 number_of_argument_register_indices = (i32)input_symbol.value.type->function.parameters.number_of_elements;
        // TODO: This is not true! since some "registers" might take up more than one GPR
        ASSERT(number_of_argument_register_indices <= 6);

        u8 const x64_argument_registers[] = { rdi, rsi, rdx, rcx, r8, r9 };

        i32 register_index = 0;
        for (i32 parameter_index = 0; parameter_index < number_of_argument_register_indices; parameter_index += 1) {
            Type * type = unwrap_type(input_symbol.value.type->function.parameters[parameter_index]);
            // ASSERT((type->flags & TYPE_FLAG_INTEGRAL) || type == &type_boolean || type->type == TYPE_POINTER || type->type == TYPE_STRING || type->type == TYPE_SPAN);
            ASSERT((type->flags & TYPE_FLAG_INTEGRAL) || type == &type_boolean || type->type == TYPE_POINTER);

            // if (type->type == TYPE_STRING || type->type == TYPE_SPAN) {
            //     // We know that register_index is the pointer, register_index + 1 is the length
            //     x64_store(buffer, registers, register_index, x64_argument_registers[register_index]);
            //     register_index += 1;

            //     x64_store(buffer, registers, register_index, x64_argument_registers[register_index]);
            //     register_index += 1;
            // } else {
                ASSERT(registers[register_index].type == type);

                x64_store(buffer, registers, register_index, x64_argument_registers[register_index]);
                register_index += 1;
            // }
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
                // TODO: We can use instructions with smaller operands, so we don't have to clear them
                x64_xor_r_r(buffer, rax, rax);
                x64_xor_r_r(buffer, rcx, rcx);

                x64_load(buffer, registers, instruction.left_register_index, rax);
                x64_load(buffer, registers, instruction.right_register_index, rcx);

                ASSERT(registers[instruction.left_register_index].type == registers[instruction.right_register_index].type);

                switch (instruction.type) {
                    case INSTRUCTION_ADD: {
                        x64_add_r_r(buffer, rax, rcx);
                    } break;

                    case INSTRUCTION_SUBTRACT: {
                        x64_sub_r_r(buffer, rax, rcx);
                    } break;

                    case INSTRUCTION_MULTIPLY: {
                        x64_imul_r_r(buffer, rax, rcx);
                    } break;

                    case INSTRUCTION_DIVIDE: {
                        if (registers[instruction.left_register_index].type->flags & TYPE_FLAG_SIGNED) {
                            // Sign extend RAX into RDX since we're dividing RDX:RAX
                            x64_cqo(buffer);
                            x64_idiv_r(buffer, rcx);
                        } else {
                            // Zero RDX since we're dividing RDX:RAX
                            x64_xor_r_r(buffer, rdx, rdx);
                            x64_div_r(buffer, rcx);
                        }
                    } break;

                    case INSTRUCTION_AND: {
                        x64_and_r_r(buffer, rax, rcx);
                    } break;

                    case INSTRUCTION_OR: {
                        x64_or_r_r(buffer, rax, rcx);
                    } break;

                    case INSTRUCTION_XOR: {
                        x64_xor_r_r(buffer, rax, rcx);
                    } break;

                    case INSTRUCTION_SHIFT_LEFT: {
                        // Since the second operand is already in rcx, we can use cl
                        x64_shl_r_cl(buffer, rax);
                    } break;

                    case INSTRUCTION_SHIFT_RIGHT: {
                        // Since the second operand is already in rcx, we can use cl
                        x64_shr_r_cl(buffer, rax);
                    } break;

                    case INSTRUCTION_LESS_THAN: {
                        x64_xor_r_r(buffer, rbx, rbx);
                        x64_cmp_r_r(buffer, registers[instruction.left_register_index].type->size, rax, rcx);
                        if (registers[instruction.left_register_index].type->flags & TYPE_FLAG_SIGNED) {
                            x64_setl_r(buffer, rbx);
                        } else {
                            x64_setb_r(buffer, rbx);
                        }

                        x64_mov_r_r(buffer, rbx, rax);
                    } break;

                    case INSTRUCTION_GREATER_THAN: {
                        x64_xor_r_r(buffer, rbx, rbx);
                        x64_cmp_r_r(buffer, registers[instruction.left_register_index].type->size, rax, rcx);
                        if (registers[instruction.left_register_index].type->flags & TYPE_FLAG_SIGNED) {
                            x64_setg_r(buffer, rbx);
                        } else {
                            x64_seta_r(buffer, rbx);
                        }

                        x64_mov_r_r(buffer, rbx, rax);
                    } break;

                    case INSTRUCTION_LESS_THAN_OR_EQUAL_TO: {
                        x64_xor_r_r(buffer, rbx, rbx);
                        x64_cmp_r_r(buffer, registers[instruction.left_register_index].type->size, rax, rcx);
                        if (registers[instruction.left_register_index].type->flags & TYPE_FLAG_SIGNED) {
                            x64_setle_r(buffer, rbx);
                        } else {
                            x64_setbe_r(buffer, rbx);
                        }

                        x64_mov_r_r(buffer, rbx, rax);
                    } break;

                    case INSTRUCTION_GREATER_THAN_OR_EQUAL_TO: {
                        x64_xor_r_r(buffer, rbx, rbx);
                        x64_cmp_r_r(buffer, registers[instruction.left_register_index].type->size, rax, rcx);
                        if (registers[instruction.left_register_index].type->flags & TYPE_FLAG_SIGNED) {
                            x64_setge_r(buffer, rbx);
                        } else {
                            x64_setae_r(buffer, rbx);
                        }

                        x64_mov_r_r(buffer, rbx, rax);
                    } break;

                    case INSTRUCTION_NOT_EQUALS: {
                        x64_xor_r_r(buffer, rbx, rbx);
                        x64_cmp_r_r(buffer, registers[instruction.left_register_index].type->size, rax, rcx);
                        x64_setne_r(buffer, rbx);

                        x64_mov_r_r(buffer, rbx, rax);
                    } break;

                    case INSTRUCTION_EQUALS: {
                        x64_xor_r_r(buffer, rbx, rbx);
                        x64_cmp_r_r(buffer, registers[instruction.left_register_index].type->size, rax, rcx);
                        x64_sete_r(buffer, rbx);

                        x64_mov_r_r(buffer, rbx, rax);
                    } break;

                    default: {
                        ASSERT_NOT_REACHED();
                    } break;
                }

                x64_store(buffer, registers, instruction.result_register_index, rax);
            } break;

            case INSTRUCTION_NOT: {
                x64_load(buffer, registers, instruction.a_register_index, rax);
                x64_xor_r_ib(buffer, rax, 0xFF); // This works because 0xFF is sign extended
                x64_store(buffer, registers, instruction.b_register_index, rax);
            } break;

            case INSTRUCTION_JUMP: {
                x64_jmp(buffer, 0);

                push(output.relocation, Relocation{
                    .type = RELOCATION_LABEL_X64_JMP,
                    .address = buffer.number_of_bytes,
                    .target = instruction.immediate,
                });
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

                i32 number_of_argument_register_indices = 0;
                for (u64 parameter_index = 0; parameter_index < type->function.parameters.number_of_elements; parameter_index += 1) {
                    Type const * parameter_type = unwrap_type(type->function.parameters[parameter_index]);

                    // ASSERT((parameter_type->flags & TYPE_FLAG_INTEGRAL) || parameter_type == &type_boolean || parameter_type->type == TYPE_POINTER || parameter_type->type == TYPE_STRING || parameter_type->type == TYPE_SPAN);
                    ASSERT((parameter_type->flags & TYPE_FLAG_INTEGRAL) || parameter_type == &type_boolean || parameter_type->type == TYPE_POINTER);

                    // if (parameter_type->type == TYPE_STRING || parameter_type->type == TYPE_SPAN) {
                    //     number_of_argument_register_indices += 2;
                    // } else {
                        number_of_argument_register_indices += 1;
                    // }
                }

                // Linux/macOS calling convention, we don't do stack passing yet
                ASSERT(number_of_argument_register_indices <= 6);

                u8 const x64_argument_registers[] = { rdi, rsi, rdx, rcx, r8, r9 };
                for (i32 register_index_offset = 0; register_index_offset < number_of_argument_register_indices; register_index_offset += 1) {
                    Register_Index register_index = instruction.a_register_index + register_index_offset;

                    if (registers[register_index].type->flags & TYPE_FLAG_SIGNED) {
                        if (registers[register_index].type->size == 1) {
                            x64_movsx_8_to_64(buffer, rsp, x64_register_offset(registers, register_index), x64_argument_registers[register_index_offset]);
                        } else if (registers[register_index].type->size == 2) {
                            x64_movsx_16_to_64(buffer, rsp, x64_register_offset(registers, register_index), x64_argument_registers[register_index_offset]);
                        } else if (registers[register_index].type->size == 4) {
                            x64_mov_32_to_32(buffer, rsp, x64_register_offset(registers, register_index), x64_argument_registers[register_index_offset]);
                        } else if (registers[register_index].type->size == 8) {
                            x64_mov_64_to_64(buffer, rsp, x64_register_offset(registers, register_index), x64_argument_registers[register_index_offset]);
                        } else {
                            ASSERT_NOT_REACHED();
                        }
                    } else {
                        if (registers[register_index].type->size == 1) {
                            x64_movzx_8_to_64(buffer, rsp, x64_register_offset(registers, register_index), x64_argument_registers[register_index_offset]);
                        } else if (registers[register_index].type->size == 2) {
                            x64_movzx_16_to_64(buffer, rsp, x64_register_offset(registers, register_index), x64_argument_registers[register_index_offset]);
                        } else if (registers[register_index].type->size == 4) {
                            x64_mov_32_to_32(buffer, rsp, x64_register_offset(registers, register_index), x64_argument_registers[register_index_offset]);
                        } else if (registers[register_index].type->size == 8) {
                            x64_mov_64_to_64(buffer, rsp, x64_register_offset(registers, register_index), x64_argument_registers[register_index_offset]);
                        } else {
                            ASSERT_NOT_REACHED();
                        }
                    }
                }

                if (instruction.type == INSTRUCTION_CALL) {
                    x64_callq(buffer, 0);

                    push(output.relocation, Relocation{
                        .type = RELOCATION_X64_BRANCH,
                        // For relocations that the linker will perform, the address is the actual
                        // 32-bit value to replace, not relative to the next instruction.
                        .address = buffer.number_of_bytes - 4,
                        .symbol = name_for_symbol(instruction.symbol),
                    });
                } else if (instruction.type == INSTRUCTION_CALL_POINTER) {
                    x64_load(buffer, registers, instruction.c_register_index, eax);

                    // ff542408            callq   *0x8(%rsp)
                    buffer_write_u8(buffer, 0xff);
                    x64_modrm(buffer, 0b11, 2, rax);
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

                        u8 const x64_return_registers[] = { eax, edx, };
                        x64_store(buffer, registers, register_index, x64_return_registers[register_index_offset]);
                    }
                }
            } break;

            case INSTRUCTION_RETURN: {
                if (instruction.a_register_index != INVALID_REGISTER_INDEX) {
                    // TODO: Maybe we can use the type of the declaration instead of `immediate` here...
                    ASSERT(instruction.immediate <= 2); // Returning multiple values on x64 is more tricky...

                    u8 const x64_return_registers[] = { eax, edx, };
                    for (i32 register_index_offset = 0; register_index_offset < (i32)instruction.immediate; register_index_offset += 1) {
                        Register_Index register_index = instruction.a_register_index + register_index_offset;

                        x64_load(buffer, registers, register_index, x64_return_registers[register_index_offset]);
                    }
                }

                x64_jmp(buffer, 0);

                // If this is the last instruction, we can just skip the jump since
                // we would just jump to the next instruction anyway, but let's skip
                // that for now and maybe let the optimizer take care of that.
                push(output.relocation, Relocation{
                    .type = RELOCATION_LABEL_X64_JMP_END_OF_FUNCTION,
                    .address = buffer.number_of_bytes,
                    .target = output_symbol.address,
                });
            } break;

            case INSTRUCTION_MOVE: {
                Register_Index destination_register_index = instruction.a_register_index;

                x64_movabs_64(buffer, rax, instruction.immediate);
                x64_store(buffer, registers, destination_register_index, rax);
            } break;

            case INSTRUCTION_MOVE_SYMBOL_POINTER: {
                Register_Index destination_register_index = instruction.a_register_index;

                Symbol const * symbol = instruction.symbol;
                ASSERT(symbol != NULL);

                if (symbol->flags & DECLARATION_FLAGS_FOREIGN_FUNCTION) {
                    x64_movq(buffer, rax, 0);

                    push(output.relocation, Relocation{
                        .type = RELOCATION_X64_GOT_LD,
                        .address = buffer.number_of_bytes - 4,
                        .symbol = name_for_symbol(symbol)
                    });
                } else {
                    x64_leaq(buffer, rax, 0);

                    push(output.relocation, Relocation{
                        .type = RELOCATION_X64_SIGNED,
                        .address = buffer.number_of_bytes - 4,
                        .symbol = name_for_symbol(symbol)
                    });
                }

                x64_store(buffer, registers, destination_register_index, eax);
            } break;

            case INSTRUCTION_JUMP_IF_ZERO: {
                Register_Index register_index = instruction.a_register_index;

                x64_load(buffer, registers, register_index, eax);
                x64_cmp_r_i_8(buffer, eax, 0);

                x64_jz(buffer, 0);

                push(output.relocation, Relocation{
                    .type = RELOCATION_LABEL_X64_JZ,
                    .address = buffer.number_of_bytes,
                    .target = instruction.immediate,
                });
            } break;

            case INSTRUCTION_JUMP_IF_NOT_ZERO: {
                Register_Index register_index = instruction.a_register_index;

                x64_load(buffer, registers, register_index, eax);
                x64_cmp_r_i_8(buffer, eax, 0);

                x64_jnz(buffer, 0);

                push(output.relocation, Relocation{
                    .type = RELOCATION_LABEL_X64_JNZ,
                    .address = buffer.number_of_bytes,
                    .target = instruction.immediate,
                });
            } break;

            case INSTRUCTION_POINTER: {
                Register_Index value_register_index = instruction.a_register_index;
                Register_Index pointer_register_index = instruction.b_register_index;

                u64 offset = x64_register_offset(registers, value_register_index);

                ASSERT(offset < 0x7FFFFFFF);
                x64_lea_rsp(buffer, rax, offset);

                x64_store(buffer, registers, pointer_register_index, rax);
            } break;
        }
    }

    output_symbol.postamble_address = buffer.number_of_bytes;

    map[input_symbol.value.size + input_symbol.value.address] = buffer.cursor;

    // Postamble
    {
        if (stack_space > 0) {
            x64_add_immediate(buffer, rsp, align_up(stack_space, 16));
        }

        // See push above!
        x64_pop_r(buffer, rbp);
    }

    // Return
    x64_ret(buffer);
}

b32 convert_x64(Array<Symbol> const & exports, Array<Register> const & registers, Code const & input, Object_Environment & output_environment, Code & output) {
    ASSERT(utf8_compare(input.target, "intermediate"));

    output.target = "x64";

    u64 * map = (u64 *)allocate((input.buffer.number_of_bytes / sizeof(Instruction) + 1) * sizeof(u64));

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

            convert_x64_function(registers, input, output, input_symbol, output_symbol, map, buffer);

            output_symbol.size = (u64)(buffer.cursor - begin);
        }
    }

    for (Relocation const & relocation : output.relocation) {
        u64 source = relocation.address;

        // We jump 4 bytes back since the source here points to the instruction
        // _after_ the branch instruction, not the branch instruction itself as
        // on ARM64.
        if (relocation.type == RELOCATION_LABEL_X64_JZ) {
            u64 target = resolve_target(map, relocation.target);
            i64 relative = ((i64)target - (i64)source);

            buffer_patch_u32(buffer, source - 4, relative);
        } else if (relocation.type == RELOCATION_LABEL_X64_JNZ) {
            u64 target = resolve_target(map, relocation.target);
            i64 relative = ((i64)target - (i64)source);

            buffer_patch_u32(buffer, source - 4, relative);
        } else if (relocation.type == RELOCATION_LABEL_X64_JMP) {
            u64 target = resolve_target(map, relocation.target);
            i64 relative = ((i64)target - (i64)source);

            buffer_patch_u32(buffer, source - 4, relative);
        } else if (relocation.type == RELOCATION_LABEL_X64_JMP_END_OF_FUNCTION) {
            // This is a special kind where we just want to branch to the end of the function
            i64 relative = 0;
            for (Object_Symbol const & symbol : output_environment.symbols) {
                if ((symbol.flags & OBJECT_SYMBOL_FUNCTION) && !(symbol.flags & OBJECT_SYMBOL_FOREIGN_FUNCTION)) {
                    if (symbol.address == relocation.target) {
                        relative = (symbol.postamble_address - relocation.address);
                    }
                }
            }

            // It must always be positive, the function postamble cannot be earlier than
            // any return, it must always be at the end of the generated machine code.
            ASSERT(relative >= 0);

            buffer_patch_u32(buffer, source - 4, relative);
        }
    }

    output.buffer = buffer;

    return TRUE;
}
