#import "library/assert.w"

main :: () #entry {
    a: u8 = 0;
    b: u16 = 0;
    c: u32 = 0;
    d: u64 = 0;
    e: i8 = 0;
    f: i16 = 0;
    g: i32 = 0;
    h: i64 = 0;

    assert(a == 0);
    assert(b == 0);
    assert(c == 0);
    assert(d == 0);
    assert(e == 0);
    assert(f == 0);
    assert(g == 0);
    assert(h == 0);

    print("Ok\n");
}
