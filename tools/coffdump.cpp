struct time_t;
struct tm;
extern "C" tm * gmtime(time_t *);
extern "C" char * asctime(tm *);

#include "../src/lib/types.hpp"
#include "../src/lib/assert.hpp"
#include "../src/lib/memory.hpp"
#include "../src/lib/print.hpp"
#include "../src/lib/array.hpp"
#include "../src/lib/unicode.hpp"
#include "../src/lib/file.hpp"
#include "../src/lib/buffer.hpp"

#define read(type, o) *(type *)(data + o); o += sizeof(type)

char const * machine_to_string(u16 machine) {
    switch (machine) {
        case 0x0: { return "IMAGE_FILE_MACHINE_UNKNOWN"; } break; // The content of this field is assumed to be applicable to any machine type
        case 0x184: { return "IMAGE_FILE_MACHINE_ALPHA"; } break; // Alpha AXP, 32-bit address space
        case 0x284: { return "IMAGE_FILE_MACHINE_ALPHA64"; } break; // Alpha 64, 64-bit address space
        case 0x1d3: { return "IMAGE_FILE_MACHINE_AM33"; } break; // Matsushita AM33
        case 0x8664: { return "IMAGE_FILE_MACHINE_AMD64"; } break; // x64
        case 0x1c0: { return "IMAGE_FILE_MACHINE_ARM"; } break; // ARM little endian
        case 0xaa64: { return "IMAGE_FILE_MACHINE_ARM64"; } break; // ARM64 little endian
        case 0x1c4: { return "IMAGE_FILE_MACHINE_ARMNT"; } break; // ARM Thumb-2 little endian
        // case 0x284: { return "IMAGE_FILE_MACHINE_AXP64"; } break; // AXP 64 (Same as Alpha 64)
        case 0xebc: { return "IMAGE_FILE_MACHINE_EBC"; } break; // EFI byte code
        case 0x14c: { return "IMAGE_FILE_MACHINE_I386"; } break; // Intel 386 or later processors and compatible processors
        case 0x200: { return "IMAGE_FILE_MACHINE_IA64"; } break; // Intel Itanium processor family
        case 0x6232: { return "IMAGE_FILE_MACHINE_LOONGARCH32"; } break; // LoongArch 32-bit processor family
        case 0x6264: { return "IMAGE_FILE_MACHINE_LOONGARCH64"; } break; // LoongArch 64-bit processor family
        case 0x9041: { return "IMAGE_FILE_MACHINE_M32R"; } break; // Mitsubishi M32R little endian
        case 0x266: { return "IMAGE_FILE_MACHINE_MIPS16"; } break; // MIPS16
        case 0x366: { return "IMAGE_FILE_MACHINE_MIPSFPU"; } break; // MIPS with FPU
        case 0x466: { return "IMAGE_FILE_MACHINE_MIPSFPU16"; } break; // MIPS16 with FPU
        case 0x1f0: { return "IMAGE_FILE_MACHINE_POWERPC"; } break; // Power PC little endian
        case 0x1f1: { return "IMAGE_FILE_MACHINE_POWERPCFP"; } break; // Power PC with floating point support
        case 0x166: { return "IMAGE_FILE_MACHINE_R4000"; } break; // MIPS little endian
        case 0x5032: { return "IMAGE_FILE_MACHINE_RISCV32"; } break; // RISC-V 32-bit address space
        case 0x5064: { return "IMAGE_FILE_MACHINE_RISCV64"; } break; // RISC-V 64-bit address space
        case 0x5128: { return "IMAGE_FILE_MACHINE_RISCV128"; } break; // RISC-V 128-bit address space
        case 0x1a2: { return "IMAGE_FILE_MACHINE_SH3"; } break; // Hitachi SH3
        case 0x1a3: { return "IMAGE_FILE_MACHINE_SH3DSP"; } break; // Hitachi SH3 DSP
        case 0x1a6: { return "IMAGE_FILE_MACHINE_SH4"; } break; // Hitachi SH4
        case 0x1a8: { return "IMAGE_FILE_MACHINE_SH5"; } break; // Hitachi SH5
        case 0x1c2: { return "IMAGE_FILE_MACHINE_THUMB"; } break; // Thumb
        case 0x169: { return "IMAGE_FILE_MACHINE_WCEMIPSV2"; } break; // MIPS little-endian WCE v2}

        default: {
            ASSERT_NOT_REACHED();
        } break;
    }

    ASSERT_NOT_REACHED();
}

int main(int argc, char ** argv) {
    Get_File_Result result = get_file(argv[1], FALSE);
    char * data = (char *)result.bytes;

    int o = 0;
    u16 machine = read(u16, o);
    u16 number_of_sections = read(u16, o);
    i32 time_date_stamp = read(i32, o);
    i32 pointer_to_symbol_table = read(i32, o);
    i32 number_of_symbols = read(i32, o);
    u16 size_of_optional_header = read(u16, o);
    u16 characteristics = read(u16, o);

    print("machine                 = 0x{x}\n", machine);
    print("                          {}\n", machine_to_string(machine));
    print("number_of_sections      = {}\n", number_of_sections);
    {
        print("time_date_stamp         = {} (", time_date_stamp);

        char * time_date_stamp_string = asctime(gmtime((time_t *)&time_date_stamp));
        print("{*})\n", utf8_length_string(time_date_stamp_string) - 1, time_date_stamp_string);
    }
    print("pointer_to_symbol_table = 0x{x}\n", pointer_to_symbol_table);
    print("number_of_symbols       = {}\n", number_of_symbols);
    print("size_of_optional_header = 0x{x}\n", size_of_optional_header);
    print("characteristics         = 0x{x}\n", characteristics);

    if (characteristics & 0x0001) { print("                          IMAGE_FILE_RELOCS_STRIPPED\n"); } // Image only, Windows CE, and Microsoft Windows NT and later. This indicates that the file does not contain base relocations and must therefore be loaded at its preferred base address. If the base address is not available, the loader reports an error. The default behavior of the linker is to strip base relocations from executable (EXE) files.
    if (characteristics & 0x0002) { print("                          IMAGE_FILE_EXECUTABLE_IMAGE\n"); } // Image only. This indicates that the image file is valid and can be run. If this flag is not set, it indicates a linker error.
    if (characteristics & 0x0004) { print("                          IMAGE_FILE_LINE_NUMS_STRIPPED\n"); } // COFF line numbers have been removed. This flag is deprecated and should be zero.
    if (characteristics & 0x0008) { print("                          IMAGE_FILE_LOCAL_SYMS_STRIPPED\n"); } // COFF symbol table entries for local symbols have been removed. This flag is deprecated and should be zero.
    if (characteristics & 0x0010) { print("                          IMAGE_FILE_AGGRESSIVE_WS_TRIM\n"); } // Obsolete. Aggressively trim working set. This flag is deprecated for Windows 2000 and later and must be zero.
    if (characteristics & 0x0020) { print("                          IMAGE_FILE_LARGE_ADDRESS_AWARE\n"); } // Application can handle > 2-GB addresses. 0x0040 //This flag is reserved for future use.
    if (characteristics & 0x0080) { print("                          IMAGE_FILE_BYTES_REVERSED_LO\n"); } // Little endian: the least significant bit (LSB) precedes the most significant bit (MSB) in memory. This flag is deprecated and should be zero.
    if (characteristics & 0x0100) { print("                          IMAGE_FILE_32BIT_MACHINE\n"); } // Machine is based on a 32-bit-word architecture.
    if (characteristics & 0x0200) { print("                          IMAGE_FILE_DEBUG_STRIPPED\n"); } // Debugging information is removed from the image file.
    if (characteristics & 0x0400) { print("                          IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP\n"); } // If the image is on removable media, fully load it and copy it to the swap file.
    if (characteristics & 0x0800) { print("                          IMAGE_FILE_NET_RUN_FROM_SWAP\n"); } // If the image is on network media, fully load it and copy it to the swap file.
    if (characteristics & 0x1000) { print("                          IMAGE_FILE_SYSTEM\n"); } // The image file is a system file, not a user program.
    if (characteristics & 0x2000) { print("                          IMAGE_FILE_DLL\n"); } // The image file is a dynamic-link library (DLL). Such files are considered executable files for almost all purposes, although they cannot be directly run.
    if (characteristics & 0x4000) { print("                          IMAGE_FILE_UP_SYSTEM_ONLY\n"); } // The file should be run only on a uniprocessor machine.
    if (characteristics & 0x8000) { print("                          IMAGE_FILE_BYTES_REVERSED_HI\n"); } // Big endian: the MSB precedes the LSB in memory. This flag is deprecated and should be zero.
    print("\n");

    for (u16 section_index = 0; section_index < number_of_sections; section_index += 1) {
        u64 name = read(u64, o);
        u32 virtual_size = read(u32, o);
        u32 virtual_address = read(u32, o);
        u32 size_of_raw_data = read(u32, o);
        u32 pointer_to_raw_data = read(u32, o);
        u32 pointer_to_relocations = read(u32, o);
        u32 pointer_to_line_numbers = read(u32, o);
        u16 number_of_relocations = read(u16, o);
        u16 number_of_line_numbers = read(u16, o);
        u32 characteristics = read(u32, o);

        print("Section {}\n", section_index);
        print("  name                    = {8}\n", (char *)&name);
        print("  virtual_size            = 0x{x}\n", virtual_size);
        print("  virtual_address         = 0x{x}\n", virtual_address);
        print("  size_of_raw_data        = 0x{x}\n", size_of_raw_data);
        print("  pointer_to_raw_data     = 0x{x}\n", pointer_to_raw_data);
        print("  pointer_to_relocations  = 0x{x}\n", pointer_to_relocations);
        print("  pointer_to_line_numbers = 0x{x}\n", pointer_to_line_numbers);
        print("  number_of_relocations   = {}\n", number_of_relocations);
        print("  number_of_line_numbers  = {}\n", number_of_line_numbers);
        print("  characteristics         = 0x{x}\n", characteristics);

        if (characteristics & 0x00000008) { print("                            IMAGE_SCN_TYPE_NO_PAD\n"); } // The section should not be padded to the next boundary. This flag is obsolete and is replaced by IMAGE_SCN_ALIGN_1BYTES. This is valid only for object files.
        if (characteristics & 0x00000020) { print("                            IMAGE_SCN_CNT_CODE\n"); } // The section contains executable code.
        if (characteristics & 0x00000040) { print("                            IMAGE_SCN_CNT_INITIALIZED_DATA\n"); } // The section contains initialized data.
        if (characteristics & 0x00000080) { print("                            IMAGE_SCN_CNT_UNINITIALIZED_DATA\n"); } // The section contains uninitialized data.
        if (characteristics & 0x00000100) { print("                            IMAGE_SCN_LNK_OTHER\n"); } // Reserved for future use.
        if (characteristics & 0x00000200) { print("                            IMAGE_SCN_LNK_INFO\n"); } // The section contains comments or other information. The .drectve section has this type. This is valid for object files only. 0x00000400 //Reserved for future use.
        if (characteristics & 0x00000800) { print("                            IMAGE_SCN_LNK_REMOVE\n"); } // The section will not become part of the image. This is valid only for object files.
        if (characteristics & 0x00001000) { print("                            IMAGE_SCN_LNK_COMDAT\n"); } // The section contains COMDAT data. For more information, see COMDAT Sections (Object Only). This is valid only for object files.
        if (characteristics & 0x00008000) { print("                            IMAGE_SCN_GPREL\n"); } // The section contains data referenced through the global pointer (GP).
        if (characteristics & 0x00020000) { print("                            IMAGE_SCN_MEM_PURGEABLE\n"); } // Reserved for future use.
        if (characteristics & 0x00020000) { print("                            IMAGE_SCN_MEM_16BIT\n"); } // Reserved for future use.
        if (characteristics & 0x00040000) { print("                            IMAGE_SCN_MEM_LOCKED\n"); } // Reserved for future use.
        if (characteristics & 0x00080000) { print("                            IMAGE_SCN_MEM_PRELOAD\n"); } // Reserved for future use.
        if ((characteristics & 0x00F00000) == 0x00100000) { print("                            IMAGE_SCN_ALIGN_1BYTES\n"); } // Align data on a 1-byte boundary. Valid only for object files.
        if ((characteristics & 0x00F00000) == 0x00200000) { print("                            IMAGE_SCN_ALIGN_2BYTES\n"); } // Align data on a 2-byte boundary. Valid only for object files.
        if ((characteristics & 0x00F00000) == 0x00300000) { print("                            IMAGE_SCN_ALIGN_4BYTES\n"); } // Align data on a 4-byte boundary. Valid only for object files.
        if ((characteristics & 0x00F00000) == 0x00400000) { print("                            IMAGE_SCN_ALIGN_8BYTES\n"); } // Align data on an 8-byte boundary. Valid only for object files.
        if ((characteristics & 0x00F00000) == 0x00500000) { print("                            IMAGE_SCN_ALIGN_16BYTES\n"); } // Align data on a 16-byte boundary. Valid only for object files.
        if ((characteristics & 0x00F00000) == 0x00600000) { print("                            IMAGE_SCN_ALIGN_32BYTES\n"); } // Align data on a 32-byte boundary. Valid only for object files.
        if ((characteristics & 0x00F00000) == 0x00700000) { print("                            IMAGE_SCN_ALIGN_64BYTES\n"); } // Align data on a 64-byte boundary. Valid only for object files.
        if ((characteristics & 0x00F00000) == 0x00800000) { print("                            IMAGE_SCN_ALIGN_128BYTES\n"); } // Align data on a 128-byte boundary. Valid only for object files.
        if ((characteristics & 0x00F00000) == 0x00900000) { print("                            IMAGE_SCN_ALIGN_256BYTES\n"); } // Align data on a 256-byte boundary. Valid only for object files.
        if ((characteristics & 0x00F00000) == 0x00A00000) { print("                            IMAGE_SCN_ALIGN_512BYTES\n"); } // Align data on a 512-byte boundary. Valid only for object files.
        if ((characteristics & 0x00F00000) == 0x00B00000) { print("                            IMAGE_SCN_ALIGN_1024BYTES\n"); } // Align data on a 1024-byte boundary. Valid only for object files.
        if ((characteristics & 0x00F00000) == 0x00C00000) { print("                            IMAGE_SCN_ALIGN_2048BYTES\n"); } // Align data on a 2048-byte boundary. Valid only for object files.
        if ((characteristics & 0x00F00000) == 0x00D00000) { print("                            IMAGE_SCN_ALIGN_4096BYTES\n"); } // Align data on a 4096-byte boundary. Valid only for object files.
        if ((characteristics & 0x00F00000) == 0x00E00000) { print("                            IMAGE_SCN_ALIGN_8192BYTES\n"); } // Align data on an 8192-byte boundary. Valid only for object files.
        if (characteristics & 0x01000000) { print("                            IMAGE_SCN_LNK_NRELOC_OVFL\n"); } // The section contains extended relocations.
        if (characteristics & 0x02000000) { print("                            IMAGE_SCN_MEM_DISCARDABLE\n"); } // The section can be discarded as needed.
        if (characteristics & 0x04000000) { print("                            IMAGE_SCN_MEM_NOT_CACHED\n"); } // The section cannot be cached.
        if (characteristics & 0x08000000) { print("                            IMAGE_SCN_MEM_NOT_PAGED\n"); } // The section is not pageable.
        if (characteristics & 0x10000000) { print("                            IMAGE_SCN_MEM_SHARED\n"); } // The section can be shared in memory.
        if (characteristics & 0x20000000) { print("                            IMAGE_SCN_MEM_EXECUTE\n"); } // The section can be executed as code.
        if (characteristics & 0x40000000) { print("                            IMAGE_SCN_MEM_READ\n"); } // The section can be read.
        if (characteristics & 0x80000000) { print("                            IMAGE_SCN_MEM_WRITE\n"); } // The section can be written to.
        print("\n");

        buffer_print_raw((void *)data, (u64)pointer_to_raw_data, (u64)size_of_raw_data);
        print("\n");

        if (number_of_relocations > 0) {
            int ot = o;
            o = pointer_to_relocations;

            print("Relocations:\n");
            for (int i = 0; i < number_of_relocations; i += 1) {
                u32 virtual_address = read(u32, o);
                u32 symbol_table_index = read(u32, o);
                u16 type = read(u16, o);

                print("  {}:\n", i);
                print("    virtual_address = 0x{}\n", virtual_address);
                print("    symbol_table_index = {}\n", symbol_table_index);
                print("    type = {} (", type);

                if (machine == 0x8664) {
                    switch (type) {
                        case 0x0000: { print("IMAGE_REL_AMD64_ABSOLUTE"); } break; // The relocation is ignored.
                        case 0x0001: { print("IMAGE_REL_AMD64_ADDR64"); } break; // The 64-bit VA of the relocation target.
                        case 0x0002: { print("IMAGE_REL_AMD64_ADDR32"); } break; // The 32-bit VA of the relocation target.
                        case 0x0003: { print("IMAGE_REL_AMD64_ADDR32NB"); } break; // The 32-bit address without an image base (RVA).
                        case 0x0004: { print("IMAGE_REL_AMD64_REL32"); } break; // The 32-bit relative address from the byte following the relocation.
                        case 0x0005: { print("IMAGE_REL_AMD64_REL32_1"); } break; // The 32-bit address relative to byte distance 1 from the relocation.
                        case 0x0006: { print("IMAGE_REL_AMD64_REL32_2"); } break; // The 32-bit address relative to byte distance 2 from the relocation.
                        case 0x0007: { print("IMAGE_REL_AMD64_REL32_3"); } break; // The 32-bit address relative to byte distance 3 from the relocation.
                        case 0x0008: { print("IMAGE_REL_AMD64_REL32_4"); } break; // The 32-bit address relative to byte distance 4 from the relocation.
                        case 0x0009: { print("IMAGE_REL_AMD64_REL32_5"); } break; // The 32-bit address relative to byte distance 5 from the relocation.
                        case 0x000A: { print("IMAGE_REL_AMD64_SECTION"); } break; // The 16-bit section index of the section that contains the target. This is used to support debugging information.
                        case 0x000B: { print("IMAGE_REL_AMD64_SECREL"); } break; // The 32-bit offset of the target from the beginning of its section. This is used to support debugging information and static thread local storage.
                        case 0x000C: { print("IMAGE_REL_AMD64_SECREL7"); } break; // A 7-bit unsigned offset from the base of the section that contains the target.
                        case 0x000D: { print("IMAGE_REL_AMD64_TOKEN"); } break; // CLR tokens.
                        case 0x000E: { print("IMAGE_REL_AMD64_SREL32"); } break; // A 32-bit signed span-dependent value emitted into the object.
                        case 0x000F: { print("IMAGE_REL_AMD64_PAIR"); } break; // A pair that must immediately follow every span-dependent value.
                        case 0x0010: { print("IMAGE_REL_AMD64_SSPAN32"); } break; // A 32-bit signed span-dependent value that is applied at link time.
                        default: {
                            ASSERT_NOT_REACHED();
                        } break;
                    }
                } else if (machine == 0xaa64) {
                    switch (type) {
                        case 0x0000: { print("IMAGE_REL_ARM64_ABSOLUTE"); } break; // The relocation is ignored.
                        case 0x0001: { print("IMAGE_REL_ARM64_ADDR32"); } break; // The 32-bit VA of the target.
                        case 0x0002: { print("IMAGE_REL_ARM64_ADDR32NB"); } break; // The 32-bit RVA of the target.
                        case 0x0003: { print("IMAGE_REL_ARM64_BRANCH26"); } break; // The 26-bit relative displacement to the target, for B and BL instructions.
                        case 0x0004: { print("IMAGE_REL_ARM64_PAGEBASE_REL21"); } break; // The page base of the target, for ADRP instruction.
                        case 0x0005: { print("IMAGE_REL_ARM64_REL21"); } break; // The 12-bit relative displacement to the target, for instruction ADR
                        case 0x0006: { print("IMAGE_REL_ARM64_PAGEOFFSET_12A"); } break; // The 12-bit page offset of the target, for instructions ADD/ADDS (immediate) with zero shift.
                        case 0x0007: { print("IMAGE_REL_ARM64_PAGEOFFSET_12L"); } break; // The 12-bit page offset of the target, for instruction LDR (indexed, unsigned immediate).
                        case 0x0008: { print("IMAGE_REL_ARM64_SECREL"); } break; // The 32-bit offset of the target from the beginning of its section. This is used to support debugging information and static thread local storage.
                        case 0x0009: { print("IMAGE_REL_ARM64_SECREL_LOW12A"); } break; // Bit 0:11 of section offset of the target, for instructions ADD/ADDS (immediate) with zero shift.
                        case 0x000A: { print("IMAGE_REL_ARM64_SECREL_HIGH12A"); } break; // Bit 12:23 of section offset of the target, for instructions ADD/ADDS (immediate) with zero shift.
                        case 0x000B: { print("IMAGE_REL_ARM64_SECREL_LOW12L"); } break; // Bit 0:11 of section offset of the target, for instruction LDR (indexed, unsigned immediate).
                        case 0x000C: { print("IMAGE_REL_ARM64_TOKEN"); } break; // CLR token.
                        case 0x000D: { print("IMAGE_REL_ARM64_SECTION"); } break; // The 16-bit section index of the section that contains the target. This is used to support debugging information.
                        case 0x000E: { print("IMAGE_REL_ARM64_ADDR64"); } break; // The 64-bit VA of the relocation target.
                        case 0x000F: { print("IMAGE_REL_ARM64_BRANCH19"); } break; // The 19-bit offset to the relocation target, for conditional B instruction.
                        case 0x0010: { print("IMAGE_REL_ARM64_BRANCH14"); } break; // The 14-bit offset to the relocation target, for instructions TBZ and TBNZ.
                        case 0x0011: { print("IMAGE_REL_ARM64_REL32"); } break; // The 32-bit relative address from the byte following the relocation.
                        default: {
                            ASSERT_NOT_REACHED();
                        } break;
                    }
                } else {
                    ASSERT_NOT_REACHED();
                }

                print(")\n");
            }

            o = ot;
        }
    }

    if (number_of_symbols > 0) {
        int ot = o;
        o = pointer_to_symbol_table;

        print("Symbols\n");
        for (i32 symbol_index = 0; symbol_index < number_of_symbols; symbol_index += 1) {
            print("  {}:\n", symbol_index);

            u32 name_zeroes = read(u32, o); // The name of the symbol, represented by a union of three structures. An array of 8 bytes is used if the name is not more than 8 bytes long. For more information, see Symbol Name Representation.
            u32 name_offset = read(u32, o);
            u32 value = read(u32, o); // The value that is associated with the symbol. The interpretation of this field depends on SectionNumber and StorageClass. A typical meaning is the relocatable address.
            u16 section_number = read(i16, o); // The signed integer that identifies the section, using a one-based index into the section table. Some values have special meaning, as defined in section 5.4.2, "Section Number Values."
            u16 type = read(u16, o); // A number that represents type. Microsoft tools set this field to 0x20 (function) or 0x0 (not a function). For more information, see Type Representation.
            u8 storage_class = read(u8, o); // An enumerated value that represents storage class. For more information, see Storage Class.
            u8 number_of_aux_symbols = read(u8, o); // The number of auxiliary symbol table entries that follow this record.

            print("    name_zeroes = {}\n", name_zeroes);
            print("    name_offset = {}\n", name_offset);
            print("    value = {}\n", value);
            print("    section_number = {}\n", section_number);
            print("    type = {}\n", type);
            print("    storage_class = {}\n", storage_class);
            print("    number_of_aux_symbols = {}\n", number_of_aux_symbols);
        }

        o = ot;
    }

    return 0;
}
