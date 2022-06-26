u64 write_macho_text_sect(Buffer & buffer, u64 offset = 0, u64 addr = 0, u64 sect_size = 0, u32 sect_offset = 0, u32 reloff = 0, u32 nreloc = 0) {
    if (offset != 0) {
        buffer_set_cursor(buffer, offset);
    }

    u64 begin = buffer_get_cursor(buffer);

    buffer_write(buffer, (u8 *)format("{16+}", "\0", "__text"), 16); // sectname
    buffer_write(buffer, (u8 *)format("{16+}", "\0", "__TEXT"), 16); // segname
    buffer_write_u64(buffer, addr); // addr
    buffer_write_u64(buffer, sect_size); // size
    buffer_write_u32(buffer, sect_offset); // offset
    buffer_write_u32(buffer, 2); // align
    buffer_write_u32(buffer, reloff); // reloff
    buffer_write_u32(buffer, nreloc); // nreloc
    buffer_write_u32(buffer, 0x80000000 | 0x00000400); // flags = S_ATTR_PURE_INSTRUCTIONS | S_ATTR_SOME_INSTRUCTIONS
    buffer_write_u32(buffer, 0); // reserved1
    buffer_write_u32(buffer, 0); // reserved2
    buffer_write_u32(buffer, 0); // reserved3

    if (offset != 0) {
        buffer_reset_cursor(buffer);
    }

    return begin;
}

u64 reserve_macho_text_sect(Buffer & buffer) {
    return write_macho_text_sect(buffer);
}

u64 write_macho_const_sect(Buffer & buffer, u64 offset = 0, u64 addr = 0, u64 sect_size = 0, u32 sect_offset = 0, u32 reloff = 0, u32 nreloc = 0) {
    if (offset != 0) {
        buffer_set_cursor(buffer, offset);
    }

    u64 begin = buffer_get_cursor(buffer);

    buffer_write(buffer, (u8 *)format("{16+}", "\0", "__const"), 16); // sectname
    buffer_write(buffer, (u8 *)format("{16+}", "\0", "__TEXT"), 16); // segname
    buffer_write_u64(buffer, addr); // addr
    buffer_write_u64(buffer, sect_size); // size
    buffer_write_u32(buffer, sect_offset); // offset
    buffer_write_u32(buffer, 0); // align
    buffer_write_u32(buffer, 0); // reloff
    buffer_write_u32(buffer, 0); // nreloc
    buffer_write_u32(buffer, 0); // flags
    buffer_write_u32(buffer, 0); // reserved1
    buffer_write_u32(buffer, 0); // reserved2
    buffer_write_u32(buffer, 0); // reserved3

    if (offset != 0) {
        buffer_reset_cursor(buffer);
    }

    return begin;
}

u64 reserve_macho_const_sect(Buffer & buffer) {
    return write_macho_const_sect(buffer);
}

u64 write_macho_data_sect(Buffer & buffer, u64 offset = 0, u64 addr = 0, u64 sect_size = 0, u32 sect_offset = 0, u32 reloff = 0, u32 nreloc = 0) {
    if (offset != 0) {
        buffer_set_cursor(buffer, offset);
    }

    u64 begin = buffer_get_cursor(buffer);

    buffer_write(buffer, (u8 *)format("{16+}", "\0", "__data"), 16); // sectname
    buffer_write(buffer, (u8 *)format("{16+}", "\0", "__DATA"), 16); // segname
    buffer_write_u64(buffer, addr); // addr
    buffer_write_u64(buffer, sect_size); // size
    buffer_write_u32(buffer, sect_offset); // offset
    buffer_write_u32(buffer, 0); // align
    buffer_write_u32(buffer, 0); // reloff
    buffer_write_u32(buffer, 0); // nreloc
    buffer_write_u32(buffer, 0); // flags
    buffer_write_u32(buffer, 0); // reserved1
    buffer_write_u32(buffer, 0); // reserved2
    buffer_write_u32(buffer, 0); // reserved3

    if (offset != 0) {
        buffer_reset_cursor(buffer);
    }

    return begin;
}

u64 reserve_macho_data_sect(Buffer & buffer) {
    return write_macho_data_sect(buffer);
}

u64 write_macho_segment_64(Buffer & buffer, u64 offset = 0, u32 cmdsize = 0, u64 vmsize = 0, u64 fileoff = 0, u64 filesize = 0, u32 nsects = 0) {
    if (offset != 0) {
        buffer_set_cursor(buffer, offset);
    }

    u64 begin = buffer_get_cursor(buffer);

    buffer_write_u32(buffer, 0x19); // cmd = LC_SEGMENT_64
    buffer_write_u32(buffer, cmdsize); // cmdsize
    buffer_reserve(buffer, 16); // segname
    buffer_write_u64(buffer, 0); // vmaddr
    buffer_write_u64(buffer, vmsize); // vmsize
    buffer_write_u64(buffer, fileoff); // fileoff
    buffer_write_u64(buffer, filesize); // filesize
    buffer_write_u32(buffer, 0x01 | 0x02 | 0x04); // maxprot = VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE
    buffer_write_u32(buffer, 0x01 | 0x02 | 0x04); // initprot = VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE
    buffer_write_u32(buffer, nsects); // nsects
    buffer_write_u32(buffer, 0); // flags

    if (offset != 0) {
        buffer_reset_cursor(buffer);
    }

    return begin;
}

u64 reserve_macho_segment_64(Buffer & buffer) {
    return write_macho_segment_64(buffer);
}

u64 write_macho_symtab(Buffer & buffer, u64 offset = 0, u32 symoff = 0, u32 nsyms = 0, u32 stroff = 0, u32 strsize = 0) {
    if (offset != 0) {
        buffer_set_cursor(buffer, offset);
    }

    u64 begin = buffer_get_cursor(buffer);

    buffer_write_u32(buffer, 0x2); // cmd = LC_SYMTAB
    u64 size_offset = buffer_reserve_32(buffer); // cmdsize
    buffer_write_u32(buffer, symoff); // symoff
    buffer_write_u32(buffer, nsyms); // nsyms
    buffer_write_u32(buffer, stroff); // stroff
    buffer_write_u32(buffer, strsize); // strsize

    buffer_patch_u32(buffer, size_offset, (u32)(buffer_get_cursor(buffer) - begin));

    if (offset != 0) {
        buffer_reset_cursor(buffer);
    }

    return begin;
}

u64 reserve_macho_symtab(Buffer & buffer) {
    return write_macho_symtab(buffer);
}

void write_macho_build_version(Buffer & buffer) {
    u64 begin = buffer_get_cursor(buffer);

    buffer_write_u32(buffer, 0x32); // cmd = LC_BUILD_VERSION
    u64 size_offset = buffer_reserve_32(buffer); // cmdsize
    buffer_write_u32(buffer, 1); // platform
    buffer_write_u32(buffer, 0xc00000); // minos
    buffer_write_u32(buffer, 0xc0100); // sdk
    buffer_write_u32(buffer, 0); // ntools

    buffer_patch_u32(buffer, size_offset, (u32)(buffer_get_cursor(buffer) - begin));
}

void write_macho_relocation_entry(Buffer & buffer, Relocation & relocation, Object_Symbol const & symbol, u32 symbolnum) {
    u32 r_symbolnum = symbolnum;
    u32 r_pcrel = 0x1;
    u32 r_length = 0x2;
    u32 r_extern = 0x1;
    u32 r_type = 0;

    switch (relocation.type) {
        case RELOCATION_BR26: {
            r_type = 2;
        } break;

        case RELOCATION_PAGOF12: {
            r_type = 4;
        } break;

        case RELOCATION_PAGE21: {
            r_type = 3;
        } break;

        case RELOCATION_GOTLDP: {
            r_type = 5;
        } break;

        case RELOCATION_GOTLDPOF: {
            r_type = 6;
        } break;

        case RELOCATION_X64_BRANCH: {
            r_type = 2;
        } break;

        case RELOCATION_X64_SIGNED: {
            r_type = 1;
        } break;

        case RELOCATION_X64_GOT_LD: {
            r_type = 3;
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

    if (r_type == 0x4 || r_type == 0x6) {
        r_pcrel = 0;
    }

    u32 entry =
        (r_symbolnum << 0) |
        (r_pcrel << 24) |
        (r_length << 25) |
        (r_extern << 27) |
        (r_type << 28);

    u32 r_address = relocation.address;

    buffer_write_u32(buffer, r_address);
    buffer_write_u32(buffer, entry);
}

void write_macho(Context & context, utf8 const * file, Object_Environment & environment, Code & code) {
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

    buffer_write_u32(buffer, 0xfeedfacf); // magic

    if (utf8_compare(code.target, "arm64")) {
        buffer_write_u32(buffer, 12 | 0x01000000); // CPU_TYPE_ARM | CPU_ARCH_ABI64
        buffer_write_u32(buffer, 0); // CPU_SUBTYPE_ARM_ALL
    } else if (utf8_compare(code.target, "x64")) {
        buffer_write_u32(buffer, 7 | 0x01000000); // CPU_TYPE_X86 | CPU_ARCH_ABI64
        buffer_write_u32(buffer, 3); // CPU_SUBTYPE_X86_ALL
    } else {
        ASSERT_NOT_REACHED();
    }

    buffer_write_u32(buffer, 0x1); // MH_OBJECT
    u64 ncmds_offset = buffer_reserve_32(buffer);
    u64 sizeofcmds_offset = buffer_reserve_32(buffer);
    buffer_write_u32(buffer, 0); // flags
    buffer_write_u32(buffer, 0); // reserved

    u64 begin = buffer_get_cursor(buffer);
    u64 segment_64_offset = reserve_macho_segment_64(buffer);
    u64 text_sect_offset = reserve_macho_text_sect(buffer);
    u64 const_sect_offset = reserve_macho_const_sect(buffer);
    u64 data_sect_offset = reserve_macho_data_sect(buffer);
    u64 segment_64_end = buffer_get_cursor(buffer);
    u64 symtab_offset = reserve_macho_symtab(buffer);
    u64 end = buffer_get_cursor(buffer);

    buffer_patch_u32(buffer, ncmds_offset, 2);
    buffer_patch_u32(buffer, sizeofcmds_offset, (u32)(end - begin));

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

    // Write consts
    u64 consts_offset = buffer_get_cursor(buffer);

    {
        u64 symbol_offsets_index = 0;
        for (Object_Symbol & symbol : environment.symbols) {
            if (symbol.flags & OBJECT_SYMBOL_CONSTANT) {
                symbol_offsets[symbol_offsets_index] = buffer_get_cursor(buffer) - code_offset;

                if (context.verbose) {
                    print("writing constant symbol {} at {}, {} bytes\n", symbol.name, symbol_offsets[symbol_offsets_index], symbol.size);
                }

                buffer_write(buffer, (u8 const *)symbol.value, symbol.size);
            }

            symbol_offsets_index += 1;
        }
    }

    u64 consts_size = (u64)(buffer_get_cursor(buffer) - consts_offset);

    // Write data
    u64 data_offset = buffer_get_cursor(buffer);

    {
        u64 symbol_offsets_index = 0;
        for (Object_Symbol & symbol : environment.symbols) {
            if (symbol.flags & OBJECT_SYMBOL_DATA) {
                symbol_offsets[symbol_offsets_index] = buffer_get_cursor(buffer) - code_offset;

                if (context.verbose) {
                    print("writing data symbol {} at {}, {} bytes\n", symbol.name, symbol_offsets[symbol_offsets_index], symbol.size);
                }

                buffer_write(buffer, (u8 const *)symbol.value, symbol.size);
            }

            symbol_offsets_index += 1;
        }
    }

    u64 data_size = (u64)(buffer_get_cursor(buffer) - data_offset);

    // Write relocation table
    u64 reloff = buffer_get_cursor(buffer);

    u32 nreloc = 0;
    for (Relocation & relocation : code.relocation) {
        if (relocation.type < 1024) {
            if (context.verbose) {
                print("Writing relocation entry for symbol {} (of {})\n", relocation.symbol, environment.symbols.number_of_elements);
            }

            write_macho_relocation_entry(buffer, relocation, *find_symbol(environment, relocation.symbol), find_symbol_index(environment, relocation.symbol));
            nreloc += 1;
        }
    }

    // Write symbol table
    u64 symoff = buffer_get_cursor(buffer);
    ASSERT(symoff < 0xFFFFFFFF);

    u64 * strx_offsets = (u64 *)allocate(environment.symbols.number_of_elements * sizeof(u64));

    {
        u64 symbol_offsets_index = 0;
        for (Object_Symbol & symbol : environment.symbols) {
            strx_offsets[symbol_offsets_index] = buffer_reserve_32(buffer); // n_strx

            if (symbol.flags & OBJECT_SYMBOL_FOREIGN_FUNCTION) {
                buffer_write_u8(buffer, 0x01); // n_type = N_EXT
                buffer_write_u8(buffer, 0); // n_sect
                buffer_write_u16(buffer, 0); // n_desc
                buffer_write_u64(buffer, 0); // n_value
            } else if (symbol.flags & OBJECT_SYMBOL_CONSTANT) {
                buffer_write_u8(buffer, 0xe); // n_type = N_SECT
                buffer_write_u8(buffer, 2); // n_sect
                buffer_write_u16(buffer, 0); // n_desc
                buffer_write_u64(buffer, symbol_offsets[symbol_offsets_index]); // n_value
            } else if (symbol.flags & OBJECT_SYMBOL_DATA) {
                buffer_write_u8(buffer, 0xe); // n_type = N_SECT
                buffer_write_u8(buffer, 3); // n_sect
                buffer_write_u16(buffer, 0); // n_desc
                buffer_write_u64(buffer, symbol_offsets[symbol_offsets_index]); // n_value
            } else {
                buffer_write_u8(buffer, 0xe | 0x01); // n_type = N_SECT | N_EXT
                buffer_write_u8(buffer, 1); // n_sect
                buffer_write_u16(buffer, 0); // n_desc
                buffer_write_u64(buffer, symbol_offsets[symbol_offsets_index]); // n_value
            }

            symbol_offsets_index += 1;
        }
    }

    // Write string table
    u64 stroff = buffer_get_cursor(buffer);
    ASSERT(stroff < 0xFFFFFFFF);

    // We need a padding byte as the first byte of the string table as n_strx = 0 means the empty string:
    //     A union that holds an index into the string table, n_strx. To specify an empty
    //     string (""), set this value to 0. The n_name field is not used in Mach-O files.
    //
    //     https://developer.apple.com/documentation/kernel/nlist
    buffer_reserve_8(buffer);

    u64 string_offsets_index = 0;
    for (Object_Symbol & symbol : environment.symbols) {
        buffer_patch_u32(buffer, strx_offsets[string_offsets_index], (u32)(buffer_get_cursor(buffer) - stroff));

        // TODO: How true is this? In what circumstances? Why?
        // On Mach-O the symbol names are prefixed with "_"
        utf8 const * name = symbol.name;
        if (symbol.flags & OBJECT_SYMBOL_FUNCTION) {
            name = format("_{}", symbol.name);
        }

        buffer_write(buffer, (u8 const *)name, utf8_length_string(name) + 1);

        string_offsets_index += 1;
    }

    u64 strsize = (u64)(buffer_get_cursor(buffer) - stroff);
    ASSERT(strsize < 0xFFFFFFFF);

    u64 addr = 0;
    write_macho_text_sect(buffer, text_sect_offset, addr, code_size, code_offset, reloff, nreloc);
    addr += code_size;

    write_macho_const_sect(buffer, const_sect_offset, addr, consts_size, consts_offset, 0, 0);
    addr += consts_size;

    write_macho_data_sect(buffer, data_sect_offset, addr, data_size, data_offset, 0, 0);
    addr += data_size;

    u64 content_offset = code_offset;
    u64 content_size = code_size + consts_size + data_size;

    write_macho_segment_64(buffer, segment_64_offset, (u64)(segment_64_end - segment_64_offset), content_size, content_offset, content_size, 3);

    ASSERT(environment.symbols.number_of_elements < 0xFFFFFFFF);
    write_macho_symtab(buffer, symtab_offset, symoff, environment.symbols.number_of_elements, stroff, strsize);

    write_file(file, buffer.bytes, buffer.number_of_bytes);
}
