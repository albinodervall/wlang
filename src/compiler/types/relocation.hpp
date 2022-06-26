// The values here are Mach-O specific
enum Relocation_Type {
    // TODO: https://opensource.apple.com/source/cctools/cctools-877.5/include/mach-o/arm64/reloc.h.auto.html
    RELOCATION_BR26 = 0,
    RELOCATION_PAGOF12 = 1,
    RELOCATION_PAGE21 = 2,
    RELOCATION_GOTLDP = 3,
    RELOCATION_GOTLDPOF = 4,

    // TODO: https://opensource.apple.com/source/xnu/xnu-1504.7.4/EXTERNAL_HEADERS/mach-o/x86_64/reloc.h.auto.html
    RELOCATION_X64_BRANCH = 5,
    RELOCATION_X64_SIGNED = 6,
    RELOCATION_X64_GOT_LD = 7,

    // Not Mach-O specific
    RELOCATION_LABEL_ARM64_CBZ = 1024,
    RELOCATION_LABEL_ARM64_CBNZ = 1025,
    RELOCATION_LABEL_ARM64_B = 1026,
    RELOCATION_LABEL_ARM64_B_COND = 1027,
    RELOCATION_LABEL_ARM64_B_END_OF_FUNCTION = 1028,

    //
    RELOCATION_LABEL_X64_JZ = 1029,
    RELOCATION_LABEL_X64_JNZ = 1030,
    RELOCATION_LABEL_X64_JMP = 1031,
    RELOCATION_LABEL_X64_JMP_END_OF_FUNCTION = 1032,
};

struct Relocation {
    Relocation_Type type;
    u64 address;

    union {
        utf8 const * symbol;
        u64 target;
    };
};
