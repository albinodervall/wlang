struct Code {
    utf8 const * target = NULL;

    Buffer buffer;

    Array<Relocation> relocation;
};
