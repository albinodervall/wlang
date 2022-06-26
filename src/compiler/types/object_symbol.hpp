enum Object_Symbol_Flags {
    OBJECT_SYMBOL_FLAGS_NONE = 0,
    OBJECT_SYMBOL_FUNCTION = 1 << 0,
    OBJECT_SYMBOL_FOREIGN_FUNCTION = 1 << 1,
    OBJECT_SYMBOL_ENTRY_FUNCTION = 1 << 2,
    OBJECT_SYMBOL_NO_ADDRESS = 1 << 3,
    OBJECT_SYMBOL_CONSTANT = 1 << 4,
    OBJECT_SYMBOL_DATA = 1 << 5,
};

// Christ almighty...
inline Object_Symbol_Flags operator|(Object_Symbol_Flags a, Object_Symbol_Flags b) {
    return (Object_Symbol_Flags)((u32)a | (u32)b);
}

inline Object_Symbol_Flags operator&(Object_Symbol_Flags a, Object_Symbol_Flags b) {
    return (Object_Symbol_Flags)((u32)a & (u32)b);
}

inline Object_Symbol_Flags & operator|=(Object_Symbol_Flags& a, Object_Symbol_Flags b) {
    return (Object_Symbol_Flags &)((u32 &)a |= (u32)b);
}

struct Object_Symbol {
    Object_Symbol_Flags flags;
    utf8 const * name;
    u64 address;
    u64 postamble_address;
    u64 size;
    void const * value;
};

struct Object_Environment {
    Array<Object_Symbol> symbols;
};

Object_Symbol * find_symbol(Object_Environment & environment, utf8 const * name) {
    for (Object_Symbol & symbol : environment.symbols) {
        if (utf8_compare(symbol.name, name)) {
            return &symbol;
        }
    }

    return NULL;
}

u32 find_symbol_index(Object_Environment & environment, utf8 const * name) {
    u64 index = 0;
    for (Object_Symbol & symbol : environment.symbols) {
        if (utf8_compare(symbol.name, name)) {
            return index;
        }

        index += 1;
    }

    ASSERT_NOT_REACHED();
}
