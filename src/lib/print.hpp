b32 bit_set_after(u64 value, u64 bit) {
    return value > (bit + (bit - 1));
}

static utf8 * utf8_copy(utf8 const * begin, utf8 const * end);

struct Format {
    u64 width = 0;
    u64 fractional = 0;
    b32 zero = FALSE;
    b32 is_fractional = FALSE;
    u32 base = 10;
    b32 repeat = FALSE;
    b32 implicit = FALSE;
    b32 uppercase = FALSE;
    utf8 const * delimiter = ", ";
    utf8 const * pad = NULL;
};

struct Formatter {
    u32 depth = 0;

    u64 offset = 0;
    u64 capacity = 0;
    utf8 * pointer = NULL;
};

static void _format_output(Formatter & formatter, utf8 value) {
    if (formatter.offset + 1 > formatter.capacity) {
        formatter.capacity = 2 * (formatter.capacity + 1);
        formatter.pointer = (utf8 *)reallocate(formatter.pointer, formatter.capacity);
    }

    *(formatter.pointer + formatter.offset) = value;
    formatter.offset += 1;
}

static void format(Formatter & formatter, Format const & specifier, u64 argument) {
    ASSERT(specifier.repeat == FALSE);

    if (argument > 0) {
        u64 begin = formatter.offset;

        u64 width = 0;
        while (argument > 0) {
            width += 1;

            u64 value = argument % specifier.base;
            argument = argument / specifier.base;

            if (specifier.uppercase) {
                utf8 const * digits = "0123456789ABCDEF";
                _format_output(formatter, digits[value]);
            } else {
                utf8 const * digits = "0123456789abcdef";
                _format_output(formatter, digits[value]);
            }
        }

        while (width < specifier.width) {
            if (specifier.zero) {
                _format_output(formatter, '0');
            } else {
                _format_output(formatter, ' ');
            }
            width += 1;
        }

        u64 end = formatter.offset;

        // Reverse
        for (u64 index = 0; index < (end - begin) / 2; index += 1) {
            utf8 temporary = formatter.pointer[begin + index];
            formatter.pointer[begin + index] = formatter.pointer[end - index - 1];
            formatter.pointer[end - index - 1] = temporary;
        }
    } else {
        if (specifier.width > 0) {
            u64 width = 0;
            while (width < specifier.width - 1) {
                if (specifier.zero) {
                    _format_output(formatter, '0');
                } else {
                    _format_output(formatter, ' ');
                }
                width += 1;
            }
        }

        _format_output(formatter, '0');
    }
}

static void format(Formatter & formatter, Format const & specifier, u32 argument) {
    format(formatter, specifier, (u64)argument);
}

static void format(Formatter & formatter, Format const & specifier, i64 argument) {
    if (argument < 0) {
        _format_output(formatter, '-');
        argument = -argument;
    }

    format(formatter, specifier, (u64)argument);
}

static void format(Formatter & formatter, Format const & specifier, i32 argument) {
    format(formatter, specifier, (i64)argument);
}

static void format(Formatter & formatter, Format const & specifier, void const * argument) {
    format(formatter, specifier, (u64)argument);
}

static void format(Formatter & formatter, Format const & specifier, utf8 const * argument) {
    if (specifier.implicit && argument == NULL) {
        return;
    }

    u64 width = 0;

    utf8 const * cursor = argument;
    while (((specifier.width == 0 && !specifier.repeat && !specifier.pad) || width < specifier.width) && (*cursor || specifier.pad)) {
        width += 1;

        if (specifier.pad && *cursor == '\0') {
            _format_output(formatter, specifier.pad[0]);
        } else {
            _format_output(formatter, *cursor);
            cursor += 1;
        }

        if (specifier.repeat && *cursor == '\0') {
            cursor = argument;
        }
    }
}

static void format(Formatter & formatter, Format const & specifier, f64 argument) {
    // TODO: There is an issue here if the argument is larger than the largest i64
    u64 integer = (i64)argument;
    // format(formatter, specifier, integer);
    format(formatter, Format{}, integer);

    if (argument < 0.0) {
        argument = -argument;
        integer = -integer;
    }

    f64 fraction = argument - integer;

    format(formatter, Format{}, ".");

    // This is not a very accurate way to print floating point numbers!
    for (u64 index = 0; index < (specifier.is_fractional ? specifier.fractional : 6); index += 1) {
        fraction = 10.0 * fraction;
        integer = (u64)fraction;
        fraction = fraction - integer;

        utf8 const * digits = "0123456789";
        _format_output(formatter, digits[integer]);
    }
}

static void format(Formatter & formatter, Format const & specifier, f32 argument) {
    format(formatter, specifier, (f64)argument);
}

static void format(Formatter &, Format const &, Format const &) {
    ASSERT_NOT_REACHED();
}

static void _format_advance(Formatter & formatter, utf8 const * & string) {
    while (*string) {
        if (*string == '{') {
            return;
        } else if (*string == '\\' && *(string + 1) == '{') {
            string += 1;
        } else if (*string == '\\' && *(string + 1) == '}') {
            string += 1;
        }

        _format_output(formatter, *string);
        string += 1;
    }
}

template<class T>
u64 get_width(T const & argument) {
    ASSERT_NOT_REACHED();
}

static u64 get_width(u64 argument) {
    return argument;
}

template<class T>
utf8 const * get_pad(T const & argument) {
    ASSERT_NOT_REACHED();
}

static utf8 const * get_pad(utf8 const * argument) {
    return argument;
}

template<class ... T>
void format(Formatter & formatter, utf8 const * string, T ... arguments) {
    // If this is a continuation, reverse one byte to
    // remove the null terminator.
    if (formatter.depth == 0 && formatter.offset > 0) {
        formatter.offset = formatter.offset - 1;
    }

    formatter.depth += 1;

    Format specifier;
    b32 deferred = FALSE;

    ([&]{
        if (deferred) {
            deferred = FALSE;

            format(formatter, specifier, arguments);
        } else {
            ASSERT(*string != '\0');

            _format_advance(formatter, string);

            ASSERT(*string == '{');
            string += 1;

            specifier = Format{};

            if (*string == '*') {
                string += 1;

                specifier.width = get_width(arguments);
                deferred = TRUE;
            }

            if (*string == '0') {
                specifier.zero = TRUE;

                string += 1;
            }

            if (*string >= '1' && *string <= '9') {
                specifier.width = (*string - '0');

                string += 1;

                while (*string && *string >= '0' && *string <= '9') {
                    specifier.width *= 10;
                    specifier.width += (*string - '0');

                    string += 1;
                }
            }

            if (*string == '.') {
                specifier.is_fractional = TRUE;
                string += 1;

                if (*string >= '1' && *string <= '9') {
                    specifier.fractional = (*string - '0');

                    string += 1;

                    while (*string && *string >= '0' && *string <= '9') {
                        specifier.fractional *= 10;
                        specifier.fractional += (*string - '0');

                        string += 1;
                    }
                }
            }

            while (*string && *string != '}') {
                if (*string == 'r') {
                    specifier.repeat = TRUE;
                } else if (*string == 'b') {
                    specifier.base = 2;
                } else if (*string == 'x') {
                    specifier.base = 16;
                } else if (*string == 'X') {
                    specifier.base = 16;
                    specifier.uppercase = TRUE;
                } else if (*string == '!') {
                    specifier.implicit = TRUE;
                } else if (*string == 'd') {
                    string += 1;

                    ASSERT(*string == '(');
                    string += 1;

                    utf8 const * begin = string;

                    while (*string && *string != ')') {
                        string += 1;
                    }

                    utf8 const * end = string;

                    specifier.delimiter = utf8_copy(begin ,end);

                    ASSERT(*string == ')');
                } else if (*string == '+') {
                    specifier.pad = get_pad(arguments);
                    deferred = TRUE;
                } else {
                    ASSERT_NOT_REACHED();
                }

                string += 1;
            }

            ASSERT(*string == '}');
            string += 1;

            if (!deferred) {
                format(formatter, specifier, arguments);
            }
        }
    }(), ...);

    if (*string != '\0') {
        _format_advance(formatter, string);
    }

    ASSERT(*string == '\0');

    formatter.depth -= 1;

    if (formatter.depth == 0) {
        _format_output(formatter, '\0');
    }
}

template<class ... T>
utf8 * format(utf8 const * string, T ... arguments) {
    Formatter formatter;
    format(formatter, string, arguments...);

    return formatter.pointer;
}

extern "C" int printf(const char *, ... );

template<class ... T>
void print(utf8 const * string, T ... arguments) {
    utf8 const * result = format(string, arguments...);
    printf("%s", result);
}
