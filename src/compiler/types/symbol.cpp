enum Symbol_Flags : u8 {
    DECLARATION_FLAGS_NONE = 0,
    DECLARATION_FLAGS_FUNCTION = 1 << 0,
    DECLARATION_FLAGS_LEAF_FUNCTION = 1 << 1,
    DECLARATION_FLAGS_FOREIGN_FUNCTION = 1 << 2,
    DECLARATION_FLAGS_CONSTANT = 1 << 3,
    DECLARATION_FLAGS_DATA = 1 << 4,
    DECLARATION_FLAGS_NO_ADDRESS = 1 << 5,

    DECLARATION_FLAGS_ENTRY_FUNCTION = 1 << 6,
    DECLARATION_FLAGS_DEFINED = 1 << 7, // If it exists it has been declared, but it might not yet be defined
};

void format(Formatter & formatter, Format const &, Symbol_Flags & argument) {
    if (argument == DECLARATION_FLAGS_NONE) {
        format(formatter, "DECLARATION_FLAGS_NONE");
    } else {
        Symbol_Flags const flags[] = {
            DECLARATION_FLAGS_FUNCTION,
            DECLARATION_FLAGS_LEAF_FUNCTION,
            DECLARATION_FLAGS_FOREIGN_FUNCTION,
            DECLARATION_FLAGS_CONSTANT,
            DECLARATION_FLAGS_DATA,
            DECLARATION_FLAGS_NO_ADDRESS,
            DECLARATION_FLAGS_ENTRY_FUNCTION,
            DECLARATION_FLAGS_DEFINED,
        };

        utf8 const * strings[] = {
            "DECLARATION_FLAGS_FUNCTION",
            "DECLARATION_FLAGS_LEAF_FUNCTION",
            "DECLARATION_FLAGS_FOREIGN_FUNCTION",
            "DECLARATION_FLAGS_CONSTANT",
            "DECLARATION_FLAGS_DATA",
            "DECLARATION_FLAGS_NO_ADDRESS",
            "DECLARATION_FLAGS_ENTRY_FUNCTION",
            "DECLARATION_FLAGS_DEFINED",
        };

        int first = TRUE;
        for (u64 index = 0; index < sizeof(flags) / sizeof(flags[0]); index += 1) {
            if (argument & flags[index]) {
                if (!first) {
                    format(formatter, " | ");
                }

                format(formatter, strings[index]);

                first = FALSE;
            }
        }
    }
}

inline Symbol_Flags operator|(Symbol_Flags a, Symbol_Flags b) {
    return (Symbol_Flags)((u8)a | (u8)b);
}

inline Symbol_Flags & operator|=(Symbol_Flags& a, Symbol_Flags b) {
    return (Symbol_Flags &)((u8 &)a |= (u8)b);
}

struct Symbol {
    b32 global;
    Symbol * previous;

    Symbol_Flags flags;
    utf8 const * name;

    struct {
        Type * type;
        Array<Symbol_Function_Parameter> parameters;
        Address address;
        u64 size;
        Address postamble_address;
        void const * value;
        i32 register_index;
    } value;
};

void format(Formatter & formatter, Format const &, Symbol & argument) {
    format(formatter, "Symbol\\{\n");
    format(formatter, "    global = {}\n", argument.global);
    format(formatter, "    previous = {}\n", argument.previous);
    format(formatter, "    flags = {}\n", argument.flags);
    format(formatter, "    name = {}\n", argument.name);
    format(formatter, "    value = \\{\n");
    format(formatter, "        type = {}\n", argument.value.type);
    format(formatter, "        parameters = {}\n", argument.value.parameters);
    format(formatter, "        address = {}\n", argument.value.address);
    format(formatter, "        size = {}\n", argument.value.size);
    format(formatter, "        postamble_address = {}\n", argument.value.postamble_address);
    format(formatter, "        value = {}\n", argument.value.value);
    format(formatter, "        register_index = {}\n", argument.value.register_index);
    format(formatter, "    \\}\n");
    format(formatter, "\\}\n");
}

// TODO: This could match for example C++'s name mangling. But it should definitely
//       be better than this.
utf8 const * name_for_symbol(Symbol const * symbol) {
    return format("{}", symbol->name);
    // return format("{}@{}", symbol->name, symbol->value.type);
}

Symbol * find_symbol(Symbol * current, utf8 const * name) {
    // print("find_symbol: {}\n", name);

    do {
        // print("  comparing with {}\n", current->symbol.name);
        if (utf8_compare(current->name, name)) {
            return current;
        }

        current = current->previous;
    } while (current);

    return NULL;
}

Symbol * push_symbol(Symbol * parent, Symbol const & symbol) {
    // TOOD: These should be linearly allocated in a tree fashion
    Symbol * child = (Symbol *)allocate(sizeof(Symbol));
    *child = symbol;
    child->previous = parent;

    return child;
}

Symbol * find_first_global_symbol(Symbol * current) {
    do {
        if (current->global) {
            return current;
        }

        current = current->previous;
    } while (current);

    return NULL;
}

Symbol * make_root() {
    Symbol * last = push_symbol(NULL, Symbol{
        .global = TRUE,
        .flags = DECLARATION_FLAGS_DEFINED,
        .name = "void",
        .value = {
            .type = type_type(&type_void),
        }
    });

    last = push_symbol(last, Symbol{
        .global = TRUE,
        .flags = DECLARATION_FLAGS_DEFINED,
        .name = "u8",
        .value = {
            .type = type_type(&type_u8),
        }
    });

    last = push_symbol(last, Symbol{
        .global = TRUE,
        .flags = DECLARATION_FLAGS_DEFINED,
        .name = "u16",
        .value = {
            .type = type_type(&type_u16),
        }
    });

    last = push_symbol(last, Symbol{
        .global = TRUE,
        .flags = DECLARATION_FLAGS_DEFINED,
        .name = "u32",
        .value = {
            .type = type_type(&type_u32),
        }
    });

    last = push_symbol(last, Symbol{
        .global = TRUE,
        .flags = DECLARATION_FLAGS_DEFINED,
        .name = "u64",
        .value = {
            .type = type_type(&type_u64),
        }
    });

    last = push_symbol(last, Symbol{
        .global = TRUE,
        .flags = DECLARATION_FLAGS_DEFINED,
        .name = "i8",
        .value = {
            .type = type_type(&type_i8),
        }
    });

    last = push_symbol(last, Symbol{
        .global = TRUE,
        .flags = DECLARATION_FLAGS_DEFINED,
        .name = "i16",
        .value = {
            .type = type_type(&type_i16),
        }
    });

    last = push_symbol(last, Symbol{
        .global = TRUE,
        .flags = DECLARATION_FLAGS_DEFINED,
        .name = "i32",
        .value = {
            .type = type_type(&type_i32),
        }
    });

    last = push_symbol(last, Symbol{
        .global = TRUE,
        .flags = DECLARATION_FLAGS_DEFINED,
        .name = "i64",
        .value = {
            .type = type_type(&type_i64),
        }
    });

    last = push_symbol(last, Symbol{
        .global = TRUE,
        .flags = DECLARATION_FLAGS_DEFINED,
        .name = "boolean",
        .value = {
            .type = type_type(&type_boolean),
        }
    });

    last = push_symbol(last, Symbol{
        .global = TRUE,
        .flags = DECLARATION_FLAGS_DEFINED,
        .name = "string",
        .value = {
            .type = type_type(type_string()),
        }
    });

    return last;
}

void print_environment(Symbol * tail) {
    do {
        print("  {}{}\n", tail->name, tail->global ? " (global)" : "");
        print("  | {}\n", tail->value.type);

        tail = tail->previous;
    } while (tail);
}
