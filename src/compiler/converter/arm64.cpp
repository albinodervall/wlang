static u32 const x0 = 0;
static u32 const x1 = 1;
static u32 const x2 = 2;
static u32 const x3 = 3;
static u32 const x4 = 4;
static u32 const x5 = 5;
static u32 const x6 = 6;
static u32 const x7 = 7;
static u32 const x8 = 8;
static u32 const x9 = 9;
static u32 const x10 = 10;
static u32 const x11 = 11;
static u32 const x12 = 12;
static u32 const x13 = 13;
static u32 const x14 = 14;
static u32 const x15 = 15;
static u32 const x16 = 16;
static u32 const x17 = 17;
static u32 const x18 = 18;
static u32 const x19 = 19;
static u32 const x20 = 20;
static u32 const x21 = 21;
static u32 const x22 = 22;
static u32 const x23 = 23;
static u32 const x24 = 24;
static u32 const x25 = 25;
static u32 const x26 = 26;
static u32 const x27 = 27;
static u32 const x28 = 28;
static u32 const x29 = 29;
static u32 const x30 = 30;
static u32 const x31 = 31;
static u32 const xzr = 31;

static u32 const w0 = 0;
static u32 const w1 = 1;
static u32 const w2 = 2;
static u32 const w3 = 3;
static u32 const w4 = 4;
static u32 const w5 = 5;
static u32 const w6 = 6;
static u32 const w7 = 7;
static u32 const w8 = 8;
static u32 const w9 = 9;
static u32 const w10 = 10;
static u32 const w11 = 11;
static u32 const w12 = 12;
static u32 const w13 = 13;
static u32 const w14 = 14;
static u32 const w15 = 15;
static u32 const w16 = 16;
static u32 const w17 = 17;
static u32 const w18 = 18;
static u32 const w19 = 19;
static u32 const w20 = 20;
static u32 const w21 = 21;
static u32 const w22 = 22;
static u32 const w23 = 23;
static u32 const w24 = 24;
static u32 const w25 = 25;
static u32 const w26 = 26;
static u32 const w27 = 27;
static u32 const w28 = 28;
static u32 const w29 = 29;
static u32 const w30 = 30;
static u32 const w31 = 31;
static u32 const wzr = 31;

static u32 const sp = 31;

// https://developer.arm.com/documentation/den0024/a/The-A64-instruction-set/Data-processing-instructions/Conditional-instructions?lang=en#CHDGDBHJ
static u32 const eq = 0b0000;
static u32 const ne = 0b0001;
static u32 const cs = 0b0010;
static u32 const hs = 0b0010;
static u32 const cc = 0b0011;
static u32 const lo = 0b0011;
static u32 const mi = 0b0100;
static u32 const pl = 0b0101;
static u32 const vs = 0b0110;
static u32 const vc = 0b0111;
static u32 const hi = 0b1000;
static u32 const ls = 0b1001;
static u32 const ge = 0b1010;
static u32 const lt = 0b1011;
static u32 const gt = 0b1100;
static u32 const le = 0b1101;
static u32 const al = 0b1110;
static u32 const nv = 0b1111;

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/MOV--wide-immediate---Move--wide-immediate---an-alias-of-MOVZ-
static void arm64_mov(Buffer & buffer, u32 sf, u32 hw, u16 imm16, u16 Rd) {
    u32 instruction = 0b01010010100000000000000000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((hw & ~0b11) == 0);
    instruction |= ((hw & 0b11) << 21);
    ASSERT((imm16 & ~0b1111111111111111) == 0);
    instruction |= ((imm16 & 0b1111111111111111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/MOVZ--Move-wide-with-zero-?lang=en
static void arm64_movz(Buffer & buffer, u32 sf, u32 hw, u32 imm16, u32 Rd) {
    u32 instruction = 0b01010010100000000000000000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((hw & ~0b11) == 0);
    instruction |= ((hw & 0b11) << 21);
    ASSERT((imm16 & ~0b1111111111111111) == 0);
    instruction |= ((imm16 & 0b1111111111111111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/MOVN--Move-wide-with-NOT-?lang=en
static void arm64_movn(Buffer & buffer, u32 sf, u32 hw, u32 imm16, u32 Rd) {
    u32 instruction = 0b00010010100000000000000000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((hw & ~0b11) == 0);
    instruction |= ((hw & 0b11) << 21);
    ASSERT((imm16 & ~0b1111111111111111) == 0);
    instruction |= ((imm16 & 0b1111111111111111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/MOVK--Move-wide-with-keep-?lang=en
static void arm64_movk(Buffer & buffer, u32 sf, u32 hw, u32 imm16, u32 Rd) {
    u32 instruction = 0b01110010100000000000000000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((hw & ~0b11) == 0);
    instruction |= ((hw & 0b11) << 21);
    ASSERT((imm16 & ~0b1111111111111111) == 0);
    instruction |= ((imm16 & 0b1111111111111111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/RET--Return-from-subroutine-
static void arm64_ret(Buffer & buffer, u32 Rn) {
    u32 instruction = 0b11010110010111110000000000000000;
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2021-09/Base-Instructions/SUB--immediate---Subtract--immediate--?lang=en
static void arm64_sub_immediate(Buffer & buffer, u32 sf, u32 sh, u32 imm12, u32 Rn, u32 Rd) {
    u32 instruction = 0b01010001000000000000000000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((sh & ~0b1) == 0);
    instruction |= ((sh & 0b1) << 22);
    ASSERT((imm12 & ~0b111111111111) == 0);
    instruction |= ((imm12 & 0b111111111111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/ADD--extended-register---Add--extended-register--
static void arm64_add_extended_register(Buffer & buffer, u32 sf, u32 Rm, u32 option, u32 imm3, u32 Rn, u32 Rd) {
    u32 instruction = 0b00001011001000000000000000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((option & ~0b111) == 0);
    instruction |= ((option & 0b111) << 13);
    ASSERT((imm3 & ~0b111) == 0);
    instruction |= ((imm3 & 0b111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/ADD--shifted-register---Add--shifted-register--?lang=en
static void arm64_add_shifted_register(Buffer & buffer, u32 sf, u32 shift, u32 Rm, u32 imm6, u32 Rn, u32 Rd) {
    u32 instruction = 0b00001011000000000000000000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((shift & ~0b11) == 0);
    instruction |= ((shift & 0b11) << 22);
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((imm6 & ~0b111111) == 0);
    instruction |= ((imm6 & 0b111111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/ORR--shifted-register---Bitwise-OR--shifted-register--
static void arm64_orr_shifted_register(Buffer & buffer, u32 sf, u32 shift, u32 Rm, u32 imm6, u32 Rn, u32 Rd) {
    u32 instruction = 0b00101010000000000000000000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((shift & ~0b11) == 0);
    instruction |= ((shift & 0b11) << 22);
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((imm6 & ~0b111111) == 0);
    instruction |= ((imm6 & 0b111111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/EOR--shifted-register---Bitwise-Exclusive-OR--shifted-register--
static void arm64_eor_shifted_register(Buffer & buffer, u32 sf, u32 shift, u32 Rm, u32 imm6, u32 Rn, u32 Rd) {
    u32 instruction = 0b01001010000000000000000000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((shift & ~0b11) == 0);
    instruction |= ((shift & 0b11) << 22);
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((imm6 & ~0b111111) == 0);
    instruction |= ((imm6 & 0b111111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/MUL--Multiply--an-alias-of-MADD-
static void arm64_mul(Buffer & buffer, u32 sf, u32 Rm, u32 Rn, u32 Rd) {
    u32 instruction = 0b00011011000000000111110000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/SDIV--Signed-Divide-
static void arm64_sdiv(Buffer & buffer, u32 sf, u32 Rm, u32 Rn, u32 Rd) {
    u32 instruction = 0b00011010110000000000110000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/SDIV--Signed-Divide-
static void arm64_udiv(Buffer & buffer, u32 sf, u32 Rm, u32 Rn, u32 Rd) {
    u32 instruction = 0b00011010110000000000100000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/MVN--Bitwise-NOT--an-alias-of-ORN--shifted-register--
static void arm64_mvn(Buffer & buffer, u32 sf, u32 shift, u32 Rm, u32 imm6, u32 Rd) {
    u32 instruction = 0b00101010001000000000001111100000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((shift & ~0b11) == 0);
    instruction |= ((shift & 0b11) << 22);
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((imm6 & ~0b111111) == 0);
    instruction |= ((imm6 & 0b111111) << 10);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/AND--shifted-register---Bitwise-AND--shifted-register--
static void arm64_and_shifted_register(Buffer & buffer, u32 sf, u32 shift, u32 Rm, u32 imm6, u32 Rn, u32 Rd) {
    u32 instruction = 0b00001010000000000000000000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((shift & ~0b11) == 0);
    instruction |= ((shift & 0b11) << 22);
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((imm6 & ~0b111111) == 0);
    instruction |= ((imm6 & 0b111111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LSL--register---Logical-Shift-Left--register---an-alias-of-LSLV-
static void arm64_lsl_register(Buffer & buffer, u32 sf, u32 Rm, u32 Rn, u32 Rd) {
    u32 instruction = 0b00011010110000000010000000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/ASR--register---Arithmetic-Shift-Right--register---an-alias-of-ASRV-
static void arm64_asr_register(Buffer & buffer, u32 sf, u32 Rm, u32 Rn, u32 Rd) {
    u32 instruction = 0b00011010110000000010100000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/ASR--register---Arithmetic-Shift-Right--register---an-alias-of-ASRV-
static void arm64_lsr_register(Buffer & buffer, u32 sf, u32 Rm, u32 Rn, u32 Rd) {
    u32 instruction = 0b00011010110000000010010000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/CMP--extended-register---Compare--extended-register---an-alias-of-SUBS--extended-register--
static void arm64_cmp_extended_register(Buffer & buffer, u32 sf, u32 Rm, u32 option, u32 imm3, u32 Rn) {
    u32 instruction = 0b01101011001000000000000000011111;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((option & ~0b111) == 0);
    instruction |= ((option & 0b111) << 13);
    ASSERT((imm3 & ~0b111) == 0);
    instruction |= ((imm3 & 0b111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/CSET--Conditional-Set--an-alias-of-CSINC-
static void arm64_cset(Buffer & buffer, u32 sf, u32 cond, u32 Rd) {
    u32 instruction = 0b00011010100111110000011111100000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    // Is one of the standard conditions, excluding AL and NV, encoded in the "cond" field with its least significant bit inverted.
    ASSERT((cond & ~0b1111) == 0);
    ASSERT(cond != al && cond != nv);
    instruction |= (((cond ^ 0b0001) & 0b1111) << 12);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/SUB--extended-register---Subtract--extended-register--
static void arm64_sub_extended_register(Buffer & buffer, u32 sf, u32 Rm, u32 option, u32 imm3, u32 Rn, u32 Rd) {
    u32 instruction = 0b01001011001000000000000000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((option & ~0b111) == 0);
    instruction |= ((option & 0b111) << 13);
    ASSERT((imm3 & ~0b111) == 0);
    instruction |= ((imm3 & 0b111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/SUB--shifted-register---Subtract--shifted-register--?lang=en
static void arm64_sub_shifted_register(Buffer & buffer, u32 sf, u32 shift, u32 Rm, u32 imm6, u32 Rn, u32 Rd) {
    u32 instruction = 0b01001011000000000000000000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((shift & ~0b11) == 0);
    instruction |= ((shift & 0b11) << 22);
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((imm6 & ~0b111111) == 0);
    instruction |= ((imm6 & 0b111111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/SUBS--extended-register---Subtract--extended-register---setting-flags-?lang=en
static void arm64_subs_extended_register(Buffer & buffer, u32 sf, u32 Rm, u32 option, u32 imm3, u32 Rn, u32 Rd) {
    u32 instruction = 0b01101011001000000000000000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((option & ~0b111) == 0);
    instruction |= ((option & 0b111) << 13);
    ASSERT((imm3 & ~0b111) == 0);
    instruction |= ((imm3 & 0b111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/SUBS--shifted-register---Subtract--shifted-register---setting-flags-?lang=en
static void arm64_subs_shifted_register(Buffer & buffer, u32 sf, u32 shift, u32 Rm, u32 imm6, u32 Rn, u32 Rd) {
    u32 instruction = 0b01101011000000000000000000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((shift & ~0b11) == 0);
    instruction |= ((shift & 0b11) << 22);
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((imm6 & ~0b111111) == 0);
    instruction |= ((imm6 & 0b111111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/STP--Store-Pair-of-Registers-?lang=en#iclass_post_indexed
static void arm64_stp_64_post_index(Buffer & buffer, i32 imm7, u32 Rt2, u32 Rn, u32 Rt) {
    u32 instruction = 0b10101000100000000000000000000000;
    ASSERT(imm7 >= -64 && imm7 <= 63);
    instruction |= ((imm7 & 0b1111111) << 15);
    ASSERT((Rt2 & ~0b11111) == 0);
    instruction |= ((Rt2 & 0b11111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/STP--Store-Pair-of-Registers-?lang=en#iclass_pre_indexed
static void arm64_stp_64_pre_index(Buffer & buffer, i32 imm7, u32 Rt2, u32 Rn, u32 Rt) {
    u32 instruction = 0b10101001100000000000000000000000;
    ASSERT(imm7 >= -64 && imm7 <= 63);
    instruction |= ((imm7 & 0b1111111) << 15);
    ASSERT((Rt2 & ~0b11111) == 0);
    instruction |= ((Rt2 & 0b11111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/STP--Store-Pair-of-Registers-?lang=en#iclass_signed_scaled_offset
static void arm64_stp_64_signed_offset(Buffer & buffer, i32 imm7, u32 Rt2, u32 Rn, u32 Rt) {
    u32 instruction = 0b10101001000000000000000000000000;
    ASSERT(imm7 >= -64 && imm7 <= 63);
    instruction |= ((imm7 & 0b1111111) << 15);
    ASSERT((Rt2 & ~0b11111) == 0);
    instruction |= ((Rt2 & 0b11111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/ADD--immediate---Add--immediate--?lang=en
static void arm64_add_immediate(Buffer & buffer, u32 sf, u32 sh, u32 imm12, u32 Rn, u32 Rd) {
    u32 instruction = 0b00010001000000000000000000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((sh & ~0b1) == 0);
    instruction |= ((sh & 0b1) << 22);
    ASSERT((imm12 & ~0b111111111111) == 0);
    instruction |= ((imm12 & 0b111111111111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/ADRP--Form-PC-relative-address-to-4KB-page-?lang=en
static void arm64_adrp(Buffer & buffer, i32 imm21, u32 Rd) {
    u32 instruction = 0b10010000000000000000000000000000;
    ASSERT(imm21 >= -2147483648 && imm21 <= 2147483647);
    ASSERT((imm21 & 0b111111111111) == 0); // imm21 must be a multiple of 4096
    instruction |= (((imm21 >> 12) & 0b11) << 29);
    instruction |= ((((imm21 >> 12) & 0b111111111111111111100) >> 2) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDP--Load-Pair-of-Registers-?lang=en#iclass_pre_indexed
static void arm64_ldp_64_pre_index(Buffer & buffer, u32 imm7, u32 Rt2, u32 Rn, u32 Rt) {
    u32 instruction = 0b10101001110000000000000000000000;
    ASSERT((imm7 & ~0b1111111) == 0);
    instruction |= ((imm7 & 0b1111111) << 15);
    ASSERT((Rt2 & ~0b11111) == 0);
    instruction |= ((Rt2 & 0b11111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDP--Load-Pair-of-Registers-?lang=en#iclass_post_indexed
static void arm64_ldp_64_post_index(Buffer & buffer, u32 imm7, u32 Rt2, u32 Rn, u32 Rt) {
    u32 instruction = 0b10101000110000000000000000000000;
    ASSERT((imm7 & ~0b1111111) == 0);
    instruction |= ((imm7 & 0b1111111) << 15);
    ASSERT((Rt2 & ~0b11111) == 0);
    instruction |= ((Rt2 & 0b11111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDP--Load-Pair-of-Registers-?lang=en#iclass_signed_scaled_offset
static void arm64_ldp_64_signed_offset(Buffer & buffer, u32 imm7, u32 Rt2, u32 Rn, u32 Rt) {
    u32 instruction = 0b10101001010000000000000000000000;
    ASSERT((imm7 & ~0b1111111) == 0);
    instruction |= ((imm7 & 0b1111111) << 15);
    ASSERT((Rt2 & ~0b11111) == 0);
    instruction |= ((Rt2 & 0b11111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDR--register---Load-Register--register--?lang=en
static void arm64_ldr_64_register(Buffer & buffer, u32 Rm, u32 option, u32 S, u32 Rn, u32 Rt) {
    u32 instruction = 0b11111000011000000000100000000000;
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((option & ~0b11111) == 0);
    instruction |= ((option & 0b11111) << 13);
    ASSERT((S & ~0b1) == 0);
    instruction |= ((S & 0b1) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDR--immediate---Load-Register--immediate--?lang=en
static void arm64_ldr_64_immediate_unsigned_offset(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt) {
    u32 instruction = 0b11111001010000000000000000000000;
    ASSERT((imm9 & 0b111) == 0); // imm9 must be a multiple of 8
    ASSERT((imm9 & ~0b111111111111111) == 0);
    instruction |= (((imm9 >> 3) & 0b111111111111) << 10);
    instruction |= ((Rn & 0b11111) << 5);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDR--immediate---Load-Register--immediate--?lang=en
static void arm64_ldr_32_immediate_unsigned_offset(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt) {
    u32 instruction = 0b10111001010000000000000000000000;
    ASSERT((imm9 & 0b11) == 0); // imm must be a multiple of 4
    ASSERT((imm9 & ~0b11111111111111) == 0);
    instruction |= (((imm9 >> 2) & 0b111111111111) << 10);
    instruction |= ((Rn & 0b11111) << 5);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/STR--register---Store-Register--register--?lang=en
static void arm64_str_64_register(Buffer & buffer, u32 Rm, u32 option, u32 S, u32 Rn, u32 Rt) {
    u32 instruction = 0b11111000001000000000100000000000;
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((option & ~0b11111) == 0);
    instruction |= ((option & 0b11111) << 13);
    ASSERT((S & ~0b1) == 0);
    instruction |= ((S & 0b1) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/STR--immediate---Store-Register--immediate--?lang=en#iclass_post_indexed
static void arm64_str_64_immediate_post_index(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt) {
    u32 instruction = 0b11111000000000000000010000000000;
    ASSERT((imm9 & ~0b111111111111) == 0);
    instruction |= ((imm9 & 0b111111111) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/STR--immediate---Store-Register--immediate--?lang=en#iclass_unsigned_scaled_offset
static void arm64_str_64_immediate_unsigned_offset(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt) {
    u32 instruction = 0b11111001000000000000000000000000;
    ASSERT((imm9 & 0b111) == 0); // imm must be a multiple of 8
    ASSERT((imm9 & ~0b111111111111111) == 0);
    instruction |= (((imm9 >> 3) & 0b111111111111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/STR--immediate---Store-Register--immediate--?lang=en#iclass_unsigned_scaled_offset
static void arm64_str_32_immediate_unsigned_offset(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt) {
    u32 instruction = 0b10111001000000000000000000000000;
    ASSERT((imm9 & 0b11) == 0); // imm must be a multiple of 4
    ASSERT((imm9 & ~0b11111111111111) == 0);
    instruction |= (((imm9 >> 2) & 0b111111111111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/STR--register---Store-Register--register--?lang=en
static void arm64_str_32_register(Buffer & buffer, u32 Rm, u32 option, u32 S, u32 Rn, u32 Rt) {
    u32 instruction = 0b10111000001000000000100000000000;
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((option & ~0b111) == 0);
    instruction |= ((option & 0b111) << 13);
    ASSERT((S & ~0b1) == 0);
    instruction |= ((S & 0b1) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDR--register---Load-Register--register--?lang=en
static void arm64_ldr_32_register(Buffer & buffer, u32 Rm, u32 option, u32 S, u32 Rn, u32 Rt) {
    u32 instruction = 0b10111000011000000000100000000000;
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((option & ~0b111) == 0);
    instruction |= ((option & 0b111) << 13);
    ASSERT((S & ~0b1) == 0);
    instruction |= ((S & 0b1) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/MOV--register---Move--register---an-alias-of-ORR--shifted-register--?lang=en
static void arm64_mov_register(Buffer & buffer, u32 sf, u32 Rm, u32 Rd) {
    u32 instruction = 0b00101010000000000000001111100000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/MOV--to-from-SP---Move-between-register-and-stack-pointer--an-alias-of-ADD--immediate--?lang=en
static void arm64_mov_to_from_sp(Buffer & buffer, u32 sf, u32 Rn, u32 Rd) {
    u32 instruction = 0b00010001000000000000000000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/BL--Branch-with-Link-?lang=en
static void arm64_bl(Buffer & buffer, i64 offset) {
    u32 instruction = 0b10010100000000000000000000000000;
    ASSERT((offset & ~0b11111111111111111111111111) == 0);
    instruction |= ((offset & 0b11111111111111111111111111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2021-09/Base-Instructions/BLR--Branch-with-Link-to-Register-?lang=en
static void arm64_blr(Buffer & buffer, u32 Rn) {
    u32 instruction = 0b11010110001111110000000000000000;
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/CBZ--Compare-and-Branch-on-Zero-?lang=en
static void arm64_cbz(Buffer & buffer, u32 sf, u32 imm19, u32 Rt) {
    u32 instruction = 0b00110100000000000000000000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((imm19 & ~0b1111111111111111111) == 0);
    instruction |= ((imm19 & 0b1111111111111111111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/CBNZ--Compare-and-Branch-on-Nonzero-?lang=en
static void arm64_cbnz(Buffer & buffer, u32 sf, u32 imm19, u32 Rt) {
    u32 instruction = 0b00110101000000000000000000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((imm19 & ~0b1111111111111111111) == 0);
    instruction |= ((imm19 & 0b1111111111111111111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/B--Branch-?lang=en
static void arm64_b(Buffer & buffer, u32 imm26) {
    u32 instruction = 0b00010100000000000000000000000000;
    ASSERT((imm26 & ~0b11111111111111111111111111) == 0);
    instruction |= ((imm26 & 0b11111111111111111111111111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/B-cond--Branch-conditionally-?lang=en
static void arm64_b_cond(Buffer & buffer, u32 imm19, u32 cond) {
    u32 instruction = 0b01010100000000000000000000000000;
    ASSERT((imm19 & ~0b1111111111111111111) == 0);
    instruction |= ((imm19 & 0b1111111111111111111) << 5);
    ASSERT((cond & ~0b1111) == 0);
    instruction |= ((cond & 0b1111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/STURH--Store-Register-Halfword--unscaled--?lang=en
static void arm64_sturh(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt) {
    u32 instruction = 0b01111000000000000000000000000000;
    ASSERT((imm9 & ~0b111111111) == 0);
    instruction |= ((imm9 & 0b111111111) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/STRH--immediate---Store-Register-Halfword--immediate--?lang=en#iclass_post_indexed
static void arm64_strh_post_index(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt)  {
    u32 instruction = 0b01111000000000000000010000000000;
    ASSERT((imm9 & ~0b111111111) == 0);
    instruction |= ((imm9 & 0b111111111) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/STRH--immediate---Store-Register-Halfword--immediate--?lang=en#iclass_pre_indexed
static void arm64_strh_pre_index(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt)  {
    u32 instruction = 0b01111000000000000000110000000000;
    ASSERT((imm9 & ~0b111111111) == 0);
    instruction |= ((imm9 & 0b111111111) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/STRH--immediate---Store-Register-Halfword--immediate--?lang=en#iclass_unsigned_scaled_offset
static void arm64_strh_unsigned_offset(Buffer & buffer, u32 imm12, u32 Rn, u32 Rt)  {
    u32 instruction = 0b01111001000000000000000000000000;
    ASSERT((imm12 & 0b1) == 0);
    ASSERT((imm12 & ~0b1111111111111) == 0);
    instruction |= (((imm12 >> 1) & 0b111111111111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDURH--Load-Register-Halfword--unscaled--?lang=en
static void arm64_ldurh(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt) {
    u32 instruction = 0b01111000010000000000000000000000;
    ASSERT((imm9 & ~0b111111111) == 0);
    instruction |= ((imm9 & 0b111111111) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDRH--immediate---Load-Register-Halfword--immediate--?lang=en#iclass_post_indexed
static void arm64_ldrh_post_index(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt)  {
    u32 instruction = 0b01111000010000000000100000000000;
    ASSERT((imm9 & ~0b111111111) == 0);
    instruction |= ((imm9 & 0b111111111) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDRH--immediate---Load-Register-Halfword--immediate--?lang=en#iclass_pre_indexed
static void arm64_ldrh_pre_index(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt)  {
    u32 instruction = 0b01111000010000000000110000000000;
    ASSERT((imm9 & ~0b111111111) == 0);
    instruction |= ((imm9 & 0b111111111) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDRH--immediate---Load-Register-Halfword--immediate--?lang=en#iclass_unsigned_scaled_offset
static void arm64_ldrh_unsigned_offset(Buffer & buffer, u32 imm12, u32 Rn, u32 Rt)  {
    u32 instruction = 0b01111001010000000000000000000000;
    ASSERT((imm12 & 0b1) == 0);
    ASSERT((imm12 & ~0b1111111111111) == 0);
    instruction |= (((imm12 >> 1) & 0b111111111111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDRSH--immediate---Load-Register-Signed-Halfword--immediate--?lang=en#iclass_post_indexed
static void arm64_ldrsh_64_post_index(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt)  {
    u32 instruction = 0b01111000100000000000010000000000;
    ASSERT((imm9 & ~0b111111111) == 0);
    instruction |= ((imm9 & 0b111111111) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDRSH--immediate---Load-Register-Signed-Halfword--immediate--?lang=en#iclass_pre_indexed
static void arm64_ldrsh_64_pre_index(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt)  {
    u32 instruction = 0b01111000100000000000110000000000;
    ASSERT((imm9 & ~0b111111111) == 0);
    instruction |= ((imm9 & 0b111111111) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDRSH--immediate---Load-Register-Signed-Halfword--immediate--?lang=en#iclass_unsigned_scaled_offset
static void arm64_ldrsh_64_unsigned_offset(Buffer & buffer, u32 imm12, u32 Rn, u32 Rt)  {
    u32 instruction = 0b01111001100000000000000000000000;
    ASSERT((imm12 & 0b1) == 0);
    ASSERT((imm12 & ~0b1111111111111) == 0);
    instruction |= (((imm12 >> 1) & 0b111111111111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/STURB--Store-Register-Byte--unscaled--?lang=en
static void arm64_sturb(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt) {
    u32 instruction = 0b00111000000000000000000000000000;
    ASSERT((imm9 & ~0b111111111) == 0);
    instruction |= ((imm9 & 0b111111111) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/STRB--immediate---Store-Register-Byte--immediate--?lang=en#iclass_post_indexed
static void arm64_strb_post_index(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt) {
    u32 instruction = 0b00111000000000000000010000000000;
    ASSERT((imm9 & ~0b111111111) == 0);
    instruction |= ((imm9 & 0b111111111) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/STRB--immediate---Store-Register-Byte--immediate--?lang=en#iclass_pre_indexed
static void arm64_strb_pre_index(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt) {
    u32 instruction = 0b00111000000000000000110000000000;
    ASSERT((imm9 & ~0b111111111) == 0);
    instruction |= ((imm9 & 0b111111111) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/STRB--immediate---Store-Register-Byte--immediate--?lang=en#iclass_unsigned_scaled_offset
static void arm64_strb_unsigned_offset(Buffer & buffer, u32 imm12, u32 Rn, u32 Rt) {
    u32 instruction = 0b00111001000000000000000000000000;
    ASSERT((imm12 & ~0b111111111111) == 0);
    instruction |= ((imm12 & 0b111111111111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDURB--Load-Register-Byte--unscaled--?lang=en
static void arm64_ldurb(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt) {
    u32 instruction = 0b00111000010000000000000000000000;
    ASSERT((imm9 & ~0b111111111) == 0);
    instruction |= ((imm9 & 0b111111111) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDRB--immediate---Load-Register-Byte--immediate--?lang=en#iclass_post_indexed
static void arm64_ldrb_post_index(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt) {
    u32 instruction = 0b00111000010000000000010000000000;
    ASSERT((imm9 & ~0b111111111) == 0);
    instruction |= ((imm9 & 0b111111111) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDRB--immediate---Load-Register-Byte--immediate--?lang=en#iclass_pre_indexed
static void arm64_ldrb_pre_index(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt) {
    u32 instruction = 0b00111000010000000000110000000000;
    ASSERT((imm9 & ~0b111111111) == 0);
    instruction |= ((imm9 & 0b111111111) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDRB--immediate---Load-Register-Byte--immediate--?lang=en#iclass_unsigned_scaled_offset
static void arm64_ldrb_unsigned_offset(Buffer & buffer, u32 imm12, u32 Rn, u32 Rt) {
    u32 instruction = 0b00111001010000000000000000000000;
    ASSERT((imm12 & ~0b111111111111) == 0);
    instruction |= ((imm12 & 0b111111111111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDRSB--immediate---Load-Register-Signed-Byte--immediate--?lang=en#iclass_post_indexed
static void arm64_ldrsb_64_post_index(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt) {
    u32 instruction = 0b00111000100000000000010000000000;
    ASSERT((imm9 & ~0b111111111) == 0);
    ASSERT((imm9 & ~0b111111111) == 0);
    instruction |= ((imm9 & 0b111111111) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDRSB--immediate---Load-Register-Signed-Byte--immediate--?lang=en#iclass_pre_indexed
static void arm64_ldrsb_64_pre_index(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt) {
    u32 instruction = 0b00111000100000000000110000000000;
    ASSERT((imm9 & ~0b111111111) == 0);
    instruction |= ((imm9 & 0b111111111) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDRSB--immediate---Load-Register-Signed-Byte--immediate--?lang=en#iclass_unsigned_scaled_offset
static void arm64_ldrsb_64_unsigned_offset(Buffer & buffer, u32 imm12, u32 Rn, u32 Rt) {
    u32 instruction = 0b00111001100000000000000000000000;
    ASSERT((imm12 & ~0b111111111111) == 0);
    instruction |= ((imm12 & 0b111111111111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDRSW--immediate---Load-Register-Signed-Word--immediate--?lang=en#iclass_post_indexed
static void arm64_ldrsw_post_index(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt) {
    u32 instruction = 0b10111000100000000000010000000000;
    ASSERT((imm9 & ~0b111111111) == 0);
    instruction |= ((imm9 & 0b111111111) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDRSW--immediate---Load-Register-Signed-Word--immediate--?lang=en#iclass_pre_indexed
static void arm64_ldrsw_pre_index(Buffer & buffer, u32 imm9, u32 Rn, u32 Rt) {
    u32 instruction = 0b10111000100000000000110000000000;
    ASSERT((imm9 & ~0b111111111) == 0);
    instruction |= ((imm9 & 0b111111111) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDRSW--immediate---Load-Register-Signed-Word--immediate--?lang=en#iclass_unsigned_scaled_offset
static void arm64_ldrsw_unsigned_offset(Buffer & buffer, u32 imm12, u32 Rn, u32 Rt) {
    u32 instruction = 0b10111001100000000000000000000000;
    ASSERT((imm12 & 0b11) == 0); // imm12 must be a multiple of 4
    ASSERT((imm12 & ~0b11111111111111) == 0);
    instruction |= (((imm12 >> 2) & 0b111111111111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/STRB--register---Store-Register-Byte--register--
static void arm64_strb_register(Buffer & buffer, u32 Rm, u32 option, u32 S, u32 Rn, u32 Rt) {
    u32 instruction = 0b00111000001000000000100000000000;
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((option & ~0b11111) == 0);
    instruction |= ((option & 0b11111) << 13);
    ASSERT((S & ~0b1) == 0);
    instruction |= ((S & 0b1) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/STRH--register---Store-Register-Halfword--register--
static void arm64_strh_register(Buffer & buffer, u32 Rm, u32 option, u32 S, u32 Rn, u32 Rt) {
    u32 instruction = 0b01111000001000000000100000000000;
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((option & ~0b11111) == 0);
    instruction |= ((option & 0b11111) << 13);
    ASSERT((S & ~0b1) == 0);
    instruction |= ((S & 0b1) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDRSB--register---Load-Register-Signed-Byte--register--?lang=en
static void arm64_ldrsb_64_register(Buffer & buffer, u32 Rm, u32 option, u32 S, u32 Rn, u32 Rt) {
    u32 instruction = 0b00111000101000000000100000000000;
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((option & ~0b111) == 0);
    instruction |= ((option & 0b111) << 13);
    ASSERT((S & ~0b1) == 0);
    instruction |= ((S & 0b1) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDRB--register---Load-Register-Byte--register--
static void arm64_ldrb_register(Buffer & buffer, u32 Rm, u32 option, u32 S, u32 Rn, u32 Rt) {
    u32 instruction = 0b00111000011000000000100000000000;
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((option & ~0b111) == 0);
    instruction |= ((option & 0b111) << 13);
    ASSERT((S & ~0b1) == 0);
    instruction |= ((S & 0b1) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDRSH--register---Load-Register-Signed-Halfword--register--
static void arm64_ldrsh_64_register(Buffer & buffer, u32 Rm, u32 option, u32 S, u32 Rn, u32 Rt) {
    u32 instruction = 0b01111000101000000000100000000000;
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((option & ~0b111) == 0);
    instruction |= ((option & 0b111) << 13);
    ASSERT((S & ~0b1) == 0);
    instruction |= ((S & 0b1) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/LDRH--immediate---Load-Register-Halfword--immediate--
static void arm64_ldrh_register(Buffer & buffer, u32 Rm, u32 option, u32 S, u32 Rn, u32 Rt) {
    u32 instruction = 0b01111000011000000000100000000000;
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((option & ~0b111) == 0);
    instruction |= ((option & 0b111) << 13);
    ASSERT((S & ~0b1) == 0);
    instruction |= ((S & 0b1) << 12);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rt & ~0b11111) == 0);
    instruction |= ((Rt & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}

// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/MADD--Multiply-Add-?lang=en
static void arm64_mad(Buffer & buffer, u32 sf, u32 Rm, u32 Ra, u32 Rn, u32 Rd) {
    u32 instruction = 0b00011011000000000000000000000000;
    ASSERT((sf & ~0b1) == 0);
    instruction |= ((sf & 0b1) << 31);
    ASSERT((Rm & ~0b11111) == 0);
    instruction |= ((Rm & 0b11111) << 16);
    ASSERT((Ra & ~0b11111) == 0);
    instruction |= ((Ra & 0b11111) << 10);
    ASSERT((Rn & ~0b11111) == 0);
    instruction |= ((Rn & 0b11111) << 5);
    ASSERT((Rd & ~0b11111) == 0);
    instruction |= ((Rd & 0b11111) << 0);

    buffer_write_u32(buffer, instruction);
}
