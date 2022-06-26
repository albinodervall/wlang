static u8 const eax = 0b000; // 0
static u8 const ecx = 0b001; // 1
static u8 const edx = 0b010; // 2
static u8 const ebx = 0b011; // 3
static u8 const esp = 0b100; // 4
static u8 const ebp = 0b101; // 5
static u8 const esi = 0b110; // 6
static u8 const edi = 0b111; // 7

static u8 const rax = 0b000; // 0
static u8 const rcx = 0b001; // 1
static u8 const rdx = 0b010; // 2
static u8 const rbx = 0b011; // 3
static u8 const rsp = 0b100; // 4
static u8 const rbp = 0b101; // 5
static u8 const rsi = 0b110; // 6
static u8 const rdi = 0b111; // 7

// These are not the actual values that will be put in the instructions,
// the top bit is the bit that will be put in the REX prefix.
static u8 const r8 = 0b1000;
static u8 const r9 = 0b1001;
static u8 const r10 = 0b1010;
static u8 const r11 = 0b1011;
static u8 const r12 = 0b1100;
static u8 const r13 = 0b1101;
static u8 const r14 = 0b1110;
static u8 const r15 = 0b1111;

static void x64_rex(Buffer & buffer, u8 w, u8 r, u8 x, u8 b) {
    ASSERT((w & ~0b1) == 0);
    ASSERT((r & ~0b1) == 0);
    ASSERT((x & ~0b1) == 0);
    ASSERT((b & ~0b1) == 0);

    u8 rex = 0b01000000;
    rex |= (w & 0b1) << 3;
    rex |= (r & 0b1) << 2;
    rex |= (x & 0b1) << 1;
    rex |= (b & 0b1) << 0;

    buffer_write_u8(buffer, rex);
}

static void x64_modrm(Buffer & buffer, u8 mod, u8 reg, u8 rm) {
    ASSERT((mod & ~0b11) == 0);
    ASSERT((reg & ~0b111) == 0);
    ASSERT((rm & ~0b111) == 0);

    u8 modrm = 0;
    modrm |= (mod & 0b11) << 6;
    modrm |= (reg & 0b111) << 3;
    modrm |= (rm & 0b111) << 0;

    buffer_write_u8(buffer, modrm);
}

static void x64_sib(Buffer & buffer, u8 scale, u8 index, u8 base) {
    ASSERT((scale & ~0b11) == 0);
    ASSERT((index & ~0b111) == 0);
    ASSERT((base & ~0b111) == 0);

    u8 sib = 0;
    sib |= ((scale & 0b11) << 6);
    sib |= ((index & 0b111) << 3);
    sib |= ((base & 0b111) << 0);

    buffer_write_u8(buffer, sib);
}

static void x64_push_r(Buffer & buffer, u8 r) {
    ASSERT(r < 0b1000);
    buffer_write_u8(buffer, 0x50 + (r & 0b111));
}

static void x64_mov_r_r(Buffer & buffer, u8 a, u8 b) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0x89);
    x64_modrm(buffer, 0b11, a, b);
}

static void x64_mov_r_i(Buffer & buffer, u8 reg, u32 imm) {
    ASSERT(reg < 0b1000);
    buffer_write_u8(buffer, 0xb8 + (reg & 0b111));
    buffer_write_u32(buffer, imm);
}

static void x64_pop_r(Buffer & buffer, u8 r) {
    ASSERT(r < 0b1000);
    buffer_write_u8(buffer, 0x58 + (r & 0b111));
}

// https://www.felixcloutier.com/x86/ret.html
static void x64_ret(Buffer & buffer) {
    buffer_write_u8(buffer, 0xC3);
}

static void x64_sub_immediate(Buffer & buffer, u8 r, i32 imm) {
    if (imm >= I8_MIN && imm <= I8_MAX) {
        x64_rex(buffer, 1, 0, 0, 0);
        buffer_write_u8(buffer, 0x83);
        x64_modrm(buffer, 0b11, 5, r);
        buffer_write_u8(buffer, imm);
    } else {
        x64_rex(buffer, 1, 0, 0, 0);
        buffer_write_u8(buffer, 0x81);
        x64_modrm(buffer, 0b11, 5, r);
        buffer_write_u32(buffer, *(u32 *)&imm);
    }
}

static void x64_add_immediate(Buffer & buffer, u8 r, i32 imm) {
    if (imm >= I8_MIN && imm <= I8_MAX) {
        x64_rex(buffer, 1, 0, 0, 0);
        buffer_write_u8(buffer, 0x83);
        x64_modrm(buffer, 0b11, 0, r);
        buffer_write_u8(buffer, imm);
    } else {
        x64_rex(buffer, 1, 0, 0, 0);
        buffer_write_u8(buffer, 0x81);
        x64_modrm(buffer, 0b11, 0, r);
        buffer_write_u32(buffer, imm);
    }
}

static void x64_mov_r_ro(Buffer & buffer, u8 size, u8 rd, i32 offset, u8 rs) {
    // Extract the REX prefix values for R and B, and mask it out of the arguments
    u8 r = ((rs & 0b1000) >> 3);
    rs = (rs & ~0b1000);

    u8 b = ((rd & 0b1000) >> 3);
    rd = (rd & ~0b1000);

    if (size == 1) {
        x64_rex(buffer, 0, r, 0, b);
        buffer_write_u8(buffer, 0x88);
    } else if (size == 2) {
        if (r != 0 || b != 0) {
            x64_rex(buffer, 0, r, 0, b);
        }
        buffer_write_u8(buffer, 0x66);
        buffer_write_u8(buffer, 0x89);
    } else if (size == 4) {
        if (r != 0 || b != 0) {
            x64_rex(buffer, 0, r, 0, b);
        }
        buffer_write_u8(buffer, 0x89);
    } else if (size == 8) {
        x64_rex(buffer, 1, r, 0, b);
        buffer_write_u8(buffer, 0x89);
    }

    u8 mod = 0b00;
    if (offset >= I8_MIN && offset <= I8_MAX) {
        mod = 0b01;
    } else {
        mod = 0b10;
    }

    if (rd == esp) {
        x64_modrm(buffer, mod, rs, rd);
        if (offset >= I8_MIN && offset <= I8_MAX) {
            x64_sib(buffer, 0b00, 0b100, esp);
        } else {
            x64_sib(buffer, 0b10, 0b100, esp);
        }
    } else {
        x64_modrm(buffer, mod, rs, rd);
    }

    if (mod == 0b01) {
        buffer_write_u8(buffer, offset & 0xFFFFFFFF);
    } else if (mod == 0b10) {
        buffer_write_u32(buffer, *(u32 *)&offset);
    }
}

static void x64_mov_ro_r(Buffer & buffer, u8 size, u8 rd, i32 offset, u8 rs) {
    if (size == 1) {
        x64_rex(buffer, 0, 0, 0, 0);
        buffer_write_u8(buffer, 0x8a);
    } else if (size == 2) {
        buffer_write_u8(buffer, 0x66);
        buffer_write_u8(buffer, 0x8b);
    } else if (size == 4) {
        buffer_write_u8(buffer, 0x8b);
    } else if (size == 8) {
        x64_rex(buffer, 1, 0, 0, 0);
        buffer_write_u8(buffer, 0x8b);
    }

    u8 mod = 0b00;
    if (offset >= I8_MIN && offset <= I8_MAX) {
        mod = 0b01;
    } else {
        mod = 0b10;
    }

    if (rd == rsp) {
        x64_modrm(buffer, mod, rs, rd);
        if (offset >= I8_MIN && offset <= I8_MAX) {
            x64_sib(buffer, 0b00, 0b100, esp);
        } else {
            x64_sib(buffer, 0b10, 0b100, esp);
        }
    } else {
        x64_modrm(buffer, mod, rs, rd);
    }

    if (mod == 0b01) {
        buffer_write_u8(buffer, offset & 0xFFFFFFFF);
    } else if (mod == 0b10) {
        buffer_write_u32(buffer, *(u32 *)&offset);
    }
}

static void x64_mov_ror_r(Buffer & buffer, u8 size, u8 rb, u8 ri, u8 rt) {
    if (size == 1) {
        x64_rex(buffer, 0, 0, 0, 0);
        buffer_write_u8(buffer, 0x8a);
    } else if (size == 2) {
        buffer_write_u8(buffer, 0x66);
        buffer_write_u8(buffer, 0x8b);
    } else if (size == 4) {
        buffer_write_u8(buffer, 0x8b);
    } else if (size == 8) {
        x64_rex(buffer, 1, 0, 0, 0);
        buffer_write_u8(buffer, 0x8b);
    }

    x64_modrm(buffer, 0b00, rt, 0b100);
    x64_sib(buffer, 0b00, ri, rb);
}

static void x64_mov_r_ror(Buffer & buffer, u8 size, u8 rb, u8 ri, u8 rt) {
    if (size == 1) {
        x64_rex(buffer, 0, 0, 0, 0);
        buffer_write_u8(buffer, 0x88);
    } else if (size == 2) {
        buffer_write_u8(buffer, 0x66);
        buffer_write_u8(buffer, 0x89);
    } else if (size == 4) {
        buffer_write_u8(buffer, 0x89);
    } else if (size == 8) {
        x64_rex(buffer, 1, 0, 0, 0);
        buffer_write_u8(buffer, 0x89);
    }

    x64_modrm(buffer, 0b00, rt, 0b100);
    x64_sib(buffer, 0b00, ri, rb);
}

static void x64_movabs_64(Buffer & buffer, u8 rd, u64 immediate) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0xb8 + rd);
    buffer_write_u64(buffer, immediate);
}

static void x64_add_r_r(Buffer & buffer, u8 rd, u8 rs) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0x03);
    x64_modrm(buffer, 0b11, rd, rs);
}

static void x64_imul_r_r(Buffer & buffer, u8 rd, u8 rs) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0x0f);
    buffer_write_u8(buffer, 0xaf);
    x64_modrm(buffer, 0b11, rd, rs);
}

static void x64_sub_r_r(Buffer & buffer, u8 rd, u8 rs) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0x2b);
    x64_modrm(buffer, 0b11, rd, rs);
}

static void x64_xor_r_r(Buffer & buffer, u8 rd, u8 rs) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0x33);
    x64_modrm(buffer, 0b11, rd, rs);
}

static void x64_div_r(Buffer & buffer, u8 rs) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0xf7);
    x64_modrm(buffer, 0b11, 6, rs);
}

static void x64_idiv_r(Buffer & buffer, u8 rs) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0xf7);
    x64_modrm(buffer, 0b11, 7, rs);
}

static void x64_cqo(Buffer & buffer) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0x99);
}

static void x64_and_r_r(Buffer & buffer, u8 rd, u8 rs) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0x23);
    x64_modrm(buffer, 0b11, rd, rs);
}

static void x64_and_r_ib(Buffer & buffer, u8 rd, u8 ib) {
    if (rd == rax) {
        buffer_write_u8(buffer, 0x24);
        buffer_write_u8(buffer, ib);
    } else {
        // REX + 80 /4 ib
        ASSERT_NOT_REACHED();
    }
}

static void x64_or_r_r(Buffer & buffer, u8 rd, u8 rs) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0x0b);
    x64_modrm(buffer, 0b11, rd, rs);
}

static void x64_shl_r_cl(Buffer & buffer, u8 rd) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0xd3);
    x64_modrm(buffer, 0b11, 4, rd);
}

static void x64_shr_r_cl(Buffer & buffer, u8 rd) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0xd3);
    x64_modrm(buffer, 0b11, 5, rd);
}

static void x64_xor_r_ib(Buffer & buffer, u8 rd, i8 ib) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0x83);
    x64_modrm(buffer, 0b11, 6, rd);
    buffer_write_u8(buffer, *(u8 *)&ib);
}

static void x64_cmp_r_r(Buffer & buffer, u8 size, u8 rd, u8 rs) {
    if (size == 1) {
        x64_rex(buffer, 0, 0, 0, 0);
        buffer_write_u8(buffer, 0x38);
        x64_modrm(buffer, 0b11, rs, rd);
    } else if (size == 2) {
        buffer_write_u8(buffer, 0x66);
        buffer_write_u8(buffer, 0x39);
        x64_modrm(buffer, 0b11, rs, rd);
    } else if (size == 4) {
        buffer_write_u8(buffer, 0x39);
        x64_modrm(buffer, 0b11, rs, rd);
    } else if (size == 8) {
        x64_rex(buffer, 1, 0, 0, 0);
        buffer_write_u8(buffer, 0x39);
        x64_modrm(buffer, 0b11, rs, rd);
    } else {
        ASSERT_NOT_REACHED();
    }
}

static void x64_cmp_r_i_8(Buffer & buffer, u8 r, i8 imm) {
    x64_rex(buffer, 0, 0, 0, 0);
    buffer_write_u8(buffer, 0x80);
    x64_modrm(buffer, 0b11, 7, r);
    ASSERT(r != esp);
    buffer_write_u8(buffer, *(u8 *)&imm);
}

static void x64_setb_r(Buffer & buffer, u8 rs) {
    x64_rex(buffer, 0, 0, 0, 0);
    buffer_write_u8(buffer, 0x0f);
    buffer_write_u8(buffer, 0x92);
    x64_modrm(buffer, 0b11, 0, rs);
}

static void x64_seta_r(Buffer & buffer, u8 rs) {
    x64_rex(buffer, 0, 0, 0, 0);
    buffer_write_u8(buffer, 0x0f);
    buffer_write_u8(buffer, 0x97);
    x64_modrm(buffer, 0b11, 0, rs);
}

static void x64_setbe_r(Buffer & buffer, u8 rs) {
    x64_rex(buffer, 0, 0, 0, 0);
    buffer_write_u8(buffer, 0x0f);
    buffer_write_u8(buffer, 0x96);
    x64_modrm(buffer, 0b11, 0, rs);
}

static void x64_setae_r(Buffer & buffer, u8 rs) {
    x64_rex(buffer, 0, 0, 0, 0);
    buffer_write_u8(buffer, 0x0f);
    buffer_write_u8(buffer, 0x93);
    x64_modrm(buffer, 0b11, 0, rs);
}

static void x64_setl_r(Buffer & buffer, u8 rs) {
    x64_rex(buffer, 0, 0, 0, 0);
    buffer_write_u8(buffer, 0x0f);
    buffer_write_u8(buffer, 0x9c);
    x64_modrm(buffer, 0b11, 0, rs);
}

static void x64_setg_r(Buffer & buffer, u8 rs) {
    x64_rex(buffer, 0, 0, 0, 0);
    buffer_write_u8(buffer, 0x0f);
    buffer_write_u8(buffer, 0x9f);
    x64_modrm(buffer, 0b11, 0, rs);
}

static void x64_setle_r(Buffer & buffer, u8 rs) {
    x64_rex(buffer, 0, 0, 0, 0);
    buffer_write_u8(buffer, 0x0f);
    buffer_write_u8(buffer, 0x9e);
    x64_modrm(buffer, 0b11, 0, rs);
}

static void x64_setge_r(Buffer & buffer, u8 rs) {
    x64_rex(buffer, 0, 0, 0, 0);
    buffer_write_u8(buffer, 0x0f);
    buffer_write_u8(buffer, 0x9d);
    x64_modrm(buffer, 0b11, 0, rs);
}

static void x64_sete_r(Buffer & buffer, u8 rs) {
    x64_rex(buffer, 0, 0, 0, 0);
    buffer_write_u8(buffer, 0x0f);
    buffer_write_u8(buffer, 0x94);
    x64_modrm(buffer, 0b11, 0, rs);
}

static void x64_setne_r(Buffer & buffer, u8 rs) {
    x64_rex(buffer, 0, 0, 0, 0);
    buffer_write_u8(buffer, 0x0f);
    buffer_write_u8(buffer, 0x95);
    x64_modrm(buffer, 0b11, 0, rs);
}

static void x64_jz(Buffer & buffer, u32 offset) {
    buffer_write_u8(buffer, 0x0f);
    buffer_write_u8(buffer, 0x84);
    buffer_write_u32(buffer, offset);
}

static void x64_jnz(Buffer & buffer, u32 offset) {
    buffer_write_u8(buffer, 0x0f);
    buffer_write_u8(buffer, 0x85);
    buffer_write_u32(buffer, offset);
}

static void x64_jmp(Buffer & buffer, u32 offset) {
    buffer_write_u8(buffer, 0xe9);
    buffer_write_u32(buffer, offset);
}

static void x64_test_r_i_8(Buffer & buffer, u8 r, u8 imm) {
    buffer_write_u8(buffer, 0xf6);
    x64_modrm(buffer, 0b11, 0, r);
    buffer_write_u8(buffer, imm);
}

static void x64_callq(Buffer & buffer, u32 offset) {
    buffer_write_u8(buffer, 0xe8);
    buffer_write_u32(buffer, offset);
}

static void x64_movq(Buffer & buffer, u8 r, u32 offset) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0x8b);
    x64_modrm(buffer, 0b00, r, 0b101);
    buffer_write_u32(buffer, offset);
}

static void x64_leaq(Buffer & buffer, u8 r, u32 offset) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0x8d);
    x64_modrm(buffer, 0b00, r, 0b101);
    buffer_write_u32(buffer, offset);
}

static void x64_lea_rsp(Buffer & buffer, u8 r, u32 offset) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0x8d);
    x64_modrm(buffer, 0b01, r, rsp);
    if (offset < 0xFF) {
        x64_sib(buffer, 0b00, 0b100, esp);
        buffer_write_u8(buffer, offset & 0b11111111);
    } else {
        x64_sib(buffer, 0b10, 0b100, esp);
        buffer_write_u32(buffer, offset);
    }
}

static void x64_movsx_8_to_64(Buffer & buffer, u8 source_register, i32 source_offset, u8 destination_register) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0x0f);
    buffer_write_u8(buffer, 0xbe);

    u8 mod = 0b00;
    if (source_offset == 0) {
        mod = 0b00;
    } else if (source_offset >= I8_MIN && source_offset <= I8_MAX) {
        mod = 0b01;
    } else {
        mod = 0b10;
    }

    x64_modrm(buffer, mod, destination_register, source_register);

    if (source_register == esp) {
        x64_sib(buffer, 0b00, 0b100, source_register);
    }

    if (source_offset == 0) {
    } else if (source_offset >= I8_MIN && source_offset <= I8_MAX) {
        buffer_write_u8(buffer, source_offset & 0b11111111);
    } else {
        buffer_write_u32(buffer, *(i32 *)&source_offset);
    }
}

static void x64_movsx_16_to_64(Buffer & buffer, u8 source_register, i32 source_offset, u8 destination_register) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0x0f);
    buffer_write_u8(buffer, 0xbf);

    u8 mod = 0b00;
    if (source_offset == 0) {
        mod = 0b00;
    } else if (source_offset >= I8_MIN && source_offset <= I8_MAX) {
        mod = 0b01;
    } else {
        mod = 0b10;
    }

    x64_modrm(buffer, mod, destination_register, source_register);

    if (source_register == esp) {
        x64_sib(buffer, 0b00, 0b100, source_register);
    }

    if (source_offset == 0) {
    } else if (source_offset >= I8_MIN && source_offset <= I8_MAX) {
        buffer_write_u8(buffer, source_offset & 0b11111111);
    } else {
        buffer_write_u32(buffer, *(i32 *)&source_offset);
    }
}

static void x64_movzx_8_to_64(Buffer & buffer, u8 source_register, i32 source_offset, u8 destination_register) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0x0f);
    buffer_write_u8(buffer, 0xb6);

    u8 mod = 0b00;
    if (source_offset == 0) {
        mod = 0b00;
    } else if (source_offset >= I8_MIN && source_offset <= I8_MAX) {
        mod = 0b01;
    } else {
        mod = 0b10;
    }

    x64_modrm(buffer, mod, destination_register, source_register);

    if (source_register == esp) {
        x64_sib(buffer, 0b00, 0b100, source_register);
    }

    if (source_offset == 0) {
    } else if (source_offset >= I8_MIN && source_offset <= I8_MAX) {
        buffer_write_u8(buffer, source_offset & 0b11111111);
    } else {
        buffer_write_u32(buffer, *(i32 *)&source_offset);
    }
}

static void x64_movzx_16_to_64(Buffer & buffer, u8 source_register, i32 source_offset, u8 destination_register) {
    x64_rex(buffer, 1, 0, 0, 0);
    buffer_write_u8(buffer, 0x0f);
    buffer_write_u8(buffer, 0xb7);

    u8 mod = 0b00;
    if (source_offset == 0) {
        mod = 0b00;
    } else if (source_offset >= I8_MIN && source_offset <= I8_MAX) {
        mod = 0b01;
    } else {
        mod = 0b10;
    }

    x64_modrm(buffer, mod, destination_register, source_register);

    if (source_register == esp) {
        x64_sib(buffer, 0b00, 0b100, source_register);
    }

    if (source_offset == 0) {
    } else if (source_offset >= I8_MIN && source_offset <= I8_MAX) {
        buffer_write_u8(buffer, source_offset & 0b11111111);
    } else {
        buffer_write_u32(buffer, *(i32 *)&source_offset);
    }
}

static void x64_mov_32_to_32(Buffer & buffer, u8 source_register, i32 source_offset, u8 destination_register) {
    buffer_write_u8(buffer, 0x8b);

    u8 mod = 0b00;
    if (source_offset == 0) {
        mod = 0b00;
    } else if (source_offset >= I8_MIN && source_offset <= I8_MAX) {
        mod = 0b01;
    } else {
        mod = 0b10;
    }

    x64_modrm(buffer, mod, destination_register, source_register);

    if (source_register == esp) {
        x64_sib(buffer, 0b00, 0b100, source_register);
    }

    if (source_offset == 0) {
    } else if (source_offset >= I8_MIN && source_offset <= I8_MAX) {
        buffer_write_u8(buffer, source_offset & 0b11111111);
    } else {
        buffer_write_u32(buffer, *(i32 *)&source_offset);
    }
}

static void x64_mov_64_to_64(Buffer & buffer, u8 source_register, i32 source_offset, u8 destination_register) {
    // Extract the REX prefix values for R and B, and mask it out of the arguments
    u8 r = ((destination_register & 0b1000) >> 3);
    destination_register = (destination_register & ~0b1000);

    u8 b = ((source_register & 0b1000) >> 3);
    source_register = (source_register & ~0b1000);

    x64_rex(buffer, 1, r, 0, b);
    buffer_write_u8(buffer, 0x8b);

    u8 mod = 0b00;
    if (source_offset == 0) {
        mod = 0b00;
    } else if (source_offset >= I8_MIN && source_offset <= I8_MAX) {
        mod = 0b01;
    } else {
        mod = 0b10;
    }

    x64_modrm(buffer, mod, destination_register, source_register);

    if (source_register == esp) {
        x64_sib(buffer, 0b00, 0b100, source_register);
    }

    if (source_offset == 0) {
    } else if (source_offset >= I8_MIN && source_offset <= I8_MAX) {
        buffer_write_u8(buffer, source_offset & 0b11111111);
    } else {
        buffer_write_u32(buffer, *(i32 *)&source_offset);
    }
}


