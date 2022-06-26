#import "library/assert.w"

main :: () #entry {
    a: u8 = 255;
    b: u16 = 65535;
    c: u32 = 4294967295;
    d: u64 = 18446744073709551615;
    e: i8 = 127;
    f: i16 = 32767;
    g: i32 = 2147483647;
    h: i64 = 9223372036854775807;

    assert(a == 255);
    assert(b == 65535);
    assert(c == 4294967295);
    assert(d == 18446744073709551615);
    assert(e == 127);
    assert(f == 32767);
    assert(g == 2147483647);
    assert(h == 9223372036854775807);

    print("Ok\n");
}
