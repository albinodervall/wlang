#import "library/assert.w"

main :: () #entry {
    u8_maximum: u8 = 255;
    assert(u8_maximum == 255);

    i8_minimum: i8 = -128;
    assert(i8_minimum == -128);
    i8_maximum: i8 = 127;
    assert(i8_maximum == 127);

    u16_minimum: u16 = 0;
    assert(u16_minimum == 0);
    u16_maximum: u16 = 65535;
    assert(u16_maximum == 65535);

    i16_minimum: i16 = -32768;
    assert(i16_minimum == -32768);
    i16_maximum: i16 = 32767;
    assert(i16_maximum == 32767);

    u32_maximum: u32 = 4294967295;
    assert(u32_maximum == 4294967295);

    i32_minimum: i32 = -2147483648;
    assert(i32_minimum == -2147483648);
    i32_maximum: i32 = 2147483647;
    assert(i32_maximum == 2147483647);

    u64_maximum: u64 = 18446744073709551615;
    assert(u64_maximum == 18446744073709551615);

    i64_minimum: i64 = -9223372036854775808;
    assert(i64_minimum == -9223372036854775808);
    i64_maximum: i64 = 9223372036854775807;
    assert(i64_maximum == 9223372036854775807);

    print("Ok\n");
}
