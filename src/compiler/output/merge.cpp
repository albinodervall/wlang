

struct Merge_Result{
    Code code;
    Object_Environment environment;
};

Merge_Result merge(Context & context) {
    Code code;
    Object_Environment environment;

    for (Unit & unit : context.units) {
        if (unit.stage == STAGE_DISCARDED) {
            continue;
        }

        if (context.verbose) {
            print("{}\n", unit.label);
        }

        if (code.target == NULL) {
            code.target = unit.machine_code.target;
        } else {
            ASSERT(utf8_compare(code.target, unit.machine_code.target));
        }

        u64 location = code.buffer.number_of_bytes;
        buffer_copy(code.buffer, unit.machine_code.buffer);
        if (context.verbose) {
            print("size of code buffer is {} (0x{x})\n", unit.machine_code.buffer.number_of_bytes, unit.machine_code.buffer.number_of_bytes);
            print("size of the final buffer is now {} (0x{x})\n", code.buffer.number_of_bytes, code.buffer.number_of_bytes);
        }

        for (Relocation const & relocation : unit.machine_code.relocation) {
            Relocation new_relocation = Relocation{
                .type = relocation.type,
                .address = location + relocation.address,
            };

            if (relocation.type < 1024) {
                new_relocation.symbol = relocation.symbol;
            } else {
                new_relocation.target = location + relocation.target;
            }

            push(code.relocation, new_relocation);
        }

        for (Object_Symbol & symbol : unit.output_environment.symbols) {
            if (context.verbose) {
                print("copying over symbol {}, size is {}\n", symbol.name, symbol.size);
            }

            Object_Symbol new_symbol = symbol;

            if (new_symbol.address != INVALID_ADDRESS && !(new_symbol.flags & OBJECT_SYMBOL_NO_ADDRESS)) {
                if (context.verbose) {
                    print("  from 0x{x} (location = 0x{x})\n", new_symbol.address, location);
                }
                new_symbol.address = location + new_symbol.address;
                if (context.verbose) {
                    print("  to 0x{x}\n", new_symbol.address);
                }
            }

            if (new_symbol.flags & OBJECT_SYMBOL_FUNCTION) {
                if (new_symbol.postamble_address != INVALID_ADDRESS && !(new_symbol.flags & OBJECT_SYMBOL_NO_ADDRESS)) {
                    new_symbol.postamble_address = location + new_symbol.postamble_address;
                }
            }

            push(environment.symbols, new_symbol);
        }
    }

    return Merge_Result{
        .code = code,
        .environment = environment,
    };
}
