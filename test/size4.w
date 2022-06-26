#import "library/assert.w"

main :: () #entry {
    a: i8 = -128;
    b: i16 = -32768;
    c: i32 = -2147483648;
    d: i64 = -9223372036854775808;

    assert(a == -128);
    assert(b == -32768);
    assert(c == -2147483648);
    assert(d == -9223372036854775808);

    print("Ok\n");
}
