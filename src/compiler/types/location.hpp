struct Location {
    utf8 const * file = NULL;
    utf8 const * source = NULL;

    struct {
        u32 line = 0;
        u32 column = 0;
        utf8 const * pointer = NULL;
    } begin;

    struct {
        u32 line = 0;
        u32 column = 0;
        utf8 const * pointer = NULL;
    } end;
};

static Location span(Location const & begin, Location const & end) {
    ASSERT(begin.file == end.file);
    ASSERT(begin.source == end.source);

    return Location{
        .file = begin.file,
        .source = begin.source,

        .begin = {
            .line = begin.begin.line,
            .column = begin.begin.column,
            .pointer = begin.begin.pointer
        },

        .end = {
            .line = end.end.line,
            .column = end.end.column,
            .pointer = end.end.pointer
        }
    };
}

static void format(Formatter & formatter, Format const &, Location const & location) {
    if (location.begin.line != location.end.line) {
        u32 line = location.begin.line;
        utf8 const * pointer = location.begin.pointer;

        while (line <= location.end.line) {
            if (line < location.begin.line + 5 || line > location.end.line - 5) {
                utf8 const * line_pointer = line_from_begin(location.source, pointer);
                format(formatter, "\u001b[2m" "{4}:" "\u001b[0m" " " "\u001b[1m" ">" "\u001b[0m" " {*}\n", line, (u64)string_length_to_new_line(line_pointer), line_pointer);
            } else if (line == location.begin.line + 5) {
                format(formatter, "      " "\u001b[1m" ">" "\u001b[0m" " " "\u001b[2m" "..." "\u001b[0m" "\n");
            }

            line += 1;
            pointer += string_length_to_new_line(pointer) + 1;
        }
    } else {
        format(formatter, "\u001b[2m" "{4}:" "\u001b[0m" "   {*}\n", location.begin.line, (u64)string_length_to_new_line(line_from_begin(location.source, location.begin.pointer)), line_from_begin(location.source, location.begin.pointer));
        format(formatter, "        {*r}" "\u001b[1m", (u64)(location.begin.column - 1), " ");
        for (u32 column = location.begin.column; column < location.end.column; column += 1) {
            format(formatter, "^");
        }
        format(formatter, "\u001b[0m" "\n");
    }
}
