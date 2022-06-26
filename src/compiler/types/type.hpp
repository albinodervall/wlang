struct Type;

struct Structure_Field {
    utf8 const * name;
    Type * type;
    u64 offset;
};

void format(Formatter & formatter, Format const &, Structure_Field & argument) {
    format(formatter, "{}: {}", argument.name, argument.type);
}

enum Type_Type : u32 {
    TYPE_NONE,
    TYPE_VOID,
    TYPE_INTEGRAL,
    TYPE_U8,
    TYPE_U16,
    TYPE_U32,
    TYPE_U64,
    TYPE_I8,
    TYPE_I16,
    TYPE_I32,
    TYPE_I64,
    TYPE_STRING,
    TYPE_BOOLEAN,
    TYPE_FUNCTION,
    TYPE_RANGE,
    TYPE_STRUCTURE,
    TYPE_TYPE,
    TYPE_POINTER,
    TYPE_OPAQUE,
    TYPE_ARRAY,
    TYPE_SPAN,

    // Not an actual instantiable type, but a type used for type inference
    TYPE_ITERABLE,
};

enum Type_Flags : u32 {
    TYPE_FLAGS_NONE = 0,
    TYPE_FLAG_INTEGRAL = (1 << 0),
    TYPE_FLAG_SIGNED = (1 << 1),
};

// Christ almighty...
inline Type_Flags operator|(Type_Flags a, Type_Flags b) {
    return (Type_Flags)((u8)a | (u8)b);
}

inline Type_Flags operator&(Type_Flags a, Type_Flags b) {
    return (Type_Flags)((u8)a & (u8)b);
}

inline Type_Flags & operator|=(Type_Flags & a, Type_Flags b) {
    return (Type_Flags &)((u8 &)a |= (u8)b);
}

struct Type {
    Type_Type type;
    Type_Flags flags;

    u32 size;
    u32 alignment;

    union {
        struct {
            u64 minimum;
            u64 maximum;
        } integral;

        struct {
            Type * return_type;
            Array<Type *> parameters;
        } function;

        struct {
        } structure;

        struct {
            Type * base;
        } type_type;

        struct {
            Type * base;
        } pointer;

        struct {
            u64 count;
            Type * base;
        } array;

        struct {
            Type * base;
        } span;

        struct {
            Type * base;
        } range;

        struct {
            utf8 const * name;
        } opaque;

        struct {
            Type * base;
        } iterable;

        struct {
        } string;
    };

    Array<Structure_Field> fields;
};

Type * type_pointer(Type * base);

Type type_none = Type{ .type = TYPE_NONE, .flags = TYPE_FLAGS_NONE, .size = 0, .alignment = 0 };
Type type_void = Type{ .type = TYPE_VOID, .flags = TYPE_FLAGS_NONE, .size = 0, .alignment = 0 };
Type type_integral = Type{ .type = TYPE_INTEGRAL, .flags = TYPE_FLAGS_NONE, .size = 0, .alignment = 0 }; // TODO: Should this be TYPE_FLAG_INTEGRAL | TYPE_FLAG_SIGNED? I guess not cause we use TYPE_FLAG_INTEGRAL to denote a concrete integer type...
Type type_u8 = Type{ .type = TYPE_U8, .flags = TYPE_FLAG_INTEGRAL, .size = 1, .alignment = 1, .integral = { .minimum = 0, .maximum = 255 } };
Type type_u16 = Type{ .type = TYPE_U16, .flags = TYPE_FLAG_INTEGRAL, .size = 2, .alignment = 2, .integral = { .minimum = 0, .maximum = 65535 } };
Type type_u32 = Type{ .type = TYPE_U32, .flags = TYPE_FLAG_INTEGRAL, .size = 4, .alignment = 4, .integral = { .minimum = 0, .maximum = 4294967295 } };
Type type_u64 = Type{ .type = TYPE_U64, .flags = TYPE_FLAG_INTEGRAL, .size = 8, .alignment = 8, .integral = { .minimum = 0, .maximum = 18446744073709551615ULL } };
Type type_i8 = Type{ .type = TYPE_I8, .flags = TYPE_FLAG_INTEGRAL | TYPE_FLAG_SIGNED, .size = 1, .alignment = 1, .integral = { .minimum = 128, .maximum = 127 } };
Type type_i16 = Type{ .type = TYPE_I16, .flags = TYPE_FLAG_INTEGRAL | TYPE_FLAG_SIGNED, .size = 2, .alignment = 2, .integral = { .minimum = 32768, .maximum = 32767 } };
Type type_i32 = Type{ .type = TYPE_I32, .flags = TYPE_FLAG_INTEGRAL | TYPE_FLAG_SIGNED, .size = 4, .alignment = 4, .integral = { .minimum = 2147483648, .maximum = 2147483647 } };
Type type_i64 = Type{ .type = TYPE_I64, .flags = TYPE_FLAG_INTEGRAL | TYPE_FLAG_SIGNED, .size = 8, .alignment = 8, .integral = { .minimum = 9223372036854775808ULL, .maximum = 9223372036854775807ULL } };
Type type_boolean = Type{ .type = TYPE_BOOLEAN, .flags = TYPE_FLAGS_NONE, .size = 1, .alignment = 1 };

void format(Formatter & formatter, Format const &, Type const * argument) {
    if (argument != NULL) {
        switch (argument->type) {
            case TYPE_NONE: {
                format(formatter, "none");
            } break;

            case TYPE_VOID: {
                format(formatter, "void");
            } break;

            case TYPE_INTEGRAL: {
                format(formatter, "integral");
            } break;

            case TYPE_U8: {
                format(formatter, "u8");
            } break;

            case TYPE_U16: {
                format(formatter, "u16");
            } break;

            case TYPE_U32: {
                format(formatter, "u32");
            } break;

            case TYPE_U64: {
                format(formatter, "u64");
            } break;

            case TYPE_I8: {
                format(formatter, "i8");
            } break;

            case TYPE_I16: {
                format(formatter, "i16");
            } break;

            case TYPE_I32: {
                format(formatter, "i32");
            } break;

            case TYPE_I64: {
                format(formatter, "i64");
            } break;

            case TYPE_BOOLEAN: {
                format(formatter, "boolean");
            } break;

            case TYPE_STRING: {
                format(formatter, "string");
            } break;

            case TYPE_FUNCTION: {
                format(formatter, "function ({}) {}", argument->function.parameters, argument->function.return_type);
            } break;

            case TYPE_RANGE: {
                format(formatter, "range({})", argument->range.base);
            } break;

            case TYPE_STRUCTURE: {
                format(formatter, "structure");
            } break;

            case TYPE_TYPE: {
                format(formatter, "type<{}>", argument->type_type.base);
            } break;

            case TYPE_POINTER: {
                format(formatter, "&{}", argument->pointer.base);
            } break;

            case TYPE_OPAQUE: {
                format(formatter, "'{}'", argument->opaque.name);
            } break;

            case TYPE_SPAN: {
                format(formatter, "[]{}", argument->span.base);
            } break;

            case TYPE_ARRAY: {
                format(formatter, "[{}]{}", argument->array.count, argument->array.base);
            } break;

            case TYPE_ITERABLE: {
                format(formatter, "iterable({})", argument->iterable.base);
            } break;
        }
    } else {
        format(formatter, "none");
    }
}

b32 types_are_compatible(Type const * a, Type const * b) {
    if (a == b) {
        return TRUE;
    } else if (a->type == TYPE_POINTER && b->type == TYPE_POINTER) {
        // TODO: Consider if this is actually what we want... Passing any pointer
        //       to void seems fine, but is the other way around fine too?
        if (a->pointer.base == &type_void || b->pointer.base == &type_void) {
            return TRUE;
        }
    } else if (a == &type_integral && (b->flags & TYPE_FLAG_INTEGRAL)) {
        return TRUE;
    } else if ((a->flags & TYPE_FLAG_INTEGRAL) && b == &type_integral) {
        return TRUE;
    } else if (a->type == TYPE_POINTER && b->type == TYPE_FUNCTION) {
        if (a->pointer.base == b) {
            return TRUE;
        }
    } else if (a->type == TYPE_SPAN && b->type == TYPE_ARRAY) {
        if (a->span.base == b->array.base) {
            return TRUE;
        }
    // } else if (a->type == TYPE_POINTER && b->type == TYPE_ARRAY) {
    //     if (a->pointer.base == b->array.base) {
    //         return TRUE;
    //     }
    }

    return FALSE;
}

b32 type_can_be_casted_to_type(Type * from, Type * to) {
    if (from == to) {
        return TRUE;
    } else if (((from->flags & TYPE_FLAG_INTEGRAL) || from == &type_integral) && (to->flags & TYPE_FLAG_INTEGRAL)) {
        // TODO: Shud we maybe has a checked cast?
        return TRUE;
    } else if (from->type == TYPE_POINTER && to->type == TYPE_POINTER) {
        // TODO: Unsure if we should allow this
        return TRUE;
    } else if (from->type == TYPE_FUNCTION && to->type == TYPE_FUNCTION) {
        return TRUE;
    }

    return FALSE;
}

b32 type_is_complex(Type const * type) {
    switch (type->type) {
        case TYPE_NONE:
        case TYPE_VOID:
        case TYPE_INTEGRAL:
        case TYPE_U8:
        case TYPE_U16:
        case TYPE_U32:
        case TYPE_U64:
        case TYPE_I8:
        case TYPE_I16:
        case TYPE_I32:
        case TYPE_I64: {
            return FALSE;
        } break;

        case TYPE_STRING: {
            return TRUE;
        } break;

        case TYPE_BOOLEAN: {
            return FALSE;
        } break;

        case TYPE_FUNCTION: {
            return FALSE;
        } break;

        case TYPE_RANGE: {
            return TRUE;
        } break;

        case TYPE_STRUCTURE: {
            return TRUE;
        } break;

        case TYPE_TYPE: {
            return TRUE;
        } break;

        case TYPE_POINTER: {
            return FALSE;
        } break;

        case TYPE_OPAQUE: {
            return FALSE;
        } break;

        case TYPE_ARRAY: {
            return TRUE;
        } break;

        case TYPE_SPAN: {
            return TRUE;
        } break;

        // Not an actual instantiable type, but a type used for type inference
        case TYPE_ITERABLE: {
            ASSERT_NOT_REACHED();
        } break;
    }
}

Array<Type *> types;

// For now, all structs are unique. But maybe we want anonymous structs that can be
// duck typed.
Type * type_structure(u64 size, u64 alignment, Array<Structure_Field> & fields) {
    Type * type = (Type *)allocate(sizeof(Type));
    type->type = TYPE_STRUCTURE;
    type->size = size;
    type->alignment = alignment;
    type->fields = fields;

    return type;
}

// For now, all functions are unique. But we probably want functions to match only on return type and parameter.
Type * type_function(Type * return_type, Array<Type *> parameters) {
    for (Type * candidate_type : types) {
        if (candidate_type->type == TYPE_FUNCTION) {
            if (candidate_type->function.return_type == return_type) {
                if (candidate_type->function.parameters.number_of_elements == parameters.number_of_elements) {
                    b32 matches = TRUE;
                    for (u64 parameters_index = 0; parameters_index < candidate_type->function.parameters.number_of_elements; parameters_index += 1) {
                        // Name is also important because we have named function arguments
                        Type * expected_type = candidate_type->function.parameters[parameters_index];
                        if (expected_type != parameters[parameters_index]) {
                            matches = FALSE;
                            break;
                        }
                    }

                    if (matches) {
                        return candidate_type;
                    }
                }
            }
        }
    }

    Type * type = (Type *)allocate(sizeof(Type));
    type->type = TYPE_FUNCTION;
    type->function.return_type = return_type;
    type->function.parameters = parameters;
    type->size = 8;
    type->alignment = 8;

    push(types, type);

    return type;
}

Type * type_type(Type * base) {
    for (Type * type : types) {
        if (type->type == TYPE_TYPE && type->type_type.base == base) {
            return type;
        }
    }

    Type * type = (Type *)allocate(sizeof(Type));
    type->type = TYPE_TYPE;
    type->type_type.base = base;

    push(types, type);

    return type;
}

Type * type_pointer(Type * base) {
    for (Type * type : types) {
        if (type->type == TYPE_POINTER && type->pointer.base == base) {
            return type;
        }
    }

    Type * type = (Type *)allocate(sizeof(Type));
    type->type = TYPE_POINTER;
    type->pointer.base = base;
    type->size = 8;
    type->alignment = 8;

    push(types, type);

    return type;
}

Type * type_span(Type * base) {
    for (Type * type : types) {
        if (type->type == TYPE_SPAN && type->span.base == base) {
            return type;
        }
    }

    Type * type = (Type *)allocate(sizeof(Type));
    type->type = TYPE_SPAN;
    type->span.base = base;
    type->size = 16;
    type->alignment = 8;

    push(type->fields, Structure_Field{ .name = "pointer", .type = type_pointer(base), .offset = 0 });
    push(type->fields, Structure_Field{ .name = "number_of_elements", .type = &type_u64, .offset = 8 });

    push(types, type);

    return type;
}

// NOTE: Since all fields are now in an array, we cannot statically define this...
Type * type_string() {
    for (Type * type : types) {
        if (type->type == TYPE_STRING) {
            return type;
        }
    }

    Type * type = (Type *)allocate(sizeof(Type));
    type->type = TYPE_STRING;
    type->flags = TYPE_FLAGS_NONE;
    type->size = 16;
    type->alignment = 8;

    push(type->fields, Structure_Field{ .name = "pointer", .type = type_pointer(&type_u8), .offset = 0 });
    push(type->fields, Structure_Field{ .name = "number_of_bytes", .type = &type_u64, .offset = 8 });

    push(types, type);

    return type;
}

Type * type_array(u64 count, Type * base) {
    for (Type * type : types) {
        if (type->type == TYPE_ARRAY && type->array.count == count && type->array.base == base) {
            return type;
        }
    }

    Type * type = (Type *)allocate(sizeof(Type));
    type->type = TYPE_ARRAY;
    type->array.count = count;
    type->array.base = base;
    type->size = count * base->size;
    type->alignment = base->alignment;

    push(type->fields, Structure_Field{ .name = "pointer", .type = type_pointer(base), .offset = 0 });
    push(type->fields, Structure_Field{ .name = "number_of_elements", .type = &type_u64, .offset = 8 });

    push(types, type);

    return type;
}

Type * type_range(Type * base) {
    for (Type * type : types) {
        if (type->type == TYPE_RANGE && type->range.base == base) {
            return type;
        }
    }

    Type * type = (Type *)allocate(sizeof(Type));
    type->type = TYPE_RANGE;
    type->range.base = base;
    type->size = 2 * base->size;
    type->alignment = base->alignment;

    push(types, type);

    return type;
}

Type * type_opaque(utf8 const * name) {
    for (Type * type : types) {
        if (type->type == TYPE_OPAQUE && utf8_compare(type->opaque.name, name)) {
            return type;
        }
    }

    Type * type = (Type *)allocate(sizeof(Type));
    type->type = TYPE_OPAQUE;
    type->opaque.name = name;

    push(types, type);

    return type;
}

Type * type_iterable(Type * base) {
    for (Type * type : types) {
        if (type->type == TYPE_ITERABLE && type->iterable.base == base) {
            return type;
        }
    }

    Type * type = (Type *)allocate(sizeof(Type));
    type->type = TYPE_ITERABLE;
    type->iterable.base = base;

    push(types, type);

    return type;
}

Type * unwrap_type(Type * type) {
    ASSERT(type != NULL);

    if (type->type == TYPE_TYPE) {
        type = type->type_type.base;
    }

    ASSERT(type != NULL);

    switch (type->type) {
        case TYPE_NONE:
        case TYPE_VOID:
        case TYPE_INTEGRAL:
        case TYPE_U8:
        case TYPE_U16:
        case TYPE_U32:
        case TYPE_U64:
        case TYPE_I8:
        case TYPE_I16:
        case TYPE_I32:
        case TYPE_I64:
        case TYPE_STRING:
        case TYPE_BOOLEAN:
        case TYPE_FUNCTION:
        case TYPE_RANGE:
        case TYPE_OPAQUE:
        case TYPE_STRUCTURE:
        case TYPE_ITERABLE: {
            return type;
        } break;

        case TYPE_TYPE: {
            return unwrap_type(type);
        } break;

        case TYPE_POINTER: {
            return type_pointer(unwrap_type(type->pointer.base));
        } break;

        case TYPE_SPAN: {
            return type_span(unwrap_type(type->span.base));
        } break;

        case TYPE_ARRAY: {
            return type_array(type->array.count, unwrap_type(type->array.base));
        } break;
    }

    ASSERT_NOT_REACHED();
}

utf8 const * type_to_string(Type * type) {
    return format("{}", type);
}
