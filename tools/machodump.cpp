#include "../src/lib/types.hpp"
#include "../src/lib/assert.hpp"
#include "../src/lib/memory.hpp"
#include "../src/lib/print.hpp"
#include "../src/lib/array.hpp"
#include "../src/lib/unicode.hpp"
#include "../src/lib/file.hpp"
#include "../src/lib/buffer.hpp"
#include "../src/lib/leb128.hpp"

#include "dwarf5.hpp"
#include "macho.hpp"

#define read(type, o) *(type *)(data + o); o += sizeof(type)
#define read_based(base, type, o) *(type *)(base + o); o += sizeof(type)
#define read_into(d, bytes, o) copy(data + o, d, bytes); o += bytes

int main(int argc, char ** argv) {
    Get_File_Result result = get_file(argv[1], FALSE);
    u8 * data = (u8 *)result.bytes;

    int o = 0;

    // struct mach_header_64 {
    //     uint32_t    magic;      /* mach magic number identifier */
    //     cpu_type_t  cputype;    /* cpu specifier */
    //     cpu_subtype_t   cpusubtype; /* machine specifier */
    //     uint32_t    filetype;   /* type of file */
    //     uint32_t    ncmds;      /* number of load commands */
    //     uint32_t    sizeofcmds; /* the size of all the load commands */
    //     uint32_t    flags;      /* flags */
    //     uint32_t    reserved;   /* reserved */
    // };

    u32 magic = read(u32, o);
    i32 cputype = read(i32, o);
    i32 cpusubtype = read(i32, o);
    u32 filetype = read(u32, o);
    u32 ncmds = read(u32, o);
    u32 sizeofcmds = read(u32, o);
    u32 flags = read(u32, o);
    u32 reserved = read(u32, o);

    print("     magic = \u001b[1m0x{x}\u001b[0m\n", magic);
    print("   cputype = \u001b[1m0x{x}\u001b[0m (\u001b[1m{}\u001b[0m)\n", cputype, cpu_type_to_string(cputype)); // typedef int cpu_type_t;
    print("cpusubtype = \u001b[1m0x{x}\u001b[0m (\u001b[1m{}\u001b[0m)\n", cpusubtype, cpu_subtype_to_string(cputype, cpusubtype)); // typedef int cpu_subtype_t;
    print("  filetype = \u001b[1m0x{x}\u001b[0m ({})\n", filetype, file_type_to_string(filetype));
    print("     ncmds = \u001b[1m{}\u001b[0m\n", ncmds);
    print("sizeofcmds = \u001b[1m0x{x}\u001b[0m (\u001b[1m{} bytes\u001b[0m)\n", sizeofcmds, sizeofcmds);
    print("     flags = \u001b[1m0x{x}\u001b[0m {}\n", flags, mh_flags_to_string(flags));
    print("  reserved = \u001b[1m0x{x}\u001b[0m\n", reserved);
    print("\n");

    for (u32 i = 0; i < ncmds; i += 1) {
        int cmdoffset = o;

        // struct load_command {
        //     uint32_t cmd;       /* type of load command */
        //     uint32_t cmdsize;   /* total size of command in bytes */
        // };

        LC cmd = read(LC, o);
        u32 cmdsize = read(u32, o);

        print("Segment #{} \u001b[1m0x{x}\u001b[0m \u001b[1m{}\u001b[0m (0x{x} or {} bytes)\n", i, cmd, lc_to_string(cmd), cmdsize, cmdsize);

        if (cmd == 0x19) { // LC_SEGMENT_64
            // struct segment_command_64 { /* for 64-bit architectures */
            //     uint32_t    cmd;        /* LC_SEGMENT_64 */
            //     uint32_t    cmdsize;    /* includes sizeof section_64 structs */
            //     char        segname[16];    /* segment name */
            //     uint64_t    vmaddr;     /* memory address of this segment */
            //     uint64_t    vmsize;     /* memory size of this segment */
            //     uint64_t    fileoff;    /* file offset of this segment */
            //     uint64_t    filesize;   /* amount to map from the file */
            //     vm_prot_t   maxprot;    /* maximum VM protection */
            //     vm_prot_t   initprot;   /* initial VM protection */
            //     uint32_t    nsects;     /* number of sections in segment */
            //     uint32_t    flags;      /* flags */
            // };

            char segname[16] = { 0 };
            read_into(segname, 16, o);
            u64 vmaddr = read(u64, o);
            u64 vmsize = read(u64, o);
            u64 fileoff = read(u64, o);
            u64 filesize = read(u64, o);
            u32 maxprot = read(u32, o); // typedef int vm_prot_t;
            u32 initprot = read(u32, o); // typedef int vm_prot_t;
            u32 nsects = read(u32, o);
            u32 flags = read(u32, o);

            print("|   segname = \u001b[1m\"{16}\"\u001b[0m\n", segname);
            print("|   vmaddr = \u001b[1m0x{x}\u001b[0m (\u001b[1m{} bytes\u001b[0m)\n", vmaddr, vmaddr);
            print("|   vmsize = \u001b[1m0x{x}\u001b[0m (\u001b[1m{} bytes\u001b[0m)\n", vmsize, vmsize);
            print("|   fileoff = \u001b[1m0x{x}\u001b[0m (\u001b[1m{} bytes\u001b[0m)\n", fileoff, fileoff);
            print("|   filesize = \u001b[1m0x{x}\u001b[0m (\u001b[1m{} bytes\u001b[0m)\n", filesize, filesize);
            print("|   maxprot = \u001b[1m0x{x}\u001b[0m", maxprot);
            print(" (");
            if (maxprot == 0) { print("\u001b[1mVM_PROT_NONE\u001b[0m"); }
            if (maxprot & 0x01) { print("\u001b[1mVM_PROT_READ\u001b[0m{}", bit_set_after(maxprot, 0x01) ? " | " : ""); }
            if (maxprot & 0x02) { print("\u001b[1mVM_PROT_WRITE\u001b[0m{}", bit_set_after(maxprot, 0x02) ? " | " : ""); }
            if (maxprot & 0x04) { print("\u001b[1mVM_PROT_EXECUTE\u001b[0m{}", bit_set_after(maxprot, 0x04) ? " | " : ""); }
            print(")\n");
            print("|   initprot = \u001b[1m0x{x}\u001b[0m", initprot);
            print(" (");
            if (initprot == 0) { print("\u001b[1mVM_PROT_NONE\u001b[0m"); }
            if (initprot & 0x01) { print("\u001b[1mVM_PROT_READ\u001b[0m{}", bit_set_after(initprot, 0x01) ? " | " : ""); }
            if (initprot & 0x02) { print("\u001b[1mVM_PROT_WRITE\u001b[0m{}", bit_set_after(initprot, 0x02) ? " | " : ""); }
            if (initprot & 0x04) { print("\u001b[1mVM_PROT_EXECUTE\u001b[0m{}", bit_set_after(initprot, 0x04) ? " | " : ""); }
            print(")\n");
            print("|   nsects = \u001b[1m{}\u001b[0m\n", nsects);
            print("|   flags = \u001b[1m0x{x}\u001b[0m\n", flags);
            if (flags & 0x1) { print("|           \u001b[1mSG_HIGHVM\u001b[0m\n"); } // the file contents for this segment is for the high part of the VM space, the low part is zero filled (for stacks in core files)
            if (flags & 0x2) { print("|           \u001b[1mSG_FVMLIB\u001b[0m\n"); } // this segment is the VM that is allocated by a fixed VM library, for overlap checking in the link editor
            if (flags & 0x4) { print("|           \u001b[1mSG_NORELOC\u001b[0m\n"); } // this segment has nothing that was relocated in it and nothing relocated to it, that is it maybe safely replaced without relocation
            if (flags & 0x8) { print("|           \u001b[1mSG_PROTECTED_VERSION_1\u001b[0m\n"); } // This segment is protected.  If the segment starts at file offset 0, the first page of the segment is not protected.  All other pages of the segment are protected.

            print("|\n");

            // Do a prepass so we can find everything
            u8 const * dwarf_debug_abbrev = NULL;
            u8 const * dwarf_debug_info = NULL;
            u8 const * dwarf_debug_str_offs = NULL;
            u8 const * dwarf_debug_str = NULL;
            u8 const * dwarf_debug_addr = NULL;
            u8 const * dwarf_debug_frame = NULL;
            u8 const * dwarf_apple_names = NULL;
            u8 const * dwarf_apple_objc = NULL;
            u8 const * dwarf_apple_namespac = NULL;
            u8 const * dwarf_apple_types = NULL;
            u8 const * dwarf_debug_line = NULL;
            u8 const * dwarf_debug_line_str = NULL;

            {
                int ot = o;

                for (u32 j = 0; j < nsects; j += 1) {
                    char sectname[16] = { 0 };
                    read_into(sectname, 16, o);
                    char segname[16] = { 0 };
                    read_into(segname, 16, o);

                    o += 2 * 8;

                    u32 offset = read(u32, o);

                    o += 7 * 4;

                    if (utf8_compare(segname, "__DWARF", 16)) {
                        if (utf8_compare(sectname, "__debug_info", 16)) {
                            dwarf_debug_info = data + offset;
                        } else if (utf8_compare(sectname, "__debug_abbrev", 16)) {
                            dwarf_debug_abbrev = data + offset;
                        } else if (utf8_compare(sectname, "__debug_info", 16)) {
                            dwarf_debug_info = data + offset;
                        } else if (utf8_compare(sectname, "__debug_str_offs", 16)) {
                            dwarf_debug_str_offs = data + offset;
                        } else if (utf8_compare(sectname, "__debug_str", 16)) {
                            dwarf_debug_str = data + offset;
                        } else if (utf8_compare(sectname, "__debug_addr", 16)) {
                            dwarf_debug_addr = data + offset;
                        } else if (utf8_compare(sectname, "__debug_frame", 16)) {
                            dwarf_debug_frame = data + offset;
                        } else if (utf8_compare(sectname, "__apple_names", 16)) {
                            dwarf_apple_names = data + offset;
                        } else if (utf8_compare(sectname, "__apple_objc", 16)) {
                            dwarf_apple_objc = data + offset;
                        } else if (utf8_compare(sectname, "__apple_namespac", 16)) {
                            dwarf_apple_namespac = data + offset;
                        } else if (utf8_compare(sectname, "__apple_types", 16)) {
                            dwarf_apple_types = data + offset;
                        } else if (utf8_compare(sectname, "__debug_line", 16)) {
                            dwarf_debug_line = data + offset;
                        } else if (utf8_compare(sectname, "__debug_line_str", 16)) {
                            dwarf_debug_line_str = data + offset;
                        }
                    }
                 }

                o = ot;
            }


            for (u32 j = 0; j < nsects; j += 1) {
                print("|   Section #{}\n", j);

                // struct section_64 { /* for 64-bit architectures */
                //     char        sectname[16];   /* name of this section */
                //     char        segname[16];    /* segment this section goes in */
                //     uint64_t    addr;       /* memory address of this section */
                //     uint64_t    size;       /* size in bytes of this section */
                //     uint32_t    offset;     /* file offset of this section */
                //     uint32_t    align;      /* section alignment (power of 2) */
                //     uint32_t    reloff;     /* file offset of relocation entries */
                //     uint32_t    nreloc;     /* number of relocation entries */
                //     uint32_t    flags;      /* flags (section type and attributes)*/
                //     uint32_t    reserved1;  /* reserved (for offset or index) */
                //     uint32_t    reserved2;  /* reserved (for count or sizeof) */
                //     uint32_t    reserved3;  /* reserved */
                // };

                char sectname[16] = { 0 };
                read_into(sectname, 16, o);
                char segname[16] = { 0 };
                read_into(segname, 16, o);
                u64 addr = read(u64, o);
                u64 size = read(u64, o);
                u32 offset = read(u32, o);
                u32 align = read(u32, o);
                u32 reloff = read(u32, o);
                u32 nreloc = read(u32, o);

                print("|   | sectname = \u001b[1m\"{16}\"\u001b[0m\n", sectname);
                print("|   | segname = \u001b[1m\"{16}\"\u001b[0m\n", segname);
                print("|   | addr = \u001b[1m0x{x}\u001b[0m (\u001b[1m{} bytes\u001b[0m)\n", addr, addr);
                print("|   | size = \u001b[1m0x{x}\u001b[0m (\u001b[1m{} bytes\u001b[0m)\n", size, size);
                print("|   | offset = \u001b[1m0x{x}\u001b[0m (\u001b[1m{} bytes\u001b[0m)\n", offset, offset);
                print("|   | align = \u001b[1m0x{x}\u001b[0m (\u001b[1m{} byte{}\u001b[0m)\n", align, 1 << align, align > 0 ? "s" : "");
                print("|   | reloff = \u001b[1m0x{x}\u001b[0m (\u001b[1m{} bytes\u001b[0m)\n", reloff, reloff);
                print("|   | nreloc = \u001b[1m{}\u001b[0m\n", nreloc);

                for (u32 k = 0; k < nreloc; k += 1) {
                    print("|   |   Relocation #{}\n", k);

                    int ot = o;
                    o = (int)reloff;

                    u32 r_address = read(u32, o);
                    u32 r_info = read(u32, o);

                    u32 r_symbolnum = (r_info >> 0) & 0x0FFF;
                    u32 r_pcrel = (r_info >> 24) & 1;
                    u32 r_length = (r_info >> 25) & 0b11;
                    u32 r_extern = (r_info >> 27) & 1;
                    u32 r_type = (r_info >> 28) & 0b1111;

                    print("|   |   | r_address = \u001b[1m0x{x}\u001b[0m\n", r_address);
                    print("|   |   | r_info = \u001b[1m0x{x}\u001b[0m\n", r_info);
                    print("|   |   |   r_symbolnum = \u001b[1m{}\u001b[0m\n", r_symbolnum);
                    print("|   |   |   r_pcrel = \u001b[1m{}\u001b[0m\n", r_pcrel);
                    print("|   |   |   r_length = \u001b[1m0x{x}\u001b[0m", r_length);
                    if (r_length == 0) {
                        print(" (\u001b[1m1 byte\u001b[0m)\n");
                    } else if (r_length == 1) {
                        print(" (\u001b[1m2 bytes\u001b[0m)\n");
                    } else if (r_length == 2) {
                        print(" (\u001b[1m4 bytes\u001b[0m)\n");
                    } else if (r_length == 3) {
                        print(" (\u001b[1m8 bytes\u001b[0m)\n");
                    }
                    print("|   |   |   r_extern = \u001b[1m{}\u001b[0m\n", r_extern);
                    print("|   |   |   r_type = \u001b[1m0x{x}\u001b[0m", r_type);

                    if (cputype == 0x1000007) {
                        switch (r_type) {
                            case 0: { print(" (\u001b[1mX86_64_RELOC_UNSIGNED\u001b[0m)\n"); } break;      // for absolute addresses
                            case 1: { print(" (\u001b[1mX86_64_RELOC_SIGNED\u001b[0m)\n"); } break;        // for signed 32-bit displacement
                            case 2: { print(" (\u001b[1mX86_64_RELOC_BRANCH\u001b[0m)\n"); } break;        // a CALL/JMP instruction with 32-bit displacement
                            case 3: { print(" (\u001b[1mX86_64_RELOC_GOT_LOAD\u001b[0m)\n"); } break;      // a MOVQ load of a GOT entry
                            case 4: { print(" (\u001b[1mX86_64_RELOC_GOT\u001b[0m)\n"); } break;           // other GOT references
                            case 5: { print(" (\u001b[1mX86_64_RELOC_SUBTRACTOR\u001b[0m)\n"); } break;    // must be followed by a X86_64_RELOC_UNSIGNED
                            case 6: { print(" (\u001b[1mX86_64_RELOC_SIGNED_1\u001b[0m)\n"); } break;      // for signed 32-bit displacement with a -1 addend
                            case 7: { print(" (\u001b[1mX86_64_RELOC_SIGNED_2\u001b[0m)\n"); } break;      // for signed 32-bit displacement with a -2 addend
                            case 8: { print(" (\u001b[1mX86_64_RELOC_SIGNED_4\u001b[0m)\n"); } break;      // for signed 32-bit displacement with a -4 addend

                            default: {
                                ASSERT_NOT_REACHED();
                            } break;
                        }
                    } else if (cputype == 0x1000012) {
                        switch (r_type) {
                            case 0: { print(" (\u001b[1mARM64_RELOC_UNSIGNED\u001b[0m)\n"); } break;     // for pointers
                            case 1: { print(" (\u001b[1mARM64_RELOC_SUBTRACTOR\u001b[0m)\n"); } break;       // must be followed by a ARM64_RELOC_UNSIGNED
                            case 2: { print(" (\u001b[1mARM64_RELOC_BRANCH26\u001b[0m)\n"); } break;         // a B/BL instruction with 26-bit displacement
                            case 3: { print(" (\u001b[1mARM64_RELOC_PAGE21\u001b[0m)\n"); } break;           // pc-rel distance to page of target
                            case 4: { print(" (\u001b[1mARM64_RELOC_PAGEOFF12\u001b[0m)\n"); } break;        // offset within page, scaled by r_length
                            case 5: { print(" (\u001b[1mARM64_RELOC_GOT_LOAD_PAGE21\u001b[0m)\n"); } break;  // pc-rel distance to page of GOT slot
                            case 6: { print(" (\u001b[1mARM64_RELOC_GOT_LOAD_PAGEOFF12\u001b[0m)\n"); } break; // offset within page of GOT slot, scaled by r_length
                            case 7: { print(" (\u001b[1mARM64_RELOC_POINTER_TO_GOT\u001b[0m)\n"); } break;   // for pointers to GOT slots
                            case 8: { print(" (\u001b[1mARM64_RELOC_TLVP_LOAD_PAGE21\u001b[0m)\n"); } break; // pc-rel distance to page of TLVP slot
                            case 9: { print(" (\u001b[1mARM64_RELOC_TLVP_LOAD_PAGEOFF12\u001b[0m)\n"); } break; // offset within page of TLVP slot, scaled by r_length
                            case 10: { print(" (\u001b[1mARM64_RELOC_ADDEND\u001b[0m)\n"); } break;        // must be followed by PAGE21 or PAGEOFF12

                            default: {
                                ASSERT_NOT_REACHED();
                            } break;
                        }
                    }

                    if (k < nreloc - 1) {
                        print("|   |\n");
                    }

                    o = ot;
                }

                u32 flags = read(u32, o);
                u32 reserved1 = read(u32, o);
                u32 reserved2 = read(u32, o);
                u32 reserved3 = read(u32, o);

                print("|   | flags = \u001b[1m0x{x}\u001b[0m\n", flags);
                print("|   |   type = \u001b[1m0x{x}\u001b[0m", flags & 0x000000ff);
                if ((flags & 0x000000ff) == 0x0) {
                    print(" (\u001b[1mS_REGULAR\u001b[0m)\n");  // regular section
                } else if ((flags & 0x000000ff) == 0x1) {
                    print(" (\u001b[1mS_ZEROFILL\u001b[0m)\n");  // zero fill on demand section
                } else if ((flags & 0x000000ff) == 0x2) {
                    print(" (\u001b[1mS_CSTRING_LITERALS\u001b[0m)\n");  // section with only literal C strings
                } else if ((flags & 0x000000ff) == 0x3) {
                    print(" (\u001b[1mS_4BYTE_LITERALS\u001b[0m)\n");  // section with only 4 byte literals
                } else if ((flags & 0x000000ff) == 0x4) {
                    print(" (\u001b[1mS_8BYTE_LITERALS\u001b[0m)\n");  // section with only 8 byte literals
                } else if ((flags & 0x000000ff) == 0x5) {
                    print(" (\u001b[1mS_LITERAL_POINTERS\u001b[0m)\n");  // section with only pointers to literals
                } else if ((flags & 0x000000ff) == 0x6) {
                    print(" (\u001b[1mS_NON_LAZY_SYMBOL_POINTERS\u001b[0m)\n"); //  section with only non-lazy symbol pointers */
                } else if ((flags & 0x000000ff) == 0x7) {
                    print(" (\u001b[1mS_LAZY_SYMBOL_POINTERS\u001b[0m)\n"); //  section with only lazy symbol pointers */
                } else if ((flags & 0x000000ff) == 0x8) {
                    print(" (\u001b[1mS_SYMBOL_STUBS\u001b[0m)\n"); //  section with only symbol stubs, byte size of stub in the reserved2 field */
                } else if ((flags & 0x000000ff) == 0x9) {
                    print(" (\u001b[1mS_MOD_INIT_FUNC_POINTERS\u001b[0m)\n"); //  section with only function pointers for initialization*/
                } else if ((flags & 0x000000ff) == 0xa) {
                    print(" (\u001b[1mS_MOD_TERM_FUNC_POINTERS\u001b[0m)\n"); //  section with only function pointers for termination */
                } else if ((flags & 0x000000ff) == 0xb) {
                    print(" (\u001b[1mS_COALESCED\u001b[0m)\n"); //  section contains symbols that are to be coalesced */
                } else if ((flags & 0x000000ff) == 0xc) {
                    print(" (\u001b[1mS_GB_ZEROFILL\u001b[0m)\n"); //  zero fill on demand section (that can be larger than 4 gigabytes) */
                } else if ((flags & 0x000000ff) == 0xd) {
                    print(" (\u001b[1mS_INTERPOSING\u001b[0m)\n"); //  section with only pairs of function pointers for interposing */
                } else if ((flags & 0x000000ff) == 0xe) {
                    print(" (\u001b[1mS_16BYTE_LITERALS\u001b[0m)\n"); //  section with only 16 byte literals */
                } else if ((flags & 0x000000ff) == 0xf) {
                    print(" (\u001b[1mS_DTRACE_DOF\u001b[0m)\n"); //  section contains  DTrace Object Format */
                } else if ((flags & 0x000000ff) == 0x10) {
                    print(" (\u001b[1mS_LAZY_DYLIB_SYMBOL_POINTERS\u001b[0m)\n"); //  /* section with only lazy symbol pointers to lazy loaded dylibs */
                } else if ((flags & 0x000000ff) == 0x11) {
                    print(" (\u001b[1mS_THREAD_LOCAL_REGULAR\u001b[0m)\n"); // * template of initial values for TLVs */
                } else if ((flags & 0x000000ff) == 0x12) {
                    print(" (\u001b[1mS_THREAD_LOCAL_ZEROFILL\u001b[0m)\n"); // * template of initial values for TLVs */
                } else if ((flags & 0x000000ff) == 0x13) {
                    print(" (\u001b[1mS_THREAD_LOCAL_VARIABLES\u001b[0m)\n"); // * TLV descriptors */
                } else if ((flags & 0x000000ff) == 0x14) {
                    print(" (\u001b[1mS_THREAD_LOCAL_VARIABLE_POINTERS\u001b[0m)\n"); // * pointers to TLV descriptors */
                } else if ((flags & 0x000000ff) == 0x15) {
                    print(" (\u001b[1mS_THREAD_LOCAL_INIT_FUNCTION_POINTERS\u001b[0m)\n"); // * functions to call to initialize TLV values */
                } else {
                    ASSERT_NOT_REACHED();
                }
                print("|   |   attributes = \u001b[1m0x{x}\u001b[0m", flags & 0xffffff00);
                if (flags != 0) {
                    print(" (");
                    if (flags & 0x00000100) { print("\u001b[1mS_ATTR_LOC_RELOC\u001b[0m{}", bit_set_after(flags, 0x00000100) ? " | " : ""); } //  section has local relocation entries */
                    if (flags & 0x00000200) { print("\u001b[1mS_ATTR_EXT_RELOC\u001b[0m{}", bit_set_after(flags, 0x00000200) ? " | " : ""); } //  section has external relocation entries */
                    if (flags & 0x00000400) { print("\u001b[1mS_ATTR_SOME_INSTRUCTIONS\u001b[0m{}", bit_set_after(flags, 0x00000400) ? " | " : ""); } //  section contains some machine instructions */
                    if (flags & 0x02000000) { print("\u001b[1mS_ATTR_DEBUG\u001b[0m{}", bit_set_after(flags, 0x02000000) ? " | " : ""); } //  a debug section */
                    if (flags & 0x04000000) { print("\u001b[1mS_ATTR_SELF_MODIFYING_CODE\u001b[0m{}", bit_set_after(flags, 0x04000000) ? " | " : ""); } // /* Used with i386 code stubs written on by dyld */
                    if (flags & 0x08000000) { print("\u001b[1mS_ATTR_LIVE_SUPPORT\u001b[0m{}", bit_set_after(flags, 0x08000000) ? " | " : ""); } //  blocks are live if they reference live blocks */
                    if (flags & 0x10000000) { print("\u001b[1mS_ATTR_NO_DEAD_STRIP\u001b[0m{}", bit_set_after(flags, 0x10000000) ? " | " : ""); } //  no dead stripping */
                    if (flags & 0x20000000) { print("\u001b[1mS_ATTR_STRIP_STATIC_SYMS\u001b[0m{}", bit_set_after(flags, 0x20000000) ? " | " : ""); } //  ok to strip static symbols in this section in files with the MH_DYLDLINK flag */
                    if (flags & 0x40000000) { print("\u001b[1mS_ATTR_NO_TOC\u001b[0m{}", bit_set_after(flags, 0x40000000) ? " | " : ""); } //  section contains coalesced symbols that are not to be in a ranlib table of contents */
                    if (flags & 0x80000000) { print("\u001b[1mS_ATTR_PURE_INSTRUCTIONS\u001b[0m{}", bit_set_after(flags, 0x80000000) ? " | " : ""); } //  section contains only true machine instructions */
                    print(")\n");
                }

                print("|   | reserved1 = \u001b[1m0x{x}\u001b[0m\n", reserved1);
                print("|   | reserved2 = \u001b[1m0x{x}\u001b[0m\n", reserved2);
                print("|   | reserved3 = \u001b[1m0x{x}\u001b[0m\n", reserved3);

                print("|   |\n");

                buffer_print_raw(data, offset, size, "|   |   ");

                print("|   |\n");

                if (utf8_compare(segname, "__DWARF", 16)) {
                    if (utf8_compare(sectname, "__debug_info", 16)) {
                        int ot = o;
                        o = offset;

                        dwarf_debug_info = data + o;

                        u64 base = offset;
                        while ((u64)(o - offset) < size) {
                            u64 tag_offset = o - base;

                            // Check format
                            u32 format = 0; // 32

                            u32 ones = read(u32, o);
                            if (ones == 0xFFFFFFFF) {
                                format = 1; // 64
                            }

                            // Rewind
                            o -= 4;

                            // clang on macOS can only output DWARF32, so that is
                            // what we also output and expect for now.
                            ASSERT(format == 0);

                            u64 length;
                            u64 length_size;
                            if (format == 1) {
                                length = read(u64, o);
                                length_size = 8;
                            } else {
                                length = read(u32, o);
                                length_size = 4;
                            }

                            u16 version = read(u16, o);
                            ASSERT(version == 5);
                            u8 unit_type = read(u8, o);
                            u8 address_size = read(u8, o);
                            ASSERT(address_size == 8);
                            u64 debug_abbrev_offset;
                            if (format == 1) {
                                debug_abbrev_offset = read(u64, o);
                            } else {
                                debug_abbrev_offset = read(u32, o);
                            }

                            print("|   | 0x{08x}\n", tag_offset);
                            print("|   | DWARF{}\n", format == 1 ? "64" : "32");
                            print("|   | length = \u001b[1m0x{x}\u001b[0m (\u001b[1m{}\u001b[0m)\n", length, length);
                            print("|   | version = \u001b[1m0x{x}\u001b[0m (\u001b[1m{}\u001b[0m)\n", version, version);
                            print("|   | unit_type = \u001b[1m0x{x}\u001b[0m (\u001b[1m{}\u001b[0m)\n", unit_type, dw_ut_to_string(unit_type));
                            print("|   | address_size = \u001b[1m0x{x}\u001b[0m (\u001b[1m{}\u001b[0m)\n", address_size, address_size);
                            print("|   | debug_abbrev_offset = \u001b[1m0x{x}\u001b[0m (\u001b[1m{}\u001b[0m)\n", debug_abbrev_offset, debug_abbrev_offset);
                            print("|   |\n");

                            if (unit_type == DW_UT_compile) {
                                // We put this out here since we will only find it in the first unit, but we use
                                // it in subsequent units too.
                                u64 str_offsets_base = 0;
                                while ((u64)(o - base) < (length + length_size)) {
                                    u64 unit_offset = o - base;

                                    // LEB128
                                    u64 abbreviation_code = leb_decode(data, o);

                                    if (abbreviation_code == 0) {
                                        continue;
                                    }


                                    u64 abbreviation_tag;

                                    u8 const * abbreviations = dwarf_debug_abbrev + debug_abbrev_offset;
                                    int abbreviations_offset = 0;

                                    {
                                        // Skip up to the abbreviation we're interested in
                                        while (1) {
                                            u64 candidate_abbreviation_code = leb_decode(abbreviations, abbreviations_offset);
                                            if (candidate_abbreviation_code == abbreviation_code) {
                                                abbreviation_tag = leb_decode(abbreviations, abbreviations_offset);;
                                                read_based(abbreviations, DW_CHILDREN, abbreviations_offset);
                                                break;
                                            } else if (candidate_abbreviation_code == 0) {
                                                ASSERT_NOT_REACHED();
                                            } else {
                                                leb_decode(abbreviations, abbreviations_offset);
                                                read_based(abbreviations, DW_CHILDREN, abbreviations_offset);

                                                while (1) {
                                                    u64 attribute_name = leb_decode(abbreviations, abbreviations_offset);
                                                    u64 attribute_form = leb_decode(abbreviations, abbreviations_offset);
                                                    if (attribute_name == 0 && attribute_form == 0) {
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    print("|   |   0x{08x}    0x{x}    0x{x} {}\n", unit_offset, abbreviation_code, abbreviation_tag, dw_tag_to_string(abbreviation_tag));

                                    // Do a prepass to find DW_AT_str_offsets_base

                                    {
                                        int ot = o;
                                        int abbreviations_offset_t = abbreviations_offset;

                                        while (1) {
                                            u64 attribute_name = leb_decode(abbreviations, abbreviations_offset);
                                            u64 attribute_form = leb_decode(abbreviations, abbreviations_offset);

                                            if (attribute_name == 0 && attribute_form == 0) {
                                                break;
                                            }

                                            switch (attribute_form) {
                                                case DW_FORM_strx1: {
                                                    o += 1;
                                                } break;

                                                case DW_FORM_data1: {
                                                    o += 1;
                                                } break;

                                                case DW_FORM_data2: {
                                                    o += 2;
                                                } break;

                                                case DW_FORM_data4: {
                                                    o += 4;
                                                } break;

                                                case DW_FORM_sec_offset: {
                                                    if (attribute_name == DW_AT_str_offsets_base) {
                                                        str_offsets_base = read(u32, o);
                                                    } else if (attribute_name == DW_AT_stmt_list) {
                                                        o += 4;
                                                    } else if (attribute_name == DW_AT_addr_base) {
                                                        o += 4;
                                                    } else {
                                                        ASSERT_NOT_REACHED();
                                                    }
                                                } break;

                                                case DW_FORM_addrx: {
                                                    leb_decode(data, o);
                                                } break;

                                                case DW_FORM_flag: {
                                                    o += 1;
                                                } break;

                                                case DW_FORM_flag_present: {
                                                } break;

                                                case DW_FORM_exprloc: {
                                                    u64 length = leb_decode(data, o);
                                                    o += length;
                                                } break;

                                                case DW_FORM_ref4: {
                                                    o += 4;
                                                } break;

                                                default: {
                                                    ASSERT_NOT_REACHED();
                                                } break;
                                            }
                                        }

                                        abbreviations_offset = abbreviations_offset_t;
                                        o = ot;
                                    }

                                    while (1) {
                                        u64 attribute_name = leb_decode(abbreviations, abbreviations_offset);
                                        u64 attribute_form = leb_decode(abbreviations, abbreviations_offset);

                                        if (attribute_name == 0 && attribute_form == 0) {
                                            break;
                                        }

                                        print("|   |   |    0x{04x} \u001b[1m{32+}\u001b[0m    0x{04x} \u001b[1m{32+}\u001b[0m", attribute_name, " ", dw_at_to_string(attribute_name), attribute_form, " ", dw_form_to_string(attribute_form));

                                        switch (attribute_form) {
                                            case DW_FORM_strx1: {
                                                u8 offset1 = read(u8, o);

                                                // Assumes DWARF32
                                                u64 actual_offset = *(u32 *)(dwarf_debug_str_offs + str_offsets_base + offset1 * 4);

                                                print("0x{08x} {}\n", offset1, (utf8 const *)dwarf_debug_str + actual_offset);
                                            } break;

                                            case DW_FORM_data1: {
                                                u8 data1 = read(u8, o);
                                                print("0x{08x}\n", data1);
                                            } break;

                                            case DW_FORM_data2: {
                                                u16 data2 = read(u16, o);
                                                print("0x{08x}", data2);

                                                if (attribute_name == DW_AT_language) {
                                                    print(" {}\n", dw_lang_to_string(data2));
                                                }
                                            } break;

                                            case DW_FORM_data4: {
                                                u32 data4 = read(u32, o);
                                                print("0x{08x}\n", data4);
                                            } break;

                                            case DW_FORM_sec_offset: {
                                                if (attribute_name == DW_AT_str_offsets_base) {
                                                    u32 str_offsets_base = read(u32, o);
                                                    print("0x{08x}\n", str_offsets_base);
                                                } else if (attribute_name == DW_AT_stmt_list) {
                                                    u32 str_offsets_base = read(u32, o);
                                                    print("0x{08x}\n", str_offsets_base);
                                                } else if (attribute_name == DW_AT_addr_base) {
                                                    u32 addr_base = read(u32, o);
                                                    print("0x{08x}\n", addr_base);
                                                } else {
                                                    ASSERT_NOT_REACHED();
                                                }
                                            } break;

                                            case DW_FORM_addrx: {
                                                u64 offset = leb_decode(data, o);
                                                print("0x{08x}\n", offset);
                                            } break;

                                            case DW_FORM_flag: {
                                                u8 flag = read(u8, o);
                                                print("0x{08x}\n", flag);
                                            } break;

                                            case DW_FORM_flag_present: {
                                                print("\n");
                                            } break;

                                            case DW_FORM_exprloc: {
                                                u64 length = leb_decode(data, o);
                                                o += length;

                                                print("\n");
                                            } break;

                                            case DW_FORM_ref4: {
                                                u32 ref = read(u32, o);
                                                print("0x{08x}\n", ref);
                                            } break;

                                            default: {
                                                ASSERT_NOT_REACHED();
                                            } break;
                                        }
                                    }
                                }
                            } else {
                                ASSERT_NOT_REACHED();
                            }

                            ASSERT((u64)(o - base) == (length + length_size));
                        }

                        ASSERT((u64)(o - offset) == size);
                        o = ot;
                    } else if (utf8_compare(sectname, "__debug_abbrev", 16)) {
                        int ot = o;
                        o = offset;

                        dwarf_debug_abbrev = data + o;

                        while (1) {
                            u64 abbreviation_code = leb_decode(data, o);

                            // TODO: Is this always right? What about siblings and stuff..?
                            if (abbreviation_code == 0) {
                                break;
                            }

                            u64 abbreviation_tag = leb_decode(data, o);
                            DW_CHILDREN children = read(DW_CHILDREN, o);

                            print("|   |   \u001b[1m0x{04x}\u001b[0m 0x{04x} \u001b[1m{}\u001b[0m 0x{02x} \u001b[1m{}\u001b[0m\n", abbreviation_code, abbreviation_tag, dw_tag_to_string(abbreviation_tag), children, dw_children_to_string(children));

                            while (1) {
                                u64 attribute_name = leb_decode(data, o);
                                u64 attribute_form = leb_decode(data, o);

                                if (attribute_name == 0 && attribute_form == 0) {
                                    break;
                                }

                                print("|   |   |    0x{04x} \u001b[1m{32+}\u001b[0m    0x{04x} \u001b[1m{32+}\u001b[0m\n", attribute_name, " ", dw_at_to_string(attribute_name), attribute_form, " ", dw_form_to_string(attribute_form));
                            }
                        }

                        ASSERT((u64)(o - offset) == size);
                        o = ot;
                    } else if (utf8_compare(sectname, "__debug_str", 16)) {
                        int ot = o;
                        o = offset;

                        dwarf_debug_str = data + o;

                        while ((u64)o < offset + size) {
                            print("|   |   | 0x{08x} \u001b[1m{}\u001b[0m\n", o - offset, (utf8 const *)(data + o));
                            o += utf8_string_length((char *)(data + o)) + 1;
                        }

                        ASSERT((u64)(o - offset) == size);

                        o = ot;
                    } else if (utf8_compare(sectname, "__debug_str_offs", 16)) {
                        int ot = o;
                        o = offset;

                        dwarf_debug_str_offs = data + o;

                        // Check format
                        u32 format = 0; // 32

                        u32 ones = read(u32, o);
                        if (ones == 0xFFFFFFFF) {
                            format = 1; // 64
                        }

                        // Rewind
                        o = offset;

                        // clang on macOS can only output DWARF32, so that is
                        // what we also output and expect for now.
                        ASSERT(format == 0);

                        u64 length;
                        if (format == 1) {
                            length = read(u64, o);
                        } else {
                            length = read(u32, o);
                        }

                        u16 version = read(u16, o);
                        ASSERT(version == 5);

                        u16 padding = read(u16, o);
                        ASSERT(padding == 0);

                        print("|   |   | length = \u001b[1m0x{x}\u001b[0m (\u001b[1m{}\u001b[0m)\n", length, length);
                        print("|   |   | version = \u001b[1m0x{x}\u001b[0m (\u001b[1m{}\u001b[0m)\n", version, version);
                        print("|   |   | padding = \u001b[1m0x{x}\u001b[0m (\u001b[1m{}\u001b[0m)\n", padding, padding);
                        print("|   |   |\n");

                        while ((u64)o < offset + size) {
                            if (format == 1) {
                                u64 offset = read(u64, o);
                                print("|   |   | \u001b[1m0x{016x}\u001b[0m\n", offset);
                            } else {
                                u32 offset = read(u32, o);
                                print("|   |   | \u001b[1m0x{08x}\u001b[0m\n", offset);
                            }
                        }

                        ASSERT((u64)(o - offset) == size);

                        o = ot;
                    } else if (utf8_compare(sectname, "__apple_names", 16)) {
                        int ot = o;
                        o = offset;

                        dwarf_apple_names = data + o;

                        ASSERT((u64)(o - offset) == size);
                        o = ot;
                    } else if (utf8_compare(sectname, "__apple_objc", 16)) {
                        int ot = o;
                        o = offset;

                        dwarf_apple_objc = data + o;

                        ASSERT((u64)(o - offset) == size);
                        o = ot;
                    } else if (utf8_compare(sectname, "__apple_namespac", 16)) {
                        int ot = o;
                        o = offset;

                        dwarf_apple_namespac = data + o;

                        ASSERT((u64)(o - offset) == size);
                        o = ot;
                    } else if (utf8_compare(sectname, "__apple_types", 16)) {
                        int ot = o;
                        o = offset;

                        dwarf_apple_types = data + o;

                        ASSERT((u64)(o - offset) == size);
                        o = ot;
                    } else if (utf8_compare(sectname, "__debug_frame", 16)) {
                        int ot = o;
                        o = offset;

                        dwarf_debug_frame = data + o;

                        // Check format
                        u32 format = 0; // 32

                        u32 ones = read(u32, o);
                        if (ones == 0xFFFFFFFF) {
                            format = 1; // 64
                        }

                        // Rewind
                        o = offset;

                        // clang on macOS can only output DWARF32, so that is
                        // what we also output and expect for now.
                        ASSERT(format == 0);

                        u64 length;
                        if (format == 1) {
                            length = read(u64, o);
                        } else {
                            length = read(u32, o);
                        }

                        u64 cie_id;
                        if (format == 1) {
                            cie_id = read(u64, o);
                        } else {
                            cie_id = read(u32, o);
                        }

                        ASSERT(cie_id == 0xFFFFFFFF);

                        // NOTE: This is not the dwarf version number!
                        u8 version = read(u8, o);
                        ASSERT(version == 4);

                        utf8 const * augmentation = (utf8 const *)(data + o);
                        while (*(data + o++)) {
                        }

                        print("*augmentation = {}\n", *augmentation);
                        print("augmentation = {}\n", augmentation);

                        u8 address_size = read(u8, o);
                        u8 segment_selector_size = read(u8, o);
                        u64 code_alignment_factor = leb_decode(data, o);
                        i64 data_alignment_factor = leb_decode_signed(data, o);
                        u64 return_address_register = leb_decode(data, o);

                        u8 const * initial_instructions = data + o;
                        // "read" the instructions
                        while ((u64)(o - offset) < size) {
                            o += 1;
                        }

                        print("length = 0x{x}\n", length);
                        print("CIE_id = 0x{x}\n", cie_id);
                        print("version = {}\n", version);
                        print("augmentation = \"{}\"\n", augmentation);
                        print("address_size = {}\n", address_size);
                        print("segment_selector_size = {}\n", segment_selector_size);
                        print("code_alignment_factor = {}\n", code_alignment_factor);
                        print("data_alignment_factor = {}\n", data_alignment_factor);
                        print("return_address_register = {}\n", return_address_register);

                        ASSERT((u64)(o - offset) == size);
                        o = ot;
                    } else if (utf8_compare(sectname, "__debug_line", 16)) {
                        int ot = o;
                        o = offset;

                        dwarf_debug_line = data + o;

                        // Check format
                        u32 format = 0; // 32

                        u32 ones = read(u32, o);
                        if (ones == 0xFFFFFFFF) {
                            format = 1; // 64
                        }

                        // Rewind
                        o = offset;

                        // clang on macOS can only output DWARF32, so that is
                        // what we also output and expect for now.
                        ASSERT(format == 0);

                        u64 length;
                        u64 length_size;
                        if (format == 1) {
                            length = read(u64, o);
                            length_size = 8;
                        } else {
                            length = read(u32, o);
                            length_size = 4;
                        }

                        ASSERT(length + length_size == size);

                        // NOTE: This version is independent of the DWARF version!
                        u16 version = read(u16, o);
                        ASSERT(version == 5);

                        u8 address_size = read(u8, o);
                        u8 segment_selector_size = read(u8, o);
                        u64 header_length;
                        u64 header_length_size;
                        if (format == 1) {
                            header_length = read(u64, o);
                            header_length_size = 8;
                        } else {
                            header_length = read(u32, o);
                            header_length_size = 4;
                        }

                        u8 minimum_instruction_length = read(u8, o);
                        u8 maximum_operations_per_instructions = read(u8, o);
                        u8 default_is_stmt = read(u8, o);
                        i8 line_base = read(i8, o);
                        u8 line_range = read(u8, o);
                        u8 opcode_base = read(u8, o);

                        print("|   |   length = 0x{x}\n", length);
                        print("|   |   version = {}\n", version);
                        print("|   |   address_size = {}\n", address_size);
                        print("|   |   segment_selector_size = {}\n", segment_selector_size);
                        print("|   |   header_length = 0x{x}\n", header_length);
                        print("|   |   minimum_instruction_length = {}\n", minimum_instruction_length);
                        print("|   |   maximum_operations_per_instructions = {}\n", maximum_operations_per_instructions);
                        print("|   |   default_is_stmt = {}\n", default_is_stmt);
                        print("|   |   line_base = {}\n", line_base);
                        print("|   |   line_range = {}\n", line_range);
                        print("|   |   opcode_base = {}\n", opcode_base);

                        for (int i = 1; i < opcode_base; i += 1) {
                            u8 standard_opcode_length = read(u8, o);
                            print("|   |   standard_opcode_lengths[{}] = {}\n", dw_lns_to_string(i), standard_opcode_length);
                        }

                        u8 directory_entry_format_count = read(u8, o);
                        print("|   |   directory_entry_format_count = {}\n", directory_entry_format_count);

                        u64 * directory_entry_format_content_type_code = (u64 *)allocate(directory_entry_format_count * sizeof(u64));
                        u64 * directory_entry_format_form_code = (u64 *)allocate(directory_entry_format_count * sizeof(u64));
                        for (u8 i = 0; i < directory_entry_format_count; i += 1) {
                            directory_entry_format_content_type_code[i] = leb_decode(data, o);
                            directory_entry_format_form_code[i] = leb_decode(data, o);

                            print("|   |   | directory_entry_format_content_type_code[{}] = {} ({})\n", i, directory_entry_format_content_type_code[i], dw_lnct_to_string(directory_entry_format_content_type_code[i]));
                            print("|   |   | directory_entry_format_form_code[{}] = {} ({})\n", i, directory_entry_format_form_code[i], dw_form_to_string(directory_entry_format_form_code[i]));
                        }

                        u8 directories_count = read(u8, o);
                        print("|   |   directories_count = {}\n", directories_count);

                        for (u8 i = 0; i < directories_count; i += 1) {
                            for (u8 j = 0; j < directory_entry_format_count; j += 1) {
                                switch (directory_entry_format_form_code[j]) {
                                    case DW_FORM_line_strp: {
                                        u64 offset;
                                        if (format == 1) {
                                            offset = read(u64, o);
                                        } else {
                                            offset = read(u32, o);
                                        }

                                        print("|   |   | offset = {}\n", offset);
                                    } break;

                                    default: {
                                        print("{}\n", dw_form_to_string(directory_entry_format_form_code[i]));
                                        ASSERT_NOT_REACHED();
                                    } break;
                                }
                            }
                        }

                        u8 file_name_entry_format_count = read(u8, o);
                        print("|   |   file_name_entry_format_count = {}\n", file_name_entry_format_count);

                        u64 * file_name_entry_format_content_type_code = (u64 *)allocate(file_name_entry_format_count * sizeof(u64));
                        u64 * file_name_entry_format_form_code = (u64 *)allocate(file_name_entry_format_count * sizeof(u64));
                        for (u8 i = 0; i < file_name_entry_format_count; i += 1) {
                            file_name_entry_format_content_type_code[i] = leb_decode(data, o);
                            file_name_entry_format_form_code[i] = leb_decode(data, o);

                            print("|   |   | file_name_entry_format_content_type_code[{}] = {} ({})\n", i, file_name_entry_format_content_type_code[i], dw_lnct_to_string(file_name_entry_format_content_type_code[i]));
                            print("|   |   | file_name_entry_format_form_code[{}] = {} ({})\n", i, file_name_entry_format_form_code[i], dw_form_to_string(file_name_entry_format_form_code[i]));
                        }

                        u64 file_names_count = leb_decode(data, o);
                        print("|   |   file_names_count = {}\n", file_names_count);

                        for (u64 i = 0; i  < file_names_count; i += 1) {
                            for (u8 j = 0; j < file_name_entry_format_count; j += 1) {
                                switch (file_name_entry_format_form_code[j]) {
                                    case DW_FORM_line_strp: {
                                        u64 offset;
                                        if (format == 1) {
                                            offset = read(u64, o);
                                        } else {
                                            offset = read(u32, o);
                                        }

                                        print("|   |   | offset = {}\n", offset);
                                    } break;

                                    case DW_FORM_udata: {
                                        u64 value = leb_decode(data, o);
                                        print("|   |   | value = {}\n", value);
                                    } break;

                                    case DW_FORM_data16: {
                                        ASSERT (file_name_entry_format_content_type_code[j] == DW_LNCT_MD5);

                                        print("|   |   | ");
                                        u8 value[16];
                                        for (int k = 0; k < 16; k += 1) {
                                            value[k] = read(u8, o);
                                            print("{02x}", value[k]);
                                        }

                                        print("\n");
                                    } break;

                                    default: {
                                        print("|   |   | {}\n", dw_form_to_string(file_name_entry_format_form_code[j]));
                                        ASSERT_NOT_REACHED();
                                    } break;
                                }
                            }
                        }

                        // Now comes the actual "line number program", ...
                        u64 program_length = (length + length_size) - (o - offset);
                        o += program_length;

                        ASSERT((u64)(o - offset) == size);
                        o = ot;
                    } else if (utf8_compare(sectname, "__debug_addr", 16)) {
                        int ot = o;
                        o = offset;

                        dwarf_debug_addr = data + o;

                        while ((u64)(o - offset) < size) {
                            // Check format
                            u32 format = 0; // 32

                            u32 ones = read(u32, o);
                            if (ones == 0xFFFFFFFF) {
                                format = 1; // 64
                            }

                            // Rewind
                            o -= 4;

                            // clang on macOS can only output DWARF32, so that is
                            // what we also output and expect for now.
                            ASSERT(format == 0);

                            u64 length;
                            u64 length_size;
                            if (format == 1) {
                                length = read(u64, o);
                                length_size = 8;
                            } else {
                                length = read(u32, o);
                                length_size = 4;
                            }

                            u16 version = read(u16, o);
                            ASSERT(version == 5);

                            u8 address_size = read(u8, o);
                            ASSERT(address_size == 8);

                            u8 segment_selector_size = read(u8, o);
                            ASSERT(segment_selector_size == 0);

                            print("|   | length = \u001b[1m0x{x}\u001b[0m (\u001b[1m{}\u001b[0m)\n", length, length);
                            print("|   | version = \u001b[1m0x{x}\u001b[0m (\u001b[1m{}\u001b[0m)\n", version, version);
                            print("|   | address_size = \u001b[1m0x{x}\u001b[0m (\u001b[1m{}\u001b[0m)\n", address_size, address_size);
                            print("|   | segment_selector_size = \u001b[1m0x{x}\u001b[0m (\u001b[1m{}\u001b[0m)\n", segment_selector_size, segment_selector_size);
                            print("|   |\n");

                            u64 base = o;
                            while ((u64)(o - base) < length - 2 - 1 - 1) {
                                u64 address = read(u64, o);
                                print("|   | \u001b[1m0x{08x}\u001b[0m\n", address);
                            }
                        }

                        ASSERT((u64)(o - offset) == size);
                        o = ot;
                    } else if (utf8_compare(sectname, "__debug_line_str", 16)) {
                        int ot = o;
                        o = offset;

                        dwarf_debug_line_str = data + o;

                        while ((u64)o < offset + size) {
                            print("|   |   | 0x{08x} \u001b[1m{}\u001b[0m\n", o - offset, (utf8 const *)(data + o));
                            o += utf8_string_length((char *)(data + o)) + 1;
                        }

                        ASSERT((u64)(o - offset) == size);
                        o = ot;
                    } else {
                        print("Unrecognized sectname \"{16}\"\n", sectname);
                        ASSERT_NOT_REACHED();
                    }
                } else if (utf8_compare(segname, "__TEXT", 16)) {
                } else if (utf8_compare(segname, "__DATA", 16)) {
                } else {
                    print("Unrecognized segname \"{16}\"\n", segname);
                    ASSERT_NOT_REACHED();
                }

                if (j < nsects - 1) {
                    print("|\n");
                }
            }
        } else if (cmd == 0x32) {
            // struct build_version_command {
            //     uint32_t    cmd;        /* LC_BUILD_VERSION */
            //     uint32_t    cmdsize;    /* sizeof(struct build_version_command) plus */
            //                                 /* ntools * sizeof(struct build_tool_version) */
            //     uint32_t    platform;   /* platform */
            //     uint32_t    minos;      /* X.Y.Z is encoded in nibbles xxxx.yy.zz */
            //     uint32_t    sdk;        /* X.Y.Z is encoded in nibbles xxxx.yy.zz */
            //     uint32_t    ntools;     /* number of tool entries following this */
            // };

            u32 platform = read(u32, o);
            u32 minos = read(u32, o);
            u32 sdk = read(u32, o);
            u32 ntools = read(u32, o);
            print("|  platform = \u001b[1m0x{x}\u001b[0m (", platform);
            switch (platform) {
                case 1: { print("\u001b[1mPLATFORM_MACOS\u001b[0m)\n"); } break;
                case 2: { print("\u001b[1mPLATFORM_IOS\u001b[0m)\n"); } break;
                case 3: { print("\u001b[1mPLATFORM_TVOS\u001b[0m)\n"); } break;
                case 4: { print("\u001b[1mPLATFORM_WATCHOS\u001b[0m)\n"); } break;
                case 5: { print("\u001b[1mPLATFORM_BRIDGEOS\u001b[0m)\n"); } break;
                case 6: { print("\u001b[1mPLATFORM_IOSMAC\u001b[0m)\n"); } break;
                case 7: { print("\u001b[1mPLATFORM_IOSSIMULATOR\u001b[0m)\n"); } break;
                case 8: { print("\u001b[1mPLATFORM_TVOSSIMULATOR\u001b[0m)\n"); } break;
                case 9: { print("\u001b[1mPLATFORM_WATCHOSSIMULATOR\u001b[0m)\n"); } break;

                default: {
                    ASSERT_NOT_REACHED();
                } break;
            }
            print("|  minos = \u001b[1m0x{x}\u001b[0m (\u001b[1m{}.{}.{}\u001b[0m)\n", minos, ((minos >> 16) & 0xFFFF), ((minos >> 8) & 0xFF), ((minos >> 0) & 0xFF));
            print("|  sdk = \u001b[1m0x{x}\u001b[0m (\u001b[1m{}.{}.{}\u001b[0m)\n", sdk, ((sdk >> 16) & 0xFFFF), ((sdk >> 8) & 0xFF), ((sdk >> 0) & 0xFF));
            print("|  ntools = \u001b[1m{}\u001b[0m\n", ntools);

            // struct build_tool_version {
            //     uint32_t    tool;       /* enum for the tool */
            //     uint32_t    version;    /* version number of the tool */
            // };

            for (u32 j = 0; j < ntools; j += 1) {
                u32 tool = read(u32, o);
                u32 version = read(u32, o);
                print("    tool = \u001b[1m0x{x}\u001b[0m (", tool);
                switch (tool) {
                    case 1: { print("\u001b[1mTOOL_CLANG\u001b[0m)\n"); }; break;
                    case 2: { print("\u001b[1mTOOL_SWIFT\u001b[0m)\n"); }; break;
                    case 3: { print("\u001b[1mTOOL_LD\u001b[0m)\n"); }; break;

                    default: {
                        ASSERT_NOT_REACHED();
                    } break;
                }
                print("    version = \u001b[1m0x{x}\u001b[0m\n", version);
            }
        } else if (cmd == 0x2) {
            // struct symtab_command {
            //     uint32_t    cmd;        /* LC_SYMTAB */
            //     uint32_t    cmdsize;    /* sizeof(struct symtab_command) */
            //     uint32_t    symoff;     /* symbol table offset */
            //     uint32_t    nsyms;      /* number of symbol table entries */
            //     uint32_t    stroff;     /* string table offset */
            //     uint32_t    strsize;    /* string table size in bytes */
            // };

            u32 symoff = read(u32, o);
            u32 nsyms = read(u32, o);
            u32 stroff = read(u32, o);
            u32 strsize = read(u32, o);
            print("|   symoff  = \u001b[1m0x{x}\u001b[0m (\u001b[1m{} bytes\u001b[0m)\n", symoff, symoff);
            print("|   nsyms   = \u001b[1m{}\u001b[0m\n", nsyms);
            print("|   stroff  = \u001b[1m0x{x}\u001b[0m (\u001b[1m{} bytes\u001b[0m)\n", stroff, stroff);
            print("|   strsize = \u001b[1m0x{x}\u001b[0m (\u001b[1m{} bytes\u001b[0m)\n", strsize, strsize);

            // dump(buffer.pointer, stroff, strsize);

            if (nsyms > 0) {
                print("|\n");
            }

            for (u32 j = 0; j < nsyms; j += 1) {
                int ot = o;
                o = (int)symoff;

                // struct nlist_64 {
                //     union {
                //         uint32_t  n_strx; /* index into the string table */
                //     } n_un;
                //     uint8_t n_type;        /* type flag, see below */
                //     uint8_t n_sect;        /* section number or NO_SECT */
                //     uint16_t n_desc;       /* see <mach-o/stab.h> */
                //     uint64_t n_value;      /* value of this symbol (or stab offset) */
                // };
                print("|   Symbol #{}\n", j);

                u32 n_strx = read(u32, o);
                u8 n_type = read(u8, o);

                print("|   | n_strx = \u001b[1m{}\u001b[0m (\u001b[1m\"{}\"\u001b[0m)\n", n_strx, (utf8 const *)(data + stroff + n_strx));
                print("|   |     = \u001b[1m0x{x}\u001b[0m\n", n_type);

                print("|   | | N_STAB = \u001b[1m0x{x}\u001b[0m\n", (u8)(n_type & 0xe0));
                print("|   | | N_PEXT = \u001b[1m0x{x}\u001b[0m\n", (u8)(n_type & 0x10));
                print("|   | | N_TYPE = \u001b[1m0x{x}\u001b[0m\n", (u8)(n_type & 0x0e));
                print("|   | | N_EXT = \u001b[1m0x{x}\u001b[0m\n", (u8)(n_type & 0x01));

                u8 n_sect = read(u8, o);
                u16 n_desc = read(u16, o);
                u64 n_value = read(u64, o);

                print("|   | n_sect = \u001b[1m{}\u001b[0m", n_sect);
                if (n_sect == 0) {
                    print(" (\u001b[1mNO_SECT\u001b[0m)");
                }
                print("\n");
                print("|   | n_desc = \u001b[1m0x{x}\u001b[0m\n", n_desc);
                print("|   | n_value = \u001b[1m0x{x}\u001b[0m (\u001b[1m{}\u001b[0m)\n", n_value, n_value);

                if (j < nsyms - 1) {
                    print("|\n");
                }

                o = ot;
            }
        } else if (cmd == 0xb) {
            // struct dysymtab_command {
            //     uint32_t cmd;   /* LC_DYSYMTAB */
            //     uint32_t cmdsize;   /* sizeof(struct dysymtab_command) */
            //     uint32_t ilocalsym; /* index to local symbols */
            //     uint32_t nlocalsym; /* number of local symbols */
            //     uint32_t iextdefsym;/* index to externally defined symbols */
            //     uint32_t nextdefsym;/* number of externally defined symbols */
            //     uint32_t iundefsym; /* index to undefined symbols */
            //     uint32_t nundefsym; /* number of undefined symbols */
            //     uint32_t tocoff;    /* file offset to table of contents */
            //     uint32_t ntoc;  /* number of entries in table of contents */
            //     uint32_t modtaboff; /* file offset to module table */
            //     uint32_t nmodtab;   /* number of module table entries */
            //     uint32_t extrefsymoff;  /* offset to referenced symbol table */
            //     uint32_t nextrefsyms;   /* number of referenced symbol table entries */
            //     uint32_t indirectsymoff; /* file offset to the indirect symbol table */
            //     uint32_t nindirectsyms;  /* number of indirect symbol table entries */
            //     uint32_t extreloff; /* offset to external relocation entries */
            //     uint32_t nextrel;   /* number of external relocation entries */
            //     uint32_t locreloff; /* offset to local relocation entries */
            //     uint32_t nlocrel;   /* number of local relocation entries */
            // };

            u32 ilocalsym = read(u32, o);
            u32 nlocalsym = read(u32, o);
            u32 iextdefsym = read(u32, o);
            u32 nextdefsym = read(u32, o);
            u32 iundefsym = read(u32, o);
            u32 nundefsym = read(u32, o);
            u32 tocoff = read(u32, o);
            u32 ntoc = read(u32, o);
            u32 modtaboff = read(u32, o);
            u32 nmodtab = read(u32, o);
            u32 extrefsymoff = read(u32, o);
            u32 nextrefsyms = read(u32, o);
            u32 indirectsymoff = read(u32, o);
            u32 nindirectsyms = read(u32, o);
            u32 extreloff = read(u32, o);
            u32 nextrel = read(u32, o);
            u32 locreloff = read(u32, o);
            u32 nlocrel = read(u32, o);

            print("|   ilocalsym      = \u001b[1m{}\u001b[0m\n", ilocalsym);
            print("|   nlocalsym      = \u001b[1m{}\u001b[0m\n", nlocalsym);
            print("|   iextdefsym     = \u001b[1m{}\u001b[0m\n", iextdefsym);
            print("|   nextdefsym     = \u001b[1m{}\u001b[0m\n", nextdefsym);
            print("|   iundefsym      = \u001b[1m{}\u001b[0m\n", iundefsym);
            print("|   nundefsym      = \u001b[1m{}\u001b[0m\n", nundefsym);
            print("|   tocoff         = \u001b[1m0x{x}\u001b[0m (\u001b[1m{} bytes\u001b[0m)\n", tocoff, tocoff);
            print("|   ntoc           = \u001b[1m{}\u001b[0m\n", ntoc);
            print("|   modtaboff      = \u001b[1m0x{x}\u001b[0m (\u001b[1m{} bytes\u001b[0m)\n", modtaboff, modtaboff);
            print("|   nmodtab        = \u001b[1m{}\u001b[0m\n", nmodtab);
            print("|   extrefsymoff   = \u001b[1m0x{x}\u001b[0m (\u001b[1m{} bytes\u001b[0m)\n", extrefsymoff, extrefsymoff);
            print("|   nextrefsyms    = \u001b[1m{}\u001b[0m\n", nextrefsyms);
            print("|   indirectsymoff = \u001b[1m0x{x}\u001b[0m (\u001b[1m{} bytes\u001b[0m)\n", indirectsymoff, indirectsymoff);
            print("|   nindirectsyms  = \u001b[1m{}\u001b[0m\n", nindirectsyms);
            print("|   extreloff      = \u001b[1m0x{x}\u001b[0m (\u001b[1m{} bytes\u001b[0m)\n", extreloff, extreloff);
            print("|   nextrel        = \u001b[1m{}\u001b[0m\n", nextrel);
            print("|   locreloff      = \u001b[1m0x{x}\u001b[0m (\u001b[1m{} bytes\u001b[0m)\n", locreloff, locreloff);
            print("|   nlocrel        = \u001b[1m{}\u001b[0m\n", nlocrel);
        } else {
            ASSERT_NOT_REACHED();
        }

        if (i < ncmds - 1) {
            print("\n");
        }

        o = cmdoffset + cmdsize;
    }

    return 0;
}
