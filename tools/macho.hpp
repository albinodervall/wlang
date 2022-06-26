enum LC : u32 {
    LC_SEGMENT_64 = 0x19,
    LC_BUILD_VERSION = 0x32,
    LC_SYMTAB = 0x02,
    LC_DSYMTAB = 0x0b,
};

utf8 const * lc_to_string(LC lc) {
    switch (lc) {
        case LC_SEGMENT_64: { return "LC_SEGMENT_64"; } break;
        case LC_BUILD_VERSION: { return "LC_BUILD_VERSION"; } break;
        case LC_SYMTAB: { return "LC_SYMTAB"; } break;
        case LC_DSYMTAB: { return "LC_DSYMTAB"; } break;

        default: {
            ASSERT_NOT_REACHED();
        } break;
    }
}

#define CPU_ARCH_ABI64 0x1000000
#define CPU_TYPE_I386 0x7
#define CPU_TYPE_ARM 0xc

utf8 const * cpu_type_to_string(i32 cputype) {
    if (cputype == 0x1000007) {
        return "CPU_TYPE_I386 | CPU_ARCH_ABI64";
    } else if (cputype == 0x100000c) {
        return "CPU_TYPE_ARM | CPU_ARCH_ABI64";
    } else {
        ASSERT_NOT_REACHED();
    }
}

utf8 const * cpu_subtype_to_string(i32 cputype, i32 cpusubtype) {
    if (cputype == 0x1000007 && cpusubtype == 0x3) {
        return "CPU_SUBTYPE_X86_64_ALL";
    } else if (cputype == 0x100000c && cpusubtype == 0x0) {
        return "CPU_SUBTYPE_ARM_ALL";
    } else {
        ASSERT_NOT_REACHED();
    }
}

#define MH_OBJECT 0x01

utf8 const * file_type_to_string(u32 filetype) {
    if (filetype == 0x1) {
        return "MH_OBJECT";
    } else {
        ASSERT_NOT_REACHED();
    }
}

#define MH_NOUNDEFS 0x1
#define MH_INCRLINK 0x2
#define MH_DYLDLINK 0x4
#define MH_BINDATLOAD 0x8
#define MH_PREBOUND 0x10
#define MH_SPLIT_SEGS 0x20
#define MH_LAZY_INIT 0x40
#define MH_TWOLEVEL 0x80
#define MH_FORCE_FLAT 0x100
#define MH_NOMULTIDEFS 0x200
#define MH_NOFIXPREBINDING 0x400
#define MH_PREBINDABLE 0x800
#define MH_ALLMODSBOUND 0x1000
#define MH_SUBSECTIONS_VIA_SYMBOLS 0x2000
#define MH_CANONICAL 0x4000
#define MH_WEAK_DEFINES 0x8000
#define MH_BINDS_TO_WEAK 0x10000
#define MH_ALLOW_STACK_EXECUTION 0x20000
#define MH_ROOT_SAFE 0x40000
#define MH_SETUID_SAFE 0x80000
#define MH_NO_REEXPORTED_DYLIBS 0x100000
#define MH_PIE 0x200000
#define MH_DEAD_STRIPPABLE_DYLIB 0x400000
#define MH_HAS_TLV_DESCRIPTORS 0x800000
#define MH_NO_HEAP_EXECUTION 0x1000000

utf8 const * mh_flags_to_string(u32 flags) {
    Formatter formatter;

    if (flags != 0) {
        if (flags & 0x1) { format(formatter, "MH_NOUNDEFS{}", bit_set_after(flags, 0x1) ? " | " : ""); } // the object file has no undefined references
        if (flags & 0x2) { format(formatter, "MH_INCRLINK{}", bit_set_after(flags, 0x2) ? " | " : ""); } // the object file is the output of an incremental link against a base file and can't be link edited again
        if (flags & 0x4) { format(formatter, "MH_DYLDLINK{}", bit_set_after(flags, 0x4) ? " | " : ""); } // the object file is input for the dynamic linker and can't be staticly link edited again
        if (flags & 0x8) { format(formatter, "MH_BINDATLOAD{}", bit_set_after(flags, 0x8) ? " | " : ""); } // the object file's undefined references are bound by the dynamic linker when loaded.
        if (flags & 0x10) { format(formatter, "MH_PREBOUND{}", bit_set_after(flags, 0x10) ? " | " : ""); } // the file has its dynamic undefined references prebound.
        if (flags & 0x20) { format(formatter, "MH_SPLIT_SEGS{}", bit_set_after(flags, 0x20) ? " | " : ""); } // the file has its read-only and read-write segments split
        if (flags & 0x40) { format(formatter, "MH_LAZY_INIT{}", bit_set_after(flags, 0x40) ? " | " : ""); } // the shared library init routine is to be run lazily via catching memory faults to its writeable segments (obsolete)
        if (flags & 0x80) { format(formatter, "MH_TWOLEVEL{}", bit_set_after(flags, 0x80) ? " | " : ""); } // the image is using two-level name space bindings
        if (flags & 0x100) { format(formatter, "MH_FORCE_FLAT{}", bit_set_after(flags, 0x100) ? " | " : ""); } // the executable is forcing all images to use flat name space bindings
        if (flags & 0x200) { format(formatter, "MH_NOMULTIDEFS{}", bit_set_after(flags, 0x200) ? " | " : ""); } // this umbrella guarantees no multiple defintions of symbols in its sub-images so the two-level namespace hints can always be used.
        if (flags & 0x400) { format(formatter, "MH_NOFIXPREBINDING{}", bit_set_after(flags, 0x400) ? " | " : ""); } // do not have dyld notify the prebinding agent about this executable
        if (flags & 0x800) { format(formatter, "MH_PREBINDABLE{}", bit_set_after(flags, 0x800) ? " | " : ""); } // the binary is not prebound but can have its prebinding redone. only used when MH_PREBOUND is not set.
        if (flags & 0x1000) { format(formatter, "MH_ALLMODSBOUND{}", bit_set_after(flags, 0x1000) ? " | " : ""); } // indicates that this binary binds to all two-level namespace modules of its dependent libraries. only used when MH_PREBINDABLE and MH_TWOLEVEL are both set.
        if (flags & 0x2000) { format(formatter, "MH_SUBSECTIONS_VIA_SYMBOLS{}", bit_set_after(flags, 0x2000) ? " | " : ""); } // safe to divide up the sections into sub-sections via symbols for dead code stripping
        if (flags & 0x4000) { format(formatter, "MH_CANONICAL{}", bit_set_after(flags, 0x4000) ? " | " : ""); } // the binary has been canonicalized via the unprebind operation
        if (flags & 0x8000) { format(formatter, "MH_WEAK_DEFINES{}", bit_set_after(flags, 0x8000) ? " | " : ""); } // the final linked image contains external weak symbols
        if (flags & 0x10000) { format(formatter, "MH_BINDS_TO_WEAK{}", bit_set_after(flags, 0x10000) ? " | " : ""); } // the final linked image uses weak symbols
        if (flags & 0x20000) { format(formatter, "MH_ALLOW_STACK_EXECUTION{}", bit_set_after(flags, 0x20000) ? " | " : ""); } // When this bit is set, all stacks  in the task will be given stack execution privilege.  Only used in MH_EXECUTE filetypes.
        if (flags & 0x40000) { format(formatter, "MH_ROOT_SAFE{}", bit_set_after(flags, 0x40000) ? " | " : ""); } // When this bit is set, the binary  declares it is safe for use in processes with uid zero
        if (flags & 0x80000) { format(formatter, "MH_SETUID_SAFE{}", bit_set_after(flags, 0x80000) ? " | " : ""); } // When this bit is set, the binary  declares it is safe for use in processes when issetugid() is true
        if (flags & 0x100000) { format(formatter, "MH_NO_REEXPORTED_DYLIBS{}", bit_set_after(flags, 0x100000) ? " | " : ""); } // When this bit is set on a dylib,  the static linker does not need to examine dependent dylibs to see if any are re-exported
        if (flags & 0x200000) { format(formatter, "MH_PIE{}", bit_set_after(flags, 0x200000) ? " | " : ""); } // When this bit is set, the OS will load the main executable at a random address.  Only used in MH_EXECUTE filetypes.
        if (flags & 0x400000) { format(formatter, "MH_DEAD_STRIPPABLE_DYLIB{}", bit_set_after(flags, 0x400000) ? " | " : ""); } // Only for use on dylibs.  When linking against a dylib that has this bit set, the static linker will automatically not create a LC_LOAD_DYLIB load command to the dylib if no symbols are being referenced from the dylib.
        if (flags & 0x800000) { format(formatter, "MH_HAS_TLV_DESCRIPTORS{}", bit_set_after(flags, 0x800000) ? " | " : ""); } // Contains a section of type  S_THREAD_LOCAL_VARIABLES
        if (flags & 0x1000000) { format(formatter, "MH_NO_HEAP_EXECUTION{}", bit_set_after(flags, 0x1000000) ? " | " : ""); } // When this bit is set, the OS will run the main executable with a non-executable heap even on platforms (e.g. i386) that don't require it. Only used in MH_EXECUTE filetypes.
    } else {
        format(formatter, "{}", "");
    }

    return formatter.pointer;
}
