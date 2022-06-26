u64 write_coff_text_section(Buffer & buffer, u64 offset = 0, u32 virtual_size = 0, u32 virtual_address = 0, u32 size_of_raw_data = 0, u32 pointer_to_raw_data = 0, u32 pointer_to_relocations = 0, u16 number_of_relocations = 0, u32 characteristics = 0) {
    if (offset != 0) {
        buffer_set_cursor(buffer, offset);
    }

    u64 begin = buffer_get_cursor(buffer);

    buffer_write(buffer, (u8 *)format("{8+}", "\0", ".text"), 8); // name
    buffer_write_u32(buffer, virtual_size); // virtual_size
    buffer_write_u32(buffer, virtual_address); // virtual_address
    buffer_write_u32(buffer, size_of_raw_data); // size_of_raw_data
    buffer_write_u32(buffer, pointer_to_raw_data); // pointer_to_raw_data
    buffer_write_u32(buffer, pointer_to_relocations); // pointer_to_relocations
    buffer_write_u32(buffer, 0); // pointer_to_line_numbers
    buffer_write_u16(buffer, number_of_relocations); // number_of_relocations
    buffer_write_u16(buffer, 0); // number_of_line_numbers
    buffer_write_u32(buffer, characteristics); // characteristics

    if (offset != 0) {
        buffer_reset_cursor(buffer);
    }

    return begin;
}

u64 reserve_coff_text_section(Buffer & buffer) {
    return write_coff_text_section(buffer);
}

void write_coff_relocation_entry(Buffer & buffer, Relocation & relocation, Object_Symbol const & symbol, u32 symbolnum) {
    buffer_write_u32(buffer, relocation.address); // virtual_address
    buffer_write_u32(buffer, symbolnum); // symbol_table_index

    switch (relocation.type) {
        case RELOCATION_BR26:
        case RELOCATION_PAGOF12:
        case RELOCATION_PAGE21:
        case RELOCATION_GOTLDP:
        case RELOCATION_GOTLDPOF: {
            ASSERT_NOT_REACHED();
        } break;

        case RELOCATION_X64_BRANCH: {
            buffer_write_u16(buffer, 0x0004); // type = IMAGE_REL_AMD64_REL32
        } break;

        case RELOCATION_X64_SIGNED: {
            ASSERT_NOT_REACHED();
            buffer_write_u16(buffer, 0); // type
        } break;

        case RELOCATION_X64_GOT_LD: {
            ASSERT_NOT_REACHED();
            buffer_write_u16(buffer, 0); // type
        } break;

        case RELOCATION_LABEL_ARM64_CBZ:
        case RELOCATION_LABEL_ARM64_CBNZ:
        case RELOCATION_LABEL_ARM64_B:
        case RELOCATION_LABEL_ARM64_B_COND:
        case RELOCATION_LABEL_ARM64_B_END_OF_FUNCTION:
        case RELOCATION_LABEL_X64_JZ:
        case RELOCATION_LABEL_X64_JNZ:
        case RELOCATION_LABEL_X64_JMP:
        case RELOCATION_LABEL_X64_JMP_END_OF_FUNCTION: {
            ASSERT_NOT_REACHED();
        } break;
    }
}

void write_coff(Context & context, utf8 const * file, Object_Environment & environment, Code & code) {
    ASSERT(utf8_compare(code.target, "arm64") || utf8_compare(code.target, "x64"));

    if (context.verbose) {
        print("Writing {}\n", file);

        print("Symbol entries ({} symbols):\n", environment.symbols.number_of_elements);
        for (Object_Symbol & symbol : environment.symbols) {
            if (symbol.flags & OBJECT_SYMBOL_FUNCTION) {
                if (symbol.flags & OBJECT_SYMBOL_FOREIGN_FUNCTION) {
                    print("  {}: foreign\n", symbol.name);
                } else {
                    print("  {}: at {016x}, {} bytes{}\n", symbol.name, symbol.address, symbol.size, (symbol.flags & OBJECT_SYMBOL_ENTRY_FUNCTION) ? ", #entry" : "");
                }
            } else {
                if (symbol.flags & OBJECT_SYMBOL_NO_ADDRESS) {
                    // if (symbol.type->type == TYPE_STRING) {
                    //     print("  {}: \"{}\" {} bytes\n", symbol.name, (utf8 const *)symbol.value, symbol.size);
                    // } else {
                        print("  {}: {} bytes\n", symbol.name, symbol.size);
                    // }
                } else {
                    print("  {}: at {016x}, {} bytes\n", symbol.name, symbol.address, symbol.size);
                }
            }
        }

        print("Relocation entries ({}):\n", code.relocation.number_of_elements);
        for (Relocation & relocation : code.relocation) {
            if (relocation.type < 1024) {
                print("  {08x}\t", relocation.address);
                switch (relocation.type) {
                    case RELOCATION_BR26: {
                        print("BR26\n");
                    } break;

                    case RELOCATION_PAGOF12: {
                        print("PAGOF12\n");
                    } break;

                    case RELOCATION_PAGE21: {
                        print("PAGE21\n");
                    } break;

                    case RELOCATION_GOTLDP: {
                        print("GOTLDP\n");
                    } break;

                    case RELOCATION_GOTLDPOF: {
                        print("GOTLDPOF\n");
                    } break;

                    case RELOCATION_X64_BRANCH: {
                        print("BRANCH\n");
                    } break;

                    case RELOCATION_X64_SIGNED: {
                        print("SIGNED\n");
                    } break;

                    case RELOCATION_X64_GOT_LD: {
                        print("GOT_LD\n");
                    } break;

                    case RELOCATION_LABEL_ARM64_CBZ:
                    case RELOCATION_LABEL_ARM64_CBNZ:
                    case RELOCATION_LABEL_ARM64_B:
                    case RELOCATION_LABEL_ARM64_B_COND:
                    case RELOCATION_LABEL_ARM64_B_END_OF_FUNCTION:
                    case RELOCATION_LABEL_X64_JZ:
                    case RELOCATION_LABEL_X64_JNZ:
                    case RELOCATION_LABEL_X64_JMP:
                    case RELOCATION_LABEL_X64_JMP_END_OF_FUNCTION: {
                        ASSERT_NOT_REACHED();
                    } break;
                }
            }
        }
    }

    Buffer buffer;

    if (utf8_compare(code.target, "arm64")) {
        buffer_write_u16(buffer, 0xaa64); // IMAGE_FILE_MACHINE_ARM64
    } else if (utf8_compare(code.target, "x64")) {
        buffer_write_u16(buffer, 0x8664); // IMAGE_FILE_MACHINE_AMD64
    } else {
        ASSERT_NOT_REACHED();
    }

    u16 number_of_sections_offset = buffer_reserve_16(buffer);

    #if defined(_WIN64)
    {
        const i64 UNIX_TIME_START = 0x019DB1DED53E8000; //January 1, 1970 (start of Unix epoch) in "ticks"
        const i64 TICKS_PER_SECOND = 10000000; //a tick is 100ns

        FILETIME ft;
        GetSystemTimeAsFileTime(&ft); //returns ticks in UTC

        //Copy the low and high parts of FILETIME into a LARGE_INTEGER
        //This is so we can access the full 64-bits as an Int64 without causing an alignment fault
        LARGE_INTEGER li;
        li.DUMMYSTRUCTNAME.LowPart  = ft.dwLowDateTime;
        li.DUMMYSTRUCTNAME.HighPart = ft.dwHighDateTime;

        //Convert ticks since 1/1/1970 into seconds
        i32 timestamp = (i32)((li.QuadPart - UNIX_TIME_START) / TICKS_PER_SECOND);

        buffer_write_i32(buffer, timestamp); // time_date_stamp
    }
    #else
        ASSERT_NOT_REACHED();
    #endif

    i32 pointer_to_symbol_table_offset = buffer_reserve_32(buffer);
    i32 number_of_symbols_offset = buffer_reserve_32(buffer);
    buffer_write_u16(buffer, 0); // size_of_optional_header
    buffer_write_u16(buffer, 0); // characteristics

    u64 text_section_offset = reserve_coff_text_section(buffer);

    // Write code
    u64 code_offset = buffer_get_cursor(buffer);

    // TODO: Alignment?
    u64 * symbol_offsets = (u64 *)allocate(environment.symbols.number_of_elements * sizeof(u64));

    {
        u64 symbol_offsets_index = 0;
        for (Object_Symbol & symbol : environment.symbols) {
            if (symbol.flags & OBJECT_SYMBOL_FUNCTION && !(symbol.flags & OBJECT_SYMBOL_FOREIGN_FUNCTION)) {
                symbol_offsets[symbol_offsets_index] = buffer_get_cursor(buffer) - code_offset;

                if (context.verbose) {
                    print("writing symbol {} at {}, {} bytes\n", symbol.name, symbol_offsets[symbol_offsets_index], symbol.size);
                }

                ASSERT(symbol.address != INVALID_ADDRESS);
                buffer_write(buffer, code.buffer.bytes + symbol.address, symbol.size);
            }

            symbol_offsets_index += 1;
        }
    }

    u64 code_size = (u64)(buffer_get_cursor(buffer) - code_offset);

    // // // Write consts
    // // u64 consts_offset = buffer_get_cursor(buffer);

    // // {
    // //     u64 symbol_offsets_index = 0;
    // //     for (Object_Symbol & symbol : environment.symbols) {
    // //         if (symbol.flags & OBJECT_SYMBOL_CONSTANT) {
    // //             symbol_offsets[symbol_offsets_index] = buffer_get_cursor(buffer) - code_offset;

    // //             if (context.verbose) {
    // //                 print("writing constant symbol {} at {}, {} bytes\n", symbol.name, symbol_offsets[symbol_offsets_index], symbol.size);
    // //             }

    // //             buffer_write(buffer, (u8 const *)symbol.value, symbol.size);
    // //         }

    // //         symbol_offsets_index += 1;
    // //     }
    // // }

    // // u64 consts_size = (u64)(buffer_get_cursor(buffer) - consts_offset);

    // // // Write data
    // // u64 data_offset = buffer_get_cursor(buffer);

    // // {
    // //     u64 symbol_offsets_index = 0;
    // //     for (Object_Symbol & symbol : environment.symbols) {
    // //         if (symbol.flags & OBJECT_SYMBOL_DATA) {
    // //             symbol_offsets[symbol_offsets_index] = buffer_get_cursor(buffer) - code_offset;

    // //             if (context.verbose) {
    // //                 print("writing data symbol {} at {}, {} bytes\n", symbol.name, symbol_offsets[symbol_offsets_index], symbol.size);
    // //             }

    // //             buffer_write(buffer, (u8 const *)symbol.value, symbol.size);
    // //         }

    // //         symbol_offsets_index += 1;
    // //     }
    // // }

    // // u64 data_size = (u64)(buffer_get_cursor(buffer) - data_offset);

    // Write relocation table
    u64 relocation_table_offset = buffer_get_cursor(buffer);

    u32 number_of_relocations = 0;
    for (Relocation & relocation : code.relocation) {
        if (relocation.type < 1024) {
            if (context.verbose) {
                print("Writing relocation entry for symbol {} (of {})\n", relocation.symbol, environment.symbols.number_of_elements);
            }

            write_coff_relocation_entry(buffer, relocation, *find_symbol(environment, relocation.symbol), find_symbol_index(environment, relocation.symbol));
            number_of_relocations += 1;
        }
    }

    u64 relocation_table_size = (u64)(buffer_get_cursor(buffer) - relocation_table_offset);

    // Write symbol table
    u64 symbol_table_offset = buffer_get_cursor(buffer);

    u64 * symbol_string_offsets = (u64 *)allocate(environment.symbols.number_of_elements * sizeof(u64));

    {
        u64 symbol_offsets_index = 0;
        for (Object_Symbol & symbol : environment.symbols) {
            buffer_write_u32(buffer, 0); // name.zeroes
            symbol_string_offsets[symbol_offsets_index] = buffer_reserve_32(buffer); // name.offset

            if (symbol.flags & OBJECT_SYMBOL_FOREIGN_FUNCTION) {
                ASSERT_NOT_REACHED();
                // buffer_write_u8(buffer, 0x01); // n_type = N_EXT
                // buffer_write_u8(buffer, 0); // n_sect
                // buffer_write_u16(buffer, 0); // n_desc
                // buffer_write_u64(buffer, 0); // n_value
            } else if (symbol.flags & OBJECT_SYMBOL_CONSTANT) {
                ASSERT_NOT_REACHED();
                // buffer_write_u8(buffer, 0xe); // n_type = N_SECT
                // buffer_write_u8(buffer, 2); // n_sect
                // buffer_write_u16(buffer, 0); // n_desc
                // buffer_write_u64(buffer, symbol_offsets[symbol_offsets_index]); // n_value
            } else if (symbol.flags & OBJECT_SYMBOL_DATA) {
                ASSERT_NOT_REACHED();
                // buffer_write_u8(buffer, 0xe); // n_type = N_SECT
                // buffer_write_u8(buffer, 3); // n_sect
                // buffer_write_u16(buffer, 0); // n_desc
                // buffer_write_u64(buffer, symbol_offsets[symbol_offsets_index]); // n_value
            } else {
                buffer_write_u32(buffer, (u32)symbol_offsets[symbol_offsets_index]); // value // TODO cast
                buffer_write_u16(buffer, 1); // section number - 1 based index
                buffer_write_u16(buffer, 0x20); // type = function
                buffer_write_u8(buffer, 2); // storage class = IMAGE_SYM_CLASS_EXTERNAL
                buffer_write_u8(buffer, 0); // number of aux symbols
            }

            symbol_offsets_index += 1;
        }
    }

    u64 symbol_table_size = (u64)(buffer_get_cursor(buffer) - symbol_table_offset);

    // Write string table
    //
    //   Immediately following the COFF symbol table is the COFF string table. The position
    //   of this table is found by taking the symbol table address in the COFF header and
    //   adding the number of symbols multiplied by the size of a symbol.
    //
    //   At the beginning of the COFF string table are 4 bytes that contain the total size
    //   (in bytes) of the rest of the string table. This size includes the size field itself,
    //   so that the value in this location would be 4 if no strings were present.
    //
    //   Following the size are null-terminated strings that are pointed to by symbols in
    //   the COFF symbol table.
    //
    //     https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#coff-string-table
    u64 string_table_offset = buffer_get_cursor(buffer);

    u64 string_table_size_offset = buffer_reserve_32(buffer);

    u64 string_offsets_index = 0;
    for (Object_Symbol & symbol : environment.symbols) {
        buffer_patch_u32(buffer, symbol_string_offsets[string_offsets_index], (u32)(buffer_get_cursor(buffer) - string_table_offset));

        buffer_write(buffer, (u8 const *)symbol.name, utf8_length_string(symbol.name) + 1);

        string_offsets_index += 1;
    }

    u64 string_table_size = (u64)(buffer_get_cursor(buffer) - string_table_offset);

    buffer_patch_u32(buffer, string_table_size_offset, string_table_size);


    u32 text_section_characteristics = 0;
    text_section_characteristics |= 0x00000020; // IMAGE_SCN_CNT_CODE
    text_section_characteristics |= 0x00500000; // IMAGE_SCN_ALIGN_16BYTES
    text_section_characteristics |= 0x20000000; // IMAGE_SCN_MEM_EXECUTE
    text_section_characteristics |= 0x40000000; // IMAGE_SCN_MEM_READ

    write_coff_text_section(buffer, text_section_offset, 0, 0, code_size, code_offset, relocation_table_offset, number_of_relocations, text_section_characteristics);

    buffer_patch_u16(buffer, number_of_sections_offset, 1);
    buffer_patch_i32(buffer, pointer_to_symbol_table_offset, symbol_table_offset);
    buffer_patch_i32(buffer, number_of_symbols_offset, (i32)environment.symbols.number_of_elements); // TODO: cast

    write_file(file, buffer.bytes, buffer.number_of_bytes);
}
